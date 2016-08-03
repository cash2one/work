#include "StdAfx.h"
#include "TaskProcess.h"
#include "StringUtils.h"
#include "FeatureFit.h"
#include "InternetHttp.h"

using namespace FeatureFit;
typedef void (WINAPI *LPFN_PGNSI)(LPSYSTEM_INFO);
#pragma comment(lib, "Version.lib")

CTaskProcess::CTaskProcess(void)
{
	bIsWait = FALSE;
/*
	//获取网站code;
	CStdString strNsCode;
	std::vector<CStdString> vTmpcode;
	StringUtils Util;
	g_Config->ReadString(_T("OBJECTWEB"),_T("NsCode"),strNsCode);
	if (strNsCode.GetLength() > 0)
	{
		Util.SplitStringEx(strNsCode,_T(","),vTmpcode,false);
		for (int i = 0; i < vTmpcode.size(); ++i)
		{
			vNsCodeOfObject.push_back(_ttol(vTmpcode[i]));
		}
	}
	else
	{
		vNsCodeOfObject.push_back(40000519);
		vNsCodeOfObject.push_back(40006512);
		vNsCodeOfObject.push_back(40001567);
		vNsCodeOfObject.push_back(49994692);
		vNsCodeOfObject.push_back(40012911);
		vNsCodeOfObject.push_back(40042556);
		vNsCodeOfObject.push_back(40020637);
		vNsCodeOfObject.push_back(40000511);
	}*/

}


CTaskProcess::~CTaskProcess(void)
{
}

/*
@breif  连接内核；
*/
void CTaskProcess::ConnectKernel(CCommServer &Comserver,int &iPort, pfnDataHandler fn)
{
	int i = 0;
	while (i < 10)
	{
		if (Comserver.Init(iPort,fn) == 0)
		{
			break;
		}

		iPort = GetTickCount()%(65535 - 10000) + 10000; //随机端口
		i++;
	}
}

/*
@breif 启动4代内核
@param Comserver socket连接；
@param iPort    端口号
@param hndEngine 内核进程句柄
@param lRunEngine	运行中的内核；
@Return True：连接成功
*/
BOOL CTaskProcess::RunEngine( CCommServer &Comserver, int iPort,HANDLE &hndEngine,long &lRunEngine )
{
	InterlockedIncrement(&lRunEngine);
	//防止启动多个内核；
	if (lRunEngine > 1)
	{
		InterlockedDecrement(&lRunEngine);
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("已有4代核正在启动，退出"));
		return FALSE;
	}

	if(hndEngine != NULL)
	{
		Comserver.CloseClient();
		Sleep(2000);
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("内核未退出，调用TerminateProcess!"));
		TerminateProcess(hndEngine,0);
		CloseHandle(hndEngine);
	}

	//保存内核进程信息 -----begin------
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	//不显示鼠标忙碌状态
	si.dwFlags = STARTF_FORCEOFFFEEDBACK;
	ZeroMemory( &pi, sizeof(pi) );
	//------end-------

	TCHAR szPath[MAX_PATH] = {0};
	CString strEnginePath = _T("");
	CString strParam = _T("");
	CString strPtmp = _T("");  //临时参数
	GetModuleFileName(NULL,szPath,MAX_PATH);
	PathAppend(szPath,_T("..\\"));

	int iMode = 0;
	g_Config->ReadInt(_T("MC"), _T("Kernel4"), _T("mode"), iMode);

	T_LocalGlobalData *pLocalGlobalData =
		(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS,eUseCommReadWithMulti);

	if (pLocalGlobalData != NULL
		&& pLocalGlobalData->iMode == rmBrowser)
		iMode = pLocalGlobalData->iMode;

	if (pLocalGlobalData != NULL 
		&& pLocalGlobalData->bIsYunTask)
	{
		strPtmp = _T("-y");
	}

	strEnginePath.Format(_T("%sEngine.exe"),szPath);
	//如果需要远程打码，调整为启动附加参数样式
	if (pLocalGlobalData->dwOCRType == 1)
	{
		strParam.Format(_T(" -p%d -m%d -f %s"), iPort, iMode,strPtmp);
	}
	else
	{
		strParam.Format(_T(" -p%d -m%d %s"), iPort, iMode,strPtmp);
	}

	//启动内核
	if (!CreateProcess(strEnginePath.GetBuffer(0),strParam.GetBuffer(0),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi ) )
	{
		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("启动4代核失败,文件路径为：%s"),strEnginePath.GetString());
		InterlockedDecrement(&lRunEngine);
		return FALSE;
	}

	hndEngine = pi.hProcess;
	//g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("创建核进程成功"));

	strEnginePath.ReleaseBuffer();
	strParam.ReleaseBuffer();
	int iCount = 20;
	while((!Comserver.IsClientConnected())&& (iCount>0))
	{
		iCount--;
		Sleep(500);
	}

	InterlockedDecrement(&lRunEngine);

	return TRUE;

}

