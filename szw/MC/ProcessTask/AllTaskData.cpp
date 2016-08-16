#include "StdAfx.h"
#include "KernelProcessThread.h"
#include "AllTaskData.h"
#include "StationData.h"
#include "BlockData.h"
#include "EnDeCode.h"
#include "StringUtils.h"
#include "CommFunc.h"
#include "3rdparty/json/json.h"
#include <TlHelp32.h>


#define  WAIT_SECOND		180*1000
CAllTaskData  *gAllTaskData = NULL;
extern CKernelProcessThread  *gThreadMessage;


CAllTaskData::CAllTaskData(void)
{
	m_nPort = 32106 ;  //读取不出时默认端口号
	lIsRunEngine = 0;
	lngKernelCnt = 0;
	iWaitTime = WAIT_SECOND;
	bRun = true;
	m_hndEngine = NULL;
	m_hndSingleEngine = NULL;
	m_eCurIeVer = IE8;
	bObjectRunFinish = FALSE;
	m_bCheckRun = FALSE;

	InitData();
}

CAllTaskData::~CAllTaskData(void)
{
	bRun = false;
	Sleep(100);

	m_Connection.UnInit();
	m_ConnectionByObject.UnInit();
	Sleep(1000);

	//关掉多线程的事件
	for(int i=0; i<THREAD_MAX_COUNT; i++)
	{
		if (hEvents[i] != NULL)
		{
			CloseHandle(hEvents[i]);
		}

		hEvents[i] = NULL;

		if (hThreads[i] != NULL)
		{
			if (WaitForSingleObject(hThreads[i], 500) != WAIT_OBJECT_0)
			{
				TerminateThread(hThreads[i], 0);
			}
			CloseHandle(hThreads[i]);
		}
		
		hThreads[i] = NULL;
	}

	//关掉单线程的事件
	if (m_hEvent != NULL)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	if (m_hThread != NULL)
	{
		if (WaitForSingleObject(m_hThread,500) != WAIT_OBJECT_0)
		{
			TerminateThread(m_hThread,0);
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	


}

/*
	@breif插入任务索引到队列
	@param  dwIndex 任务索引
	@param  bSingle 是否Object网站；
	*/
void CAllTaskData::PushTaskIndex( DWORD dwIndex, BOOL bSingle,E_IeLimit ieMode)
{
	m_TaskLock.Lock();
	if (bSingle)
	{
		//单线程		
		m_objectQueueTask[ieMode].push(dwIndex);
	}
	else
	{
		m_allQueueTask[ieMode].push(dwIndex);
	}
	
	m_TaskLock.Unlock();
}

/*
	@breif  取得任务索引，并清除
	@return 返回任务索引
	*/
DWORD CAllTaskData::GetTaskIndex(E_IeLimit eVer)
{
	DWORD dwIndex = 0;
	m_TaskLock.Lock();

	if (m_allQueueTask[eVer].size() > 0)
	{
		dwIndex =m_allQueueTask[eVer].front();
		m_allQueueTask[eVer].pop();
	}

	m_TaskLock.Unlock();

	return dwIndex;
}

/*
	@breif  开始启动任务线程
	@param  bIsOneThread  是否创建单个线程
	*/
void CAllTaskData::StartWork(BOOL bSingleKernel /*= FALSE*/)
{
	if (bSingleKernel)
	{
		//启动单线程的内核
		m_ObjPort = m_nPort+10;  //不同于多线程内核的端口;	
		m_Taskprocess.ConnectKernel(m_ConnectionByObject,m_ObjPort,GetObjectData);
	}
	else
	{
		m_Taskprocess.ConnectKernel(m_Connection,m_nPort,GetData);
	}
	
	int i = 0;
	while (i < 3)
	{
		if (RunEngine(bSingleKernel))
		{
			break;
		}
		if(i == 2)
		{
			g_log.Trace(LOGL_HIG,LOGT_ERROR,__TFILE__,__LINE__,_T("启动内核3次失败，退出主控"));
			ExitProcess(0);	//退出主控
		}
		i++;
	}

	//复制备份邮箱
	T_LocalGlobalData *pGlobalData = NULL;
	pGlobalData = (T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	strMailBak = pGlobalData->aSystemInfo.userInfo.szMailBak;
	strMail = pGlobalData->aSystemInfo.userInfo.szMail;

	gAllTaskData = this;
	int iThreadCount = 0;
	g_Config->ReadInt(_T("MC"), _T("kernel4"), _T("threadcount"), iThreadCount);
	g_Config->ReadInt(_T("MC"), _T("kernel4"), _T("waittime"), iWaitTime);

	if (iThreadCount <= 0)
		iThreadCount = 1;
	if (iThreadCount > THREAD_MAX_COUNT)
		iThreadCount = THREAD_MAX_COUNT;

	if (iWaitTime < 120)
		iWaitTime = 120;
	if (iWaitTime > 600)
		iWaitTime = 600;
	iWaitTime = iWaitTime*1000;
	
	if(bSingleKernel)
	{
		DWORD dwThreadId;
		int i = 0;
		m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		m_hThread = CreateThread(NULL,0,DoSingleTask,(void*)i,0,&dwThreadId);

		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("启动任务单线程完毕"));
	}
	else
	{
		for (int i=0; i<iThreadCount; i++)
		{
			DWORD dwThreadId;
			hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
			hThreads[i] = CreateThread(NULL, 0, DoTask, (void*)i, 0, &dwThreadId);
		}

		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("启动任务多线程完毕"));
	}

}

/*
	@breif  任务线程
	@param  this指针
	*/
DWORD WINAPI CAllTaskData::DoTask( LPVOID lp )
{
	int iCurrentIndex = (int)lp;
	HANDLE  hEvent = NULL;
	bool isRestartEngine = false;

	if (iCurrentIndex < THREAD_MAX_COUNT)
	{
		hEvent = gAllTaskData->hEvents[iCurrentIndex];
	}
	if (hEvent == NULL)  //事件对象创建失败
		return 0;
	
	ASSERT(gAllTaskData != NULL);
	while(gAllTaskData->bRun)
	{
		DWORD dwIndex = gAllTaskData->GetTaskIndex(gAllTaskData->m_eCurIeVer); //取任务

		if (dwIndex > 0)
		{
			gAllTaskData->DoCommonTask(dwIndex,hEvent,FALSE,isRestartEngine);
		}
		else
		{
			//如果是XP系统都不用
			if (!gAllTaskData->bObjectRunFinish && gAllTaskData->m_objectQueueTask[gAllTaskData->m_eCurIeVer].size() <= 0 &&
				gAllTaskData->m_allQueueTask[gAllTaskData->m_eCurIeVer].size() <= 0 && gAllTaskData->CheckMultiTask())
			{
				E_IeLimit eIe =  gAllTaskData->m_eCurIeVer;	
				if (gAllTaskData->CheckRunOver(eIe, FALSE))
				{
					gAllTaskData->RunEngine();
					Sleep(200);
					//如果Object没有这IE类型de网站，则不启动内核
					if (gAllTaskData->m_hndSingleEngine != NULL)
					{
						gAllTaskData->RunEngine(TRUE);
					}
					g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, _T("开始执行新的IE版本，IE版本为:%d"), eIe);
					gAllTaskData->m_eCurIeVer = eIe;
					gAllTaskData->m_bCheckRun = FALSE;
					Sleep(200);
				}
			}
		}

		Sleep(500);
	}

	return 0;
}

