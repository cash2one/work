#include "StdAfx.h"
#include ".\preparedatathread.h"

//CLogTrace g_ThreadMessage(_T("ThreadMessage.log"), NULL);

#define ThreadMessage 1


CPrePareDataThread::CPrePareDataThread(void)
:IThreadUnit(E_DATAPRE_THREAD, 0xFFFF)
{
	g_Config->ReadInteger(_T("CONTROL"), _T("LocalTest"), m_nLocalTest);

	m_bAllNetStationLastUpdateTimeGetted = FALSE;
/*	m_ParamItemTable.clear();*/
}

CPrePareDataThread::~CPrePareDataThread(void)
{
	EmptyFun();
}

DWORD CPrePareDataThread::CreateThread()
{
	return 0;
}

DWORD CPrePareDataThread::DestoryThread()
{
	return 0;
}


//消息处理函数
DWORD CPrePareDataThread::DispatchMessage(T_Message *Msg)
{
	TRapalceDataMsg tRapalceDataTemp;
	BOOL bDoFlag=FALSE;

	switch(Msg->dwMsgParam)
	{
	case E_PPD_LASTUPDATE_GETTED_MSG:
		{
			ProcessGetServerDataTimeMsg(Msg->dwData);            //不管 ,这里出错
			break;
		}
	case E_PPD_START_MSG:
		{
			bDoFlag=PreProcessTaskData(Msg->dwData, tRapalceDataTemp, TRUE);
			break;
		}
	case E_PPD_RESTART_MSG:
		{
			bDoFlag=PreProcessTaskData(Msg->dwData, tRapalceDataTemp, FALSE);
			break;
		}
	case E_PPD_RETURNNETSTATIONBASEINFO_MSG:
		{
			ProcessUpDateNetStationMsg(Msg->dwData);
			break;
		}
	case E_PPD_SERVERRETURNERR_MSG:
		{
			ProcessUpDateErrMsg(Msg->dwData);
			break;
		}
	case MSG_RESETTHREAD:
		{
			ProcessResetThreadMsg(Msg);
			break;
		}
	default:
		{
			break;
		}
	}

	if (bDoFlag)
	{
		PrecessTaskData(Msg->dwData, tRapalceDataTemp);
	}

	return 0;
}