/*
@breif  是否为Object对象的网站
@param	dwWebid 网站IDBOOL CTaskProcess::IsObjectWeb( const DWORD &dwWebid )
{
	for (int i = 0; i < vNsCodeOfObject.size(); ++i)
	{
		if (dwWebid == vNsCodeOfObject[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}*/

/*
	@breif  OBJECT网站需单线程执行
	@param	dwWebid 网站ID
	@param  lngRunCnt object类型网站数量
	@param  lngRuningCnt 运行中的数量
	@param  iEventCnt	事件对应任务索引的数量
*/
BOOL CTaskProcess::RunWebBySingle(DWORD dwWebid, long &lngRunCnt, int iEventCnt, long lngRuningCnt, BOOL bIsObject)
{
	while (bIsWait)			
	{
		//如果当前有Object网站运行，则一直等待；
		Sleep(100);
	}

	//判断当前网站是否是Object网站；
	if (bIsObject)
	{
		if (lngRunCnt > 0)
		{
			//如果刚好前面已经有网站准备运行，返回
			return FALSE;
		}

		++lngRunCnt;		//开始计数

		if ((lngRuningCnt > 0) && (iEventCnt != 0))
		{
			//如果发现有Object网站在运行中，或者有其它网站在运行，返回（Object网站只能单线程运行）
			-- lngRunCnt;
			return FALSE;
		}

		bIsWait = TRUE;

		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("单线程推广OBJECT网站,网站id为%d"), dwWebid);
	}
	

	return TRUE;
}

/*
@brief 获取任务数据块数据
@param stationData 当前网站数据；
@param funData 功能块数据；
@param curFunction 功能类型；
@param pTask  任务数据；
@param pData  任务块数据；
*/
CTaskData* CTaskProcess::GetTaskData(CStationData &stationData, CFunctionData &funcData, E_StationFunction &curFunction, T_TaskData *pTask, long iStep, CStdString &strOCR, DWORD dwMailId)
{
	CStationData StationEmbedData; //嵌入网站数据；
	bool bEmbedData;
	T_EmbedItemData tEmbedItem;

	//根据功能获取当前功能是否有嵌入网站
	CStdString strStrWebid;
	strStrWebid.Format(_T("%d"), dwMailId);
	bEmbedData = funcData.GetEmbedInfo(curFunction,tEmbedItem,strStrWebid);
	
	//判断是否一键登录
	if (pTask->eTaskType != ETypeAutoLogin &&
		!pTask->bGetLogin)
	{
		funcData.SetActiveIndex(iStep);
		//嵌入功能
		if (funcData.GetCurTaskType() == ttDataStationFunction)
		{
			//先判断是否有嵌入数据
			if (bEmbedData)
			{				
				StationEmbedData.SetSource(pTask->tTaskDataExser.mapEmbedData[dwMailId]);
				strOCR = pTask->tTaskDataExser.strExOCRParam;
				return StationEmbedData.GetTaskData((E_TaskType)tEmbedItem.nData);
			}
		}
		else
		{
			strOCR = pTask->strOCRParam;
			return stationData.GetTaskData(funcData.GetCurTaskType());
		}
	}
	else
	{
		strOCR = pTask->strOCRParam;
		return stationData.GetTaskData(ttDataLogin); //直接获取一键登录的数据；
	}

	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("pTada 为NULL, 取具体功能任务失败 返回0，网站id为%d,执行功能：%d"), pTask->dwWebID,funcData.GetCurTaskType());

	return NULL;

}

/*
@brief 组织要发送给内核的数据
@param dwWebID 网站ID;
@param stationData 当前网站数据；
@param pTData	任务块数据；
@return 字符串数据
*/
CStdString CTaskProcess::GetSendData(DWORD dwWebID, CStationData &stationData, CTaskData *pTData, CStdString &strOCRParam)
{
	//发送的任务数据
	CStdString strTaskData= _T("");

	T_DATA_IN dataToEngine;
	dataToEngine.strCmd = _T("EXEC");
	dataToEngine.strNsName = stationData.GetNsName();
	dataToEngine.strInDataset = pTData->GetSource();
	dataToEngine.bNoImage = stationData.GetIsNoImages();
	dataToEngine.dwMaxViewCount = stationData.GetViewCount();
	dataToEngine.strOCRParam = strOCRParam;

	CStdString strDataIn = dataToEngine.GetSource();
	CString strTaskID;
	strTaskID.Format(_T("%d"),dwWebID);
	
	strTaskData = (CStdString)strTaskID + _T(",") + strDataIn;

	return strTaskData;

}

