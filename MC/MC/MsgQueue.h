// MsgQueue.h: interface for the CMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGQUEUE_H__2E162580_810B_4C9B_B614_21C9D8BC0AE6__INCLUDED_)
#define AFX_MSGQUEUE_H__2E162580_810B_4C9B_B614_21C9D8BC0AE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMsgQueue.h"

class CMsgQueue  : public IMsgQueue
{
private:
	std::deque<T_Message*> m_MsgQueue;
	CRITICAL_SECTION m_cs;
public:
	CMsgQueue();
	virtual ~CMsgQueue();
private:
	void lock(); //消息队列锁定
	void unlock(); //释放消息队列锁定
	void PostBackMsg(T_Message *pMsg); //从尾部压入消息队列
	void PostFrontMsg(T_Message *pMsg); //从头部压入消息队列(用于时间消息)
	void PostMsg(DWORD dwSourWork,DWORD dwDestWork,
					DWORD dwMsgParam, DWORD dwData,
						DWORD dwMsgType,HANDLE hSynHandle,DWORD dwLevel);//压入消息
	T_Message *GetMsg(DWORD dwDestWork); //检测当前消息类型，并返回
 	bool Empty();  //判断是否为空
	unsigned int size();	//消息数量
};

#endif // !defined(AFX_MSGQUEUE_H__2E162580_810B_4C9B_B614_21C9D8BC0AE6__INCLUDED_)
