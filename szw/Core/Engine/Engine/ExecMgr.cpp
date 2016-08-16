#include "StdAfx.h"
#include "ExecMgr.h"
#include "EngineView.h"
#include "Engine.h"
#include <iostream>
extern std::map<CStdString, DWORD> g_mapStopTaskMgr;
extern CLock g_lockStop;

CExecMgr::CExecMgr(CCommClient *pCommChannel, CStdString strTaskID)
{
	m_vBlockData.clear();
	m_bStop = false;
	m_pActiveView = NULL;

	m_pCommChannel = pCommChannel;
	m_strTaskID = strTaskID;
	m_dwAmbientVal = 0;
	m_nOneTaskMaxPage = 2;
	m_nCurVLIndexValue = 0;

	m_bIdentifyOK = false;

	m_bExitProcessOneCircle = false;
	m_nRunMode = ((CSumengine2App*)AfxGetApp())->GetCurrentRunMode();

	m_hTaskConfilict = NULL;
	//启动控制线程
	m_hThread = CreateThread(NULL, 0, &CExecMgr::ThreadProc, this, 0, NULL);

	if (NULL != m_hThread)
		g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("CExecMgr初始化成功, taskid:%s！"), CString(strTaskID));
	else
		g_runlog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("CExecMgr初始化失败 err:%d, taskid:%s！"), GetLastError(), CString(strTaskID));
}

CExecMgr::~CExecMgr(void)
{
	m_bStop = true;
	m_bExitProcessOneCircle = true;
}

CStdString CExecMgr::GetTaskWebName()
{
	return m_strNsName;
}

void CExecMgr::PushBlockData(CStdString strData, CStdString strNsName)
{
	//保护列表
	CLocalLock lock(&m_lock);

	m_vBlockData.push_back(strData);

	m_strNsName = strNsName;

	CStdString strLogName = _T("");
	strLogName.Format(_T("Core\\%s.log"), m_strTaskID.GetBuffer());
	if (!m_log.IsLogFileOpened())
		m_log.OpenLogFile(strLogName.GetBuffer(),NULL,TRUE);

	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("收到执行块数据"));
}

bool CExecMgr::GetFrontBlockData(CStdString &strData)
{
	//保护列表
	CLocalLock lock(&m_lock);

	if (m_vBlockData.empty())
		return false;

	strData = m_vBlockData.at(0);

	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("获取到执行块数据"));
	//取出后将其从缓存列表中移除
	m_vBlockData.erase(m_vBlockData.begin());

	return true;
}

void CExecMgr::SetStopFlag(void)
{
	m_bStop = true;
	m_bExitProcessOneCircle = true;//线程stop，这样就可以通知正在执行processonecircle的程序退出
}

//停止任务关闭和其相关的视图资源
bool CExecMgr::Stop(void)
{
	//关闭任务视图
	UpdateTaskViews();
	if (m_vTaskViewWnd.size() > 0)
	{
		std::vector<CSumengine2View *>::iterator itView = m_vTaskViewWnd.begin();
		for (; itView != m_vTaskViewWnd.end(); ++itView)
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("发送数据到VIew来关闭资源"));
			LRESULT lRes;
			if (0 == SendMessageTimeout((*itView)->m_hWnd, WM_FREE_VIEW_RESOURCE, 0, 0, SMTO_BLOCK, 2000, (DWORD*)&lRes))//如果发送失败
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("没有关闭的VIEW"));
			}
		}
	}

	m_vTaskViewWnd.clear();
	g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("停止网站任务成功：%s"), CString(m_strNsName));

	return true;
}

DWORD WINAPI CExecMgr::ThreadProc(LPVOID lpParameter)
{
	CExecMgr *pMgr = (CExecMgr *)lpParameter;

	while (!pMgr->m_bStop)
	{
		CStdString strData = _T("");

		if (pMgr->GetFrontBlockData(strData))
		{
			T_DATA_IN dataIn;
			dataIn.SetSource(strData);
			g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("+++++++++++++++++++++++++++接受到执行快+++++++++++++++++++++++++++"));
			try
			{
				if (dataIn.strCmd == _T("EXEC"))
				{
					//执行数据块
					g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("+++++++++++++++++++++++++++开始执行数据块+++++++++++++++++"));
					pMgr->Exec(dataIn);
					g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("+++++++++++++++++++++++++++执行完成数据块+++++++++++++++++"));
				}
			}
			catch (...)
			{	
				g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("+++++++++++++++++++++++++++执行数据块异常+++++++++++++++++"));
				pMgr->ReportException();

				T_STEP_CTRL_PARAM ctrlStop;
				ctrlStop.eStepRes = srFinishedException;
				CStdString strRetDataSet = _T("");
				CStdString strCmd = _T("EXEC");
				pMgr->SendExecResult(strCmd, strRetDataSet, ctrlStop);
				continue;
			}
		}
		Sleep(100);
	}

	pMgr->Stop();
	delete pMgr;

	return 0;
}

int CExecMgr::GetSubBlockLoopTimes(const CBlockData &blockData)//子块需要重复操作的次数
{
	int ret = 1;

	if (blockData.GetLoopFlag())
	{
		CString strType = _T("");
		switch (blockData.GetLoopType())
		{
		case eltLoopMax:
			//不限制具体的循环次数
			//循环结束条件：本次循环操作的所有对象不同于之前任一次
			ret = INT_MAX;

			strType = ToString(eltLoopMax);
			break;
		case eltLoopRandom:
			//随机循环 次数为0~最大次数（变量值）之间的随机数
			if (_ttoi(blockData.GetLoopValue()) <= 0)
				ret = 0;
			else
				ret = rand() % _ttoi(blockData.GetLoopValue());

			strType = ToString(eltLoopRandom);
			break;
		case eltLoopFixed:
			//固定循环 次数固定
			ret = _ttoi(blockData.GetLoopValue());

			//固定循环引用的动参可能提取失败，默认执行1次
			if (ret == 0)
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("指定固定循环次数为0（GetLoopValue=%s），将执行1次！"), CString(blockData.GetLoopValue()));
				ret = 1;
			}

			strType = ToString(eltLoopFixed);
			break;

		case eltLoopFixedLessOne://固定循环次数-1
			ret = _ttoi(blockData.GetLoopValue());
			ret--;

			//固定循环引用的动参可能提取失败，默认执行1次
			if (ret == 0)
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("指定固定循环次数为0（GetLoopValue=%s），将执行1次！"), CString(blockData.GetLoopValue()));
				ret = 1;
			}
			strType = ToString(eltLoopFixedLessOne);
			break;

		default:
			ret = 0;
		}

		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("执行块循环类型：%s, 循环次数：%d！"), strType, m_nBlockTargetLoopTimes);
	}

	return ret;
}

//执行执行块的循环设置
void CExecMgr::ParseBlockLoopSetting(const CBlockData &blockData)
{
	if (blockData.GetLoopFlag())
	{
		CString strType = _T("");
		switch (blockData.GetLoopType())
		{
		case eltLoopMax:
			//不限制具体的循环次数
			//循环结束条件：本次循环操作的所有对象不同于之前任一次
			m_nBlockTargetLoopTimes = INT_MAX;

			strType = ToString(eltLoopMax);
			break;
		case eltLoopRandom:
			//随机循环 次数为0~最大次数（变量值）之间的随机数
			if (_ttoi(blockData.GetLoopValue()) <= 0)
				m_nBlockTargetLoopTimes = 0;
			else
				m_nBlockTargetLoopTimes = rand() % _ttoi(blockData.GetLoopValue());

			strType = ToString(eltLoopRandom);
			break;
		case eltLoopFixed:
			//固定循环 次数固定
			m_nBlockTargetLoopTimes = _ttoi(blockData.GetLoopValue());

			//固定循环引用的动参可能提取失败，默认执行1次
			if (m_nBlockTargetLoopTimes == 0)
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("指定固定循环次数为0（GetLoopValue=%s），将执行1次！"), CString(blockData.GetLoopValue()));
				m_nBlockTargetLoopTimes = 1;
			}

			strType = ToString(eltLoopFixed);
			break;

		case eltLoopFixedLessOne://固定循环次数-1
			m_nBlockTargetLoopTimes = _ttoi(blockData.GetLoopValue());
			m_nBlockTargetLoopTimes--;

			//固定循环引用的动参可能提取失败，默认执行1次
			if (m_nBlockTargetLoopTimes == 0)
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("指定固定循环次数为0（GetLoopValue=%s），将执行1次！"), CString(blockData.GetLoopValue()));
				m_nBlockTargetLoopTimes = 1;
			}
			strType = ToString(eltLoopFixedLessOne);
			break;

		default:
			m_nBlockTargetLoopTimes = 0;
		}

		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("执行块循环类型：%s, 循环次数：%d！"), strType, m_nBlockTargetLoopTimes);
	}
	else
	{
		m_nBlockTargetLoopTimes = 1;
	}
}