/*
@brief 替换发送数据
@param cParam 要用到的数据；
@param strTaskData	任务数据；
@param dwFalg; 0:产品替换名称，1.刷新商铺地址
*/
void CTaskProcess::ReplaceSendData( CommonParam &cParam, CStdString &strTaskData)
{
	if (cParam.dwTaskType == ttDataOnceKeyProduct)
	{
		if (cParam.dwLoopPdtCnt > 0)
		{
			strTaskData = cParam.strSendDataBak;
			//替换要提取的产品标题
			if (cParam.vPdtLists.size() >= cParam.dwCurPdtCnt )
			{
				strTaskData.Replace(_T("<<ProName>>"),cParam.vPdtLists[cParam.dwCurPdtCnt++]);
			}
		}

	}

}

/*
@brief 单线程运行时清除运行数量
@param iSingle  是否运行双内核；
@param  lngRunCnt object类型网站数量
@param  lngRuningCnt 运行中的数量
*/
void CTaskProcess::ClearRunObjectCnt( int iSingle, long &lngRunCnt, long &lngRuningCnt )
{
	if (iSingle == 1)
	{
		bIsWait = FALSE;
		lngRuningCnt --;  //运行中的任务减1
		if(lngRunCnt > 0)
		{
			lngRunCnt--;
		}
	}
}

/*
@brief 结束主控
@param eTaskType;
*/
void CTaskProcess::ExitMC( E_TASK_TYPE eTaskType )
{
	//一键登入，数据发出后就直接退出；
	if (eTaskType == ETypeAutoLogin)
	{
		Sleep(2000);
		g_log.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__,_T("一键登入发送数据之后，主控退出"));
		ExitProcess(0);	//退出主控
	}
}

/*
@brief 获取网站失败结果类型
@param pTask	任务数据；
@param pTData   任务块数据；
@param tDataOut 返回数据；
@param vResult  失败集合；
*/
int CTaskProcess::GetFailedResult( T_TaskData *pTask,CTaskData *pTData,T_DATA_OUT &tDataOut,std::vector<CStdString> &vResult )
{
	int iReturn = 0;
	T_FILTER_STRUCTURE *ptResult;

	if (vResult.size() > 0)
	{
		int i;
		vResult[0] += tDataOut.strErrDesc;  //将错误描述也加上，增加特征标记
		for (i=1; i<rftTotalCount; i++)  //遍历失败原因
		{
			ptResult = pTData->GetFilterStructure((E_RESULT_FILTER_TYPE)i);

			if (ptResult != NULL)
			{
				int iResult = FitFilter2(vResult[0], *ptResult, false, pTask->tTaskResultInfo.strResultInfo);
				if (iResult == 0)
				{
					break;
				}
			}
			ptResult = NULL;
		}

		switch(i)
		{
		case rftRePost:
		case rftReReg:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_DUPLICATE;
			break;
		case rftMsgFull:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_POSTFULL;
			break;
		case rftFail:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
			break;
		case rftDataAddExtra:
			{
				pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDEXTRAFAIL;
				if (pTask->eTaskType == ETTypePost
					&& pTask->bIsSendExtra == FALSE)
				{
					pTask->bIsSendExtra = TRUE;
					iReturn = 2;
				}
			}

			break;
			//14.6.25
		case rftAddPhoto:
			{
				pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDPICFAIL;
				if (pTask->eTaskType == ETTypePost
					&& pTask->bIsSendPic == FALSE)
				{
					pTask->bIsSendPic = TRUE;
					iReturn = 3;
				}
			}
			break;
		case rftReEmail:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REEMAIL;
			iReturn = 6;
			break;
		case rftReCompany:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_RECOMPANY;
			iReturn = 4;
			break;

			//账号无效
		case rftInvalidAccount:    
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_INVALIDACCOUNT;
			break;
			//账号未审核
		case rftAccountNotAduit:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ACCOUNT_NOT_AUDIT;
			break;
			//注册缺少资料
		case rftRegLackInfo:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REG_LACK_INFO;
			break;
			//完善缺少资料
		case rftAddLackInfo:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADD_LACK_INFO;
			break;
			//发布缺少资料
		case rftPostLackInfo:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_POST_LACK_INFO;
			break;
			//手机号重复
		case rftRePhoneNumber:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REPHONENUMBER;
			break;
		default:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
			break;
		}
	}

	if (tDataOut.strCodeOcrInfo.length() >= 4)
	{
		CStdString tmp = tDataOut.strCodeOcrInfo.Trim().Left(3);
		g_log.Trace(LOGL_MID, LOGT_DEBUG, __TFILE__, __LINE__, _T("验证码返回为：%s"), tDataOut.strCodeOcrInfo.c_str());
		//验证码错误情况
		if (tmp.Find(_T("1")) == -1)
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_AUTH_CODE_ERROR;
	}

	if ((pTask->eTaskType == ETTypeRegaccount) || (pTask->eTaskType == ETTypeReRegaccount))
	{
		if (pTData->GetType() == ttDataLogin)  //注册时，注册步骤已经执行成功
		{
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_INVALIDACCOUNT;
		}
	}
	else if (pTask->eTaskType == ETTypePost)
	{
/*
		if (pTask->tTaskResultInfo.eResultFlag != e_TASK_RESULT_ADD_LACK_INFO)
		{
			if (pTData->GetType() == ttDataAddExtra)  //发布时需要完成资料失败
			{
				pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDEXTRAFAIL;
			}
		}
*/

		if (pTData->GetType() == ttDataUpdatePhoto) //发布时需要完善相册失败
		{
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDPICFAIL;
		}
	}

	if (tDataOut.strErrDesc.length() > 0)
		pTask->tTaskResultInfo.strResultInfo = tDataOut.strErrDesc;
	
	//只有当错误描述大于200个字符，则截取前200个；
	if (pTask->tTaskResultInfo.strResultInfo.GetLength()>200)
		pTask->tTaskResultInfo.strResultInfo = pTask->tTaskResultInfo.strResultInfo.Left(200);

	if (vResult.size() >= 2)
		pTask->tTaskResultInfo.strHtml = vResult[1];

	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("任务执行失败,网站id：%d,错误描述：%s, 错误码：%d"),pTask->dwWebID,tDataOut.strErrDesc.c_str(), pTask->tTaskResultInfo.eResultFlag);

	return iReturn;

}