/*
	@breif 启动4代内核
	*/
BOOL CAllTaskData::RunEngine(BOOL bSingleKernel)
{
	if (bSingleKernel)
	{
		if (m_Taskprocess.RunEngine(m_ConnectionByObject,m_ObjPort,m_hndSingleEngine,lngKernelCnt))
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("启动单线程4代核成功"));
			return TRUE;
		}
	}
	else
	{
		if(m_Taskprocess.RunEngine(m_Connection,m_nPort,m_hndEngine,lIsRunEngine))
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("启动多线程4代核成功"));
			return TRUE;
		}
	}

	return FALSE;
}

/*
	@brief  处理返回数据
	@param  dwWebId网站id
	@param  pTaskData任务数据
	@param *pTData任务执行块
	@return 0表示此步骤失败不需要继续执行下去，1表示此步骤成功继续执行下去，2表示发布时遇到完善资料，回到上一步
	*/
int CAllTaskData::HandleRetData(DWORD dwWebId, T_TaskData* pTaskData,const DWORD &dwFunType, CTaskData *pTData)
{
	int iReturn = 0;
	T_DATA_OUT tDataOut ;
	CStdString *pResult = NULL;

	std::vector<CStdString> resultNs;
	StringUtils StringUtil;

	pResult = GetResult(dwWebId);

	ASSERT(pTaskData != NULL || pTData != NULL);
	if (pResult != NULL)
	{
		tDataOut.SetSource(*pResult);
		g_log.Trace(LOGL_MID, LOGT_DEBUG, __TFILE__, __LINE__, _T("收到内核返回的数据：%s"),(*pResult).c_str());
		StringUtil.SplitString(tDataOut.strQuickLook, MAIN_SPLITOR,resultNs,true);

		if ( tDataOut.strResponseType == _T("EXEC") )
		{
			switch (tDataOut.eResult)
			{
			case srNoneResult:
			case srFinishedFail:
			case srTimeOut:
				{
					iReturn = 0;
					//检测注册是否成功的登录功能直接退出
					if (pTaskData->bGetLogin)
					{
						g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("登录验证数据失败，直接返回注册失败"));
						return iReturn;
					}
					pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
					pTaskData->tTaskResultInfo.strMigrateResult = _T("");   //清空上次执行提取的字段；
					pTaskData->tTaskResultInfo.dwSuccFieldCnt = 0;
					//为失败分类并返回接下来执行步骤；
					iReturn = m_Taskprocess.GetFailedResult(pTaskData,pTData,tDataOut,resultNs);
					
				}
				break;
			case srFinishedOK:
			case srFinishedOKTolerantFail:
				pTaskData->tTaskResultInfo.strResultInfo = _T("");
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_SUCC;

				if (pTaskData->bGetLogin)
				{
					g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("通过注册调用登录验证数据成功，继续执行注册任务；"));
					pTaskData->bGetLogin = FALSE;
					return 1;
				}

				if (pTaskData->tTaskResultInfo.strHtml.length() == 0)
				{
					if (resultNs.size() >= 2)
						pTaskData->tTaskResultInfo.strHtml = resultNs[1];
				}
				
				iReturn = m_Taskprocess.GetSuccResult(pTaskData,tDataOut,dwFunType);

				break;
			case srFinishedException:
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("返回内核检测到步骤异常,直接结束该网站id：%d"), dwWebId);
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
				break;
			case srFinishedTaskConfilict:
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("返回内核正在运行,返回继续压入id：%d"), dwWebId);
				iReturn = 5;
				break;
			case srUploadFileError:
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("返回内核检测到上传图片失败,直接结束该网站id：%d"), dwWebId);
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAILUPLOADPIC;
				pTaskData->tTaskResultInfo.strResultInfo = tDataOut.strErrDesc;
				break;
			case srFinishPost:				
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
				resultNs.clear(); 
				StringUtil.SplitString(tDataOut.strErrDesc, DESC_SPLITOR, resultNs, true);
				if (resultNs.size() < 0)
				{					
					break;
				}

				iReturn = GetFormData(resultNs[1], pTaskData->dwCompanyId, pTaskData->tTaskResultInfo.strResultInfo);
				if (iReturn >= 0)
				{
					pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_SUCC;					 
				}				
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("返回内核执行Post表单数据,该网站id：%d,返回ID:%d"), dwWebId,iReturn);
				iReturn = 0;
				break;
			default:
				{
					pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
					iReturn = 0;
				}
				
				break;
			}

			if (tDataOut.strCodeOcrInfo.length() > 0)
			{
				pTaskData->tTaskResultInfo.listOcrCode.push_back(tDataOut.strCodeOcrInfo);
			}
		}

		delete pResult;
	}
	else
	{
		pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
	}
	RemoveResult(dwWebId);

	return iReturn;
}

