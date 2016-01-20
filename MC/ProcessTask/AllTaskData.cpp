#include "StdAfx.h"
#include "KernelProcessThread.h"
#include "AllTaskData.h"
#include "StationData.h"
#include "BlockData.h"
#include "EnDeCode.h"
#include "StringUtils.h"
#include <TlHelp32.h>


#define  WAIT_SECOND		180*1000
CAllTaskData  *gAllTaskData = NULL;
extern CKernelProcessThread  *gThreadMessage;


CAllTaskData::CAllTaskData(void)
{
	m_nPort = 32106 ;  //��ȡ����ʱĬ�϶˿ں�
	lIsRunEngine = 0;
	lngKernelCnt = 0;
	iWaitTime = WAIT_SECOND;
	bRun = true;
	m_hndEngine = NULL;
	m_hndSingleEngine = NULL;
	m_eCurIeVer = IE8;
	bObjectRunFinish = FALSE;

	InitData();
}

CAllTaskData::~CAllTaskData(void)
{
	bRun = false;
	Sleep(100);

	m_Connection.UnInit();
	m_ConnectionByObject.UnInit();
	Sleep(1000);

	//�ص����̵߳��¼�
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

	//�ص����̵߳��¼�
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
	@breif������������������
	@param  dwIndex ��������
	@param  bSingle �Ƿ�Object��վ��
	*/
void CAllTaskData::PushTaskIndex( DWORD dwIndex, BOOL bSingle,E_IeLimit ieMode)
{
	m_TaskLock.Lock();
	if (bSingle)
	{
		//���߳�		
		m_objectQueueTask[ieMode].push(dwIndex);
	}
	else
	{
		m_allQueueTask[ieMode].push(dwIndex);
	}
	
	m_TaskLock.Unlock();
}

/*
	@breif  ȡ�����������������
	@return ������������
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
	@breif  ��ʼ���������߳�
	@param  bIsOneThread  �Ƿ񴴽������߳�
	*/
void CAllTaskData::StartWork(BOOL bSingleKernel /*= FALSE*/)
{
	if (bSingleKernel)
	{
		//�������̵߳��ں�
		m_ObjPort = m_nPort+10;  //��ͬ�ڶ��߳��ں˵Ķ˿�;	
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
			g_log.Trace(LOGL_HIG,LOGT_ERROR,__TFILE__,__LINE__,_T("�����ں�3��ʧ�ܣ��˳�����"));
			ExitProcess(0);	//�˳�����
		}
		i++;
	}

	//���Ʊ�������
	T_LocalGlobalData *pGlobalData = NULL;
	pGlobalData = (T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	strMailBak = pGlobalData->aSystemInfo.userInfo.szMailBak;
	strMail = pGlobalData->aSystemInfo.userInfo.szMail;

	gAllTaskData = this;
	int iThreadCount = 0;
	g_Config->ReadInteger(_T("kernel4"), _T("threadcount"), iThreadCount);
	g_Config->ReadInteger(_T("kernel4"), _T("waittime"), iWaitTime);

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

		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("���������߳����"));
	}
	else
	{
		for (int i=0; i<iThreadCount; i++)
		{
			DWORD dwThreadId;
			hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
			hThreads[i] = CreateThread(NULL, 0, DoTask, (void*)i, 0, &dwThreadId);
		}

		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("����������߳����"));
	}

}

/*
	@breif  �����߳�
	@param  thisָ��
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
	if (hEvent == NULL)  //�¼����󴴽�ʧ��
		return 0;
	
	ASSERT(gAllTaskData != NULL);
	while(gAllTaskData->bRun)
	{
		DWORD dwIndex = gAllTaskData->GetTaskIndex(gAllTaskData->m_eCurIeVer); //ȡ����

		if (dwIndex > 0)
		{
			gAllTaskData->DoCommonTask(dwIndex,hEvent,FALSE,isRestartEngine);
		}
		else
		{
			//�����XPϵͳ������
			if (!gAllTaskData->bObjectRunFinish && gAllTaskData->m_objectQueueTask[gAllTaskData->m_eCurIeVer].size() <= 0 &&
				gAllTaskData->m_allQueueTask[gAllTaskData->m_eCurIeVer].size() <= 0 && gAllTaskData->CheckMultiTask())
			{
				if (gAllTaskData->CheckRunOver(gAllTaskData->m_eCurIeVer, FALSE))
				{
					gAllTaskData->RunEngine();
					Sleep(200);
					//���Objectû����IE����de��վ���������ں�
					if (gAllTaskData->m_hndSingleEngine != NULL)
					{
						gAllTaskData->RunEngine(TRUE);
					}
					g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, _T("��ʼִ���µ�IE�汾��IE�汾Ϊ:%d"), gAllTaskData->m_eCurIeVer);
					Sleep(200);
				}
			}
		}

		Sleep(500);
	}

	return 0;
}

/*
	@breif ����4���ں�
	*/
