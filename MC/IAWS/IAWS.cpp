// WebServiceEx.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "WebService.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" __declspec(dllexport) IThreadUnit * CreateIThread(IGlobalDataBlock* pFindDataByID,
															 IServerSocket *pSocket,
															 IConfigInfo *pConfig)
{
	g_GlobalDataBlock = pFindDataByID;
	g_ServerSocket = pSocket;
	g_Config = pConfig;
	IThreadUnit *p = new CWebService();
 
	 return p;
}