//任务预数据
BOOL CPrePareDataThread::PreProcessTaskData(DWORD dwIndex,
											TRapalceDataMsg& tTRapalceDataMsg,
											BOOL bUpdateDataFlag)
{
	T_TaskData* pTaskData = NULL;
	int nReturn;
	//DWORD dwMsgType = E_WEBS_GET_NETSTATION_BASEINFO;   //默认执行下载网站的基本数据文件

	pTaskData=(T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pTaskData!=NULL)
	{
		tTRapalceDataMsg.nNetItemCode=pTaskData->tTaskNetItem.dwNetStationItemID;
		tTRapalceDataMsg.strNetCode.Format(_T("%d"), pTaskData->dwWebID);
		tTRapalceDataMsg.eTaskType = pTaskData->eTaskType;
		if (!pTaskData->tTaskNetItem.vInfoID.empty())
		{
			tTRapalceDataMsg.strInfoID = _T("<#>");
			for (int i = 1; i<pTaskData->tTaskNetItem.vInfoID.size(); i++)
			{
				//信息ID;
				tTRapalceDataMsg.strInfoID += pTaskData->tTaskNetItem.vInfoID[i]+_T("<#>");
			}
			g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("网站数据准备 网站信息ID:%s"), tTRapalceDataMsg.strInfoID.c_str());
		}
		
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("网站数据准备 网站编号:%d"), pTaskData->dwWebID);
	}
	else
	{
		return FALSE;
	}

	if (CheckWebIsNewAndDown(pTaskData->dwWebID,dwIndex,pTaskData->bUpdateDataFlag,bUpdateDataFlag) != 2)
	{
		return FALSE;
	}
				
	//判断当前网站是否含有嵌入网站并检查网站是否最新；
	T_NetstationBaseInfo tNsData;
	int dRet;
	//从网站模板中得到任务数据
	try
	{
		dRet=m_PrepareData.GetValueFromDB(tNsData);
	}
	catch (...)
	{
		g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("读取网站模板数据异常，可能文件不存在或格式不正确 nNetCode:%d!"), pTaskData->dwWebID);
		PostMessage(E_TASKMGR_THREAD,E_PPD_NOWEBDATA_MSG,dwIndex);  //数据异常；
		return FALSE;
	}

	if (tNsData.NsEmbedCode.GetLength() > 6)
	{
		//先判断嵌入的网站是否为最新；
		StringUtils Utl;
		DWORD dwResult = TRUE;
		std::vector<CStdString> result;
		Utl.SplitStringEx(tNsData.NsEmbedCode,_T(","),result,false);
		for (int i = 0; i< result.size(); ++i)
		{
			//如果当前网址包含在不需要更新的网站里面，则跳过
			if (pTaskData->strNotUpdateWeb.Find(result[i]) != -1)
			{
				continue;
			}

			DWORD dwNscode = StrToLong(result[i]);
			//判断嵌入的ID是否需要
			dwResult = CheckWebIsNewAndDown(dwNscode, dwIndex, pTaskData->bUpdateDataFlag);
			
			if (dwResult == 1)
			{
				g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("嵌入网站需要更新，网站ID: %D"),dwNscode);
				pTaskData->bUpdateDataFlag = FALSE;
				m_PrepareData.m_NetData.CloseWebSiteData();
				return FALSE;
			}
			else if (dwResult == 0)
			{
				g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("当前嵌入网站不需要更新，也不是最新，网站id:%d"), dwNscode);
				pTaskData->strNotUpdateWeb += result[i] + _T(",");
			}
		}
	}
	m_PrepareData.m_NetData.CloseWebSiteData();
	//获取帐号和密码
	if ((pTaskData->eTaskType != ETTypeRegaccount) || (pTaskData->eTaskType != ETypeRegEmail)
		|| (pTaskData->eTaskType != ETTypeReRegaccount))
	{
		GetIdAndPsd(pTaskData->dwWebID, pTaskData);
	}

	//读取系统设置等信息，如果失败，设置测试值
	if (GetSysparam(tTRapalceDataMsg, dwIndex)==10)
	{
		GetSysparamForTest(tTRapalceDataMsg);
	}

	tTRapalceDataMsg.strWebName = pTaskData->szAccount;
	tTRapalceDataMsg.strWebPass = pTaskData->szPassword;

	g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("preprocess end netid= %d"), pTaskData->tTaskNetItem.dwWebId);
	return TRUE;
}

//将暂时不能执行且不需要由TaskMgr调度的任务缓存到列表中，以便在条件成熟时重新发起数据准备的消息
void CPrePareDataThread::CacheTask(DWORD dwNetID, DWORD dwTaskIndex)
{
	std::vector<DWORD>::iterator iter;
	iter = find(m_mapCachedTask[dwNetID].begin(),m_mapCachedTask[dwNetID].end(),dwTaskIndex);
	if (iter == m_mapCachedTask[dwNetID].end())
	{
		m_mapCachedTask[dwNetID].push_back(dwTaskIndex);
	}
}

//处理任务处理
int CPrePareDataThread::PrecessTaskData(DWORD dwIndex, TRapalceDataMsg &tTRapalceDataMsg)
{

	T_TaskData *pTaskData = NULL;
	DWORD dRet;
 
	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pTaskData==NULL)
	{
		m_PrepareData.m_NetData.CloseWebSiteData();
		this->PostMessage(E_TASKMGR_THREAD, E_NEWERR, dwIndex);
		return 0;
	}

	g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("PrecessTaskData start netid= %d"), pTaskData->dwWebID);

	dRet=m_PrepareData.ProcessData(pTaskData->dwWebID, pTaskData->eTaskType, tTRapalceDataMsg, pTaskData,dwIndex);
	

	//任务数据准备成功
	if (dRet==0 && tTRapalceDataMsg.eErrType==PPD_SUCCESS)
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("PrecessTaskData  任务数据准备成功 netid= %d"), pTaskData->dwWebID);

		if ((pTaskData->eTaskType == ETTypeRegaccount) || (pTaskData->eTaskType == ETypeRegEmail)
			|| (pTaskData->eTaskType == ETTypeReRegaccount))
		{
			pTaskData->szAccount = tTRapalceDataMsg.strWebName;
			pTaskData->szPassword = tTRapalceDataMsg.strWebPass;
		}

		this->PostMessage(E_TASKMGR_THREAD, eTask_PrepareFinished, dwIndex);
		
	}
	//其他错误类型
	else
	{
	
		this->PostMessage(E_TASKMGR_THREAD, E_PPD_NOWEBDATA_MSG, dwIndex);
	}

	m_PrepareData.m_NetData.CloseWebSiteData();
	g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("PrecessTaskData  end netid= %d"), pTaskData->dwWebID);
	return 0;
}

