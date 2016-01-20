#include "StdAfx.h"
#include ".\taskmanger.h"
#include <ShellAPI.h>

CTaskManger::CTaskManger(void)
:IThreadUnit(E_TASKMGR_THREAD,0xFFFF)	//��1���������߳���𣬵�2������(��)�ǳ�ʱ����(0xFFFF��ʾ����ʱ)
{
	m_TaskDoingMax = 40;
	m_TaskDoingCount = 0;
// 	vNsCodeOfObject.clear();
// 	vNsCodeOfObject.push_back(40000519);
// 	vNsCodeOfObject.push_back(40006512);
// 	vNsCodeOfObject.push_back(40001567);
// 	vNsCodeOfObject.push_back(49994692);
// 	vNsCodeOfObject.push_back(40012911);
}

CTaskManger::~CTaskManger(void)
{
	
}

//�޸�Ϊ���Ȼ�ȡ��������û�����ȡ��������
BOOL CTaskManger::GetIdleTask(DWORD &dwTaskIndex,DWORD &dwLevel)
{
	if(m_TaskDescMap.size()==0) return FALSE;
	std::map<DWORD,T_TaskDesc*>::iterator pIter;
	for(pIter=m_TaskDescMap.begin();pIter!=m_TaskDescMap.end();pIter++)
	{
		DWORD index= pIter->first;

		T_TaskDesc *pTaskDesc = pIter->second;	
		if(pTaskDesc->Data->eTaskType == ETypeMailCount)
		{
			if ((m_mapTaskCount[ETTypeRegaccount] > 0) || (m_mapTaskCount[ETTypeReRegaccount]>0))
			{
				continue;
			}
		}

// 		for (int i = 0; i< vNsCodeOfObject.size(); ++i)
// 		{
// 			if ((vNsCodeOfObject[i] == pTaskDesc->dwWebId) && (m_TaskDescMap.size() > 6))
// 			{
// 				continue;
// 			}
// 		}

		if(pTaskDesc==NULL)
		{
			continue;
		}
		
		if(pTaskDesc->TaskState == e_dataprepared) //��������!=ԭʼ���� = ����׼����ɵĲ��ܽ�����ѹ��ExSer
		{
			switch(pTaskDesc->eLevel) 
			{
			case ETRegaccountAsync://ע�ἶ
				{
					dwTaskIndex = pTaskDesc->dwTaskIndex;
					dwLevel = 0;
					return TRUE;
				}
				break;
			case ETPostAsync://������
				{
					dwTaskIndex = pTaskDesc->dwTaskIndex;
					dwLevel = 0;
					return TRUE;
				}
				break;
			case ETPostSync://������ע��(һ�����ֶ�������׼���õ�)
				{
					if(pTaskDesc->dwPrvTaskID>0) //ǰ������>0
					{
						dwTaskIndex = pTaskDesc->dwTaskIndex;
						dwLevel = 0;
						return TRUE;
					}
					else
					{
						dwTaskIndex = pTaskDesc->dwTaskIndex;
						dwLevel = 1;
						return TRUE;
					}
				}
				break;
			default:
				g_tskLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("���������޷�ȷ�� taskid = %d pTaskDesc->eLevel:%d"), dwTaskIndex, pTaskDesc->eLevel);
			}
		}
	}

	return FALSE;
}


BOOL CTaskManger::SetTaskState(DWORD dwTaskIndex,E_TASKSTATE eTaskState)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	if(pTaskDesc!=NULL)
	{
		pTaskDesc->eLevel = pTaskDesc->Data->eTaskLevel; //����
		pTaskDesc->TaskState = eTaskState;
		/*switch(pTaskDesc->TaskState)
		{
		case e_updateerr:
		case e_updatefinished:
			{
				pTaskDesc->TaskState  = e_dataprepare;
				PostMessage(E_DATAPRE_THREAD, E_PPD_RESTART_MSG, dwTaskIndex);
			}
			break;
		}*/
		return TRUE;
	}
	return FALSE;
}


BOOL CTaskManger::SetTaskStartTime(DWORD dwTaskIndex,DWORD dwTime)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	if(pTaskDesc!=NULL)
	{
		pTaskDesc->dwStartTime = dwTime;
		return TRUE;
	}
	return FALSE;
}

T_TaskDesc *CTaskManger::GetTask(DWORD dwTaskIndex)
{
	std::map<DWORD,T_TaskDesc *>::iterator pIter;
	pIter = m_TaskDescMap.find(dwTaskIndex);
	if(pIter!=m_TaskDescMap.end()) return pIter->second;
	return NULL;
}

