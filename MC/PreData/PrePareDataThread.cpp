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


//��Ϣ������
DWORD CPrePareDataThread::DispatchMessage(T_Message *Msg)
{
	TRapalceDataMsg tRapalceDataTemp;
	BOOL bDoFlag=FALSE;

	switch(Msg->dwMsgParam)
	{
	case E_PPD_LASTUPDATE_GETTED_MSG:
		{
			ProcessGetServerDataTimeMsg(Msg->dwData);            //���� ,�������
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

//����Ԥ����
BOOL CPrePareDataThread::PreProcessTaskData(DWORD dwIndex,
											TRapalceDataMsg& tTRapalceDataMsg,
											BOOL bUpdateDataFlag)
{
	T_TaskData* pTaskData = NULL;
	int nReturn;
	//DWORD dwMsgType = E_WEBS_GET_NETSTATION_BASEINFO;   //Ĭ��ִ��������վ�Ļ��������ļ�

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
				//��ϢID;
				tTRapalceDataMsg.strInfoID += pTaskData->tTaskNetItem.vInfoID[i]+_T("<#>");
			}
			g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��վ����׼�� ��վ��ϢID:%s"), tTRapalceDataMsg.strInfoID.c_str());
		}
		
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��վ����׼�� ��վ���:%d"), pTaskData->dwWebID);
	}
	else
	{
		return FALSE;
	}

	if (CheckWebIsNewAndDown(pTaskData->dwWebID,dwIndex,pTaskData->bUpdateDataFlag,bUpdateDataFlag) != 2)
	{
		return FALSE;
	}
				
	//�жϵ�ǰ��վ�Ƿ���Ƕ����վ�������վ�Ƿ����£�
	T_NetstationBaseInfo tNsData;
	int dRet;
	//����վģ���еõ���������
	try
	{
		dRet=m_PrepareData.GetValueFromDB(tNsData);
	}
	catch (...)
	{
		g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("��ȡ��վģ�������쳣�������ļ������ڻ��ʽ����ȷ nNetCode:%d!"), pTaskData->dwWebID);
		PostMessage(E_TASKMGR_THREAD,E_PPD_NOWEBDATA_MSG,dwIndex);  //�����쳣��
		return FALSE;
	}

	if (tNsData.NsEmbedCode.GetLength() > 6)
	{
		//���ж�Ƕ�����վ�Ƿ�Ϊ���£�
		StringUtils Utl;
		DWORD dwResult = TRUE;
		std::vector<CStdString> result;
		Utl.SplitStringEx(tNsData.NsEmbedCode,_T(","),result,false);
		for (int i = 0; i< result.size(); ++i)
		{
			//�����ǰ��ַ�����ڲ���Ҫ���µ���վ���棬������
			if (pTaskData->strNotUpdateWeb.Find(result[i]) != -1)
			{
				continue;
			}

			DWORD dwNscode = StrToLong(result[i]);
			//�ж�Ƕ���ID�Ƿ���Ҫ
			dwResult = CheckWebIsNewAndDown(dwNscode, dwIndex, pTaskData->bUpdateDataFlag);
			
			if (dwResult == 1)
			{
				g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("Ƕ����վ��Ҫ���£���վID: %D"),dwNscode);
				pTaskData->bUpdateDataFlag = FALSE;
				m_PrepareData.m_NetData.CloseWebSiteData();
				return FALSE;
			}
			else if (dwResult == 0)
			{
				g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("��ǰǶ����վ����Ҫ���£�Ҳ�������£���վid:%d"), dwNscode);
				pTaskData->strNotUpdateWeb += result[i] + _T(",");
			}
		}
	}
	m_PrepareData.m_NetData.CloseWebSiteData();
	//��ȡ�ʺź�����
	if ((pTaskData->eTaskType != ETTypeRegaccount) || (pTaskData->eTaskType != ETypeRegEmail)
		|| (pTaskData->eTaskType != ETTypeReRegaccount))
	{
		GetIdAndPsd(pTaskData->dwWebID, pTaskData);
	}

	//��ȡϵͳ���õ���Ϣ�����ʧ�ܣ����ò���ֵ
	if (GetSysparam(tTRapalceDataMsg, dwIndex)==10)
	{
		GetSysparamForTest(tTRapalceDataMsg);
	}

	tTRapalceDataMsg.strWebName = pTaskData->szAccount;
	tTRapalceDataMsg.strWebPass = pTaskData->szPassword;

	g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("preprocess end netid= %d"), pTaskData->tTaskNetItem.dwWebId);
	return TRUE;
}

//����ʱ����ִ���Ҳ���Ҫ��TaskMgr���ȵ����񻺴浽�б��У��Ա�����������ʱ���·�������׼������Ϣ
void CPrePareDataThread::CacheTask(DWORD dwNetID, DWORD dwTaskIndex)
{
	std::vector<DWORD>::iterator iter;
	iter = find(m_mapCachedTask[dwNetID].begin(),m_mapCachedTask[dwNetID].end(),dwTaskIndex);
	if (iter == m_mapCachedTask[dwNetID].end())
	{
		m_mapCachedTask[dwNetID].push_back(dwTaskIndex);
	}
}