/*
	@breif  将网站网站id与事件对象关联
	@param  dwWebId 网站id
	*/
void CAllTaskData::LinkWeiIdToEvent( DWORD dwWebId, HANDLE hEvent )
{
	m_EventLock.Lock();
//	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("增加网站ID事件,网站id：%d,事件数量：%d"),dwWebId,IndexToEvent.size());
	IndexToEvent[dwWebId] = hEvent;
	m_EventLock.Unlock();
}

/*
	@breif  移除该网站id的事件对象
	@param  dwWebId 网站id
	*/
void CAllTaskData::RemoveWebIdEvent( DWORD dwWebId )
{
	m_EventLock.Lock();

	std::map<DWORD, HANDLE>::iterator iter = IndexToEvent.find(dwWebId);
	if (iter != IndexToEvent.end())
	{
	//	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("移除网站ID事件,网站id：%d,事件数量：%d"),dwWebId,IndexToEvent.size());
		IndexToEvent.erase(iter);
	//	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("移除网站ID事件后,网站id：%d,事件数量：%d"),dwWebId,IndexToEvent.size());
	}
	
	m_EventLock.Unlock();
}

/*
	@breif  保存某网站操作结果
	@param  dwWebId 网站id
	@param  dwWebId 结果描述
	*/
void CAllTaskData::PushResult( DWORD dwWebId, const CStdString &strResult )
{
	m_TaskResultLock.Lock();

	webIdToResult[dwWebId] = new CStdString(strResult);

	m_TaskResultLock.Unlock();
}

/*
	@breif  去除网站操作结果
	@param  dwWebId 网站id
	*/
void CAllTaskData::RemoveResult( DWORD dwWebId )
{
	m_TaskResultLock.Lock();

	std::map<DWORD, CStdString*>::iterator iter = webIdToResult.find(dwWebId);
	if (iter != webIdToResult.end())
	{
// 		delete iter->second;
// 		iter->second = NULL;

		webIdToResult.erase(iter);   //防止迭代器失效
	}

	m_TaskResultLock.Unlock();
}

/*
	@breif  取得网站操作结果
	@param  dwWebId 网站id
	*/
CStdString * CAllTaskData::GetResult( DWORD dwWebId)
{
	CStdString *pReturn = NULL;
	m_TaskResultLock.Lock();
	pReturn = webIdToResult[dwWebId];
	m_TaskResultLock.Unlock();

	return pReturn;
}

/*
@breif  继续网站数据操作
@param  dwWebId 网站id
*/
void CAllTaskData::ContinueOperate(DWORD dwWebId )
{
	SetEvent(IndexToEvent[dwWebId]);
}

/*
	@breif  处理任务结束后，后继收尾操作
	@param  dwWebId 网站id
	@param  strWebName  网站名称
	@param  dwIndex  任务索引
	@param  bSendStop 当前是否需要告知四代核停止该任务
	*/
