// SaveModule.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SaveThread.h"
#include "../common/IThreadUnit.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


extern "C" __declspec(dllexport) IThreadUnit* CreateIThread(IGlobalDataBlock* pFindDataByID,IServerSocket *pSocket,IConfigInfo *pConfig)
{
	g_ServerSocket = pSocket;
	g_GlobalDataBlock = pFindDataByID;
	g_Config = pConfig;
	IThreadUnit * thread = new CSaveThread();	

	printf("SaveThread Create\n");
	return thread;
}