DWORD CTaskManger::RemoveTask(DWORD dwTaskIndex)
{
	T_TaskData *pTaskData =
		(T_TaskData *)g_GlobalDataBlock->DeleteDataNode(
		dwTaskIndex,E_AllTaskData,E_THREADTASKMGR_CLASS,eUseBlockWithOne);
	
	std::map<DWORD,T_TaskDesc *>::iterator pIter=m_TaskDescMap.find(dwTaskIndex);
	if(pIter != m_TaskDescMap.end())
	{
		T_TaskDesc *pTaskDesc = pIter->second;
		g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("RemoveTask webid: [ %d ]"),pTaskDesc->Data->dwWebID);
		m_TaskDescMap.erase(dwTaskIndex);
		_DELETE(pTaskDesc);
	}
      
	_DELETE(pTaskData);

	return 0;
}

//�������
DWORD CTaskManger::SchedulerTask()
{
	BOOL bRet = FALSE;
	BOOL bEngineTask = FALSE;
	
	//���䱾������
	for(int i=0;i<m_TaskDoingMax;i++)
	{
		DWORD dwTaskIndex = 0;
		DWORD dwLevel = 0;
		bRet = this->GetIdleTask(dwTaskIndex,dwLevel);
		if(bRet != TRUE)
			break;
		if(m_TaskDoingCount ==0)
		{
			g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("������Ƚ���4���ں�ִ�� taskid:%d"), dwTaskIndex);
		}

		m_TaskDoingCount++;
		SetTaskState(dwTaskIndex,e_doing);
		PostMessage(E_4KERNELWORK_THREAD, E_4KERNEL_TASK_IN, dwTaskIndex, dwLevel);
		
	       
	}

	return 0;
}


//���������б�
DWORD CTaskManger::BuildTaskList(DWORD dwGUIDataIndex)
{
	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pTaskItem = NULL;
	T_LocalGlobalData *pLocalGlobalData = NULL;

	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);

	pTaskItem = (std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,
		dwGUIDataIndex,E_Save_CLASS,eUseBlockWithOne);

	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*>::iterator iterType;
	std::map<DWORD,T_TaskNetItem*>::iterator iterTask;
	ASSERT(pTaskItem != NULL && pLocalGlobalData != NULL);

	//��������Ϣд��ini�ļ�
	g_Config->WriteInteger(_T("INPUTOCR"), _T("MaxRetryTimes"), pLocalGlobalData->dwRetry);
	g_Config->WriteInteger(_T("INPUTOCR"), _T("Display"), pLocalGlobalData->dwShow);
	g_Config->WriteInteger(_T("INPUTOCR"), _T("ShowTime"), pLocalGlobalData->dwShowTime);
	g_Config->WriteInteger(_T("INPUTOCR"), _T("InputTime"), pLocalGlobalData->dwInputTime);
	g_Config->WriteInteger(_T("INPUTOCR"), _T("Type"), pLocalGlobalData->dwOCRType);

	for(iterType = pTaskItem->begin(); iterType != pTaskItem->end(); ++iterType)
	{
		pLocalGlobalData->dwTaskType = iterType->first;
		m_mapTaskCount[pLocalGlobalData->dwTaskType] = 0;
		for (iterTask = iterType->second->begin(); iterTask != iterType->second->end(); iterTask++)
		{
			T_TaskData *pTaskData = NULL;
			_NEW(pTaskData,T_TaskData);

			DWORD dwIndex = g_GlobalDataBlock->GetGlobalID();

			T_TaskNetItem *pTaskItem = (T_TaskNetItem *)iterTask->second;
			pTaskData->dwWebID = pTaskItem->dwWebId;

			pTaskData->eTaskType = (E_TASK_TYPE)pLocalGlobalData->dwTaskType;
			pTaskData->dwPostType = pLocalGlobalData->dwPostType;
			pTaskData->dwIndex = dwIndex;
			pTaskData->bUpdateDataFlag = pLocalGlobalData->bIsUpdate;

			pTaskData->szAccount = _T("");
			pTaskData->szPassword = _T("");
			CopyMemory(&(pTaskData->tTaskNetItem),pTaskItem,sizeof(T_TaskNetItem));

			pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL; //Ĭ����ʧ��

			pTaskData->eTaskLevel = ETRegaccountAsync;
			if ((pLocalGlobalData->dwTaskType == ETTypeRegaccount) ||(pLocalGlobalData->dwTaskType == ETTypeReRegaccount))
				pTaskData->eTaskLevel = ETRegaccountAsync;
			else if (pLocalGlobalData->dwTaskType == ETTypePost)
				pTaskData->eTaskLevel = ETPostSync;

			g_GlobalDataBlock->WriteRealData(dwIndex,E_AllTaskData,pTaskData,E_THREADTASKMGR_CLASS,eUseBlockWithOne);
			g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��վID: [ %d ]"),pTaskData->dwWebID);
		
		
			T_TaskDesc *pTaskDesc = NULL;
			_NEW(pTaskDesc,T_TaskDesc);

			pTaskDesc->Data = pTaskData;
			pTaskDesc->dwTaskIndex = dwIndex;
			pTaskDesc->TaskState = e_dataprepare;

			m_TaskDescMap.insert(Task_Pair(dwIndex,pTaskDesc));
			PostMessage(E_DATAPRE_THREAD,E_PPD_START_MSG,dwIndex);//��������׼��
		}
		Sleep(50);
		m_mapTaskCount[pLocalGlobalData->dwTaskType]=iterType->second->size(); //  pTaskItem[pLocalGlobalData->dwTaskType].size();    //��������������վ����
	}
	g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("�ֽ�������[%d] "),m_TaskDescMap.size());

	return (DWORD)m_TaskDescMap.size();
}