BOOL CAllTaskData::RunEngine(BOOL bSingleKernel)
{
	if (bSingleKernel)
	{
		if (m_Taskprocess.RunEngine(m_ConnectionByObject,m_ObjPort,m_hndSingleEngine,lngKernelCnt))
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("�������߳�4���˳ɹ�"));
			return TRUE;
		}
	}
	else
	{
		if(m_Taskprocess.RunEngine(m_Connection,m_nPort,m_hndEngine,lIsRunEngine))
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("�������߳�4���˳ɹ�"));
			return TRUE;
		}
	}

	return FALSE;
}

/*
	@brief  ����������
	@param  dwWebId��վid
	@param  pTaskData��������
	@param *pTData����ִ�п�
	@return 0��ʾ�˲���ʧ�ܲ���Ҫ����ִ����ȥ��1��ʾ�˲���ɹ�����ִ����ȥ��2��ʾ����ʱ�����������ϣ��ص���һ��
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
		g_log.Trace(LOGL_MID, LOGT_DEBUG, __TFILE__, __LINE__, _T("�յ��ں˷��ص����ݣ�%s"),(*pResult).c_str());
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
					//���ע���Ƿ�ɹ��ĵ�¼����ֱ���˳�
					if (pTaskData->bGetLogin)
					{
						g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("��¼��֤����ʧ�ܣ�ֱ�ӷ���ע��ʧ��"));
						return iReturn;
					}
					pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
					pTaskData->tTaskResultInfo.strMigrateResult = _T("");   //����ϴ�ִ����ȡ���ֶΣ�
					pTaskData->tTaskResultInfo.dwSuccFieldCnt = 0;
					//Ϊʧ�ܷ��ಢ���ؽ�����ִ�в��裻
					iReturn = m_Taskprocess.GetFailedResult(pTaskData,pTData,tDataOut,resultNs);
					
				}
				break;
			case srFinishedOK:
			case srFinishedOKTolerantFail:
				pTaskData->tTaskResultInfo.strResultInfo = _T("");
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_SUCC;

				if (pTaskData->bGetLogin)
				{
					g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("ͨ��ע����õ�¼��֤���ݳɹ�������ִ��ע������"));
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
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("�����ں˼�⵽�����쳣,ֱ�ӽ�������վid��%d"), dwWebId);
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
				break;
			case srFinishedTaskConfilict:
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("�����ں���������,���ؼ���ѹ��id��%d"), dwWebId);
				iReturn = 5;
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
	@breif  ����վ��վid���¼��������
	@param  dwWebId ��վid
	*/
void CAllTaskData::LinkWeiIdToEvent( DWORD dwWebId, HANDLE hEvent )
{
	m_EventLock.Lock();
//	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("������վID�¼�,��վid��%d,�¼�������%d"),dwWebId,IndexToEvent.size());
	IndexToEvent[dwWebId] = hEvent;
	m_EventLock.Unlock();
}

/*
	@breif  �Ƴ�����վid���¼�����
	@param  dwWebId ��վid
	*/
void CAllTaskData::RemoveWebIdEvent( DWORD dwWebId )
{
	m_EventLock.Lock();

	std::map<DWORD, HANDLE>::iterator iter = IndexToEvent.find(dwWebId);
	if (iter != IndexToEvent.end())
	{
	//	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("�Ƴ���վID�¼�,��վid��%d,�¼�������%d"),dwWebId,IndexToEvent.size());
		IndexToEvent.erase(iter);
	//	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("�Ƴ���վID�¼���,��վid��%d,�¼�������%d"),dwWebId,IndexToEvent.size());
	}
	
	m_EventLock.Unlock();
}