bool CPrePareDataThread::CheckConnDBfinished(void)
{
	static bool s_bFinished = false;

	if (s_bFinished) return true;

	//所有网站及所有三代引擎任务文件的最后更新时间已获取时才认为数据准备好
	if (m_bAllNetStationLastUpdateTimeGetted )
	{
		this->PostMessage(E_TASKMGR_THREAD, E_PPD_CONNDBOK_MSG, 1);
		s_bFinished = true;

		//重新启动被缓冲的任务
		if (m_mapCachedTask.size() > 0)
		{
			std::vector<DWORD>::iterator it;
			std::map<DWORD,std::vector<DWORD>>::iterator iter;
			for (iter = m_mapCachedTask.begin(); iter != m_mapCachedTask.end(); ++iter)
			{
				for (it = iter->second.begin(); it != iter->second.end(); ++it)
				{
					this->PostMessage(E_DATAPRE_THREAD, E_PPD_START_MSG, *it, 1);
				}
				iter->second.clear();		
			}

			m_mapCachedTask.clear();
		}

		return true;
	}

	return false;
}

//处理获取服务器数据时间消息
int CPrePareDataThread::ProcessGetServerDataTimeMsg(DWORD dwIndex)
{
	//获得所有网站的最新数据时间，用来判断数据是否最新
	int nReturn=GetServerDbTime(dwIndex);
	if (nReturn==0)
	{
		this->PostMessage(E_WEBSERVICE_THREAD, E_WEBS_LASTUPDATE_GETTED, dwIndex);
		m_bAllNetStationLastUpdateTimeGetted = TRUE;
		CheckConnDBfinished();
	}
	
	return 0;
}

//处理更新netstation表消息
int CPrePareDataThread::ProcessUpDateNetStationMsg(DWORD dwIndex)
{
	std::vector<DWORD>::iterator iter;

	if (m_NetUpdateinfo[dwIndex].size()> 0)
	{
		for (iter = m_NetUpdateinfo[dwIndex].begin(); iter != m_NetUpdateinfo[dwIndex].end(); ++iter)
		{
			this->PostMessage(E_TASKMGR_THREAD, E_PPD_SERVERRETURNDATA_MSG, *iter);
		}
		m_NetUpdateinfo[dwIndex].pop_back();
		m_NetUpdateinfo.erase(dwIndex);
	}
	else
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("更新列表无数据！"));
	}

	//网站数据下载完成，对于QuickTask重新准备数据
	std::map<DWORD,std::vector<DWORD>>::iterator it = m_mapCachedTask.find(dwIndex);

	if (it != m_mapCachedTask.end())
	{
		for (iter = it->second.begin(); iter != it->second.end(); ++iter)
		{
			this->PostMessage(E_DATAPRE_THREAD, E_PPD_RESTART_MSG, *iter, 1);
		}
		it->second.pop_back();
		m_mapCachedTask.erase(it);

	}

	return 0;
}


//处理获取更新数据失败消息
int CPrePareDataThread::ProcessUpDateErrMsg(DWORD dwIndex)
{
	//表数据更新失败
	std::vector<DWORD>::iterator iter;

	if (m_NetUpdateinfo[dwIndex].size()> 0)
	{
		for (iter = m_NetUpdateinfo[dwIndex].begin(); iter != m_NetUpdateinfo[dwIndex].end(); ++iter)
		{
			this->PostMessage(E_TASKMGR_THREAD, E_PPD_SERVERRETURNERR_MSG, *iter);
		}
		m_NetUpdateinfo[dwIndex].pop_back();
		m_NetUpdateinfo.erase(dwIndex);
	}

	return 0;
}



//释放内存和变量内容清空
int CPrePareDataThread::EmptyFun()
{
	//对没有释放的new内存，进行释放和清空
	m_NetUpdateinfo.clear();
	m_DBRefrshTimeInfo.clear();

	m_NetUpdateinfo.empty();
	m_DBRefrshTimeInfo.empty();

	m_EngineDBRefrshTimeInfo.clear();
	m_EngineDBRefrshTimeInfo.empty();

	return 0;
}