/*
@brief 对执行成功网站分类处理
@param pTask	任务数据；
@param pTData   任务块数据；
@param tDataOut 返回数据；
*/
int CTaskProcess::GetSuccResult( T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType)
{
	int iResult = 0; //获取结果；

	//-----提取内核返回数据---------
	/*GetValueByName(tDataOut.strOutDataset, _T("%PostPageURL%"), pTask->tTaskResultInfo.strHtml);*/
	GetValueByName(tDataOut.strOutDataset, _T("$CompanyPopularity$"), pTask->tTaskResultInfo.strCompPopular);
	GetValueByName(tDataOut.strOutDataset, _T("$InfoID$"), pTask->tTaskResultInfo.strInfoID);             //获取信息ID;
	GetValueByName(tDataOut.strOutDataset, _T("$MailCount$"), pTask->tTaskResultInfo.strMailCount);		  //获取右键数量；			
	/*if (pTask->tTaskResultInfo.strHtml.length() == 0)
	{
		GetValueByName(tDataOut.strOutDataset, _T("$PostPageURL$"), pTask->tTaskResultInfo.strHtml);
	}*/

	if (pTask->tTaskResultInfo.strCompPopular.GetLength()> 0)
	{
		pTask->tTaskResultInfo.strCompPopular.Replace(_T(","),_T(""));
	}
	// --------提取数据end-----

	//----注册成功处理----
	if ((pTask->eTaskType == ETTypeRegaccount) || (pTask->eTaskType == ETTypeReRegaccount))
	{
		ProcessRegData(pTask,tDataOut);
	}

	//----发布 --抓取搜索引擎权重数据网站
	if (pTask->eTaskType == ETTypePost && pTask->dwWebID == 40000010) 
	{
		pTask->tTaskResultInfo.strResultInfo = tDataOut.strOutDataset;
	}

	//----刷新成功处理-----
	if (pTask->eTaskType == ETypeRefresh)
	{
		ProcessRefreData(pTask,tDataOut);
	}

	//----获取产品列表处理------
	if (pTask->eTaskType == ETTypeGetPdtList)
	{
		ProcessGetPdtData(pTask,tDataOut,dwTaskType);
	}

	//一键搬家处理
	if (pTask->eTaskType == ETTypeUserInfoMigrate)
	{
		ProcessMigrateData(pTask,tDataOut,dwTaskType);
	}

	iResult = 1;

	return iResult;
}

/*
@breif  根据通配符名取得值
@param  strData 要查找的字符串
@param  strName 通配符名
@param  [out ]strValue值
*/
void CTaskProcess::GetValueByName( const CStdString &strData, const CStdString &strName, CStdString &strValue )
{
	strValue = _T("");   //先清空，防止保存的是完善资料后的图片；
	if (strData.length() <= 0 || strName.length() <= 0)
		return;
	CStdString strResult = _T("");
	int iStartPos = 0 ;
	int iPos1;
	int iPos2;
	int iNameLen = strName.length();

	while (1)
	{
		iPos1 = strData.Find(strName.c_str(),iStartPos);
		if (iPos1 ==-1)
		{
			break;
		}
		else
		{
			iPos2 = strData.Find(_T("(;,)"), iPos1 + 1);
			if (iPos2 == -1)
			{
				iPos2 =strData.length();
				if (iPos2> iPos1+iNameLen)
				{
					strValue = strData.Mid(iPos1+iNameLen+1, iPos2-(iPos1+iNameLen+1));
					strResult = strValue;
					break;
				}
			}
			else
			{
				if (iPos2> iPos1+iNameLen)
				{
					strValue = strData.Mid(iPos1+iNameLen+1, iPos2-(iPos1+iNameLen+1));
					iStartPos =iPos2;
					if (strResult.length() > 0)
					{
						strResult = strResult + _T(",")+strValue;
					}
					else
					{
						strResult = strValue;
					}
				}
			}
		}
	}

	strValue = strResult;

}