void CAllTaskData::HandleCompleteWeb( DWORD dwWebId, const CStdString &strWebName, DWORD dwIndex, bool bSendStop,bool bSingleThread)
{
	if (bSendStop)
	{
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("网站%d 任务结束"), dwWebId);

		T_DATA_IN dataToEngine ;
		dataToEngine.strCmd = _T("STOP");
		dataToEngine.strNsName = strWebName;

		CString strTaskID ;
		strTaskID.Format(_T("%d") , dwWebId) ;
		CStdString strTaskData = (CStdString)strTaskID + _T(",") + dataToEngine.GetSource() ;
		
		if (bSingleThread)
		{
			if (m_ConnectionByObject.SendData( strTaskData ) != -1)
			{
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("网站任务%d 发送停止成功"), dwWebId);
			}
		}
		else
		{
			if (m_Connection.SendData( strTaskData ) != -1)
			{
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("网站任务%d 发送停止成功"), dwWebId);
			}
		}
	}
	T_TaskData* pTaskData = NULL;
	pTaskData=(T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (gThreadMessage != NULL && pTaskData != NULL) 
	{

/*
		if ((pTaskData->bIsAddExtra || pTaskData->bIsAddPic)
			&& pTaskData->tTaskResultInfo.eResultFlag == e_TASK_RESULT_SUCC)  //完善资料步骤  并成功所以重新发布 14.6.25
		{
			//将任务重新压入队列
			pTaskData->bAddExtraIsSucc = TRUE;
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("发布时完善资料或完善图片成功，网站id：%d"), dwWebId);
			PushTaskIndex(dwIndex,bSingleThread,pTaskData->limitRefresh.eIeEdtion);  
		}
		else //当前任务不是完善资料步骤时结束此任务
		{*/
		if (pTaskData->bIsAddExtra)
		{
			pTaskData->bAddExtraIsSucc = TRUE;
			g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("发布时完善资料或完善图片失败，网站id：%d"), dwWebId);
		}
		gThreadMessage->PostMessage(E_TASKMGR_THREAD,E_4KERNEL_TASK_OUT, dwIndex);
		//}
	}
}

/*
	@brief 将所有等待操作的网站 不在等待
	*/
void CAllTaskData::ContinueAllOperate()
{
	std::map<DWORD, HANDLE>::iterator iter;
	m_EventLock.Lock();
	if (IndexToEvent.size() == 0)
	{
		m_EventLock.Unlock();
		return;
	}

	for(iter = IndexToEvent.begin(); iter != IndexToEvent.end(); ++iter)
	{
		SetEvent(iter->second);		
	}
	m_EventLock.Unlock();
}

/*
@breif  根据通配符名取得值*/
bool CAllTaskData::FindEvent( DWORD dwWebId )
{
	m_EventLock.Lock();
	std::map<DWORD,HANDLE>::iterator iter;

	if (IndexToEvent.size() == 0)
	{
		m_EventLock.Unlock();
		return false;
	}	

	for(iter = IndexToEvent.begin(); iter != IndexToEvent.end(); ++iter)
	{
		if (iter->first == dwWebId)
		{
			m_EventLock.Unlock();
			return true;
		}
	}

	m_EventLock.Unlock();
	return false;
}

/*
	@breif  执行发送数据并处理返回数据
	@param	pTData 执行任务数据；
	@param  pTaskData 任务数据结构；
	@param  curStep 当前执行第几个功能块；
	@param  iFunCount  功能块总数；
	@param	strPreCompName 公司名称；
	@param  curFunction  当前任务类型；
	return	接下要执行的步骤 1.CurTaskComplete 2.TASKSTEP 3.REPLACESTEP
*/
DWORD CAllTaskData::SendTaskData( CTaskData *pTData,T_TaskData* pTaskData,long &curStep,
	E_StationFunction &curFunction,DWORD &dwRet,CStdString &strTaskData, CommonParam &cParam)
{
	if (dwRet == WAIT_OBJECT_0) //操作完成
	{
		int iResultCode = HandleRetData(pTaskData->dwWebID, pTaskData,cParam.dwTaskType,pTData);
		if (iResultCode == 0) //操作失败
		{
			if ((pTaskData->eTaskType == ETTypeRegaccount || pTaskData->eTaskType == ETTypeReRegaccount)
				&& pTaskData->tTaskResultInfo.eResultFlag == e_TASK_RESULT_FAIL
				&& !pTaskData->bGetLogin)
			{
				//需要使用一键登录
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("注册失败，并返回正常错误，调用登录数据验证,网站ID: %d"),pTaskData->dwWebID);
				pTaskData->bGetLogin = TRUE;
				return 5;
			}
				
			return 1;										
		}
		else if (iResultCode == 1)  //操作成功
		{
			if (curStep == (cParam.iFunCount - 1))
			{
				if (cParam.dwTaskType == ttDataOnceKeyProduct && cParam.dwLoopPdtCnt > cParam.dwCurPdtCnt)    //多产品循环提取；
				{
					return 4;
				}
				return 1;
			}
		}
		else if	(iResultCode == 2)  //发布时需要完善资料
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("发布时遇到完善资料,网站id为%d"), pTaskData->dwWebID);
			pTaskData->bIsAddExtra = TRUE;  //当前步骤为完善资料步骤
			curFunction = sfAddExtra;  //加载完善资料功能
			return 2;
		}
		else if (iResultCode == 3)  //14.6.25
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("发布时遇到完善图片,网站id为%d"), pTaskData->dwWebID);
			pTaskData->bIsAddPic = TRUE; //当前为完善图片步骤
			curFunction = sfUpdatePhoto;
			return 2;
		}
		else if(iResultCode == 4)  //公司名已经存在
		{
			g_log.Trace(LOGL_LOW,LOGT_WARNING,__TFILE__,__LINE__,_T("公司名已经存在，网站id为%d"),pTaskData->dwWebID);
			if (m_Taskprocess.ReplaceCompName(strTaskData,cParam.strPreCompName,cParam.iRepCompanyStep))
			{
				g_log.Trace(LOGL_LOW,LOGT_PROMPT,__TFILE__,__LINE__,_T("替换公司成功，重新执行！"));
				return 3;
			}
		}
		else if (iResultCode == 5) //已有网站在执行，重新放入队列
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("检测到已有网站在执行，重新放入队列！"));
			return 6;
		}
		else if (iResultCode == 6) //邮箱重复，使用备用邮箱；
		{
			if ((strMail != strMailBak)
				&& (strTaskData.Find(strMail) != -1)
				&& strMailBak != _T(""))
			{
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("将原邮箱：%s,替换为备份邮箱：%s"), strMail.GetBuffer(), strMailBak.GetBuffer());
				strTaskData.Replace(strMail, strMailBak);
				return 3;
			}
		}	
	}
	else if (dwRet == WAIT_TIMEOUT) //等待任务操作超时
	{
		pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
		pTaskData->tTaskResultInfo.strResultInfo += _T("<@@@>等待内核超时");
		if ((pTaskData->eTaskType == ETTypeRegaccount) || (pTaskData->eTaskType == ETTypeReRegaccount))
		{
/*
			if (pTData->GetType() > ttDataReg)  //注册时，如果注册步骤已执行则返回完成资料失败
			{
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDEXTRAFAIL;
			}*/
		}
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("操作超时,网站id为%d"), pTaskData->dwWebID);
		return 1;
	}	

	return 0;
}
/*
	@breif  获取当期任务类型
	@param	eType :任务类型
	@param  curFunction : */
