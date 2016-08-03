// ThreadManage.cpp: implementation of the CThreadManage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThreadManage.h"
#include "IThreadUnit.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static UINT TIMER_PERIOD = 6*1000; //定时器的时间间隔(线程超时)
static UINT TASK_TIME_OUT = 2*1000; //线程超时

 

CThreadManage::CThreadManage()
{
	
}

CThreadManage::~CThreadManage()
{

}

//线程入口点
DWORD WINAPI CThreadManage::StaticThreadFunc(LPVOID lpParam)
{
	CThreadManage * pThread = (CThreadManage *)lpParam;
    
    if (pThread)
    {
        return pThread->Run();
    } 
	return 0;
}

//线程运行
DWORD CThreadManage::Run()
{
	while (m_ExitFlag)
	{
		/*::WaitForSingleObject(m_hTimer,INFINITE);*/
		Sleep(6 * 1000);
		CheckThreadState();	//检测线程状态
	}
	return 0;
}

DWORD CThreadManage::Create()
{
	m_ExitFlag = TRUE;
	m_PrintLog = TRUE;
/*
	m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);	
    if (NULL == m_hTimer)
    {
		return 1;
    }
    LARGE_INTEGER DueTimeOther;
    DueTimeOther.QuadPart = 0;
    if (!SetWaitableTimer(m_hTimer, &DueTimeOther, TIMER_PERIOD,
        NULL, NULL, TRUE))
	{
		return 2;
	}*/
	DWORD dwThreadID = 0;
	m_hHandle = ::CreateThread(NULL, 0, 
					StaticThreadFunc, 
                    this, 0, &dwThreadID);

    if (!m_hHandle || !dwThreadID) //线程创建失败
    {
		return 1;
    }

	return 0;
}

DWORD CThreadManage::Destory()
{
	m_PrintLog = FALSE;
	m_ExitFlag = FALSE;
	if(WaitForSingleObject(m_hHandle,200)==WAIT_TIMEOUT)
	{
		//线程停止超时 强制停止
		if(TerminateThread(m_hHandle,0)==FALSE)
			return -1;
	}
	CloseHandle(m_hHandle);

	std::list<IThreadUnit*>::iterator IterThread;
	for(IterThread=m_listThread.begin();
			IterThread!=m_listThread.end();IterThread++)
	{
		IThreadUnit *pThread = (*IterThread);
		BOOL bRet = pThread->Terminated(); //终止
		if(bRet==FALSE)printf("%d 终止失败\n",pThread->m_dwThreadType);
		printf("%d 终止成功\n",pThread->m_dwThreadType);
		delete pThread;
	}
	m_listThread.clear();

	return 0;
}

void CThreadManage::CheckThreadState()
{
	if (m_PrintLog)
	{
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
			_T("%d 检测线程状态 线程数量(%d)\n"),GetCurrentThreadId() ,m_listThread.size());
		m_PrintLog = FALSE;
	}
	std::list<IThreadUnit*>::iterator IterThread;
	for(IterThread=m_listThread.begin();
			IterThread!=m_listThread.end();IterThread++)
	{

		IThreadUnit *pThread = (*IterThread);
		//线程超时
		DWORD dwCurTime = ::GetTickCount();
		if(pThread->m_CurMsg.EnterTime!=0xFFFFFFFF) //线程当前没有处理消息
		{
			if(pThread->m_dwTimeOut!=0xFFFF)
			{
				if( dwCurTime - pThread->m_CurMsg.EnterTime > pThread->m_dwTimeOut*1000 && pThread->m_Busy == FALSE)
				{
				g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,\
 					_T("%d线程消息处理消息%d发过来的(%d)超时,终止后重启 %d\n"),
 					pThread->m_CurMsg.dwSourWork,
 					pThread->m_CurMsg.dwDestWork,
 					pThread->m_CurMsg.dwMsgParam,
 					pThread->m_dwThreadType);
 					printf("%d线程消息处理消息%d发过来的(%d)超时,终止后重启 %d\n",
 						pThread->m_CurMsg.dwSourWork,
 						pThread->m_CurMsg.dwDestWork,
 						pThread->m_CurMsg.dwMsgParam,
 						pThread->m_dwThreadType);
					//broadcastMsg(pThread->m_CurMsg.dwData,MSG_RESETTHREAD,pThread->m_dwThreadType);
					PostMessage(pThread->m_CurMsg.dwDestWork,pThread->m_CurMsg.dwSourWork,MSG_RESETTHREAD,pThread->m_CurMsg.dwData,0,0,1);
					pThread->m_isRest = TRUE; 
					pThread->Terminated(); //终止
					
					pThread->Create(m_MsgQueueMap[pThread->m_dwThreadType]);

					//pThread->PostMessage(pThread->m_CurMsg.dwDestWork,pThread->m_CurMsg.dwMsgParam,pThread->m_CurMsg.dwData);
				}

			}
		}
	}
	