//处理线程重起消息
int CPrePareDataThread::ProcessResetThreadMsg(T_Message *Msg)
{

	if  (Msg->dwSourWork==E_WEBSERVICE_THREAD)
	{
		std::vector<DWORD>::iterator iter;

		if (m_NetUpdateinfo[Msg->dwData].size()> 0)
		{
			for (iter = m_NetUpdateinfo[Msg->dwData].begin(); iter != m_NetUpdateinfo[Msg->dwData].end(); ++iter)
			{
				this->PostMessage(E_TASKMGR_THREAD, E_PPD_SERVERRETURNERR_MSG, *iter);
				
			}
		}
	}

	return 0;
}

//功能:获取帐号和密码
//输入:dwNetId,网站编号
//输出:
//返回值:
//其他说明:
int CPrePareDataThread::GetIdAndPsd(DWORD dwNetId, T_TaskData* pTask)
{
	T_SaveToServerAccount *pSaveAccountPass = NULL;

	//从全局map读取网站的用户名和密码
	std::map<DWORD,T_SaveToServerAccount*> *pMapAccountPass = NULL;
	pMapAccountPass = (std::map<DWORD,T_SaveToServerAccount*>  *)g_GlobalDataBlock->ReadRealData(E_MAP_SaveAccountPass,
		MAG_ACCOUNT_INDEX,E_PREPAREDATA_CLASS,eUseBlockWithOne);
	
	//判断指针是否为空
	if (pMapAccountPass!=NULL && pTask != NULL)
	{
		pSaveAccountPass = (*pMapAccountPass)[dwNetId];
		if (pSaveAccountPass!=NULL)
		{
			pTask->szAccount = pSaveAccountPass->szAccount;
			pTask->szPassword = pSaveAccountPass->szPassword;	
			pTask->szConfirmMail = pSaveAccountPass->szConfirmMail;
			pTask->szMailPassword = pSaveAccountPass->szMailPassword;
		}
		//指针为空赋默认值
		else
		{
			pTask->szAccount = _T("");
			pTask->szPassword = _T("");	
			pTask->szConfirmMail = _T("");
			pTask->szMailPassword = _T("");
		}
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("GetIdAndPsd id = %d, strWebName =%s, strWebPass=%s"), dwNetId, pTask->szAccount.c_str(), pTask->szPassword.c_str());
	}
    
	return 0;
}

//功能:判断网站id本地数据是不是最新
//输入:dwNetId,网站编号
//输出:tNetStationInfo,网站数据更新标志设置
//返回值:-1,未知的网站id;1,需要更新数据;0,数据是最新的 3模板更新信息没准备好
//其他说明:
int CPrePareDataThread::NetDataIsNew(DWORD dwNetId)
{
	int nRetFlag=0;
	std::map<DWORD,DWORD> ::iterator pIter;

	if (!CheckConnDBfinished())
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("模板更新信息没有准备好！"));
		return 3;
	}
	
	pIter = m_DBRefrshTimeInfo.find(dwNetId);
	if (pIter == m_DBRefrshTimeInfo.end())
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("找不到网站的更新信息，可能该网站已被下架, netid= %d"), dwNetId);
		return -1;
	}
	
	DWORD lastUpdateTime = m_PrepareData.m_NetData.GetNetLastUpdateTime();//获取本地数据的最近跟新时间。
	g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("infiletime  =%d  ,lasttime =%d, netid= %d"), lastUpdateTime, pIter->second,  dwNetId);
	if (pIter->second > lastUpdateTime)
	{
		nRetFlag = 1;
	}
	else if (pIter ->second < lastUpdateTime)
	{
		nRetFlag = 1 ;
	}
	else if (pIter ->second == lastUpdateTime)
	{
		nRetFlag = 2;
	}

	if (nRetFlag==1)
	{
		return 1;
	}

	return 0;
}

//功能:判断需要更新数据的表，发送更新数据请求消息，并返回任务数据不是最新需要更新数据消息
//输入:dwNetId,网站编号;dwIndex,消息的内定编号（任务ID）;tNetStationInfo,数据需要更新标志;
//输出:
//返回值:
//其他说明:
void CPrePareDataThread::ProcessServerDataMsg(DWORD dwNetId, DWORD dwIndex)
{
	std::map<DWORD,std::vector<DWORD>>::iterator iter =m_NetUpdateinfo.find(dwNetId) ;
	if (iter != m_NetUpdateinfo.end())
	{
	}
	else
	{
		PostMessage(E_WEBSERVICE_THREAD, E_WEBS_GET_NETSTATION_BASEINFO, dwNetId); //重新下载网站基本信息文件
	}
	m_NetUpdateinfo[dwNetId].push_back(dwIndex);
	PostMessage(E_TASKMGR_THREAD,E_PPD_GETNETDATAFROMSERVER_MSG,dwIndex);   //改变任务的状态为e_updatedata
}


