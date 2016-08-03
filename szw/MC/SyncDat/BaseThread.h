#pragma once

class CBaseThread
{
public:
	CBaseThread(void);
	virtual ~CBaseThread(void);
	BOOL CreateThread();

private:
	DWORD m_dwTimeOut; //线程超时时间
	HANDLE m_hHandle;	//线程句柄
	HANDLE m_MsgHandle; //用于同步消息句柄
	DWORD ExitFlag;		// 退出标志
	BOOL m_Busy; //标志如果为TRUE,哪么，不重启线程，为FALSE否则才重启

	
	static DWORD WINAPI StaticThreadFunc(LPVOID lpParam);

public:
	virtual BOOL InitInstance() = 0;
	virtual BOOL ExitInstance() = 0;
	virtual BOOL Run();
};