/*
@brief 对注册成功数据处理；
@param pTask	任务数据；
@param tDataOut 返回数据；
*/
void CTaskProcess::ProcessRegData( T_TaskData *pTask,T_DATA_OUT &tDataOut )
{
	CStdString strValue;
	GetValueByName(tDataOut.strOutDataset, _T("%AsNName%"), strValue);

	if (strValue.length() == 0)
	{
		GetValueByName(tDataOut.strOutDataset, _T("$AsNName$"), strValue);
	}

	if (strValue.length() > 0)
	{
		pTask->szAccount = strValue;
	}

	if (pTask->siteType.eAccountState == asEmailConfirm)
	{
		pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REGMAIL_FAIL;
	}
	else if (pTask->siteType.eAccountState == asNeedVerify)
	{
		pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REGAUDIT_FAIL;
	}
}

/*
@brief 对刷新成功数据处理；
@param pTask	任务数据；
@param tDataOut 返回数据；
*/
void CTaskProcess::ProcessRefreData( T_TaskData *pTask,T_DATA_OUT &tDataOut )
{
	CStdString strTotalCout;
	GetValueByName(tDataOut.strOutDataset, _T("%TotalPostCount%"), strTotalCout);

	if (strTotalCout.length() == 0)
	{
		GetValueByName(tDataOut.strOutDataset, _T("$TotalPostCount$"), strTotalCout);
	}

	if (strTotalCout.length() > 0)
	{
		DWORD dwTotal = _wtol(strTotalCout.c_str());
		DWORD dwValue = 1;  //默认刷第一条
		DWORD dwRefreshTimes = dwTotal;  //默认刷全部

		if (pTask->limitRefresh.eNewPosRefresh == npStart)  //每次刷最后面一条
		{
			dwValue = dwTotal;
		}

		if (!(pTask->limitRefresh.dwSingleRefreshCount == -1
			|| pTask->limitRefresh.dwSingleRefreshCount >= dwTotal))  
		{
			//不限刷新条数和可刷新条数大于等于已发条数
			//则刷新全部
			dwRefreshTimes = pTask->limitRefresh.dwSingleRefreshCount;
		}

		for (DWORD dwIndex=1; dwIndex<= dwRefreshTimes; dwIndex++)
		{
			pTask->vRefreshIndex.push_back(dwValue);
		}

	}
}

/*
@brief 对提取产品列表成功数据处理；
@param pTask	任务数据；
@param tDataOut 返回数据；
*/
void CTaskProcess::ProcessGetPdtData( T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType)
{
	//如果是登录就不要进去
	if (dwTaskType == 1)
	{
		return ;
	}

	CStdString strSource = _T("");
	CStdString strResult = _T("");
	strSource = tDataOut.strOutDataset;
	if (!tDataOut.strOutDataset.IsEmpty())
	{
		GetProductList(strSource,strResult);
		pTask->tTaskResultInfo.strMigrateResult = strResult;
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("上层提取产品列表数据失败！"));
	}
}

/*
@breif 获取内核返回的产品列表数据
@param strSource 源数据
@param strResult 要返回的数据
*/
void CTaskProcess::GetProductList( CStdString strSource, CStdString &strResult )
{
	std::vector<CStdString> vProductName; 
	StringUtils strUtil;
	CStdString strTmp;
	//将产品名称前面的字符替换成空
	strSource.Replace(_T("$ProName$="),_T(""));

	vProductName.clear();
	int iCount = strUtil.SplitString(strSource,_T("(;,)"),vProductName,false);

	for (int i = 0; i < iCount ; ++i)
	{
		strTmp = vProductName[i];
		//去除重复的数据
		if (strResult.Find(strTmp) != -1)
		{
			continue;
		}

		if (strResult == _T(""))
		{
			//第一次不需要加上分隔符
			strResult = strTmp;
		}
		else
		{
			strResult = strResult + _T("(;1)")+ strTmp;
		}
	}
}

