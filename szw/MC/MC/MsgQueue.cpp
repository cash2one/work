// MsgQueue.cpp: implementation of the CMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsgQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgQueue::CMsgQueue()
{
	InitializeCriticalSection(&m_cs);
}

CMsgQueue::~CMsgQueue()
{
	DeleteCriticalSection(&m_cs);
}

void CMsgQueue::lock()
{
	EnterCriticalSection(&m_cs);
}
void CMsgQueue::unlock()
{
	LeaveCriticalSection(&m_cs);
}

void CMsgQueue::PostBackMsg(T_Message *pMsg)
{
	m_MsgQueue.push_back(pMsg);
}

void CMsgQueue::PostMsg(DWORD dwSourWork,DWORD dwDestWork,
							DWORD dwMsgParam, DWORD dwData,
								DWORD dwMsgType,HANDLE hSynHandle,DWORD dwLevel)
{
	T_Message *pMsg = IMsgQueue::New_Message();
	pMsg->dwSourWork = dwSourWork;
	pMsg->dwDestWork = dwDestWork;
	pMsg->dwMsgParam = dwMsgParam;
	pMsg->dwData = dwData;
	pMsg->dwMsgType = dwMsgType;
	pMsg->hSynHandle = hSynHandle;
	if(dwLevel==0)
		PostBackMsg(pMsg);
	else
		PostFrontMsg(pMsg);
}

void CMsgQueue::PostFrontMsg(T_Message *pMsg) 
{
		m_MsgQueue.push_front(pMsg);
}

T_Message *CMsgQueue::GetMsg(DWORD dwDestWork)
{
	T_Message *pMsg = NULL;
	if(m_MsgQueue.empty() == false)
	{
		pMsg = m_MsgQueue.front();
		if(pMsg->dwDestWork == dwDestWork)
		{
			m_MsgQueue.pop_front();
			pMsg->EnterTime = ::GetTickCount(); //消息出队列时间
			return pMsg;
		}
	}	
	return NULL;
}


bool CMsgQueue::Empty()
{
	bool bRet = m_MsgQueue.empty();
	return bRet;
}

unsigned int CMsgQueue::size()
{
	unsigned int iSize = m_MsgQueue.size();
	return iSize;
}