void CExecMgr::ReplaceLoopIndex(CStdString &strDataSet, int &nLoopTimes, CBlockData &objBlockData)
{
	if (strDataSet.IsEmpty())
	{
		return;
	}

	CStdString strLoopIndex;
	strLoopIndex.Format(_T("%d"), nLoopTimes);
	//strDataSet.Replace(_T("%LoopIndex%"), strLoopIndex) ;
	strDataSet.Replace(_T("#LoopIndex#"), strLoopIndex);
	strLoopIndex.Format(_T("%d"), nLoopTimes + 1);
	//strDataSet.Replace(_T("%LoopIndexMoreOne%"), strLoopIndex) ;
	strDataSet.Replace(_T("#LoopIndexMoreOne#"), strLoopIndex);
	strLoopIndex.Format(_T("%d"), nLoopTimes - 1);
	//strDataSet.Replace(_T("%LoopIndexLessOne%"), strLoopIndex) ;
	strDataSet.Replace(_T("#LoopIndexLessOne#"), strLoopIndex);
	objBlockData.SetSource(strDataSet);
}

void CExecMgr::Exec(T_DATA_IN dataIn)
{
	m_bFirstModuleFail = true;//没有失败和第一次失败都为true
	int nVagueLocIndex = -1; //模糊定位对象的序号
	int nLoopTimes = 0;   //循环次数
	CStdString strRetDataSet = _T("");     //提取数据集
	CBlockData objBlockData;
	HISTORY_STEP_HTML vHistoryStepHTMLOfEachCycle; //循环历史中操作对象的HTML
	T_STEP_CTRL_PARAM tStepCtrl;

	bool bSuccess;//是否成功执行完成当前模块
	CTaskData objTaskData;
	objTaskData.SetSource(dataIn.strInDataset);
	int nBlockCount = objTaskData.GetCount();
	CStdString strBlock;
	m_strParamOCR = dataIn.strOCRParam;

	for (int i = 0; i < nBlockCount;)//循环执行task里面的每个block
	{
		bSuccess = true;
		nVagueLocIndex = -1; //模糊定位对象的序号
		nLoopTimes = 0;   //循环次数
		m_nOrcResultFailTimes = 0;
		vHistoryStepHTMLOfEachCycle.clear(); //循环历史中操作对象的HTML
		tStepCtrl.Reset();
		m_bIdentifyOK = false;

		objTaskData.SetActiveIndex(i);//设置task里面当前活动的block
		if (!objTaskData.GetCurObject(objBlockData))//获取当前活动的block
		{
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("获取第%d个block失败，无法执行！顺序跳转到下一个执行"), i);
			continue;
		}
		strBlock = objBlockData.GetSource();

		if (!dataIn.bNoImage)
		{
			m_dwAmbientVal |= DLCTL_DLIMAGES;
		}

		m_strQuickLook = _T("");
		m_strOcrResultInfo = _T("");

		m_nOneTaskMaxPage = dataIn.dwMaxViewCount;       //页面限制

		tStepCtrl.pActiveView = GetTaskView(false);
		if (NULL == tStepCtrl.pActiveView)
		{
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("获取第%d任务视图失败，无法执行数据块！顺序跳转到下一个执行"), i);
			continue;
		}
		tStepCtrl.pActiveView->SetOcrParamFillRP(m_strParamOCR);
		tStepCtrl.pActiveView->ReplaceIIDKs(strBlock);
		objBlockData.SetSource(strBlock);
		ParseBlockLoopSetting(objBlockData);

		//循环处理
		while (true)
		{
			//判断循环是否结束(注意循环可能一次也不执行)
			if (IsLimitedLoopEnd(nLoopTimes) || IsUnlimitedLoopEnd(m_vStepHTML, vHistoryStepHTMLOfEachCycle))
			{
				break;
			}

			tStepCtrl.Reset();
			m_vStepHTML.clear();
			tStepCtrl.pActiveView = GetTaskView(false);
			if (NULL == tStepCtrl.pActiveView)
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("获取任务视图失败，无法执行数据块！"));
				break;
			}

			nLoopTimes++;
			m_nCurVLIndexValue = nLoopTimes;//执行块执行到第几次，VLIndex参数就如何
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("开始执行数据块:%s, 当前执行次数: %d"), CString(objBlockData.GetDescription()), nLoopTimes);

			//替换%LoopIndex%为循环序号，作为定位页数的值
			CStdString strBlockReplace = strBlock;
			ReplaceLoopIndex(strBlockReplace, nLoopTimes, objBlockData);

			tStepCtrl.pActiveView->InitCodeStyleFlag();//初始化3个打码标志；等这个block结束后就知道用过那些，没有用过那些方式打码
			m_nOrcResultFailTimes = 0;//设置由于成功标记失败而导致的block重试的次数
			m_nRestartFrom1thStepTimes = 0;
			if (!ProcessOneCycle(objBlockData, tStepCtrl, strRetDataSet, nVagueLocIndex) || m_bExitProcessOneCircle)
			{
				if (m_bExitProcessOneCircle)
				{
					m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("正在执行的ProcessOneCycle需要强行退出"));
					//向上层返回数据
					tStepCtrl.eStepRes = srFinishedException;
					goto RESULT;
				}
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("ExecMgr:ProcessOneCycle退出,失败"));
				if (objBlockData.GetFaultTolerant() != TRUE)//模块容错的话就算成功跳转条件，现在容错算失败处理，这样这个模块就不会被执行多次，但是算成功跳转
				{
					bSuccess = false;
				}
				break;
			}
		}

		i = JumpToWhichModule(objBlockData, i, nBlockCount, bSuccess);
		if (-1 == i)//模块执行失败，并且不跳转，退出
		{
			break;
		}
	}

RESULT:
	SendExecResult(dataIn.strCmd, strRetDataSet, tStepCtrl);
}

bool CExecMgr::IsConditionJumpOK(const CBlockData &objBlockData)
{
	bool bOK = false;

	if (objBlockData.GetIsJumpCondition())//如果勾选条件跳转
	{
		CStdString strLeft = objBlockData.GetLeftCondition();
		CStdString strRight = objBlockData.GetRightCondition();
		long nJumpType = objBlockData.GetConJumpType();//0整数比较1字符串比较
		long nCompareType = objBlockData.GetConditionComp();//大于2等于0小于1
		if (0 == nJumpType)//整数比较
		{
			CString strTmp(strLeft.GetBuffer());
			int lLeft = _ttoi(strTmp);

			CString strTmp1(strRight.GetBuffer());
			int lRight = _ttoi(strTmp1);

			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("整数比较，left = %d,right =%d,比较条件为%d，大于2等于0小于1"), lLeft, lRight, nCompareType);

			if (((lLeft > lRight) && (nCompareType == 2)) || ((lLeft < lRight) && (nCompareType == 1)) || ((lLeft == lRight) && (nCompareType == 0)))
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("整数比较成立"));
				bOK = true;
			}
		}
		else//字符串比较
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("整数跳转，left = %s,right =%s,比较条件为%d，大于2等于0小于1"), strLeft, strRight, nCompareType);
			if (((strLeft > strRight) && (nCompareType == 2)) || ((strLeft < strRight) && (nCompareType == 1)) || ((strLeft == strRight) && (nCompareType == 0)))
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("字符串比较成立"));
				bOK = true;
			}
		}
	}

	return bOK;
}