//����������
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
	

	//��������׼���ɹ�
	if (dRet==0 && tTRapalceDataMsg.eErrType==PPD_SUCCESS)
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("PrecessTaskData  ��������׼���ɹ� netid= %d"), pTaskData->dwWebID);

		if ((pTaskData->eTaskType == ETTypeRegaccount) || (pTaskData->eTaskType == ETypeRegEmail)
			|| (pTaskData->eTaskType == ETTypeReRegaccount))
		{
			pTaskData->szAccount = tTRapalceDataMsg.strWebName;
			pTaskData->szPassword = tTRapalceDataMsg.strWebPass;
		}

		this->PostMessage(E_TASKMGR_THREAD, eTask_PrepareFinished, dwIndex);
		
	}
	//������������
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

	//������վ�������������������ļ���������ʱ���ѻ�ȡʱ����Ϊ����׼����
	if (m_bAllNetStationLastUpdateTimeGetted )
	{
		this->PostMessage(E_TASKMGR_THREAD, E_PPD_CONNDBOK_MSG, 1);
		s_bFinished = true;

		//�������������������
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

//�����ȡ����������ʱ����Ϣ
int CPrePareDataThread::ProcessGetServerDataTimeMsg(DWORD dwIndex)
{
	//���������վ����������ʱ�䣬�����ж������Ƿ�����
	int nReturn=GetServerDbTime(dwIndex);
	if (nReturn==0)
	{
		this->PostMessage(E_WEBSERVICE_THREAD, E_WEBS_LASTUPDATE_GETTED, dwIndex);
		m_bAllNetStationLastUpdateTimeGetted = TRUE;
		CheckConnDBfinished();
	}
	
	return 0;
}

//�������netstation����Ϣ
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
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("�����б������ݣ�"));
	}

	//��վ����������ɣ�����QuickTask����׼������
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


//�����ȡ��������ʧ����Ϣ
int CPrePareDataThread::ProcessUpDateErrMsg(DWORD dwIndex)
{
	//�����ݸ���ʧ��
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



//�ͷ��ڴ�ͱ����������
int CPrePareDataThread::EmptyFun()
{
	//��û���ͷŵ�new�ڴ棬�����ͷź����
	m_NetUpdateinfo.clear();
	m_DBRefrshTimeInfo.clear();

	m_NetUpdateinfo.empty();
	m_DBRefrshTimeInfo.empty();

	m_EngineDBRefrshTimeInfo.clear();
	m_EngineDBRefrshTimeInfo.empty();

	return 0;
}


//�����߳�������Ϣ
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

//����:��ȡ�ʺź�����
//����:dwNetId,��վ���
//���:
//����ֵ:
//����˵��:
int CPrePareDataThread::GetIdAndPsd(DWORD dwNetId, T_TaskData* pTask)
{
	T_SaveToServerAccount *pSaveAccountPass = NULL;

	//��ȫ��map��ȡ��վ���û���������
	std::map<DWORD,T_SaveToServerAccount*> *pMapAccountPass = NULL;
	pMapAccountPass = (std::map<DWORD,T_SaveToServerAccount*>  *)g_GlobalDataBlock->ReadRealData(E_MAP_SaveAccountPass,
		MAG_ACCOUNT_INDEX,E_PREPAREDATA_CLASS,eUseBlockWithOne);
	
	//�ж�ָ���Ƿ�Ϊ��
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
		//ָ��Ϊ�ո�Ĭ��ֵ
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

//����:�ж���վid���������ǲ�������
//����:dwNetId,��վ���
//���:tNetStationInfo,��վ���ݸ��±�־����
//����ֵ:-1,δ֪����վid;1,��Ҫ��������;0,���������µ� 3ģ�������Ϣû׼����
//����˵��:
int CPrePareDataThread::NetDataIsNew(DWORD dwNetId)
{
	int nRetFlag=0;
	std::map<DWORD,DWORD> ::iterator pIter;

	if (!CheckConnDBfinished())
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("ģ�������Ϣû��׼���ã�"));
		return 3;
	}
	
	pIter = m_DBRefrshTimeInfo.find(dwNetId);
	if (pIter == m_DBRefrshTimeInfo.end())
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�Ҳ�����վ�ĸ�����Ϣ�����ܸ���վ�ѱ��¼�, netid= %d"), dwNetId);
		return -1;
	}
	
	DWORD lastUpdateTime = m_PrepareData.m_NetData.GetNetLastUpdateTime();//��ȡ�������ݵ��������ʱ�䡣
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

