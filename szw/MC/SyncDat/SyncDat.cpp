// ISaveModule.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "UpdateThread.h"
#include "IThreadUnit.h"



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) IThreadUnit * CreateIThread(IGlobalDataBlock* pFindDataByID,
															 IServerSocket *pSocket,
															 IXMLRW *pConfig)
{

	g_GlobalDataBlock = pFindDataByID;
	g_ServerSocket = pSocket;
	g_Config = pConfig;
	IThreadUnit * task = new CUpdateThread();	

	printf("IUpdateThread Create\n");
	return task;
}