int CExecMgr::JumpToWhichModule(const CBlockData &objBlockData, int curIndex, int nBlockCount, bool bSuccess)//需要跳转到那个模块
{
	int nResult = 0;
	int tempJump = -1;
	//int failNotJump = false;
	bool bCorrectIndex = false;

	//最后在这里执行跳转，如果不跳转则i++顺序执行，注意：模块序号从0开始算第一个模块
	if (objBlockData.GetIsJump())//判断当前执行的模块是不是有跳转
	{
		if (bSuccess)//假如模块执行成功
		{
			//failNotJump = false;
			//勾了条件优先和条件跳转,后面还要加是不是符合条件跳转，由提取动参决定
			if (objBlockData.GetIsConditionFirst() && IsConditionJumpOK(objBlockData))
			{
				tempJump = objBlockData.GetConditionToWhich();
				bCorrectIndex = true;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("条件优先，执行成功后条件跳转，跳转模块===module = %d==="), tempJump);
			}
			else if (objBlockData.GetIsJumpSucc())//如果不是条件先跳转了，就看成功跳转
			{
				tempJump = objBlockData.GetSuccToWhich();
				bCorrectIndex = true;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("非条件优先，执行成功跳转，跳转模块===module = %d==="), tempJump);

			}
			else if (!objBlockData.GetIsConditionFirst() && objBlockData.GetIsJumpCondition())//如果没勾条件优先，成功跳转又没跳，有条件跳转的就条件跳转
			{
				tempJump = objBlockData.GetConditionToWhich();
				bCorrectIndex = true;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("非条件优先，执行成功后条件跳转，跳转模块===module = %d==="), tempJump);
			}
		}
		else//模块执行失败
		{
			if (objBlockData.GetIsConditionFirst() && IsConditionJumpOK(objBlockData))
			{
				tempJump = objBlockData.GetConditionToWhich();
				bCorrectIndex = true;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("条件优先，执行失败条件跳转，跳转模块===module = %d==="), tempJump);
			}
			else if (objBlockData.GetIsJumpFail())//如果不是条件先跳转了，就看失败跳转
			{
				tempJump = objBlockData.GetFailToWhich();
				bCorrectIndex = true;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("非条件优先，执行失败跳转，跳转模块===module = %d==="), tempJump);
			}
			else if (!objBlockData.GetIsConditionFirst() && objBlockData.GetIsJumpCondition())//如果没勾条件优先，失败跳转又没跳，有条件跳转的就条件跳转
			{
				tempJump = objBlockData.GetConditionToWhich();
				bCorrectIndex = true;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("非条件优先，执行失败条件跳转，跳转模块===module = %d==="), tempJump);
			}
			else//如果执行块失败并且没有任何跳转，就退出
			{
				//failNotJump = true;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("没有模块跳转，执行失败，退出循环"));
			}
		}
	}

	if (bCorrectIndex)//有跳转
	{
		if (tempJump >= 0 && tempJump < nBlockCount)
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("跳转到模块%d"), tempJump);
			nResult = tempJump;
		}
		else
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("跳转到了没有的模块，退出循环"));
			nResult = -1;
		}
	}
	else//没有跳转
	{
		if (bSuccess)
		{
			curIndex++;
			nResult = curIndex;
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("顺序执行模块%d"), nResult);
		}
		else//没有任何跳转的失败
		{
			nResult = -1;
		}
	}

	return  nResult;
}

//按顺序执行一次数据块中的所有对象
bool CExecMgr::ProcessOneCycle(CBlockData &objBlockData, T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strRetDataSet, int &iVagueIndex, BOOL bIsSubRandom)
{
	bool					bBreak = false;
	bool					bNextPageToExecFromFirstStep = false;//“下一页”步骤成功时，是否从第一个步骤重新执行
	CStdString				strEventName = _T("");
	T_STEP_DATA_TO_TASKVIEW tDataToView;
	CStepData				objStepData;
	vector<int>				vecRandomSteps;

	//如果字块为随机执行字块，获取随机步骤
	if (bIsSubRandom)
	{
		GetRandomVec(_ttoi(objBlockData.GetRandomCount()), objBlockData.GetCount(), vecRandomSteps);
	}

	Wait(objBlockData.GetPerWait());
	for (long lngIndex = 0; lngIndex < objBlockData.GetCount(); /*lngIndex++*/)
	{
		//匹配随机步骤操作,步骤容器为0代表已经匹配结束
		if (bIsSubRandom)
		{
			if (vecRandomSteps.size() == 0)
			{
				break;
			}
			else
			{
				if (!IsMatchStep(lngIndex, vecRandomSteps))
				{
					++lngIndex;
					continue;
				}
			}
		}

		WaitForStepContinueSignal();				 //检查是否允许继续执行
		objBlockData.GetStep(lngIndex, objStepData); //获取要执行的step
		if (objStepData.GetAction() == amSetDataArea)//包含子操作的母step
		{
			//T_STEP_CTRL_PARAM	tSubStepCtrl;
			int					nSubVagueLocIndex = -1; //模糊定位对象的序号
			int					nSubLoopTimes = 0;   //循环次数	
			CStdString			strSubBlock;
			CBlockData			SubBlock = *(objStepData.m_pSubBlock);
			CStdString			strSubRet = _T("");
			int                 nSubBlockTimes = 1;

			lngIndex++;//母操作也是一步step，做完以后需要自加
			strSubBlock = SubBlock.GetSource();
			tStepCtrl.Reset();
			tStepCtrl.pActiveView = GetTaskView(false);
			if (NULL == tStepCtrl.pActiveView)
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("获取第%d任务视图失败，无法执行数据块！顺序跳转到下一个执行"));
				continue;
			}
			tStepCtrl.pActiveView->ReplaceIIDKs(strSubBlock);
			SubBlock.SetSource(strSubBlock);
			//ParseBlockLoopSetting(SubBlock);
			nSubBlockTimes = GetSubBlockLoopTimes(SubBlock);

			//循环处理
			while (true)
			{
				//判断循环是否结束(注意循环可能一次也不执行)
				if (nSubLoopTimes >= nSubBlockTimes)
				{
					break;
				}

				tStepCtrl.Reset();
				tStepCtrl.pActiveView = GetTaskView(false);
				if (NULL == tStepCtrl.pActiveView)
				{
					m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("-------获取任务视图失败，无法执行数据块！-------"));
					break;
				}

				nSubLoopTimes++;
				m_nCurVLIndexValue = nSubLoopTimes;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("-------开始执行数据块:%s, 当前执行次数: %d-------"), CString(objBlockData.GetDescription()), nSubLoopTimes);
				//替换%LoopIndex%为循环序号，作为定位页数的值
				CStdString strSubBlockReplace = strSubBlock;
				ReplaceLoopIndex(strSubBlockReplace, nSubLoopTimes, SubBlock);

				//这里母模块要当做一个步骤来执行，而不是block，返回的结果也这样
				if (!ProcessOneCycle(SubBlock, tStepCtrl, strSubRet, nSubVagueLocIndex, SubBlock.GetSubRandom()) || m_bExitProcessOneCircle)
				{
					if (m_bExitProcessOneCircle)
					{
						m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("正在执行的子块ProcessOneCycle需要强行退出"));
						return false;
					}

					if (SubBlock.GetFaultTolerant() == TRUE)//数据块容错成功
					{
						m_bFirstModuleFail = true;
						break;
					}
					else//步骤不容错
					{
						if (m_bFirstModuleFail)
						{
							strRetDataSet = strSubRet;
							m_bFirstModuleFail = false;
							m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("-------ExecMgr:ProcessOneCycle退出,失败-------"));
						}
						return false;
					}
				}
			}
		}
		else//普通step操作
		{
			//填充发送给任务视图的数据//每个步骤一个通知对象，确保步骤结束或视图关闭时能够及时返回
			strEventName.Format(_T("ExecMgr_Event_EXEC_%s_%d"), m_strTaskID, lngIndex);
			tDataToView.hSignalEvent = CreateEvent(NULL, FALSE, FALSE, strEventName.GetBuffer());
			
			tDataToView.strStepData = objStepData.GetSource();
			tDataToView.iObjIndexForVagueLoc = iVagueIndex;
			tDataToView.iCurVLIndexValue = m_nCurVLIndexValue;//当前执行块或者执行子块循环执行到第几次了（VLIndex）
			tDataToView.strStepID.Format(_T("%d-%d-%s"), lngIndex, objBlockData.GetCount(), objStepData.GetDescrition());
			tDataToView.nRetryVertifyTimes = m_nOrcResultFailTimes;
			tDataToView.strOCRParam = m_strParamOCR;
			tStepCtrl.objCurStep = objStepData;
			tStepCtrl.lngActiveStep = lngIndex;

			//此处新增对cpu使用率的检测，如果超过了指定的值，则等待，直至主控超时
			WaitForCpuUsage();

			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:开始执行步骤[%s],并等待其返回！"), CString(objStepData.GetDescrition()));
			tStepCtrl.pActiveView->PostMessage(WM_VIEW_TASK_DATA, (WPARAM)&tDataToView);

			WaitForSingleObject(tDataToView.hSignalEvent, INFINITE);  //等待步骤返回
			if (m_bExitProcessOneCircle)
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("m_bExitProcessOneCircle为真的返回！"));
				CloseHandle(tDataToView.hSignalEvent);
				//throw;
				return false;
			}

			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:步骤[%s]返回！"), CString(objStepData.GetDescrition()));
			
			CloseHandle(tDataToView.hSignalEvent);
			DealStepResult(objBlockData, tStepCtrl, strRetDataSet, iVagueIndex);

			if (tStepCtrl.bBreak)//步骤出错
			{	
				if (tStepCtrl.eStepRes == srFinishPost)
				{
					lngIndex++;
					continue;
				}
				//当前失败步骤为成功标记且下一步仍是成功标记的，忽略此步错误，执行下一个成功标记
				if (IgnoreStepError(objBlockData, lngIndex + 1))
				{
					lngIndex++;
					continue;
				}

				//成功标记失败后从第1步重新执行的情况
				if (objStepData.GetAction() == amSuccFeature && objStepData.GetFlagOfRestartFrom1thStep() && m_nRestartFrom1thStepTimes++ < g_cfgParam.nMaxTimesOfRestartFrom1thStep)
				{
					m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("成功标记失败后从第1步重新执行：%d次！"), m_nRestartFrom1thStepTimes);
					lngIndex = 0;
					continue;
				}
				//出错退出（验证码错误时需重试）
				if (objStepData.GetAction() != amVerifyCodeSuccFeature)
				{
					break;
				}
			}

			//步骤执行成功
			if (tStepCtrl.bDoNextStep)
			{	
				//步骤指定了取快照页面
				if (objStepData.GetPageShot())
				{
					m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:SaveQuickLook(tStepCtrl.pActiveView！"));
					SaveQuickLook(tStepCtrl.pActiveView, TRUE);
				}
				//有循环标记则保存步骤的HTML
				if (objBlockData.GetLoopFlag())
				{
					m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:tStepCtrl.tStepCtrl.pActiveView->GetStepObjHTML！"));
					m_vStepHTML.push_back(tStepCtrl.pActiveView->GetStepObjHTML());
				}
				//成功后步骤延时（容错成功时不延时）
				if (tStepCtrl.eStepRes == srFinishedOK)
				{
					Wait(objStepData.GetSurWait());
				}
				MoveToNextStep(objBlockData, lngIndex);

				//翻页循环的情况
				if (objStepData.GetAction() == amNextPage && bNextPageToExecFromFirstStep)
				{
					bNextPageToExecFromFirstStep = false;
					lngIndex = 0;
				}

			}
			else
			{
				if (objStepData.GetAction() == amVerifyCodeSuccFeature)//如果验证码标记失败，验证码也需要延时，这样好找成功标记，可能成功标记在延时那段时间出来了
				{
					Wait(objStepData.GetSurWait());
				}
			}

			//翻页
			if (tStepCtrl.eStepRes == srIndexOutOfVagueLocRange)
			{
				lngIndex = tStepCtrl.lngActiveStep;//指定执行“下一页”
				//循环模式：//1.设置执行块的“循环执行”标记//2.“下一页”自动翻页循环//当存在“循环执行”标记时，自动翻页循环功能受限（此时“下一页”仅当作普通步骤，不再跳转到第一步执行）
				if (objBlockData.GetLoopFlag() == false)
				{
					bNextPageToExecFromFirstStep = true;//不存在“循环执行”标记时，设置“下一页”后从第一步执行的标记
				}
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:tStepCtrl.eStepRes == srIndexOutOfVagueLocRange！"));
			}
			//验证码错误时回退步骤重试
			if (!tStepCtrl.objCurStep.GetFaultTolerant() && !HandleOcrResult(&tStepCtrl, objBlockData, lngIndex))
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:!tStepCtrl.objCurStep.GetFaultTolerant() && !HandleOcrResult"));
				break;
			}
		}
	}

	//每次processonecircle完成以后，都要清除view的m_objBeforeStep，m_pBrforeStep，也许下一个processonecircle同意会用这个
	tStepCtrl.pActiveView->m_objBeforeStep.Clear();
	tStepCtrl.pActiveView->m_pBrforeStep = NULL;
	tStepCtrl.pActiveView->m_objDelItemStep.Clear();
	tStepCtrl.pActiveView->m_pDelItemStep = NULL;
	tStepCtrl.pActiveView->m_objSaveSuccStep.Clear();
	tStepCtrl.pActiveView->m_pSaveSuccStep = NULL;
	tStepCtrl.pActiveView->m_objCurrentStep.Clear();
	tStepCtrl.pActiveView->m_pCurrentStep = NULL;

	if (objStepData.GetAction() != amSetDataArea)
	{	
		//数据块执行失败
		if (tStepCtrl.bBreak)
		{	
			//在所有视图都执行失败，则返回第一个失败的视图数据作为结果
			if (IsViewExist(tStepCtrl.pFirstFailView))
			{
				tStepCtrl.pActiveView = tStepCtrl.pFirstFailView;
			}
			//保存失败页面的快照
			SaveQuickLook(tStepCtrl.pActiveView, FALSE);
			//数据块容错成功
			if (objBlockData.GetFaultTolerant() == TRUE)
			{
				tStepCtrl.eStepRes = srFinishedOKTolerantFail;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:数据块执行成功！"));
				return true;
			}
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("ExecMgr:数据块执行失败！"));
			return false;
		}

		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:数据块执行成功！"));
		return true;
	}
}

