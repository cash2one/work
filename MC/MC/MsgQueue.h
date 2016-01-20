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
	void lock(); //��Ϣ��������
	void unlock(); //�ͷ���Ϣ��������
	void PostBackMsg(T_Message *pMsg); //��β��ѹ����Ϣ����
	void PostFrontMsg(T_Message *pMsg); //��ͷ��ѹ����Ϣ����(����ʱ����Ϣ)
	void PostMsg(DWORD dwSourWork,DWORD dwDestWork,
					DWORD dwMsgParam, DWORD dwData,
						DWORD dwMsgType,HANDLE hSynHandle,DWORD dwLevel);//ѹ����Ϣ
	T_Message *GetMsg(DWORD dwDestWork); //��⵱ǰ��Ϣ���ͣ�������
 	bool Empty();  //�ж��Ƿ�Ϊ��
	unsigned int size();	//��Ϣ����
};

#endif // !defined(AFX_MSGQUEUE_H__2E162580_810B_4C9B_B614_21C9D8BC0AE6__INCLUDED_)