DWORD CTaskManger::TaskResult(DWORD dwTaskIndex)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	T_TaskData *pTaskData = NULL;

	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwTaskIndex, 
		E_Save_CLASS, eUseCommReadWithMulti);
	//����ע������������ִ�������ѯ��
	if (pTaskData->eTaskType == ETTypeRegaccount)
	{
		m_mapTaskCount[ETTypeRegaccount] --;
	}
	else if (pTaskData->eTaskType == ETTypeReRegaccount)
	{
		m_mapTaskCount[ETTypeReRegaccount] --;
	}
	

	if(pTaskDesc!=NULL)
	{
		m_TaskDoingCount --;

		pTaskDesc->TaskState = e_save;
		PostMessage(E_SAVE_THREAD,E_SAVE_TASKFINISHED,dwTaskIndex);
	}

	return 0;
}


DWORD CTaskManger::TaskFialed(DWORD dwTaskIndex)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	T_TaskData *pTaskData = NULL;

	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwTaskIndex, 
		E_Save_CLASS, eUseCommReadWithMulti);
	//����ע������������ִ�������ѯ��

	if (pTaskData->eTaskType == ETTypeRegaccount)
	{
		m_mapTaskCount[ETTypeRegaccount] --;
	}
	else if (pTaskData->eTaskType == ETTypeReRegaccount)
	{
		m_mapTaskCount[ETTypeReRegaccount] --;
	}

	if(pTaskDesc!=NULL)
	{
		SetTaskState(dwTaskIndex,e_fialed);
	}
	return 0;
}
DWORD CTaskManger::TaskSucceed(DWORD dwTaskIndex)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	if(pTaskDesc!=NULL)
	{
		SetTaskState(dwTaskIndex,e_succeed);		
	}
	return 0;
}

//���û�з���������ɾ�������ļ��У�

void CTaskManger::DelPhotoPageFolder()
{
	//�ļ���ֱ����CStringʱ����־����Ч��������ת��TCHAR
	//һ����Һ��û�������Ҫ�õ�����
	if (m_mapTaskCount[ETTypePost]>0 || 
		m_mapTaskCount[ETTypeUserInfoMigrate] > 0 ||
		m_mapTaskCount[ETTypeGetPdtList] > 0)
	{
		return;
	}
	CStdString strFolder = _T("");
	g_Config->ReadString(_T("RESULTPATH"),_T("savepath"),strFolder);

	TCHAR tszPath[MAX_PATH] = {0};
	_sntprintf(tszPath, _TRUNCATE, _T("%s"), strFolder.GetBuffer());

	SHFILEOPSTRUCT op;
	memset(&op, 0, sizeof(SHFILEOPSTRUCT));
	op.wFunc = FO_DELETE;
	op.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	op.pFrom = tszPath;            
	op.pTo = NULL;
	op.lpszProgressTitle = NULL;

	if (0 != SHFileOperation(&op))
	{
		g_tskLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("ɾ���ļ���%sʧ�ܣ� err: %d"), tszPath, GetLastError());
	}
}