/*
	@breif  ����ĳ��վ�������
	@param  dwWebId ��վid
	@param  dwWebId �������
	*/
void CAllTaskData::PushResult( DWORD dwWebId, const CStdString &strResult )
{
	m_TaskResultLock.Lock();

	webIdToResult[dwWebId] = new CStdString(strResult);

	m_TaskResultLock.Unlock();
}

/*
	@breif  ȥ����վ�������
	@param  dwWebId ��վid
	*/
void CAllTaskData::RemoveResult( DWORD dwWebId )
{
	m_TaskResultLock.Lock();

	std::map<DWORD, CStdString*>::iterator iter = webIdToResult.find(dwWebId);
	if (iter != webIdToResult.end())
	{
// 		delete iter->second;
// 		iter->second = NULL;

		webIdToResult.erase(iter);   //��ֹ������ʧЧ
	}

	m_TaskResultLock.Unlock();
}

/*
	@breif  ȡ����վ�������
	@param  dwWebId ��վid
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
@breif  ������վ���ݲ���
@param  dwWebId ��վid
*/
void CAllTaskData::ContinueOperate(DWORD dwWebId )
{
	SetEvent(IndexToEvent[dwWebId]);
}

/*
	@breif  ������������󣬺����β����
	@param  dwWebId ��վid
	@param  strWebName  ��վ����
	@param  dwIndex  ��������
	@param  bSendStop ��ǰ�Ƿ���Ҫ��֪�Ĵ���ֹͣ������
	*/
void CAllTaskData::HandleCompleteWeb( DWORD dwWebId, const CStdString &strWebName, DWORD dwIndex, bool bSendStop,bool bSingleThread)
{
	if (bSendStop)
	{
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("��վ%d �������"), dwWebId);

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
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("��վ����%d ����ֹͣ�ɹ�"), dwWebId);
			}
		}
		else
		{
			if (m_Connection.SendData( strTaskData ) != -1)
			{
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("��վ����%d ����ֹͣ�ɹ�"), dwWebId);
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
			&& pTaskData->tTaskResultInfo.eResultFlag == e_TASK_RESULT_SUCC)  //�������ϲ���  ���ɹ��������·��� 14.6.25
		{
			//����������ѹ�����
			pTaskData->bAddExtraIsSucc = TRUE;
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("����ʱ�������ϻ�����ͼƬ�ɹ�����վid��%d"), dwWebId);
			PushTaskIndex(dwIndex,bSingleThread,pTaskData->limitRefresh.eIeEdtion);  
		}
		else //��ǰ�������������ϲ���ʱ����������
		{*/
		if (pTaskData->bIsAddExtra || pTaskData->bIsAddPic)
		{
			pTaskData->bAddExtraIsSucc = TRUE;
			g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("����ʱ�������ϻ�����ͼƬʧ�ܣ���վid��%d"), dwWebId);
		}
		gThreadMessage->PostMessage(E_TASKMGR_THREAD,E_4KERNEL_TASK_OUT, dwIndex);
		//}
	}
}

/*
	@brief �����еȴ���������վ ���ڵȴ�
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
@breif  ����ͨ�����ȡ��ֵ*/
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
	@breif  ִ�з������ݲ�����������
	@param	pTData ִ���������ݣ�
	@param  pTaskData �������ݽṹ��
	@param  curStep ��ǰִ�еڼ������ܿ飻
	@param  iFunCount  ���ܿ�������
	@param	strPreCompName ��˾���ƣ�
	@param  curFunction  ��ǰ�������ͣ�
	return	����Ҫִ�еĲ��� 1.CurTaskComplete 2.TASKSTEP 3.REPLACESTEP