/*
@brief 对一键搬家成功数据处理；
@param pTask	任务数据；
@param tDataOut 返回数据；
@param dwTaskType 任务类型
@param 
*/
void CTaskProcess::ProcessMigrateData( T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType )
{
	DWORD dwSuccCnt  =0;
	//如果是登录就不要进去
	if (dwTaskType == 1)
	{
		return ;
	}

	CStdString strMigrateField = _T("");
	T_LocalGlobalData *pGlobalData = NULL;

	DWORD ret = m_NetData.OpenWebSiteData(pTask->dwWebID);
	if (ret == 0)
	{
		g_log.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("文件打开失败. webid= %d",pTaskData->dwWebID));
	}
	
	pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if(dwTaskType == ttDataOnceKeyMove)
	{
		strMigrateField = pGlobalData->szMigrationCompanyField;
	}
	else if(dwTaskType == ttDataOnceKeyProduct)
	{
		strMigrateField = pGlobalData->szMigrationProductField;
	}
	else
	{
		m_NetData.CloseWebSiteData();
		return ;
	}


	dwSuccCnt = ReplaceMigrateData(tDataOut,strMigrateField);

	m_NetData.CloseWebSiteData();

	g_log.Trace(LOGL_DBG, LOGT_ERROR, __TFILE__, __LINE__,_T("替换完成，类型：%d,字符串：%s,匹配数量：%d ； "), dwTaskType,strMigrateField,dwSuccCnt);
	pTask->tTaskResultInfo.strMigrateResult = strMigrateField;
	pTask->tTaskResultInfo.dwSuccFieldCnt =dwSuccCnt;
}

/*
@brief 处理一键搬家通配符替换
@param strMigrateData;
*/
DWORD CTaskProcess::ReplaceMigrateData( T_DATA_OUT &tDataOut, CStdString &strMigrateData )
{
	CStdString strProv = _T("");
	CStdString strCity = _T("");
	CStdString strTmp = _T("");
	int dwVarId = -1;	//变量ID
	int iType = 0;		//变量类型
	DWORD dwCnt = 0;	//统计替换失败的数量
	std::vector<CStdString> vMigrateParam;
	
	StringUtils strUtil;
	vMigrateParam.clear();
	strUtil.SplitString(strMigrateData,_T(","),vMigrateParam,false);

	for (int i = 0; i < vMigrateParam.size();++i)
	{
		//获取第一个通配符
		CStdString strTmpParam;
		int iPosStart = vMigrateParam[i].Find(_T("$"));
		int iPosEnd = vMigrateParam[i].ReverseFind(_T("$"));
		strTmpParam = vMigrateParam[i].Mid(iPosStart,iPosEnd-iPosStart+1);

		if (strTmpParam.length() <= 3 || 
			strTmpParam == _T("$PProvId$") ||
			strTmpParam == _T("$CCityId$"))
		{
			continue;
		}

		//获取内核提取到通配符的数据
		GetValueByName(tDataOut.strOutDataset, strTmpParam, strTmp);
		//处理空格和网址
		strTmp = strTmp.Trim();

		if (strTmp.GetLength() <= 0)
		{
			if (strTmpParam == _T("$PProv$"))								
			{
				strMigrateData.Replace(_T("$PProvId$"),_T(""));
			}
			if (strTmpParam == _T("$CCity$"))								
			{
				strMigrateData.Replace(_T("$CCityId$"),_T(""));
			}
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("没有获取到该通配符：%s 的值,替换成空； "), strTmpParam.c_str());
			strMigrateData.Replace(strTmpParam,_T(""));
			dwCnt++;

			continue;
		}

		if (strTmpParam == _T("$WWebs$"))
		{
			//判断网址是否有http
			if ((strTmp.Find(_T("http://")) == -1)
				|| (strTmp.Find(_T("https://")) == -1))
			{
				strTmp = _T("http://") + strTmp;
			}
		}

		ReplaceNewLine(strTmp); //替换空格过多的字段；
		iType = KeyToVal(strTmpParam);

		if (iType > 0)
		{
			if (iType == 14)
			{
				strMigrateData.Replace(strTmpParam,_T(""));
				dwCnt++;
				strCity = strProv + _T("<@@>") + strTmp;
				strTmp = strCity;
			}
			if (iType == 31)
			{
				strCity = strCity +  _T("<@@>") + strTmp;
				strTmp = strCity;
			}

			dwVarId = m_NetData.GetStandardValue(iType,strTmp);
			CStdString strVarId = _T("");
			strVarId.Format(_T("%d"),dwVarId);  //转换成字符型便于后面替换
			if ((iType == 1) || iType == 14)   
			{
				//省份和城市
				if (iType == 1)
				{
					strMigrateData.Replace(strTmpParam,_T(""));
					dwCnt++;
					strProv = strTmp;
					if (dwVarId >= 0)
					{
						strMigrateData.Replace(_T("$PProvId$"),strVarId);
					}
					else
					{
						strMigrateData.Replace(_T("$PProvId$"),_T(""));
						dwCnt++;
					}
				}
				else if (iType == 14)
				{
					if ( dwVarId >= 0 )
					{
						strMigrateData.Replace(_T("$CCityId$"),strVarId);
					}
					else
					{
						strMigrateData.Replace(_T("$CCityId$"),_T(""));
						dwCnt++;
					}
				}
				else if (iType == 31)
				{
					if (dwVarId >= 0)
					{
						strMigrateData.Replace(strTmpParam,strVarId);
					}
					else
					{
						strMigrateData.Replace(strTmpParam,_T(""));
						dwCnt++;
					}
				}
				continue;
			}
			else
			{
				//其它变量
				if (dwVarId >= 0)
				{
					strMigrateData.Replace(strTmpParam,strVarId);
				}
				else
				{
					dwCnt++;
					strMigrateData.Replace(strTmpParam,_T(""));  //没有匹配
				}
			}
		}
		else if (strTmpParam == _T("$sumpic$")
			|| strTmpParam == _T("$CCompPic$")
			|| strTmpParam == _T("$Tradcert$"))
		{
			UpLoadPic(strTmpParam,strTmp,strMigrateData,dwCnt);
		}
		else
		{
			//非变量数据
			strMigrateData.Replace(strTmpParam,strTmp);
		}
	
	}

	if (vMigrateParam.size()>0)
	{
		return vMigrateParam.size() - dwCnt;
	}

	return 0;
}