//功能:获取服务器网站数据更新时间
//输入:
//输出:
//返回值:
//其他说明:
int CPrePareDataThread::GetServerDbTime(DWORD dwIndex)
{
	std::map<DWORD, T_ServerNetStationInfo*> *pAllNetStationInfo = NULL;
	
	pAllNetStationInfo=(std::map<DWORD, T_ServerNetStationInfo*>*)g_GlobalDataBlock->ReadRealData(E_ServerNetStationInfoGroup, 
		dwIndex, E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pAllNetStationInfo==NULL)
	{
		printf("tServerNetStationInfoGroup为空，GetServerDbTime失败！\n");
		return -1;
	}
	if (pAllNetStationInfo->size() <=0)
	{
		printf("tServerNetStationInfoGroup->dwNetCount<=0，GetServerDbTime失败！\n");
		return -2;
	}

	std::map<DWORD, T_ServerNetStationInfo*>::iterator iter;
	for (iter = pAllNetStationInfo->begin(); iter != pAllNetStationInfo->end(); ++iter)
	{

		DWORD &dwNetCodeTemp= (DWORD)iter->first;
		m_DBRefrshTimeInfo[dwNetCodeTemp] = 
					iter->second->dwLastUpdateTime;
	}

	return 0;
}

int CPrePareDataThread::GetServerEnineDataDbTime(DWORD dwIndex)
{
	T_ServerNetStationInfoGroup* tServerNetStationInfoGroup;
	DWORD dwNetNum=0;
	DWORD i;
	DWORD dwNetCodeTemp;
	tServerNetStationInfoGroup=NULL;
	tServerNetStationInfoGroup=(T_ServerNetStationInfoGroup*)g_GlobalDataBlock->ReadRealData(E_ServerEngineFileInfoGroup, 
		dwIndex, E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
	if (tServerNetStationInfoGroup==NULL)
	{
		printf("tServerNetStationInfoGroup为空，GetServerEnineDataDbTime失败！\n");
		return -1;
	}
	if (tServerNetStationInfoGroup->dwNetCount<=0)
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("检查EngineData的更新时间失败，请检查data2\0.dat是否存在！"));
		printf("GetServerEngineDataDbTime失败, 请检查更新信息文件是否存在！\n");
		return -2;
	}

	dwNetNum=tServerNetStationInfoGroup->dwNetCount;
	if (tServerNetStationInfoGroup->dwNetCount>MAX_NET_COUNT_LASTUPDATED)
	{
		dwNetNum=MAX_NET_COUNT_LASTUPDATED;
	}

	for (i=0; i<dwNetNum; i++)
	{
		dwNetCodeTemp=tServerNetStationInfoGroup->NetStationInfoGroup[i].dwNsCode;

		m_EngineDBRefrshTimeInfo[dwNetCodeTemp] = 
			tServerNetStationInfoGroup->NetStationInfoGroup[i].dwLastUpdateTime;
	}
	g_GlobalDataBlock->Reset(E_ServerEngineFileInfoGroup, E_PREPAREDATA_CLASS);

	return 0;
}

//功能:获取系统信息
//输入:
//输出:
//返回值:
//其他说明:
DWORD CPrePareDataThread::GetSysparam(TRapalceDataMsg& tRapalceData, DWORD dwIndex)
{
	T_TaskData* pTaskData;

	pTaskData=NULL;
	pTaskData=(T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pTaskData!=NULL)
	{
		m_Sysparam.GetSysparam(tRapalceData.tParamItemTable, pTaskData);
	}
	else 
	{
		return 10;
	}

	return 0;
}


//功能:系统信息测试函数
//输入:
//输出:
//返回值:
//其他说明:
DWORD CPrePareDataThread::GetSysparamForTest(TRapalceDataMsg& tRapalceData)
{
	m_Sysparam.GetSysparamForTest(tRapalceData.tParamItemTable);

	return 0;
}