void CAllTaskData::GetCurTaskType( E_TASK_TYPE &eType,E_StationFunction &curFunction,bool &bSendStop)
{
	switch (eType)
	{
	case ETypeMailCount:
	case ETTypePost:
		{
			curFunction = sfPostGY;
		}
		break;
	case ETypeAutoLogin:
		{
			curFunction = sfReg;  //这里自动登录，随便加载一功能
			bSendStop = false;
		}
		break;
	case ETypeRegEmail:
	case ETTypeRegaccount:
	case ETTypeReRegaccount:
		curFunction = sfReg;
		break;
	case ETypeRefresh:
		curFunction = sfRefreshInfo;
		break;
	case ETypeDelete:
		curFunction = sfDeleteInfo;
		break;
	case ETypeDomainpost:
		curFunction = sfPostDomain;
		break;
	case ETypeWebopost:
		curFunction = sfPostOther;
		break;
	case ETypeCompanyPopular:
		curFunction = sfCompanyPopular;
		break;
	case ETTypeUserInfoMigrate:
		curFunction = sfOnceKeyMove;
		break;
	case ETTypeModify:
		curFunction = sfChangeData;
		break;
	case ETTypeGetPdtList:
		curFunction = sfExtractionProduct;
		break;
	case ETTypeMailActivate:
		curFunction = sfVerify;
		break;
	case ETTypeShopTraffic:
	case ETTypeSoftPost:
		curFunction = sfPostOther;
	default:
		break;
	}
}

DWORD WINAPI CAllTaskData::DoSingleTask( LPVOID lp )
{
	bool isRestartEngine = false;
	ASSERT(gAllTaskData != NULL);

	if (gAllTaskData->m_hEvent == NULL)  //事件对象创建失败
		return 0;

	while (gAllTaskData->bRun)
	{
		
		DWORD dwIndex = gAllTaskData->GetObjectTaskIndex(gAllTaskData->m_eCurIeVer); //获取任务
		if (dwIndex > 0)
		{
			gAllTaskData->bObjectRunFinish = TRUE;
			gAllTaskData->DoCommonTask(dwIndex,gAllTaskData->m_hEvent,TRUE,isRestartEngine);
		}
		else
		{
			if (gAllTaskData->m_allQueueTask[gAllTaskData->m_eCurIeVer].size() <= 0
				&& gAllTaskData->CheckMultiTask())
			{
				E_IeLimit eIe = gAllTaskData->m_eCurIeVer;
				if (gAllTaskData->CheckRunOver(eIe, TRUE))
				{
					gAllTaskData->RunEngine(TRUE);
					Sleep(200);
					//如果没有多线程网站则不需要启动内核；
					if (gAllTaskData->m_hndEngine != NULL)
					{
						gAllTaskData->RunEngine();
					}
					g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, _T("开始执行新的IE版本，IE版本为:%d"), eIe);
					gAllTaskData->m_eCurIeVer = eIe;
					gAllTaskData->m_bCheckRun = FALSE;
					Sleep(200);
				}

				gAllTaskData->bObjectRunFinish = FALSE;
			}
			
		}

		Sleep(500);
	}

	return 0;
}

/*
	@brief 将所有Object等待操作的网站 不在等待
*/
void CAllTaskData::ContinueObjectOperate()
{	
	std::map<DWORD, HANDLE>::iterator iter;

	if (indexToObjectEvent.size() == 0)
	{
		return;
	}

	for (iter = indexToObjectEvent.begin(); iter != indexToObjectEvent.end(); ++iter)
	{
		SetEvent(iter->second);
	}

}

/*
@brief 初始化数据
*/
void CAllTaskData::InitData()
{
	//---双内核相关的-----
	iOneKernel = 0;	//配置是否要启动双内核参数
	lngRunningCount = 0;     //运行中的数量；
	lngRunObject = 0;
	//---end------

	for(int i=0; i<THREAD_MAX_COUNT; i++)
	{
		hEvents[i] = NULL;
		hThreads[i] = NULL;
	}

	m_hEvent = NULL;
	m_hThread = NULL;
	//获取IE版本
	iIEVersion = m_Taskprocess.GetIEVersion();

	g_Config->ReadInt(_T("MC"), _T("CONTROL"), _T("otherthread"), iOneKernel);

	strMailBak = _T("");
	strMail = _T("");
}