*/
DWORD CAllTaskData::SendTaskData( CTaskData *pTData,T_TaskData* pTaskData,long &curStep,
	E_StationFunction &curFunction,DWORD &dwRet,CStdString &strTaskData, CommonParam &cParam)
{
	if (dwRet == WAIT_OBJECT_0) //�������
	{
		int iResultCode = HandleRetData(pTaskData->dwWebID, pTaskData,cParam.dwTaskType,pTData);
		if (iResultCode == 0) //����ʧ��
		{
			if ((pTaskData->eTaskType == ETTypeRegaccount || pTaskData->eTaskType == ETTypeReRegaccount)
				&& pTaskData->tTaskResultInfo.eResultFlag == e_TASK_RESULT_FAIL
				&& !pTaskData->bGetLogin)
			{
				//��Ҫʹ��һ����¼
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("ע��ʧ�ܣ��������������󣬵��õ�¼������֤,��վID: %d"),pTaskData->dwWebID);
				pTaskData->bGetLogin = TRUE;
				return 5;
			}
				
			return 1;										
		}
		else if (iResultCode == 1)  //�����ɹ�
		{
			if (curStep == (cParam.iFunCount - 1))
			{
				if (cParam.dwTaskType == ttDataOnceKeyProduct && cParam.dwLoopPdtCnt > cParam.dwCurPdtCnt)    //���Ʒѭ����ȡ��
				{
					return 4;
				}
				return 1;
			}
		}
		else if	(iResultCode == 2)  //����ʱ��Ҫ��������
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("����ʱ������������,��վidΪ%d"), pTaskData->dwWebID);
			pTaskData->bIsAddExtra = TRUE;  //��ǰ����Ϊ�������ϲ���
			curFunction = sfAddExtra;  //�����������Ϲ���
			return 2;
		}
		else if (iResultCode == 3)  //14.6.25
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("����ʱ��������ͼƬ,��վidΪ%d"), pTaskData->dwWebID);
			pTaskData->bIsAddPic = TRUE; //��ǰΪ����ͼƬ����
			curFunction = sfUpdatePhoto;
			return 2;
		}
		else if(iResultCode == 4)  //��˾���Ѿ�����
		{
			g_log.Trace(LOGL_LOW,LOGT_WARNING,__TFILE__,__LINE__,_T("��˾���Ѿ����ڣ���վidΪ%d"),pTaskData->dwWebID);
			if (m_Taskprocess.ReplaceCompName(strTaskData,cParam.strPreCompName))
			{
				g_log.Trace(LOGL_LOW,LOGT_PROMPT,__TFILE__,__LINE__,_T("�滻��˾�ɹ�������ִ�У�"));
				return 3;
			}
		}
		else if (iResultCode == 5) //������վ��ִ�У����·������
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("��⵽������վ��ִ�У����·�����У�"));
			return 6;
		}
		else if (iResultCode == 6) //�����ظ���ʹ�ñ������䣻
		{
			if ((strMail != strMailBak)
				&& (strTaskData.Find(strMail) != -1)
				&& strMailBak != _T(""))
			{
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ԭ���䣺%s,�滻Ϊ�������䣺%s"), strMail.GetBuffer(), strMailBak.GetBuffer());
				strTaskData.Replace(strMail, strMailBak);
				return 3;
			}
		}	
	}
	else if (dwRet == WAIT_TIMEOUT) //�ȴ����������ʱ
	{
		pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
		pTaskData->tTaskResultInfo.strResultInfo = _T("�ȴ��ں˳�ʱ");
		if ((pTaskData->eTaskType == ETTypeRegaccount) || (pTaskData->eTaskType == ETTypeReRegaccount))
		{
/*
			if (pTData->GetType() > ttDataReg)  //ע��ʱ�����ע�Ჽ����ִ���򷵻��������ʧ��
			{
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDEXTRAFAIL;
			}*/
		}
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("������ʱ,��վidΪ%d"), pTaskData->dwWebID);
		return 1;
	}	

	return 0;
}
/*
	@breif  ��ȡ������������
	@param	eType :��������
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
			curFunction = sfReg;  //�����Զ���¼��������һ����
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
		curFunction = sfPostOther;
	default:
		break;
	}
}

DWORD WINAPI CAllTaskData::DoSingleTask( LPVOID lp )
{
	bool isRestartEngine = false;
	ASSERT(gAllTaskData != NULL);

	if (gAllTaskData->m_hEvent == NULL)  //�¼����󴴽�ʧ��
		return 0;

	while (gAllTaskData->bRun)
	{
		
		DWORD dwIndex = gAllTaskData->GetObjectTaskIndex(gAllTaskData->m_eCurIeVer); //��ȡ����
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
				if (gAllTaskData->CheckRunOver(gAllTaskData->m_eCurIeVer, TRUE))
				{
					gAllTaskData->RunEngine(TRUE);
					Sleep(200);
					//���û�ж��߳���վ����Ҫ�����ںˣ�
					if (gAllTaskData->m_hndEngine != NULL)
					{
						gAllTaskData->RunEngine();
					}
					g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, _T("��ʼִ���µ�IE�汾��IE�汾Ϊ:%d"), gAllTaskData->m_eCurIeVer);
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
	@brief ������Object�ȴ���������վ ���ڵȴ�
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
@brief ��ʼ������
*/
void CAllTaskData::InitData()
{
	//---˫�ں���ص�-----
	iOneKernel = 0;	//�����Ƿ�Ҫ����˫�ں˲���
	lngRunningCount = 0;     //�����е�������
	lngRunObject = 0;
	//---end------

	for(int i=0; i<THREAD_MAX_COUNT; i++)
	{
		hEvents[i] = NULL;
		hThreads[i] = NULL;
	}

	m_hEvent = NULL;
	m_hThread = NULL;
	//��ȡIE�汾
	iIEVersion = m_Taskprocess.GetIEVersion();

	g_Config->ReadInteger(_T("CONTROL"), _T("otherthread"), iOneKernel);

	strMailBak = _T("");
	strMail = _T("");
}