//	broadcastMsg(0,MSG_TIMER,0);
//	printf("检测线程状态 线程数量(%d)\n",m_listThread.size());
	
}
void CThreadManage::broadcastMsg(DWORD dwMsgParamOne, DWORD dwMsgParam,DWORD dwData)
{
	std::map<DWORD,IMsgQueue*>::iterator IterMsgQueue;
	for(IterMsgQueue=m_MsgQueueMap.begin();
	IterMsgQueue!=m_MsgQueueMap.end();IterMsgQueue++)
	{
		PostMessage(dwMsgParamOne,IterMsgQueue->first,dwMsgParam,dwData,0,0,1);	//发送异步定时器消息
	}
}

//发送消息
void CThreadManage::PostMessage(DWORD dwSourWork,DWORD dwDestWork,
									DWORD dwMsgParam, DWORD dwData,
											DWORD dwMsgType,HANDLE hSynHandle,
												DWORD dwLevel)
{
	IMsgQueue *MsgQueue = m_MsgQueueMap[dwDestWork];
	if(MsgQueue == NULL) //队列为空
	{
		m_MsgQueueMap[dwDestWork] = new CMsgQueue;
    	MsgQueue = 	m_MsgQueueMap[dwDestWork] ;
	}
	MsgQueue->lock();
	MsgQueue->PostMsg(dwSourWork,dwDestWork,dwMsgParam,dwData,dwMsgType,hSynHandle,dwLevel);
	MsgQueue->unlock();
}

BOOL CThreadManage::InsertThread(IThreadUnit *pThread)
{

	m_listThread.push_back(pThread);
	if(pThread->m_dwThreadType == 0xFFFF) //不能创建此线程
	{
		return  FALSE;
	}
	if(m_MsgQueueMap[pThread->m_dwThreadType]==NULL)
	{
		m_MsgQueueMap[pThread->m_dwThreadType] = new CMsgQueue;
	}
	pThread->m_ThreadManage = this; 	
	pThread->Create(m_MsgQueueMap[pThread->m_dwThreadType]);
	return TRUE;
}

//删除工作线程
void CThreadManage::RemoveThread(IThreadUnit *pThread)
{
	pThread->Terminated();
	m_listThread.remove(pThread);
	delete pThread;
}

DWORD CThreadManage::DataIn(BYTE* pData, DWORD dwLen, DWORD dwFlag, void* pReserve)
{
	T_GUIData *pGuidata;
	_NEW(pGuidata,T_GUIData);
	pGuidata->dwFlag = dwFlag;
	pGuidata->dwLen = dwLen;
	_NEWA(pGuidata->pData,BYTE,dwLen);
	 
	memcpy(pGuidata->pData,pData,dwLen);
	DWORD dwIndex = g_GlobalDataBlock->GetGlobalID();
	
	g_GlobalDataBlock->WriteRealData(dwIndex,E_GUIData,pGuidata,E_THREADMANAGER_CLASS,eUseBlockWithOne);
	PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,dwIndex); //把任务压到TaskMgrThread

	return 0;
}