//返回执行结果
void CExecMgr::SendExecResult(const CStdString &strCmd, const CStdString &strRetDataSet, T_STEP_CTRL_PARAM &tStepCtrl)
{
	//向上层返回数据
	T_DATA_OUT dataOut;
	dataOut.strResponseType = strCmd;
	dataOut.eResult = tStepCtrl.eStepRes;

	//此处优先对postfinish类型进行处理

	if (dataOut.eResult == srFinishPost)
	{
		dataOut.strErrDesc = tStepCtrl.pActiveView->GetStepErrDesc();
	}
	else
	{
		if (dataOut.eResult == srFinishedException)
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("dataOut.eResult == srFinishedException"));
			dataOut.strTaskID = m_strTaskID;
		}
		else
		{
			//以下错误按照失败处理
			if (dataOut.eResult == srIndexOutOfVagueLocRange ||
				dataOut.eResult == srFinishedError)
			{
				dataOut.eResult = srFinishedFail;
			}

			if ((srFinishedFail == dataOut.eResult || dataOut.eResult == srUploadFileError || dataOut.eResult == srTimeOut) && tStepCtrl.pActiveView != NULL)
			{
				CStdString strErrPageText = tStepCtrl.pActiveView->GetQuickLookText();   //失败页面的文本
				dataOut.strQuickLook = strErrPageText + _T("<@@@>") + m_strQuickLook;    //快照格式：页面文本<@@@>快照路径列表

				if (tStepCtrl.pActiveView->GetStepErrDesc().IsEmpty())
				{
					tStepCtrl.pActiveView->SetStepErrDesc(_T("暂未获取到有效的错误信息,请数据人员进行排查!"));
				}

				dataOut.strErrDesc = tStepCtrl.pActiveView->GetStepErrDesc() + DELIMITER_STEP_ERR_AND_UIMSG + GetUiMsgFromFailedViews();
				dataOut.strCutImg = tStepCtrl.pActiveView->GetCutImg();
			}
			else
			{
				dataOut.strQuickLook = _T("<@@@>") + this->m_strQuickLook;
				dataOut.strErrDesc = _T("");
				dataOut.strCutImg = _T("");
			}

			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ExecMgr:数据块执行结束, eRes:%d！\r\n\r\n"), dataOut.eResult);

			dataOut.strOutDataset = strRetDataSet;
			dataOut.strCodeOcrInfo = m_strOcrResultInfo;
			dataOut.strTaskID = m_strTaskID;
		}
	}
	

	CStdString strResText = m_strTaskID + _T(",") + dataOut.GetSource();

 	//正常模式下，每执行完成一个block，就要删除掉弹框信息,避免干扰下一次的结果(是否要考虑不同的模式???)
 	if (tStepCtrl.pActiveView != NULL)
 	{
 		tStepCtrl.pActiveView->ClearPopupUIMsg();
 	}
	m_pCommChannel->SendData(strResText);
}

//判断类型为eltLoopMax的循环是否执行结束，判断标准：
//当本次操作的所有对象的HTML源码都存在于历史操作记录中时说明循环结束
bool CExecMgr::IsUnlimitedLoopEnd(STEP_HTML_OF_ONE_CYCLE &vStepHTML, HISTORY_STEP_HTML &vHistory)
{
	//非eltLoopMax类型的循环不在此判断
	if (INT_MAX != m_nBlockTargetLoopTimes)
		return false;

	bool bEnd = false;

	for (int i = 0; i < vHistory.size(); i++)
	{
		if (vStepHTML.size() != vHistory[i].size())
			continue;

		int j = 0;
		for (; j < vHistory[i].size(); j++)
		{
			//若有一个元素的HTML不相等则继续下一个循环HTML的比对
			if (vStepHTML[j] != vHistory[i][j])
				break;
		}

		// 本次步骤中的HTML与历史记录中的某次完全相同
		if (j == vHistory[i].size())
		{
			bEnd = true;
			break;
		}
	}

	if (!bEnd)
		vHistory.push_back(vStepHTML);

	return bEnd;
}

//判断有限次循环是否结束
bool CExecMgr::IsLimitedLoopEnd(int iCurLoopTimes)
{
	//类型为eltLoopMax的，不在此检查
	if (INT_MAX == m_nBlockTargetLoopTimes)
		return false;

	if (iCurLoopTimes < m_nBlockTargetLoopTimes)
	{
		return false;
	}

	return true;
}