/*
@brief doTask前做准备；
@param iSingle 是否要启动2个内核；
@param eType   任务类型；
@param dwWebId 网站ID;
@result FALSE :插入回去，重新开始；
*/
BOOL CAllTaskData::DoPreJob( int iSingle,DWORD dwWebId, E_TASK_TYPE eType,CommonParam &cParam)
{
	//单线程内核不需要做启动另外一个内核操作
	if (iSingle == 10)
	{
		goto START;
	}

	//判断是否有一个OBJECT对象的网站在运行(每次只有有一个运行）
	if (iSingle == 1)
	{
		//如果用户配置成不需要另起内核执行Object网站
		
		if (!m_Taskprocess.RunWebBySingle(dwWebId,lngRunObject,IndexToEvent.size(),lngRunningCount,cParam.bIsObject))
		{
			return FALSE;
		}

		lngRunningCount ++;  //Object网站运行中的任务加1
	}
	
	if(FindEvent(dwWebId)) 
	{
		//查找是否有该网站已经在执行的任务，如果有，重新压入队列
		if (IndexToEvent[dwWebId] != NULL)
		{
			if(iSingle == 1)
				lngRunningCount--;
			return FALSE;
		}
	}

START:
	//获取邮箱类型
	if (strMail.Find(_T("@126")) != -1)
	{
		cParam.dwEmbedID = 40000003;
	}
	else if (strMail.Find(_T("@sina")) != -1)
	{
		cParam.dwEmbedID = 40000594;
	}
	//一键搬家功能
	if (eType == ETTypeUserInfoMigrate)
	{
		CStdString strProductNames = _T("");
		T_LocalGlobalData *pGlobalData = NULL;
		pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
			E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

		cParam.dwLoopPdtCnt = pGlobalData->dwMigrationProductCount;   //获取要提取的产品数量；
		if (cParam.dwLoopPdtCnt > 0)
		{
			strProductNames = pGlobalData->szMigrationProductNames;
			StringUtils strUtil;
			strUtil.SplitString(strProductNames,_T("(;1)"),cParam.vPdtLists,false);
		}

		iWaitTime = 300*1000;    //一键搬家用最大时间；
	}

	if (eType == ETTypeShopTraffic)
	{
		iWaitTime = 1000 * 1000; //最大时间；
	}

	return TRUE;
}

/*
@brief 处理一键搬家成功时步骤
@param dwTaskType 任务类型；
@param dwPdtCnt   产品数量；
@param pTask	  任务数据；
@return TRUE :没有产品需要提取；
*/
BOOL CAllTaskData::ProcessSuccMigrate( DWORD dwTaskType, DWORD dwPdtCnt, T_TaskData *pTask,DWORD dwIndex)
{
	if ((ttDataOnceKeyMove == dwTaskType) || (ttDataOnceKeyProduct ==dwTaskType) )
	{
		if (dwPdtCnt == 0)   //无需提取产品直接退出；
		{
			return TRUE;
		}
		pTask->tTaskResultInfo.dwType = dwTaskType;
		gThreadMessage->PostMessage(E_SAVE_THREAD,E_SAVE_FIRSTDATA,dwIndex);  //一键搬家功能；
	}

	return FALSE;
}

