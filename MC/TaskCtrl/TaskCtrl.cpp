// TaskMgr.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "TaskCtrlThread.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) IThreadUnit * CreateIThread(IGlobalDataBlock* pGlobalDataBlock,
															 IServerSocket *pSocket,
															 IConfigInfo *pConfig)
{
	 g_GlobalDataBlock = pGlobalDataBlock;
	 g_Socket = pSocket;
	 g_Config = pConfig;
	 IThreadUnit *pThread = new CTaskMgrThread();
	 return pThread;
}