/*
@brief doTaskǰ��׼����
@param iSingle �Ƿ�Ҫ����2���ںˣ�
@param eType   �������ͣ�
@param dwWebId ��վID;
@result FALSE :�����ȥ�����¿�ʼ��
*/
BOOL CAllTaskData::DoPreJob( int iSingle,DWORD dwWebId, E_TASK_TYPE eType,CommonParam &cParam)
{
	//���߳��ں˲���Ҫ����������һ���ں˲���
	if (iSingle == 10)
	{
		goto START;
	}

	//�ж��Ƿ���һ��OBJECT�������վ������(ÿ��ֻ����һ�����У�
	if (iSingle == 1)
	{
		//����û����óɲ���Ҫ�����ں�ִ��Object��վ
		
		if (!m_Taskprocess.RunWebBySingle(dwWebId,lngRunObject,IndexToEvent.size(),lngRunningCount,cParam.bIsObject))
		{
			return FALSE;
		}

		lngRunningCount ++;  //Object��վ�����е������1
	}
	
	if(FindEvent(dwWebId)) 
	{
		//�����Ƿ��и���վ�Ѿ���ִ�е���������У�����ѹ�����
		if (IndexToEvent[dwWebId] != NULL)
		{
			if(iSingle == 1)
				lngRunningCount--;
			return FALSE;
		}
	}

START:
	//��ȡ��������
	if (strMail.Find(_T("@126")) != -1)
	{
		cParam.dwEmbedID = 40000003;
	}
	else if (strMail.Find(_T("@sina")) != -1)
	{
		cParam.dwEmbedID = 40000594;
	}
	//һ����ҹ���
	if (eType == ETTypeUserInfoMigrate)
	{
		CStdString strProductNames = _T("");
		T_LocalGlobalData *pGlobalData = NULL;
		pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
			E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

		cParam.dwLoopPdtCnt = pGlobalData->dwMigrationProductCount;   //��ȡҪ��ȡ�Ĳ�Ʒ������
		if (cParam.dwLoopPdtCnt > 0)
		{
			strProductNames = pGlobalData->szMigrationProductNames;
			StringUtils strUtil;
			strUtil.SplitString(strProductNames,_T("(;1)"),cParam.vPdtLists,false);
		}

		iWaitTime = 300*1000;    //һ����������ʱ�䣻
	}

	if (eType == ETTypeShopTraffic)
	{
		iWaitTime = 1000 * 1000; //���ʱ�䣻
	}

	return TRUE;
}