/*
@breif 根据通配符找对象类型
@param temp  通配符
@return 变量类型
*/
int CTaskProcess::KeyToVal( CStdString& temp )
{
	if (temp == _T("$PProv$"))
	{
		return 1;
	}
	else if (temp == _T("$SSex$"))
	{
		return 2;
	}
	else if (temp == _T("$CompanyKind$"))
	{
		return 4;
	}
	else if (temp == _T("$CCity$"))
	{
		return 14;
	}
	else if (temp == _T("$PersonsCount$"))
	{
		return 29;
	}
	else if (temp == _T("$MoneyInfo$"))
	{
		return 30;
	}
	else if (temp == _T("$CompanyTrade$"))					
	{
		return 5;
	}
	else if (temp == _T("$AAreaID$"))
	{
		return 31;
	}

	return 0;
}

/*
@brief 将字符串中换行符超过3个的，换成只有三个；
@param  strSource  源字符串；
@return 
*/
void CTaskProcess::ReplaceNewLine( CStdString &strSource )
{
	int iPosBegin;
	int iPosEnd;
	int iPosTmp;

	strSource = strSource.Trim();

	iPosTmp = strSource.Find(_T("\r\n\r\n\r\n\r\n"));
	//低于三个换行符，不替换；
	if (iPosTmp == -1)
	{
		return;
	}

	CStdString strBegin;
	CStdString strEnd;
	CStdString strTmp;

	strBegin = strSource.Left(iPosTmp);

	strTmp = strSource.Right(iPosTmp+4);

	ReplaceNewLine(strTmp);

	strSource = strBegin + _T("\r\n\r\n") + strTmp;
}

/*
@brief 提取到的图片上传到服务器
@param strParam   通配符；
@param strParamValue 通配符的值;
@param strMigrateData 要替换的字符串;
@param dwCnt 替换失败数量；
*/
DWORD CTaskProcess::UpLoadPic( CStdString &strParam,CStdString &strParamValue,CStdString &strMigrateData,DWORD &dwCnt )
{
	//产品图片，需上传到服务器
	CString strPicPath = strParamValue;
	if (PathFileExists(strPicPath))
	{
		CString serverPath = _T(""); //返回服务器路径
		CInternetHttp http;
		//将提取到的网站上传到服务器并获得路径名称
		if(http.LoginAndUploadFile(strPicPath,serverPath))  
		{
			strMigrateData.Replace(strParam,serverPath);
		}
		else
		{
			g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("图片上传失败！"));
			strMigrateData.Replace(strParam,_T(""));
			dwCnt++;
		}
	}
	else
	{
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("图片路径：%s不存在，没有获取到该通配符：%s 的值,替换成空； "),strPicPath.GetBuffer(), strParam.c_str());
		strMigrateData.Replace(strParam,_T(""));
		dwCnt++;
	}	

	return 0;
}

/*
@brief 公司名重复做替换操作
@param strData  被替换的字符串；
@param strReplace 要替换的字符； 
@Return TRUE: 替换成功
*/
BOOL CTaskProcess::ReplaceCompName(CStdString &strData, CStdString &strReplace, int &iStep)
{
	T_LocalGlobalData *pGlobalData = NULL;
	pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
	
	if (pGlobalData->vCompany.size() > iStep)
	{
		if (strData.Find(pGlobalData->vCompany[iStep - 1]) != -1)
		{
			strData.Replace(pGlobalData->vCompany[iStep-1],pGlobalData->vCompany[iStep]);
			iStep++;

			return TRUE;
		}
	}
	
/*
	for (int i =0; i<pGlobalData->vCompany.size()-1; ++i)
	{
		//首次替换找第一个；
		if (strReplace == _T(""))
		{										
			if (strData.Find(pGlobalData->vCompany[i])!= -1)
			{
				strData.Replace(pGlobalData->vCompany[i],pGlobalData->vCompany[i+1]);
				g_log.Trace(LOGL_LOW,LOGT_PROMPT,__TFILE__,__LINE__,_T("第一次替换，原公司名：%s,替换公司名: %s"),pGlobalData->vCompany[i].c_str(),pGlobalData->vCompany[i+1].c_str());
				strReplace = pGlobalData->vCompany[i+1];
				return TRUE;
			}
		}
		else
		{
			//已经替换过后
			if (strReplace == pGlobalData->vCompany[i])
			{
				if (strData.Find(pGlobalData->vCompany[i])!= -1)
				{
					strData.Replace(pGlobalData->vCompany[i],pGlobalData->vCompany[i+1]);
					g_log.Trace(LOGL_LOW,LOGT_PROMPT,__TFILE__,__LINE__,_T("原公司名：%s,替换公司名: %s"),pGlobalData->vCompany[i].c_str(),pGlobalData->vCompany[i+1].c_str());
					strReplace = pGlobalData->vCompany[i+1];

					return TRUE;
				}
			}
		}

	}*/

	return FALSE;
}