int CPrePareDataThread::GetEngineDataFileLastUpdateALL(const CStdString &strData,T_ServerNetStationInfoGroup &ServerLastupdateGroup)
{
	int ItemCount;
	CStdString szDelimiter;
	CStdString szDelimiter1;
	CStdString szTemp;
	CStdString szTempItem;
	TCHAR TempTch[30];
	std::vector<CStdString> results;
	std::vector<CStdString> resultItem;
	szDelimiter=_T("(;0)");
	szDelimiter1=_T("(;1)");
	StringUtils StringUtil ;
	int nCount = StringUtil.SplitString(strData,szDelimiter,results,true);
	int intindex;

	if (nCount < 2 ) 
	{
		ServerLastupdateGroup.dwNetCount = 0;    
		return 0;
	}
	else
	{

		ServerLastupdateGroup.dwNetCount =nCount -1 ; 

		for(intindex=0;intindex<nCount-1;intindex++)
		{		
			szTemp=results[intindex+2];
			resultItem.clear () ;
			ItemCount = StringUtil.SplitString(szTemp, szDelimiter1,resultItem,true)	;
			//增加容错处理
			if (ItemCount<2)
			{
				continue;
			}
			_tcscpy(TempTch , resultItem[0]);
			ServerLastupdateGroup.NetStationInfoGroup[intindex].dwNsCode = _ttol(TempTch);

			_tcscpy(TempTch , resultItem[1]);
			ServerLastupdateGroup.NetStationInfoGroup[intindex].dwLastUpdateTime = _ttol(TempTch);
		}   

	}

	szDelimiter.Empty();
	szDelimiter1.Empty();
	szTemp.Empty();
	szTempItem.Empty();
	results.clear();
	resultItem.clear();

	return 1;
}

/*
@brief  判断网站是否为最新并下载；
@return  2:最新或者下载成功,1:不是最新需要下载，0：不是最新也不需要下载
*/
DWORD CPrePareDataThread::CheckWebIsNewAndDown(DWORD dwWebId, DWORD dwIndex, BOOL bUpdateFlag, BOOL bAlreadyUpdate)
{
	m_PrepareData.m_NetData.CloseWebSiteData();
	DWORD ret =  m_PrepareData.m_NetData.OpenWebSiteData(dwWebId);

	/* 非本地调试检查是否需要更新网站数据，为方便调试，本地调试时则不检查更新*/
	int	IsTest;
	IsTest = 0;
	g_Config->ReadInteger(_T("Control"),_T("Test"),IsTest);           //zhl 140411  无需连接到176即可推广
	if (0 != m_nLocalTest || IsTest == 1)
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("本地调试将不检查网站更新 webid= %d, taskid =%d"), dwWebId, dwIndex);
	}
	else
	{
		if (ret == 0 && bAlreadyUpdate) //数据文件不存在，需要更新)
		{
			g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("子任务文件不存在，需要更新! webid= %d, taskid =%d"),dwWebId, dwIndex);
			ProcessServerDataMsg(dwWebId, dwIndex);
			m_PrepareData.m_NetData.CloseWebSiteData();
			CacheTask(dwWebId, dwIndex);
			return 1;
		}
		else if(ret == 0) //文件不存在，不需要更新
		{
			g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("文件不存在，不需要更新,下载失败! webid= %d,taskid =%d"), dwWebId, dwIndex);

			PostMessage(E_TASKMGR_THREAD,E_PPD_NOWEBDATA_MSG,dwIndex);
			m_PrepareData.m_NetData.CloseWebSiteData();
			return 0;
		}

		if (bUpdateFlag && bAlreadyUpdate)
		{
			int nReturn=NetDataIsNew(dwWebId);
			if (1==nReturn)
			{
				ProcessServerDataMsg(dwWebId, dwIndex);
				m_PrepareData.m_NetData.CloseWebSiteData();
				CacheTask(dwWebId, dwIndex);
				return 1;
			}
			else if (-1==nReturn)
			{
				//该网站暂时被下架！
				g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("该网站已下架! webid= %d"), dwWebId);
				this->PostMessage(E_TASKMGR_THREAD, E_PPD_NETCODELESSZERO_MSG, dwIndex);
				m_PrepareData.m_NetData.CloseWebSiteData();
				return 0;
			}
			else if (3 == nReturn)
			{
				//网站更新信息没有准备好，缓存起来
				CacheTask(dwWebId, dwIndex);
				m_PrepareData.m_NetData.CloseWebSiteData();
				return 1;
			}
		}

	}

	return 2;
}