/*
@brief ����һ����ҳɹ�ʱ����
@param dwTaskType �������ͣ�
@param dwPdtCnt   ��Ʒ������
@param pTask	  �������ݣ�
@return TRUE :û�в�Ʒ��Ҫ��ȡ��
*/
BOOL CAllTaskData::ProcessSuccMigrate( DWORD dwTaskType, DWORD dwPdtCnt, T_TaskData *pTask,DWORD dwIndex)
{
	if ((ttDataOnceKeyMove == dwTaskType) || (ttDataOnceKeyProduct ==dwTaskType) )
	{
		if (dwPdtCnt == 0)   //������ȡ��Ʒֱ���˳���
		{
			return TRUE;
		}
		pTask->tTaskResultInfo.dwType = dwTaskType;
		gThreadMessage->PostMessage(E_SAVE_THREAD,E_SAVE_FIRSTDATA,dwIndex);  //һ����ҹ��ܣ�
	}

	return FALSE;
}

/*
@brief dotask��DoSingleTask��ͬ�Ĳ��֣�
@param dwIndex  ����������
@param hEvent   �¼�����
@param bSingle  ���߳�����
*/
DWORD CAllTaskData::DoCommonTask( DWORD dwIndex, HANDLE hEvent, BOOL bSingle,bool &isRestartEngine)
{
	CCommServer *comConnect =  NULL;
	bool bSendStop = true;  //������������Ƿ���Ҫ��֪�Ĵ���ֹͣ������  Ĭ����Ҫ
	bool bRePush = false;   //�Ƿ�Ҫ����ѹ�����
	CommonParam cParam;		//��������
	T_TaskData* pTaskData = NULL;

	pTaskData=(T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pTaskData != NULL)
	{
		pTaskData->bIsAddExtra = FALSE;		//�ж��Ƿ�Ҫִ����������
		pTaskData->bIsAddPic  = FALSE;		//�ж��Ƿ�Ҫִ���������
		cParam.bIsObject = pTaskData->siteType.bIsObject;

		if (bSingle)
		{
			//��ȡ��Ʒ����
			DoPreJob(10,pTaskData->dwWebID,pTaskData->eTaskType,cParam);
			comConnect = &m_ConnectionByObject;
			indexToObjectEvent[pTaskData->dwWebID] = hEvent;
		}
		else
		{
			//ִ��ǰ�ж�
			if (!DoPreJob(iOneKernel,pTaskData->dwWebID,pTaskData->eTaskType,cParam))
			{
				PushTaskIndex(dwIndex,bSingle,pTaskData->limitRefresh.eIeEdtion);
				Sleep(100);
				return 0;
			}
			comConnect = &m_Connection;
			LinkWeiIdToEvent(pTaskData->dwWebID, hEvent);  //����վid���¼��������
		}

		CStationData m_StationData;		
		m_StationData.SetSource(pTaskData->tTaskDataExser.strEnginData) ;

		E_StationFunction curFunction;
		GetCurTaskType(pTaskData->eTaskType,curFunction,bSendStop);
TASKSTEP:
		CFunctionData funcData ;
		m_StationData.LoadData(curFunction, funcData);  //���ؾ��幦��
		cParam.iFunCount = funcData.GetCount();
		DWORD dwCurtime = GetTickCount();

		if (cParam.iFunCount <= 0)  //ȷ�ϴ��������1������ִ�п�   //ȡ��������ʧ��
		{
			g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("function GetCount ����0����վidΪ%d, ��������Ϊ��%d"), pTaskData->dwWebID, curFunction);
			bSendStop = false;
			goto CurTaskComplete;
		}

		for (long j=0 ; j<cParam.iFunCount ; ++j)
		{
DATASTEP:
			CTaskData *pTData = NULL;
			CStdString strOCR = _T("");
			//��ȡ���������
			pTData = m_Taskprocess.GetTaskData(m_StationData,
				funcData,curFunction,pTaskData,j,strOCR,cParam.dwEmbedID);
			if (pTData == NULL)			//ȡ����ִ�п�����ʧ��
			{						
				bSendStop = false;
				goto CurTaskComplete;
			}
			//��ȡ��������
			CStdString strTaskData = m_Taskprocess.GetSendData(pTaskData->dwWebID,m_StationData,pTData,strOCR);
			cParam.strSendDataBak = strTaskData;  //���ݵ�ǰִ�����ݣ�������ȡ�����Ʒ�ظ�ʹ�ã�
			cParam.dwTaskType = funcData.GetCurTaskType(); //��ȡ��������
REPLACESTEP:
			//�滻��Ʒ����
			m_Taskprocess.ReplaceSendData(cParam,strTaskData);
			int iLeaveTime = iWaitTime-(GetTickCount()-dwCurtime);
			if (iLeaveTime <= 0)		//������ʱ   ��������ʱ�䳬���޶�ʱ��
			{
				pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
				pTaskData->tTaskResultInfo.strResultInfo = _T("�ȴ��ں˳�ʱ");
				g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("���������޶�ʱ��,��վidΪ%d"), pTaskData->dwWebID);
				goto CurTaskComplete;
			}
			if (comConnect->SendData( strTaskData ) != -1)
			{
				//һ�����룬���ݷ������ֱ���˳���
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
					gThreadMessage->PostMessage(E_SAVE_THREAD,E_SAVE_FIRSTDATA,dwIndex);  //һ����ҹ��ܣ�
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
			else   //��������ʧ��
			{
				g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("socket������ʧ�ܣ���վid��%d "), pTaskData->dwWebID);
				bSendStop = false;
				isRestartEngine = true;    //socket��������ʧ��
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

		//����Ƿ���Ҫ����ѹ��
		if (bRePush)
		{
			bRePush = false;
			PushTaskIndex(dwIndex, bSingle, pTaskData->limitRefresh.eIeEdtion);  //��ʧ���������¿�ʼ
			return 0;
		}

		if (isRestartEngine)
		{
			//�����ں�
			isRestartEngine = false;
			RunEngine(bSingle);									
			g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("socket����ʧ�ܣ���������4����, ��վidΪ:%d"), pTaskData->dwWebID);
			PushTaskIndex(dwIndex,bSingle,pTaskData->limitRefresh.eIeEdtion);  //��ʧ���������¿�ʼ
			Sleep(200);
		}
		else
		{
			HandleCompleteWeb(pTaskData->dwWebID, m_StationData.GetNsName(), dwIndex, bSendStop,bSingle);
		}
	}
	else
	{
		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("pTaskDataΪNULL, ��������%d"), dwIndex);
		//ȡ����������ʧ��
	}	

}