/*
@brief 对Win8以下系统进行提权
*/
BOOL CTaskProcess::RaisePrivileges()
{
	HANDLE TokenHandle;
	TOKEN_PRIVILEGES t_privileges = { 0 };

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
	{
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &t_privileges.Privileges[0].Luid))
	{
		return TRUE;
	}

	t_privileges.PrivilegeCount = 1;
	t_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(TokenHandle, FALSE, &t_privileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		CloseHandle(TokenHandle);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*
@brief 设置IE运行模式
@param eVer IE版本
*/
void CTaskProcess::SetIECoreVersion(E_IeLimit eVer)
{
	HKEY hMainKey = HKEY_CURRENT_USER;
	if (IsWOW64())
	{
		hMainKey = HKEY_LOCAL_MACHINE;
	}
	TCHAR* path = _T("SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION");
	TCHAR* valueName = _T("Engine.exe");
	long version = 8888;
	HKEY hKey;
	DWORD dwDisposition;
	//获取IE版本的值
	switch (eVer)
	{
	case IE6:
		version = 6000;
		break;
	case IE7:
		version = 7000;
		break;
	case IE8:
		version = 8888;
		break;
	case IE9:
		version = 9999;
		break;
	case IE10:
		version = 10001;
		break;
	case IE11:
		version = 11001;
		break;
	default:
		version = 8888;
		break;
	}
	long ret = RegOpenKeyEx(hMainKey, path, 0, REG_LEGAL_OPTION, &hKey);
	if (ret != ERROR_SUCCESS)
	{

		ret = RegCreateKeyEx(hMainKey, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
		if (ret != ERROR_SUCCESS)
		{
			g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("写入IE模式失败，IE版本：%d"), version);
			return;
		}
	}

	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("写入IE版本：%d"), version);

	ret = RegSetValueEx(hKey, valueName, NULL, REG_DWORD, (BYTE*)&version, sizeof(version));
	
	RegCloseKey(hKey);
	if (ret != ERROR_SUCCESS)
		return;
}

/*
@breif 判断是否64位系统
*/
BOOL CTaskProcess::IsWOW64()
{
	BOOL bRetVal = FALSE;
	SYSTEM_INFO si = { 0 };
	LPFN_PGNSI pGNSI = (LPFN_PGNSI)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if (pGNSI == NULL)
	{
		return FALSE;
	}
	pGNSI(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		bRetVal = TRUE;
	}
	return bRetVal;
}


int CTaskProcess::GetIEVersion()
{
	TCHAR szFilename[] = _T("mshtml.dll");

	int	  iVer = -1;
	DWORD dwMajorVersion = 0;
	DWORD dwHandle = 0;

	DWORD dwVerInfoSize = GetFileVersionInfoSize(szFilename, &dwHandle);

	if (dwVerInfoSize)
	{
		LPVOID lpBuffer = LocalAlloc(LPTR, dwVerInfoSize);
		if (lpBuffer)
		{
			if (GetFileVersionInfo(szFilename, dwHandle, dwVerInfoSize, lpBuffer))
			{
				VS_FIXEDFILEINFO * lpFixedFileInfo = NULL;
				UINT nFixedFileInfoSize = 0;
				if (VerQueryValue(lpBuffer, TEXT("\\"), (LPVOID*)&lpFixedFileInfo, &nFixedFileInfoSize) && (nFixedFileInfoSize))
				{					
					dwMajorVersion = HIWORD(lpFixedFileInfo->dwFileVersionMS);
					g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("当前IE版本为：%d"), dwMajorVersion);
				}
			}
			LocalFree(lpBuffer);
		}
	}

	switch (dwMajorVersion)
	{
	case 6:
		iVer = 0;
		break;
	case 7:
		iVer = 1;
		break;
	case 8:
		iVer = 2;
		break;
	case 9:
		iVer = 3;
		break;
	case 10:
		iVer = 4;
		break;
	case 11:
		iVer = 5;
		break;
	default:
		iVer = 2;
		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("获取当前IE版本失败，默认为IE8"));
		break;
	}

	return iVer;
}


