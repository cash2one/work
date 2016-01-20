#pragma once

class CBaseThread
{
public:
	CBaseThread(void);
	virtual ~CBaseThread(void);
	BOOL CreateThread();

private:
	DWORD m_dwTimeOut; //�̳߳�ʱʱ��
	HANDLE m_hHandle;	//�߳̾��
	HANDLE m_MsgHandle; //����ͬ����Ϣ���
	DWORD ExitFlag;		// �˳���־
	BOOL m_Busy; //��־���ΪTRUE,��ô���������̣߳�ΪFALSE���������

	
	static DWORD WINAPI StaticThreadFunc(LPVOID lpParam);

public:
	virtual BOOL InitInstance() = 0;
	virtual BOOL ExitInstance() = 0;
	virtual BOOL Run();
};
