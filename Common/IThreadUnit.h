// IThreadUnit.h: interface for the IThreadUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITHREADUNIT_H__F84C5776_9989_4C7A_A8BF_57769BC5A93E__INCLUDED_)
#define AFX_ITHREADUNIT_H__F84C5776_9989_4C7A_A8BF_57769BC5A93E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMsgQueue.h"
#include "IThreadManage.h"

class CThreadManage;

#define MSG_TIMER 0xFFFE	//��ʱ����Ϣ
#define MSG_RESETTHREAD 0xFFFD //�߳�����
 

class IThreadUnit  
{
	friend class CThreadManage;
private:
	BOOL m_isRest;	//is Rest Thread
	T_Message m_CurMsg; //���ڴ���ĵ�ǰ��Ϣ
	IMsgQueue *m_MsgQueue; //��Ϣ����ָ��
	DWORD m_dwThreadType;	//��Ϣ���ͣ����������̹߳���
	DWORD m_dwTimeOut; //�̳߳�ʱʱ��
	HANDLE m_hHandle;	//�߳̾��
	HANDLE m_synMsgHandle; //����ͬ����Ϣ���
	DWORD ExitFlag;		// �˳���־
	BOOL m_Busy; //��־���ΪTRUE,��ô���������̣߳�ΪFALSE���������
	//�߳���ڵ�
	static DWORD WINAPI StaticThreadFunc(LPVOID lpParam);
	//�����߳�
	DWORD Create(IMsgQueue *pIMsgQueue);
	BOOL Terminated();	//��ֹ�߳�
	IThreadManage *m_ThreadManage;
public:
	void PostMessage(DWORD dwDestWork,DWORD dwMsgParam, DWORD dwData,DWORD dwLevel=0); //�첽��Ϣ
	void SendMessage(DWORD dwDestWork,DWORD dwMsgParam, DWORD dwData); //ͬ����Ϣ
	void Lock(){m_Busy=TRUE;}//�����̲߳���������
	void UnLock() //�����������������
	{
		m_CurMsg.EnterTime = ::GetTickCount();	//�޸ĳ���ʱ��
		m_Busy=FALSE;
	}
	void ChangeTimeOutValue(DWORD dwTimeOut)	//����ʱ�޸ĳ�ʱֵ��Ŀǰֻ��ExSerʹ��
	{
		m_dwTimeOut = dwTimeOut;
	}
public:
	IThreadUnit(DWORD dwThreadType=0xFFFF,DWORD dwTimeOut=0xFFFF); //�߳����͡���ʱʱ������Ϊ��λ
	virtual ~IThreadUnit();
	//�̳߳�ʼ��
	virtual DWORD CreateThread() = 0;
	//�����߳�
	virtual DWORD RestThread() {return 0;}
	//�߳��˳�
	virtual DWORD DestoryThread() = 0;
	//�߳�����
	virtual DWORD DispatchMessage(T_Message *Msg) = 0;
	//Ĭ���̴߳���
	virtual DWORD DefaultProc(){return 0;}

};

#endif // !defined(AFX_ITHREADUNIT_H__F84C5776_9989_4C7A_A8BF_57769BC5A93E__INCLUDED_)