void CExecMgr::GetORCFlag(bool bSucc, T_STEP_CTRL_PARAM *pParam, int &nAutoReg, int &nLongRange, int &nManmade, int &nVerifyType)
{
	nVerifyType = pParam->pActiveView->GetVerifyCodeType();

	if (bSucc)
	{
		if (pParam->pActiveView->GetCodeOIT() == oitAuto)//当前是自动获取验证码成功
		{
			if (pParam->pActiveView->GetAutoReg())
			{
				nAutoReg = 1;
			}
			else
			{
				nAutoReg = 2;
			}
			if (pParam->pActiveView->GetLongRange())
			{
				nLongRange = 0;
			}
			else
			{
				nLongRange = 2;
			}
			if (pParam->pActiveView->GetManmade())
			{
				nManmade = 0;
			}
			else
			{
				nManmade = 2;
			}
		}
		else if (pParam->pActiveView->GetCodeOIT() == oitLongRange)//当前是远程打码成功
		{
			if (pParam->pActiveView->GetAutoReg())
			{
				nAutoReg = 0;
			}
			else
			{
				nAutoReg = 2;
			}
			if (pParam->pActiveView->GetLongRange())
			{
				nLongRange = 1;
			}
			else
			{
				nLongRange = 2;
			}
			if (pParam->pActiveView->GetManmade())
			{
				nManmade = 0;
			}
			else
			{
				nManmade = 2;
			}
		}
		else//人工输入
		{
			if (pParam->pActiveView->GetAutoReg())
			{
				nAutoReg = 0;
			}
			else
			{
				nAutoReg = 2;
			}
			if (pParam->pActiveView->GetLongRange())
			{
				nLongRange = 0;
			}
			else
			{
				nLongRange = 2;
			}
			if (pParam->pActiveView->GetManmade())
			{
				nManmade = 1;
			}
			else
			{
				nManmade = 2;
			}
		}
	}
	else//全失败
	{
		if (pParam->pActiveView->GetAutoReg())
		{
			nAutoReg = 0;
		}
		else
		{
			nAutoReg = 2;
		}
		if (pParam->pActiveView->GetLongRange())
		{
			nLongRange = 0;
		}
		else
		{
			nLongRange = 2;
		}
		if (pParam->pActiveView->GetManmade())
		{
			nManmade = 0;
		}
		else
		{
			nManmade = 2;
		}
	}
}

bool CExecMgr::HandleOcrResult(T_STEP_CTRL_PARAM *pParam, CBlockData &blockData, long &nStepIndex)
{
	if (pParam->objCurStep.GetAction() == amVerifyCodeSuccFeature)
	{
		if (pParam->bDoNextStep)
		{
			//验证码正确
			//nStepIndex++;
			//nRetryTimes = 0;
			//m_nOrcResultFailTimes = 0;//重置识别次数，为下个验证码做准备

			CStdString strOcrResult = _T("");
			//CStdString strOcrInfo = pParam->pActiveView->GetVerifyCodeResultInfo();
			/*if (strOcrInfo.IsEmpty())
			{
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("验证码信息为空，不做统计！"));
			return true;
			}*/
			int nAutoReg;//获取自动识别标志true
			int nLongRange;//获取远程打码标志true
			int nManmade;//获取人工输入标志true
			int nVerifyType;//
			GetORCFlag(true, pParam, nAutoReg, nLongRange, nManmade, nVerifyType);
			strOcrResult.Format(_T("%d%d%d%d"), nAutoReg, nLongRange, nManmade, nVerifyType);
			//strOcrResult = strOcrResult + strOcrInfo;
			m_strOcrResultInfo += strOcrResult + _T(",");
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("------------------->>>>>>strOcrResult = %s"), CString(strOcrResult));
			//strOcrResult = _T("1|") + strOcrInfo;
			//m_strOcrResultInfo += strOcrResult + _T(",");
		}
		else
		{
			//验证码错误，重试3次
			if (m_nOrcResultFailTimes++ < g_cfgParam.nOcrMaxRetry)
			{
				nStepIndex = 0;  //重新从第一步开始执行
			}
			else
			{
				//验证码达到最大重试次数
				//CStdString strOcrResult = _T("0|") + pParam->pActiveView->GetVerifyCodeResultInfo();
				CStdString strOcrResult = _T("");
				m_nOrcResultFailTimes = 0;//重置识别次数，为下个验证码做准备

				//CStdString strOcrInfo = pParam->pActiveView->GetVerifyCodeResultInfo();
				int nAutoReg;//获取自动识别标志true
				int nLongRange;//获取远程打码标志true
				int nManmade;//获取人工输入标志true
				int nVerifyType;//
				GetORCFlag(false, pParam, nAutoReg, nLongRange, nManmade, nVerifyType);
				strOcrResult.Format(_T("%d%d%d%d"), nAutoReg, nLongRange, nManmade, nVerifyType);
				//strOcrResult = strOcrResult + strOcrInfo;
				m_strOcrResultInfo += strOcrResult + _T(",");
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("------------------->>>>>>strOcrResult = %s"), CString(strOcrResult));
				return false;
			}
		}
	}

	return true;
}

//步骤失败后是否需要暂停执行
bool CExecMgr::PauseStepExec(T_STEP_CTRL_PARAM &tStepCtrl, BOOL bBlockFaultTolerant)
{
	//显示模式及测试模式失败后自动暂停
	if (m_nRunMode == rmShow || m_nRunMode == rmTest)
	{
		if (tStepCtrl.objCurStep.GetFaultTolerant() || bBlockFaultTolerant)
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤或数据块容错，忽略暂停！"));
			return false;
		}
		else
		{
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("执行失败，步骤暂停！"));

			AfxGetApp()->m_pMainWnd->SendMessage(WM_MAINWND_FAILED_PAUSE);
			tStepCtrl.pActiveView->SendMessage(WM_VIEW_ACTIVE_VIEW);

			return true;
		}
	}

	return false;
}

//检查步骤控制信号，直到可以继续执行
//显示模式及测试模式支持自动暂停(失败后)及手动暂停;
//采编模式支持手动暂停
void CExecMgr::WaitForStepContinueSignal(void)
{
	//仅显示模式及采编模式有效
	if (m_nRunMode != rmShow && m_nRunMode != rmCaibian && m_nRunMode != rmTest)
		return;

	if (WAIT_OBJECT_0 != WaitForSingleObject(g_hStepCtrlEvent, 0))
	{
		//m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("检测到暂停信号，步骤暂停！"));
		WaitForSingleObject(g_hStepCtrlEvent, INFINITE);
		//m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("步骤继续！"));
	}
}

//检查视图是否存在
bool CExecMgr::IsViewExist(CSumengine2View *pView)
{
	if (!pView)
	{
		return false;
	}

	UpdateTaskViews();

	for (int i = 0; i < m_vTaskViewWnd.size(); i++)
	{
		if (pView == m_vTaskViewWnd[i])
			return true;
	}

	return false;
}

//保存页面快照，bFlag指定是成功快照还是失败快照
//保存页面快照的文件路径，格式：快照1<@@>快照2...<@@>快照N
void CExecMgr::SaveQuickLook(CSumengine2View *pView, BOOL bFlag)
{
	if (!m_strQuickLook.empty())
		m_strQuickLook += _T("<@@>");

	m_strQuickLook += pView->GenQuickLookPic(bFlag);
}

void CExecMgr::HandleRecognizeResult(T_STEP_CTRL_PARAM &tStepCtrl)
{
	bool bIsTakeMoney = tStepCtrl.pActiveView->m_bIsTakeMoney;
	LONG lRecRes = tStepCtrl.pActiveView->m_RecognizeResult;

	if (tStepCtrl.objCurStep.GetAction() == amIdentify && bIsTakeMoney)//如果当前步骤是识别验证码并且失败了，还拿走了钱
	{
		tStepCtrl.pActiveView->m_RecognizeResult = 0;
		LONG lg = g_orcManmade.GetReportError(lRecRes);
		tStepCtrl.pActiveView->m_bIsTakeMoney = false;
		m_bIdentifyOK = false;
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("；；；；；；；；；；；；返还验证码识别错误现金,返回码%ldcode = %ld, 返回值lg = %ld"), lRecRes, lg);
	}

	if (m_bIdentifyOK && bIsTakeMoney && tStepCtrl.objCurStep.GetAction() == amVerifyCodeSuccFeature)//如果上一步是成功的识别验证码，紧接着这一步失败了
	{
		tStepCtrl.pActiveView->m_RecognizeResult = 0;
		LONG lg = g_orcManmade.GetReportError(lRecRes);
		tStepCtrl.pActiveView->m_bIsTakeMoney = false;
		m_bIdentifyOK = false;
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("；；；；；；；；；；；；返还验证码输入错误现金,返回码code = %ld, 返回值lg = %ld"), lRecRes, lg);
	}
}