/*
@brief dotask和DoSingleTask共同的部分；
@param dwIndex  任务索引；
@param hEvent   事件对象；
@param bSingle  单线程任务；
*/
DWORD CAllTaskData::DoCommonTask( DWORD dwIndex, HANDLE hEvent, BOOL bSingle,bool &isRestartEngine)
{
	CCommServer *comConnect =  NULL;
	bool bSendStop = true;  //该任务结束后是否需要告知四代核停止该任务  默认需要
	bool bRePush = false;   //是否要重新压入队列
	CommonParam cParam;		//公共参数
	T_TaskData* pTaskData = NULL;

	pTaskData=(T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pTaskData != NULL)
	{
		pTaskData->bIsAddExtra = FALSE;		//判断是否要执行完善资料
		pTaskData->bIsAddPic  = FALSE;		//判断是否要执行完善相册
		cParam.bIsObject = pTaskData->siteType.bIsObject;

		if (bSingle)
		{
			//获取产品名称
			DoPreJob(10,pTaskData->dwWebID,pTaskData->eTaskType,cParam);
			comConnect = &m_ConnectionByObject;
			indexToObjectEvent[pTaskData->dwWebID] = hEvent;
		}
		else
		{
			//执行前判断
			if (!DoPreJob(iOneKernel,pTaskData->dwWebID,pTaskData->eTaskType,cParam))
			{
				PushTaskIndex(dwIndex,bSingle,pTaskData->limitRefresh.eIeEdtion);
				Sleep(100);
				return 0;
			}
			comConnect = &m_Connection;
			LinkWeiIdToEvent(pTaskData->dwWebID, hEvent);  //将网站id与事件对象关联
		}

		CStationData m_StationData;		
		m_StationData.SetSource(pTaskData->tTaskDataExser.strEnginData) ;

		E_StationFunction curFunction;
		GetCurTaskType(pTaskData->eTaskType,curFunction,bSendStop);
TASKSTEP:
		CFunctionData funcData ;
		m_StationData.LoadData(curFunction, funcData);  //加载具体功能
		cParam.iFunCount = funcData.GetCount();
		DWORD dwCurtime = GetTickCount();

		if (cParam.iFunCount <= 0)  //确认此任务包含1个以上执行块   //取任务数据失败
		{
			g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("function GetCount 返回0，网站id为%d, 任务类型为：%d"), pTaskData->dwWebID, curFunction);
			bSendStop = false;
			goto CurTaskComplete;
		}

		for (long j=0 ; j<cParam.iFunCount ; ++j)
		{
DATASTEP:
			CTaskData *pTData = NULL;
			CStdString strOCR = _T("");
			//获取任务块数据
			pTData = m_Taskprocess.GetTaskData(m_StationData,
				funcData,curFunction,pTaskData,j,strOCR,cParam.dwEmbedID);
			if (pTData == NULL)			//取具体执行块数据失败
			{						
				bSendStop = false;
				goto CurTaskComplete;
			}
			//获取发送数据
			CStdString strTaskData = m_Taskprocess.GetSendData(pTaskData->dwWebID,m_StationData,pTData,strOCR);
			cParam.strSendDataBak = strTaskData;  //备份当前执行数据，方便提取多个产品重复使用；
			cParam.dwTaskType = funcData.GetCurTaskType(); //获取任务类型
REPLACESTEP:
			//替换产品标题
			m_Taskprocess.ReplaceSendData(cParam,strTaskData);
			int iLeaveTime = iWaitTime-(GetTickCount()-dwCurtime);
			if (iLeaveTime <= 0)		//操作超时   操作任务时间超过限定时间
			{
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
				pTaskData->tTaskResultInfo.strResultInfo += _T("<@@@>等待内核超时");
				g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("操作超过限定时间,网站id为%d"), pTaskData->dwWebID);
				goto CurTaskComplete;
			}
			if (comConnect->SendData( strTaskData ) != -1)
			{
				//一键登入，数据发出后就直接退出；
				m_Taskprocess.ExitMC(pTaskData->eTaskType);
				DWORD dwRet = WaitForSingleObject(hEvent, iLeaveTime);
				DWORD dwResult =SendTaskData(pTData,pTaskData,j,curFunction,dwRet,strTaskData,cParam);

				switch (dwResult)
				{
				case 1:
					goto CurTaskComplete;
					break;
				case 2:
					goto TASKSTEP;
					break;
				case 3:
					goto REPLACESTEP;
					break;
				case 4:
					pTaskData->tTaskResultInfo.dwType = cParam.dwTaskType;
					gThreadMessage->PostMessage(E_SAVE_THREAD,E_SAVE_FIRSTDATA,dwIndex);  //一键搬家功能；
					goto REPLACESTEP;
					break;
				case 5:
					goto DATASTEP;
					break;
				case 6:
					bRePush = true;
					goto CurTaskComplete;
					break;
				}

				if ((dwResult == 0) &&(ProcessSuccMigrate(cParam.dwTaskType,
					cParam.dwLoopPdtCnt,pTaskData,dwIndex)))
				{
					goto CurTaskComplete;
				}

			}
			else   //发送数据失败
			{
				g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("socket发数据失败，网站id：%d "), pTaskData->dwWebID);
				bSendStop = false;
				isRestartEngine = true;    //socket发送数据失败
				goto CurTaskComplete;
			}
		}

CurTaskComplete:
		pTaskData->tTaskResultInfo.dwType = cParam.dwTaskType;
		if (!bSingle)
		{
			m_Taskprocess.ClearRunObjectCnt(iOneKernel,
				lngRunObject,lngRunningCount);
			RemoveWebIdEvent(pTaskData->dwWebID);
		}
		else
		{
			indexToObjectEvent.erase(pTaskData->dwWebID);
		}

		//检测是否需要重新压入
		if (bRePush)
		{
			bRePush = false;
			PushTaskIndex(dwIndex, bSingle, pTaskData->limitRefresh.eIeEdtion);  //丢失的任务重新开始
			return 0;
		}

		if (isRestartEngine)
		{
			//重启内核
			isRestartEngine = false;
			RunEngine(bSingle);									
			g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("socket发送失败，重新启动4代核, 网站id为:%d"), pTaskData->dwWebID);
			PushTaskIndex(dwIndex,bSingle,pTaskData->limitRefresh.eIeEdtion);  //丢失的任务重新开始
			Sleep(200);
		}
		else
		{
			HandleCompleteWeb(pTaskData->dwWebID, m_StationData.GetNsName(), dwIndex, bSendStop,bSingle);
		}
	}
	else
	{
		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("pTaskData为NULL, 任务索引%d"), dwIndex);
		//取得任务数据失败
	}	

}

/*
	@breif  取得任务索引，并清除
	@return 返回任务索引
*/
DWORD CAllTaskData::GetObjectTaskIndex(E_IeLimit eVer)
{
	DWORD dwIndex = 0;
	m_TaskLock.Lock();

	if (m_objectQueueTask[eVer].size() > 0)
	{
		dwIndex = m_objectQueueTask[eVer].front();
		m_objectQueueTask[eVer].pop();
	}

	m_TaskLock.Unlock();

	return dwIndex;
}

