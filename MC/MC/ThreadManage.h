// ThreadManage.h: interface for the CThreadManage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADMANAGE_H__8925966E_E55A_461C_8D2F_BE420AB5ACB4__INCLUDED_)
#define AFX_THREADMANAGE_H__8925966E_E55A_461C_8D2F_BE420AB5ACB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

 
#include "MsgQueue.h"
#include "IThreadManage.h"

class IThreadUnit;

class CThreadManage :public IThreadManage 
{
private:
	std::list<IThreadUnit*>m_listThread;
	std::map<DWORD,IMsgQueue*> m_MsgQueueMap; //��Ϣ����map
	//HANDLE m_hTimer; //��ʱ��
	HANDLE m_hHandle;	//�߳̾��
	BOOL m_ExitFlag;	//�˳���־
	BOOL m_PrintLog;   //��ӡ��־
	//�߳���ڵ�
	static DWORD WINAPI StaticThreadFunc(LPVOID lpParam);
	//����߳�״̬
	void CheckThreadState();
	//�㲥��Ϣ
	void broadcastMsg(DWORD dwMsgParamOne, DWORD dwMsgParam,DWORD dwData);
	//�߳�����
	DWORD Run();
public:
	CThreadManage();
	virtual ~CThreadManage();
	DWORD Create(); //����
	DWORD Destory(); //�ͷ�
	BOOL InsertThread(IThreadUnit *pThread); //�����߳�
	void RemoveThread(IThreadUnit *pThread); //ɾ���߳�
	void PostMessage(DWORD dwSourWork,DWORD dwDestWork,
						DWORD dwMsgParam, DWORD dwData,
							DWORD dwMsgType = 0,HANDLE hSynHandle = 0,
								DWORD dwLevel=0);//ѹ����Ϣ
public:
	DWORD DataIn(BYTE* pData, DWORD dwLen, DWORD dwFlag, void* pReserve=NULL);
};

#endif // !defined(AFX_THREADMANAGE_H__8925966E_E55A_461C_8D2F_BE420AB5ACB4__INCLUDED_)