//处理步骤结果 
void CExecMgr::DealStepResult(CBlockData &blockData, T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData, int &iVagueIndex, bool bFirstCall)
{
	E_RESULT res = bFirstCall ? tStepCtrl.pActiveView->GetStepResult() : tStepCtrl.eStepRes;
	tStepCtrl.eStepRes = res;

	switch (res)
	{
	case srFinishPost:
		{
			tStepCtrl.bBreak = true;
		}
		break;
	case srFinishedOK:
		HandleStepResultOK(tStepCtrl, strBlockReturnData);
		break;

	case srTimeOut:
		HandleStepResultTimeOut(tStepCtrl);
		break;

	case srFinishedError:
		HandleStepResultFinishError(tStepCtrl, strBlockReturnData);
		break;

	case srFinishedFail:
	case srFinishedOKTolerantFail:
	case srUploadFileError:
		HandleStepResultFail(res, tStepCtrl, strBlockReturnData);
		break;

	case srIndexOutOfVagueLocRange:
		HandleStepResultOutofRange(blockData, tStepCtrl, strBlockReturnData, iVagueIndex);
		break;

	default:
		break;
	}

	//如果有点击删除发布信息的情况
	if (tStepCtrl.pActiveView)
	{
		if (tStepCtrl.pActiveView->m_sFindCanDelete.size() != 0)
		{
			int nCount = tStepCtrl.pActiveView->m_sFindCanDelete.size();
			for (int i = 0; i < nCount; i++)
			{
				tStepCtrl.pActiveView->SetSetpOutData(_T("$InfoID$"), tStepCtrl.pActiveView->m_sFindCanDelete[i]);
			}

			strBlockReturnData += tStepCtrl.pActiveView->GetStepOutData();
		}
	}
}

void CExecMgr::TolerantFailRet(T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData)//提取数据失败返回值
{
	if ((srFinishedOKTolerantFail == tStepCtrl.eStepRes) && (tStepCtrl.objCurStep.GetAction() == amGetAttribute) && (tStepCtrl.objCurStep.GetFaultTolerant()))
	{
		CStdString strTemp = tStepCtrl.pActiveView->GetStepOutData();
		strBlockReturnData += tStepCtrl.pActiveView->GetStepOutData();
	}
}

//步骤执行(容错)成功，准备下一步操作
void CExecMgr::PrepareDoNextStep(T_STEP_CTRL_PARAM &tStepCtrl)
{
	tStepCtrl.lngActiveStep++;
	tStepCtrl.bBreak = false;
	tStepCtrl.nTimeoutRetry = 0;        //重置重试次数
	tStepCtrl.nFailRetry = 0;
	tStepCtrl.nViewSwitchTimes = 0;
	tStepCtrl.bDoNextStep = true;
	tStepCtrl.pFirstFailView = NULL;
}

//检查是否是多成功标记的情况（当前失败步骤为成功标记，
//且下一步也是成功标记的返回true，否则返回false)
bool CExecMgr::IgnoreStepError(CBlockData &blockData, int iStepIndex)
{
	if (iStepIndex > blockData.GetCount())
		return false;

	CStepData objCurStep;
	blockData.GetStep(iStepIndex - 1, objCurStep);

	if (objCurStep.GetAction() == amSuccFeature || objCurStep.GetAction() == amVerifyCodeSuccFeature)
	{
		CStepData objNextStep;
		blockData.GetStep(iStepIndex, objNextStep);

		if (objNextStep.GetAction() == objCurStep.GetAction())
		{
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("成功标记检查失败，将检查下一个成功标记步骤！"));

			return true;
		}
	}

	return false;
}

//延时
void CExecMgr::Wait(int nTime)
{
	if (nTime > 0)
		Sleep(nTime);
}

void CExecMgr::Wait(const CStdString& strSleepTime)
{	
	CStdString strTemp = strSleepTime;

	strTemp.MakeLower();
	int ipos = strTemp.find(_T("#random-"));
	if (ipos == -1)
	{	
		Sleep(_ttoi(strTemp));
		return;
	}

	int ipos1 = strTemp.find(_T("#"), ipos + 8);
	if (ipos1 == -1)
	{
		return;
	}

	//替换通配符
	strTemp.Replace(_T("#random-"), _T(""));
	strTemp.Replace(_T("#"), _T(""));

	//解析出延时区间
	vector<CStdString>vecRange;
	vecRange.clear();

	CStdStrUtils strUtils;
	strUtils.SplitString(strTemp, _T(":"), vecRange, FALSE);

	if (vecRange.size() != 2 || (_ttoi(vecRange[0]) > _ttoi(vecRange[1])))
	{	
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("延时通配符配置有误！%s"), strSleepTime);
		return;
	}
	else
	{	
		DWORD dwsleep = _ttoi(vecRange[0]) + (ulrand() % (_ttoi(vecRange[1]) - _ttoi(vecRange[0])));
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤结束后随机延时%dms"), dwsleep);

		Sleep(dwsleep);
	}
}

//从失败视图中提取页面的弹框提示信息
CStdString CExecMgr::GetUiMsgFromFailedViews(void)
{
	std::vector<CSumengine2View *>::iterator itView = m_vTaskViewWnd.begin();

	CStdString strMsg = _T("");
	for (; itView != m_vTaskViewWnd.end(); ++itView)
	{
		strMsg += (*itView)->GetUiMessage();
	}

	return strMsg;
}

//跳过成功标记步骤
void CExecMgr::MoveToNextStep(CBlockData &blockData, long &tarStepIndex)
{
	tarStepIndex++;

	do
	{
		CStepData objCurStep;
		blockData.GetStep(tarStepIndex - 1, objCurStep);

		//跳过并行的成功标记
		if (objCurStep.GetAction() == amSuccFeature || objCurStep.GetAction() == amVerifyCodeSuccFeature)
		{
			CStepData objNextStep;
			blockData.GetStep(tarStepIndex, objNextStep);

			if (objCurStep.GetAction() == objNextStep.GetAction())
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("成功标记检查成功，跳过相邻的成功标记步骤！"));
				tarStepIndex++;
				continue;
			}
		}

		break;

	} while (true);
}

//跳到操作为“下一页”的步骤，跳转成功返回true，否则false
bool CExecMgr::SkipToNextPageStep(CBlockData &blockData, long &tarStepIndex)
{
	int iBlockSize = blockData.GetCount();

	do
	{
		tarStepIndex++;
		CStepData objStep;
		blockData.GetStep(tarStepIndex, objStep);

		if (objStep.GetAction() == amNextPage)
			return true;

		if (tarStepIndex >= iBlockSize)
			return false;

	} while (true);
}

void CExecMgr::UpdateTaskViews(void)
{
	std::vector<CSumengine2View *> vTaskView;
	//m_mapTaskInfos.clear();

	//查找当前任务的所有View
	CSumengine2App *pMyApp = (CSumengine2App*)AfxGetApp();
	POSITION pos = pMyApp->GetFirstDocTemplatePosition();

	while (NULL != pos)
	{
		CDocTemplate *pDocTemp = pMyApp->GetNextDocTemplate(pos);
		POSITION posDoc = pDocTemp->GetFirstDocPosition();
		while (NULL != posDoc)
		{
			CDocument *pDoc = pDocTemp->GetNextDoc(posDoc);
			POSITION posView = pDoc->GetFirstViewPosition();
			while (NULL != posView)
			{
				CView *pView = pDoc->GetNextView(posView);
				if (pView->IsKindOf(RUNTIME_CLASS(CSumengine2View)))
				{
					CStdString strTmpTaskID = ((CSumengine2View*)pView)->GetTaskID();
					if (strTmpTaskID == m_strTaskID)
					{
						vTaskView.push_back((CSumengine2View *)pView);
						m_mapTaskInfos[(CSumengine2View *)pView].dwCreationTime = ((CSumengine2View*)pView)->GetCreationTime();
						m_mapTaskInfos[(CSumengine2View *)pView].pView = (CSumengine2View *)pView;
					}
				}
			}
		}
	}

	m_vTaskViewWnd.swap(vTaskView);
}

//检查视图是否由OnNewWindow2打开且Document已经被释放了
bool CExecMgr::IsInvalidViewCreatedByOnNewWindow2(CSumengine2View *pView)
{
	//Document为空且该View之前已在m_mapTaskInfos中登记则说明视图的文档被关闭
	IHTMLDocument2 *pCurDoc = (IHTMLDocument2*)(pView->GetHtmlDocument());
	if (pCurDoc == NULL)
	{
		if (m_mapTaskInfos.find(pView) != m_mapTaskInfos.end())
		{
			return true;
		}
	}

	return false;
}