/*
	@breif  ȡ�����������������
	@return ������������
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
@breif �жϵ�ǰ�汾��������վ�Ƿ�������ɣ�
@param eIever ie�汾��
@return TRUE ����ɣ�False δ���
*/
BOOL CAllTaskData::CheckRunOver(E_IeLimit eIever, BOOL bSingle)
{
	BOOL bResult = FALSE;
	std::map<E_IeLimit, std::queue<DWORD>>* mapTmpTask;

	m_TaskLock.Lock();

	//�������һ���ں˸ı���IEģʽ������һ��ֱ���˳���
	if (eIever != m_eCurIeVer)
	{
		m_TaskLock.Unlock();
		return FALSE;
	}

	//���ݱ��ȡ��Ӧ�Ķ���
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
		//ȡ������������IE�汾��վ
		if (iCnt < iter->second.size())
		{
			iCnt = iter->second.size();
			m_eCurIeVer = iter->first;
			bResult = TRUE;
		}
	}		
	
	//�л�ע���IEģʽ
	if (bResult)
	{
		SetIEMode(m_eCurIeVer);
	}

	m_TaskLock.Unlock();

	return bResult;
}

/*
@breif ����IEģʽ
*/
void CAllTaskData::SetIEMode(E_IeLimit eVer)
{
	m_Taskprocess.RaisePrivileges();
	m_Taskprocess.SetIECoreVersion(eVer);
	m_eCurIeVer = eVer;
}

/*
@breif �ж϶��߳���վ�Ƿ���������ִ�У�
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
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("�ں��쳣�˳����������л�������"));

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
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("Object�ں��쳣�˳����������л�������"));

		if (gAllTaskData != NULL)
		{
			gAllTaskData->ContinueObjectOperate();
		}
	}
	return 0 ;

}