//����:�ж���Ҫ�������ݵı����͸�������������Ϣ���������������ݲ���������Ҫ����������Ϣ
//����:dwNetId,��վ���;dwIndex,��Ϣ���ڶ���ţ�����ID��;tNetStationInfo,������Ҫ���±�־;
//���:
//����ֵ:
//����˵��:
void CPrePareDataThread::ProcessServerDataMsg(DWORD dwNetId, DWORD dwIndex)
{
	std::map<DWORD,std::vector<DWORD>>::iterator iter =m_NetUpdateinfo.find(dwNetId) ;
	if (iter != m_NetUpdateinfo.end())
	{
	}
	else
	{
		PostMessage(E_WEBSERVICE_THREAD, E_WEBS_GET_NETSTATION_BASEINFO, dwNetId); //����������վ������Ϣ�ļ�
	}
	m_NetUpdateinfo[dwNetId].push_back(dwIndex);
	PostMessage(E_TASKMGR_THREAD,E_PPD_GETNETDATAFROMSERVER_MSG,dwIndex);   //�ı������״̬Ϊe_updatedata
}


//����:��ȡ��������վ���ݸ���ʱ��
//����:
//���:
//����ֵ:
//����˵��:
int CPrePareDataThread::GetServerDbTime(DWORD dwIndex)
{
	std::map<DWORD, T_ServerNetStationInfo*> *pAllNetStationInfo = NULL;
	
	pAllNetStationInfo=(std::map<DWORD, T_ServerNetStationInfo*>*)g_GlobalDataBlock->ReadRealData(E_ServerNetStationInfoGroup, 
		dwIndex, E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pAllNetStationInfo==NULL)
	{
		printf("tServerNetStationInfoGroupΪ�գ�GetServerDbTimeʧ�ܣ�\n");
		return -1;
	}
	if (pAllNetStationInfo->size() <=0)
	{
		printf("tServerNetStationInfoGroup->dwNetCount<=0��GetServerDbTimeʧ�ܣ�\n");
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
		printf("tServerNetStationInfoGroupΪ�գ�GetServerEnineDataDbTimeʧ�ܣ�\n");
		return -1;
	}
	if (tServerNetStationInfoGroup->dwNetCount<=0)
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("���EngineData�ĸ���ʱ��ʧ�ܣ�����data2\0.dat�Ƿ���ڣ�"));
		printf("GetServerEngineDataDbTimeʧ��, ���������Ϣ�ļ��Ƿ���ڣ�\n");
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

//����:��ȡϵͳ��Ϣ
//����:
//���:
//����ֵ:
//����˵��:
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


//����:ϵͳ��Ϣ���Ժ���
//����:
//���:
//����ֵ:
//����˵��:
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
			//�����ݴ���
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
@brief  �ж���վ�Ƿ�Ϊ���²����أ�
@return  2:���»������سɹ�,1:����������Ҫ���أ�0����������Ҳ����Ҫ����
*/
DWORD CPrePareDataThread::CheckWebIsNewAndDown(DWORD dwWebId, DWORD dwIndex, BOOL bUpdateFlag, BOOL bAlreadyUpdate)
{
	m_PrepareData.m_NetData.CloseWebSiteData();
	DWORD ret =  m_PrepareData.m_NetData.OpenWebSiteData(dwWebId);

	/* �Ǳ��ص��Լ���Ƿ���Ҫ������վ���ݣ�Ϊ������ԣ����ص���ʱ�򲻼�����*/
	int	IsTest;
	IsTest = 0;
	g_Config->ReadInteger(_T("Control"),_T("Test"),IsTest);           //zhl 140411  �������ӵ�176�����ƹ�
	if (0 != m_nLocalTest || IsTest == 1)
	{
		g_sysLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("���ص��Խ��������վ���� webid= %d, taskid =%d"), dwWebId, dwIndex);
	}
	else
	{
		if (ret == 0 && bAlreadyUpdate) //�����ļ������ڣ���Ҫ����)
		{
			g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�������ļ������ڣ���Ҫ����! webid= %d, taskid =%d"),dwWebId, dwIndex);
			ProcessServerDataMsg(dwWebId, dwIndex);
			m_PrepareData.m_NetData.CloseWebSiteData();
			CacheTask(dwWebId, dwIndex);
			return 1;
		}
		else if(ret == 0) //�ļ������ڣ�����Ҫ����
		{
			g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�ļ������ڣ�����Ҫ����,����ʧ��! webid= %d,taskid =%d"), dwWebId, dwIndex);

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
				//����վ��ʱ���¼ܣ�
				g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("����վ���¼�! webid= %d"), dwWebId);
				this->PostMessage(E_TASKMGR_THREAD, E_PPD_NETCODELESSZERO_MSG, dwIndex);
				m_PrepareData.m_NetData.CloseWebSiteData();
				return 0;
			}
			else if (3 == nReturn)
			{
				//��վ������Ϣû��׼���ã���������
				CacheTask(dwWebId, dwIndex);
				m_PrepareData.m_NetData.CloseWebSiteData();
				return 1;
			}
		}

	}

	return 2;
}