//查找与用于执行任务的视图窗口
CSumengine2View *CExecMgr::GetTaskView(bool bSwitchActiveView)
{
	bool bActiveViewFound = false;

	UpdateTaskViews();       //获取当前任务的所有视图

	//先在视图列表中查找
	std::vector<CSumengine2View *>::iterator itTaskView = m_vTaskViewWnd.begin();

	for (; itTaskView != m_vTaskViewWnd.end(); ++itTaskView)
	{
		//活动视图有效
		if (m_pActiveView == *itTaskView)
		{
			bActiveViewFound = true;
			break;
		}
	}

	//活动视图有效
	if (bActiveViewFound)
	{
		//不需要切换时直接返回活动视图
		if (!bSwitchActiveView)
		{
			return m_pActiveView;
		}
		else
		{
			//需切换视图时找活动视图的下一个视图
			//如果当前活动视图是最后一个视图则返回第一个视图
			if (++itTaskView != m_vTaskViewWnd.end())
			{
				m_pActiveView = *itTaskView;
			}
			else
			{
				m_pActiveView = *(m_vTaskViewWnd.begin());
			}
		}
	}
	else
	{
		//无有效视图时新建
		m_pActiveView = theApp.NewTaskView(m_strTaskID, m_strNsName, m_dwAmbientVal);
	}

	if (m_pActiveView)
	{
		m_pActiveView->SetIIDKs(&m_mapIIDKs);    //为新视图设置动参列表
	}

	//将视图设置为前端显示
	if (m_nRunMode == rmCaibian || m_nRunMode == rmBrowser || m_nRunMode == rmTest)
		m_pActiveView->SendMessage(WM_VIEW_ACTIVE_VIEW);

	return m_pActiveView;
}

//检测是否有需要关闭的视图，如有则关闭之（其它条件满足时默认关闭后打开的页面）
void CExecMgr::CloseUnusedViews(CSumengine2View *&pActiveView, bool bCloseLatestOpen)
{
	UpdateTaskViews();

ReCheck:
	int nCurViewsCount = m_vTaskViewWnd.size();
	CSumengine2View *pTarView = NULL;

	//超出允许的视图个数
	if (nCurViewsCount > m_nOneTaskMaxPage)
	{
		int nMaxFailTimes = -1;

		//Step0. 优先关闭空白页面
		for (int i = 0; i < nCurViewsCount; i++)
		{
			if (m_mapTaskInfos[m_vTaskViewWnd.at(i)].pView->GetViewRelatedURL() == _T(""))
			{
				pTarView = m_mapTaskInfos[m_vTaskViewWnd.at(i)].pView;
				break;
			}
		}

		if (NULL == pTarView)
		{
			//Step1. 先找出最大失败计数值
			for (int i = 0; i < nCurViewsCount; i++)
			{
				if (m_mapTaskInfos[m_vTaskViewWnd.at(i)].iFailCount > nMaxFailTimes)
					nMaxFailTimes = m_mapTaskInfos[m_vTaskViewWnd.at(i)].iFailCount;
			}

			//Step2. 找出失败计数值最大的View
			std::vector<T_TASK_VIEW_INFO> vMaxFailViews;
			for (int j = 0; j < nCurViewsCount; j++)
			{
				if (m_mapTaskInfos[m_vTaskViewWnd.at(j)].iFailCount == nMaxFailTimes)
				{
					vMaxFailViews.push_back(m_mapTaskInfos[m_vTaskViewWnd.at(j)]);
				}
			}

			//Step3. 选择要关闭的视图:如果失败计数最大的View只有一个则关闭之
			int iPos = 0;
			DWORD dwLastCreateTime = vMaxFailViews[0].dwCreationTime;
			for (int i = 1; i < vMaxFailViews.size(); i++)
			{
				if (bCloseLatestOpen)
				{
					//选择后打开的视图
					if (dwLastCreateTime < vMaxFailViews[i].dwCreationTime)
					{
						iPos = i;
						dwLastCreateTime = vMaxFailViews[i].dwCreationTime;
					}
				}
				else
				{
					//选择先打开的视图
					if (dwLastCreateTime > vMaxFailViews[i].dwCreationTime)
					{
						iPos = i;
						dwLastCreateTime = vMaxFailViews[i].dwCreationTime;
					}
				}
			}

			pTarView = vMaxFailViews[iPos].pView;
		}
	}
	else
	{
		//查找是否存在无效视图
		for (int i = 0; i < nCurViewsCount; i++)
		{
			if (IsInvalidViewCreatedByOnNewWindow2(m_vTaskViewWnd.at(i)))
			{
				pTarView = m_mapTaskInfos[m_vTaskViewWnd.at(i)].pView;
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("检测到文档被关闭的视图，将释放文档资源!"));
				break;
			}
		}
	}

	if (pTarView != NULL)
	{
		//Step4. 如果要关闭的视图是当前视图则重新选择一个视图作为当前视图
		if (pActiveView == pTarView)
		{
			pActiveView = GetTaskView(true);
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("要关闭的目标视图为当前视图，已做视图切换!"));
		}

		//Step5. 关闭视图并清理其相关信息
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("视图[%s]将被关闭!"), CString(pTarView->GetViewTitle()));

		pTarView->SendMessage(WM_FREE_VIEW_RESOURCE);
		m_mapTaskInfos.erase(pTarView);
		std::vector<CSumengine2View *>::iterator itView = m_vTaskViewWnd.begin();
		for (; itView != m_vTaskViewWnd.end(); ++itView)
		{
			if (*itView == pTarView)
			{
				m_vTaskViewWnd.erase(itView);
				break;
			}
		}

		goto ReCheck;
	}
}

void CExecMgr::ReportException(void)
{
	try
	{
		CStdString strMsg = _T("");
		strMsg.Format(_T("发现异常，任务ID: %s，NsName: %s！"), m_strTaskID, m_strNsName);
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, CString(strMsg));

		g_msgbox.Show(_T("错误"), strMsg);
	}
	catch (...)
	{
		//避免打日志时出现异常，此处不再处理
	}

	return;
}

void CExecMgr::GetRandomVec(int iRandomCount, int iStepCount, vector<int>&vecRandomStep)
{	
	//如果随机步骤次数为0则重新随机一个次数
	srand((unsigned)time(NULL));

	if (iRandomCount == 0)
	{
		iRandomCount = rand() % (iStepCount+1);
	}
	
	vecRandomStep.clear();
	//随机基数
	int idis = iStepCount;
	for (int iindex = 0; iindex < iRandomCount; ++iindex)
	{	
		vecRandomStep.push_back(rand() % idis);
	}
}

BOOL CExecMgr::IsMatchStep(int iStepindex, vector<int>&vecRandomStep)
{
	for (int iindex = 0; iindex < vecRandomStep.size(); ++iindex)
	{
		if (iStepindex == vecRandomStep[iindex])
		{
			vecRandomStep.erase(vecRandomStep.begin() + iindex);
			return TRUE;
		}
	}
	return FALSE;
}

unsigned long CExecMgr::ulrand(void)
{
	srand((unsigned)time(NULL));
	return (
		(((unsigned long)rand() << 24) & 0xFF000000ul)
		| (((unsigned long)rand() << 12) & 0x00FFF000ul)
		| (((unsigned long)rand()) & 0x00000FFFul));
}

BOOL CExecMgr::IsMutexConfilict()
{	
	CString strMutexName = CString(_T("_ENGINE_")) + CString(m_strTaskID);
	m_hTaskConfilict = CreateMutex(NULL, TRUE, strMutexName);

	if (m_hTaskConfilict && ERROR_ALREADY_EXISTS == GetLastError())
	{	
		CloseHandle(m_hTaskConfilict);
		m_hTaskConfilict = NULL;

		g_runlog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("网站任务ID:%s,MUTEX对象存在,重新压回待执行队列"), m_strTaskID);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CExecMgr::ReleaseConfilictMutex()
{
	if (m_hTaskConfilict)
	{
		ReleaseMutex(m_hTaskConfilict);
		CloseHandle(m_hTaskConfilict);
		m_hTaskConfilict = NULL;
	}
}

void CExecMgr::SendConfilictResultToMC()
{	
	//向上层返回数据
	T_DATA_OUT dataOut;
	dataOut.strResponseType = _T("EXEC");
	dataOut.eResult = srFinishedTaskConfilict;
	dataOut.strTaskID = m_strTaskID;

	CStdString strResText = m_strTaskID + _T(",") + dataOut.GetSource();
	
	m_pCommChannel->SendData(strResText);
}

void CExecMgr::WaitForCpuUsage()
{
	GetCpuUseage getCpuUseage;
	DWORD iCount = 0;

	while (true)
	{
		double fCupUseage = getCpuUseage.CpuUseage();

		if (fCupUseage >= g_cfgParam.nMaxCpuUseage)
		{
			if (iCount == 0)
			{
				++iCount;

				g_CheckCpuLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, \
					_T("当前任务网站:%s,用户CPU使用为:%.2f,超过指定阀值:%d,等待..."), \
					m_strNsName.GetBuffer(), fCupUseage, g_cfgParam.nMaxCpuUseage); \
			}

			Sleep(100);
		}
		else
		{
			if (iCount == 1)
			{
				g_CheckCpuLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, \
					_T("当前任务网站:%s,用户CPU使用为:%.2f,恢复执行..."), \
					m_strNsName.GetBuffer(), fCupUseage); \
			}
			break;
		}
	}
}

