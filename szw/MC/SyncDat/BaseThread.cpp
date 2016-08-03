#include "StdAfx.h"
#include ".\basethread.h"

CBaseThread::CBaseThread(void)
{
}

CBaseThread::~CBaseThread(void)
{
}

BOOL CBaseThread::CreateThread()
{
	DWORD dwThreadID = 0;

	//创建用于同步消息事件句柄
	m_MsgHandle = ::CreateEvent(NULL,TRUE,TRUE,NULL);

	m_hHandle = ::CreateThread(NULL, 0, StaticThreadFunc,this, 0, &dwThreadID);
	
    if (!m_hHandle || !dwThreadID) //线程创建失败
    {
		::MessageBoxW(NULL,_T("创建线程失败"),_T("CreateThread Error"),MB_OK);
		return FALSE;
    }

	return TRUE;
}
DWORD WINAPI CBaseThread::StaticThreadFunc(LPVOID lpParam)
{
	CoInitialize(NULL);
	CBaseThread * pThread = (CBaseThread *)lpParam;
    if (pThread)
    {
		pThread->InitInstance();
		pThread->Run();
    } 
	CoUninitialize ();
	return 0;
}
BOOL CBaseThread::Run()
{
	for(;;)
	{
		Sleep(200);
	}
	return TRUE;
}