/*
@breif 判断当前版本的所有网站是否都运行完成；
@param eIever ie版本号
@return TRUE 已完成；False 未完成
*/
BOOL CAllTaskData::CheckRunOver(E_IeLimit& eIever, BOOL bSingle)
{
	BOOL bResult = FALSE;
	std::map<E_IeLimit, std::queue<DWORD>>* mapTmpTask;

	m_CheckLock.Lock();

	if (m_bCheckRun)
	{	
		m_CheckLock.Unlock();
		return FALSE;
	}

	m_bCheckRun = TRUE;	

	//如果其中一个内核改变了IE模式，那另一个直接退出；
	if (eIever != m_eCurIeVer)
	{
		m_bCheckRun =FALSE;
		m_CheckLock.Unlock();
		return FALSE;
	}

	//根据标记取对应的队列
	if (bSingle)
	{
		mapTmpTask = &m_objectQueueTask;
	}
	else
	{
		mapTmpTask = &m_allQueueTask;
	}

	int iCnt = 0;
	std::map<E_IeLimit, std::queue<DWORD>>::iterator iter;
	for (iter = mapTmpTask->begin(); iter != mapTmpTask->end(); ++iter)
	{
		//取所有里面最多的IE版本网站
		if (iCnt < iter->second.size())
		{
			iCnt = iter->second.size();
			eIever = iter->first;
			bResult = TRUE;
		}
	}		
	
	//切换注册表IE模式
	if (bResult)
	{
		SetIEMode(eIever);
	}
	else
	{
		m_bCheckRun = FALSE;
	}

	m_CheckLock.Unlock();

	return bResult;
}

/*
@breif 设置IE模式
*/
void CAllTaskData::SetIEMode(E_IeLimit eVer)
{
	m_Taskprocess.RaisePrivileges();
	m_Taskprocess.SetIECoreVersion(eVer);
	/*m_eCurIeVer = eVer;*/
}

/*
@breif 判断多线程网站是否还有任务在执行；
*/
BOOL CAllTaskData::CheckMultiTask()
{
	if (IndexToEvent.size() == 0)
	{
		return TRUE;
	}

	std::map<DWORD, HANDLE>::iterator iter;

	for (iter = IndexToEvent.begin(); iter != IndexToEvent.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*
@breif 通过Json获取内核返回的Post表单数据
@param strCompId :公司ID(对方的唯一ID号）
@param strDesc : 详细描述
@return 返回的成功结果(-1)；
*/
int CAllTaskData::GetFormData(const CStdString &strData, DWORD &dwCompId, CStdString &strDesc)
{
	int iRes = -1;
	if (strData.GetLength() <= 0)
	{
		return iRes;
	}
	const char *pData = NULL;
	char *buff = NULL;	
	buff = WideToMulti(strData.c_str());

	if (buff != NULL)
	{
		Json::Reader reader;
		Json::Value json_object;

		if (reader.parse(buff, json_object))
		{
			pData = json_object["Return"]["Ret"].asCString();
			iRes = atoi(pData);
			pData = json_object["Data"]["Id"].asCString();			
			dwCompId = atoi(pData);

			pData = json_object["Return"]["Msg"].asCString();
			strDesc = CStdString(CString(pData).GetBuffer());
			//delete []pData;
		}

		delete []buff;
	}

	return iRes;
}



DWORD GetData(TCHAR *Orgbuf, DWORD dwTotalLen)
{
	CString strData(Orgbuf) ;

	std::vector<CStdString> vecData ;
	CEnDeCode EDTool ;
	EDTool.SplitStringWithSeparator((CStdString)strData , ENCODE_SEPARATOR , vecData) ;

	if (vecData.size() == 2)
	{
		const CStdString &strTaskID = vecData[0] ;
		const CStdString &strDataOut = vecData[1] ;

		if (gAllTaskData != NULL)
		{
			DWORD dwWebId = _ttol(strTaskID.c_str());
			gAllTaskData->PushResult(dwWebId, strDataOut);
			gAllTaskData->ContinueOperate(dwWebId);
		}
	}
	else if (strData.CompareNoCase(_T("disconnect")) == 0)
	{
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("内核异常退出，结束所有缓存任务！"));

		if (gAllTaskData != NULL)
		{
			gAllTaskData->ContinueAllOperate();
		}
	}
	return 0 ;
}

DWORD GetObjectData( TCHAR *Orgbuf, DWORD dwTotalLen )
{
	CString strData(Orgbuf) ;

	std::vector<CStdString> vecData ;
	CEnDeCode EDTool ;
	EDTool.SplitStringWithSeparator((CStdString)strData , ENCODE_SEPARATOR , vecData) ;

	if (vecData.size() == 2)
	{
		const CStdString &strTaskID = vecData[0] ;
		const CStdString &strDataOut = vecData[1] ;

		if (gAllTaskData != NULL)
		{
			DWORD dwWebId = _ttol(strTaskID.c_str());
			gAllTaskData->PushResult(dwWebId, strDataOut);
			gAllTaskData->ContinueObjectOperate();
		}
	}
	else if (strData.CompareNoCase(_T("disconnect")) == 0)
	{
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("Object内核异常退出，结束所有缓存任务！"));

		if (gAllTaskData != NULL)
		{
			gAllTaskData->ContinueObjectOperate();
		}
	}
	return 0 ;

}
