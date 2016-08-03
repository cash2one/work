// IThreadUnit.cpp: implementation of the IThreadUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IThreadUnit.h"
#include "IThreadManage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IThreadUnit::IThreadUnit(DWORD dwThreadType,DWORD dwTimeOut)
{
	m_dwThreadType = dwThreadType;
	m_dwTimeOut = dwTimeOut;
	m_isRest = FALSE;
	m_Busy = FALSE;
}

IThreadUnit::~IThreadUnit()
{
	
}


DWORD IThreadUnit::Create(IMsgQueue *pIMsgQueue)
{
	m_CurMsg.EnterTime = 0xFFFFFFFF; //0xFFFFFFFF时间表示没有消息
	ExitFlag = TRUE;	//表示运行
	m_MsgQueue = pIMsgQueue;
	DWORD dwThreadID = 0;
	
	//创建用于同步消息事件句柄
	m_synMsgHandle = ::CreateEvent(NULL,TRUE,TRUE,NULL);

	m_hHandle = ::CreateThread(NULL, 0, 
		StaticThreadFunc, 
		this, 0, &dwThreadID);
	
    if (!m_hHandle || !dwThreadID) //线程创建失败
    {
		::MessageBoxW(NULL,_T("创建线程失败"),_T("IThreadUnit Error"),MB_OK);
		return 1;
    }

	return 0;
}

BOOL IThreadUnit::Terminated()
{
	ExitFlag = FALSE;
	if(WaitForSingleObject(m_hHandle,200)==WAIT_TIMEOUT)
	{
		m_CurMsg.EnterTime = 0xFFFFFFFF; 
		//线程停止超时 强制停止
		if(TerminateThread(m_hHandle,0)==FALSE)
			return FALSE;
	}
	CloseHandle(m_hHandle);

	//释放用于同步消息事件句柄
	SetEvent(m_synMsgHandle);
	CloseHandle(m_synMsgHandle);

	return TRUE;
}

DWORD WINAPI IThreadUnit::StaticThreadFunc(LPVOID lpParam)
{
	CoInitialize(NULL);
	IThreadUnit * pThread = (IThreadUnit *)lpParam;
    if (pThread)
    {
		if(pThread->m_isRest == FALSE)
			pThread->CreateThread();
		else
			pThread->RestThread();
		while(pThread->ExitFlag)
		{
			pThread->DefaultProc();
			if(pThread->m_dwThreadType == 0xFFFF) //线程单元基类无法使用
				return 0;
			//默认一个线程处理5条消息后进行一次线程切换
			for(int i=0;i<3;i++)
			{
				pThread->m_MsgQueue->lock();
				T_Message *pMsg = pThread->m_MsgQueue->GetMsg(pThread->m_dwThreadType);
				pThread->m_MsgQueue->unlock();
				if(pMsg!=NULL)
				{
					::CopyMemory(&(pThread->m_CurMsg),pMsg,sizeof(T_Message));
					 
					DWORD dwRet = pThread->DispatchMessage(pMsg); //对消息进行分流处理
		 
					if(pMsg->dwMsgType == 1)
						::SetEvent(pMsg->hSynHandle);//同步消息，处理事件对象

					if(dwRet == -1) //返回-1，表示没有处理消息，再将消息返回到队列中
					{
						pThread->m_MsgQueue->lock();
						pThread->m_MsgQueue->PostFrontMsg(pMsg);
						pThread->m_MsgQueue->unlock();
					}
					else
					{
						pThread->m_CurMsg.EnterTime = 0xFFFFFFFF; //0xFFFFFFFF时间表示没有消息
						IMsgQueue::Free_Message(pMsg); //处理完了之后进行释放
					}

				}
				else	//没有消息让系统切换线程
				{
					break;
				}
			}
			Sleep(60);
		}
		pThread->DestoryThread();
    } 
	CoUninitialize ();
	return 0;
}
//异步消息发送
void IThreadUnit::PostMessage(DWORD dwDestWork,DWORD dwMsgParam, DWORD dwData,DWORD dwLevel)
{
	m_ThreadManage->PostMessage(m_dwThreadType,dwDestWork,dwMsgParam,dwData,0,0,dwLevel);
}
//同步消息发送
void IThreadUnit::SendMessage(DWORD dwDestWork,DWORD dwMsgParam, DWORD dwData)
{
	::ResetEvent(m_synMsgHandle);
	m_ThreadManage->PostMessage(m_dwThreadType,dwDestWork,dwMsgParam,dwData,1,m_synMsgHandle,1);//1级消息
	::WaitForSingleObject(m_synMsgHandle,INFINITE); //同步等待
}