void CExecMgr::HandleStepResultOK(T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData)
{
	if (tStepCtrl.objCurStep.GetAction() == amIdentify && tStepCtrl.pActiveView->m_bIsTakeMoney)//识别验证码，并且收了钱
	{
		m_bIdentifyOK = true;
		//m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("；；；；；；；；；；；；识别验证码正确，等待输入正确。。。。。。"));
	}
	else if (tStepCtrl.objCurStep.GetAction() == amVerifyCodeSuccFeature && m_bIdentifyOK)//识别成功并且输入成功
	{
		m_bIdentifyOK = false;
		tStepCtrl.pActiveView->m_bIsTakeMoney = false;
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("；；；；；；；；；；；；输入验证码正确，扣除现金成功---------"));
	}
	PrepareDoNextStep(tStepCtrl);

	m_mapTaskInfos[tStepCtrl.pActiveView].dwCreationTime = GetTickCount();
	m_mapTaskInfos[tStepCtrl.pActiveView].iFailCount = 0;          //查找对象成功时计数清0

	//提取步骤数据
	if (tStepCtrl.objCurStep.GetAction() == amGetAttribute || tStepCtrl.objCurStep.GetAction() == amDownLoadFile)
	{
		strBlockReturnData += tStepCtrl.pActiveView->GetStepOutData();
#ifdef DEBUG
		g_msgbox.Show(_T("提取结果"), tStepCtrl.pActiveView->GetStepOutData());
#endif
	}

	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤：%s 成功！值：%s"), CString(tStepCtrl.objCurStep.GetDescrition()), CString(tStepCtrl.pActiveView->GetStepOutData()));

	//步骤执行成功后检测是否有需要关闭的视图
	CloseUnusedViews(tStepCtrl.pActiveView, tStepCtrl.objCurStep.GetAction() != amClick);
}

void CExecMgr::HandleStepResultTimeOut(T_STEP_CTRL_PARAM &tStepCtrl)
{
	HandleRecognizeResult(tStepCtrl);
	tStepCtrl.nTimeoutRetry++;
	tStepCtrl.bDoNextStep = false;

	//超时重试，达到最大次数仍失败时退出
	if (tStepCtrl.nTimeoutRetry > g_cfgParam.nTimeoutRetry)
		tStepCtrl.bBreak = true;

	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤：%s 超时%d次！"), CString(tStepCtrl.objCurStep.GetDescrition()), tStepCtrl.nTimeoutRetry);
}

void CExecMgr::HandleStepResultFinishError(T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData)
{
	HandleRecognizeResult(tStepCtrl);
	//1.特殊步骤（如找到符合非条件的成功标记等）失败时不重试、不切换视图
	//2.验证码错误时回退步骤重试，但不切换视图，在ProcessOneCycle中处理
	if (tStepCtrl.objCurStep.GetFaultTolerant())
	{
		PrepareDoNextStep(tStepCtrl);
		tStepCtrl.eStepRes = srFinishedOKTolerantFail;
		TolerantFailRet(tStepCtrl, strBlockReturnData);//提取数据失败返回值
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤：%s 容错成功！ 值：%s"), CString(tStepCtrl.objCurStep.GetDescrition()), CString(tStepCtrl.pActiveView->GetStepOutData()));
	}
	else
	{
		tStepCtrl.bBreak = true;
		tStepCtrl.bDoNextStep = false;
	}
}

void CExecMgr::HandleStepResultFail(E_RESULT res, T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData)
{	
	int iTemp = 0;
	HandleRecognizeResult(tStepCtrl);
	tStepCtrl.bDoNextStep = false;

	//失败时先在当前活动视图重试
	if (tStepCtrl.nFailRetry < g_cfgParam.nFailRetry)
	{
		Sleep(1000);
		tStepCtrl.nFailRetry++;
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤：%s 失败%d次！"), CString(tStepCtrl.objCurStep.GetDescrition()), tStepCtrl.nFailRetry);
	}
	else
	{
		m_mapTaskInfos[tStepCtrl.pActiveView].iFailCount++;          //查找对象失败时计数+1

		UpdateTaskViews();       //获取当前任务的所有视图

		iTemp = m_vTaskViewWnd.size();
		if (iTemp == 1)
		{
			if (srFinishedFail == res || srUploadFileError == res)
			{
				//只有一个视图时不做切换
				tStepCtrl.bBreak = true;
				tStepCtrl.nViewSwitchTimes = 0;
				tStepCtrl.pFirstFailView = tStepCtrl.pActiveView;
				if (tStepCtrl.objCurStep.GetAction() == amVerifyCodeSuccFeature)
				{
					tStepCtrl.bVertifyFail = true;
				}
			}

			if (srFinishedOKTolerantFail == res)
			{
				TolerantFailRet(tStepCtrl, strBlockReturnData);//提取数据失败返回值

				PrepareDoNextStep(tStepCtrl);

				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤：%s 容错成功！ 值：%s"), CString(tStepCtrl.objCurStep.GetDescrition()), CString(tStepCtrl.pActiveView->GetStepOutData()));
			}

			return;
		}

		//达到最大失败次数且当前有多个视图时切换视图重试（1次）
		if (tStepCtrl.nViewSwitchTimes++ < iTemp)
		{
			//首次视图切换时保存切换视图前的活动视图，
			//当在其它视图也执行失败时，使用此视图中的执行数据作为返回结果
			if (tStepCtrl.nViewSwitchTimes == 1)
				tStepCtrl.pFirstFailView = tStepCtrl.pActiveView;

			tStepCtrl.nFailRetry = 0;

			tStepCtrl.pActiveView = GetTaskView(true);

			//切换到首次失败的视图本身时，不再累计该视图的失败计数
			if (tStepCtrl.pActiveView == tStepCtrl.pFirstFailView)
				m_mapTaskInfos[tStepCtrl.pActiveView].iFailCount--;

			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤：%s 失败,切换到视图:%s重试！"), \
				CString(tStepCtrl.objCurStep.GetDescrition()), CString(tStepCtrl.pActiveView->GetViewTitle()));
		}
		else
		{
			if (srFinishedFail == res || srUploadFileError == res)
			{
				//在所有视图中都执行失败时退出
				tStepCtrl.bBreak = true;
				tStepCtrl.nViewSwitchTimes = 0;
				if (tStepCtrl.objCurStep.GetAction() == amVerifyCodeSuccFeature)
				{
					tStepCtrl.bVertifyFail = true;
				}
			}

			if (srFinishedOKTolerantFail == res)
			{
				TolerantFailRet(tStepCtrl, strBlockReturnData);//提取数据失败返回值

				PrepareDoNextStep(tStepCtrl);

				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤：%s 容错成功！ 值：%s"), CString(tStepCtrl.objCurStep.GetDescrition()), CString(tStepCtrl.pActiveView->GetStepOutData()));
			}

			CloseUnusedViews(tStepCtrl.pActiveView);      //所有视图切换一遍后，检测是否有需要关闭的视图
		}
	}
}

void CExecMgr::HandleStepResultOutofRange(CBlockData &blockData, T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData, int &iVagueIndex)
{
	tStepCtrl.bDoNextStep = false;

	//下面代码应该放在找到了下一页步骤逻辑块中，否则模糊定位没有下一页但是指定了序号的情况就会出现问题
	//if (iVagueIndex == -1)
	//	iVagueIndex = _ttoi(tStepCtrl.objCurStep.GetVLSymbol().GetBuffer()) - tStepCtrl.pActiveView->GetVagueObjFound();
	//else
	//	iVagueIndex = iVagueIndex - tStepCtrl.pActiveView->GetVagueObjFound();

	//查找“下一页”步骤
	if (SkipToNextPageStep(blockData, tStepCtrl.lngActiveStep))
	{	
		//模糊定位序号大于找到的元素个数时，设置“下次定位序号 = 指定序号或当前序号 - 本次找到的元素个数”以便执行“下一页”后继续定位
		if (iVagueIndex == -1)
			iVagueIndex = _ttoi(tStepCtrl.objCurStep.GetVLSymbol().GetBuffer()) - tStepCtrl.pActiveView->GetVagueObjFound();
		else
			iVagueIndex = iVagueIndex - tStepCtrl.pActiveView->GetVagueObjFound();

		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤：%s 模糊定位超过范围，将定位下页的%d！"), CString(tStepCtrl.objCurStep.GetDescrition()), iVagueIndex);
	}
	else
	{
		if (tStepCtrl.objCurStep.GetFaultTolerant() == TRUE)
		{
			tStepCtrl.eStepRes = srFinishedOKTolerantFail;
		}
		else
		{
			tStepCtrl.eStepRes = srFinishedFail;
		}

		//找不到“下一页”步骤时，执行步骤重试及视图切换操作
		DealStepResult(blockData, tStepCtrl, strBlockReturnData, iVagueIndex, false);
	}
}
