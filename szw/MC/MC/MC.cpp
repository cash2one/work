// McProc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ServerSocket.h"
#include "GlobalDataBlock.h"
#include <shlobj.h>
#include <Windows.h>
#include <locale.h>

#define MCPROC_RUN_MUTEX  _T("_WEB_MC_PROC_RUN_")

BOOL GetPath(TCHAR* szPath)
{
	if (szPath == NULL)
		return FALSE;

	if (GetModuleFileName(NULL,szPath,MAX_PATH))
	{
		PathAppend(szPath, _T("..\\..\\"));
		return TRUE;
	}
	return FALSE;
}

//初始化目录，若不存在则创建
void InitDir(void)
{
	TCHAR tszInstPath[MAX_PATH] = {0};
	GetPath(tszInstPath);

	const int nFolderNum = 4;
	TCHAR tszDir[nFolderNum][MAX_PATH] = {_T("data"), _T("log"), _T("6"),_T("log\\core")};

	for (int i = 0; i < nFolderNum; i++)
	{
		TCHAR tszTarDir[MAX_PATH] = {0};
		_stprintf(tszTarDir, _T("%s%s"), tszInstPath, tszDir[i]);

		SHCreateDirectoryEx(NULL, tszTarDir, NULL);
	}
}

/*
@brief  清理全局数据
*/
void CleanGlobalData()
{
	std::map<DWORD,T_SaveToServerAccount*> *pAccountPass = NULL;

	pAccountPass =
		(std::map<DWORD,T_SaveToServerAccount*>*)g_GlobalDataBlock->DeleteDataNode(
		MAG_ACCOUNT_INDEX,E_MAP_SaveAccountPass,E_Save_CLASS,eWithoutUse);
	if (pAccountPass != NULL)
	{
		std::map<DWORD,T_SaveToServerAccount*>::iterator it;

		for(it = pAccountPass->begin(); it != pAccountPass->end(); ++it)
		{
			if (it->second != NULL)
				delete it->second;
		}

		pAccountPass->clear();
		delete pAccountPass;
	}

	T_LocalGlobalData *pLocalGlobalData = NULL;

	pLocalGlobalData =
		(T_LocalGlobalData*)g_GlobalDataBlock->DeleteDataNode(
		LOCALGLOBALDATAINDEX,E_LocalGlobalData,E_GUIDATAIN_CLASS,eWithoutUse);
	if (pLocalGlobalData != NULL)
	{
		delete pLocalGlobalData;
	}

	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pNetStation = NULL;

	pNetStation =(std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *)g_GlobalDataBlock->DeleteDataNode(
		MAG_STATION_INDEX,E_MAP_NetStation,E_Save_CLASS,eWithoutUse);

	if (pNetStation != NULL)
	{
		std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*>::iterator it;
		for (it = pNetStation->begin(); it != pNetStation->end(); ++it)
		{
			std::map<DWORD,T_TaskNetItem*>::iterator iter;
			for (iter = it->second->begin(); iter != it->second->end(); ++iter)
			{
				if (iter->second != NULL)
				{
					delete iter->second;
				}

			}
			if (it->second != NULL)
				delete it->second;
		}

		pNetStation->clear();
		delete pNetStation;
	}
}

/*
@brief  初始化全局数据
*/
DWORD WriteInitGlobalData()
{
	T_LocalGlobalData *pLocalGlobalData = NULL;
	_NEW(pLocalGlobalData, T_LocalGlobalData);

	g_GlobalDataBlock->WriteRealData(LOCALGLOBALDATAINDEX,
		E_LocalGlobalData, pLocalGlobalData, E_GUIDATAIN_CLASS,eWithoutUse); 

	std::map<DWORD,T_SaveToServerAccount*> *pAccountPass = NULL;
	pAccountPass = new std::map<DWORD,T_SaveToServerAccount*>;
	if (pAccountPass)
	{
		g_GlobalDataBlock->WriteRealData(MAG_ACCOUNT_INDEX,	E_MAP_SaveAccountPass,pAccountPass, 
			E_Save_CLASS,eWithoutUse);               
	}

	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pNetStation = NULL;
	pNetStation = new std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*>;
	if (pNetStation)
	{
		g_GlobalDataBlock->WriteRealData(MAG_STATION_INDEX,	E_MAP_NetStation, pNetStation, 
			E_Save_CLASS,eWithoutUse);               
	}
	return 1;
}

DWORD MCServiceRun(int  argc, char  *argv[]) 
{ 
	g_log.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__,__LINE__,_T("Eneter MCServiceRun"));

	int nPort = 27015; //单机版管道号27015，WEB版管道号27016
	BOOL bIsYun = FALSE; //是否云任务推广
	TCHAR szPath[MAX_PATH] = {0};

	if (GetPath(szPath))
	{
		PathAppend(szPath, _T("data2\\mc.dat"));
	}

	InitDir();

	g_Config = new IXMLRW;
	g_Config->init(szPath);

#ifdef _DEBUG
	MessageBox(NULL, L"webMcProc entry", L"DEBUG", MB_OK);
#else
	int nDbgFlag = 0;
	g_Config->ReadInt(_T("MC"), _T("CONTROL"), _T("DEBUG"), nDbgFlag); //读出配置文件启动端口
	
	if (nDbgFlag == 1)
		MessageBox(NULL, L"webMcProc entry", L"DEBUG", MB_OK);
#endif

	g_Config->ReadInt(_T("MC"), _T("SERVICE"), _T("PORT"), nPort); //读出配置文件启动端口
	if(nPort==0)nPort = 27019;

	if(argc>=2)
	{
		nPort = atoi(argv[1]);
	}
	printf("端口 %d \n",nPort);

	if (argc >= 3)
	{
		CStringA strPara= argv[2];
		if (strPara.Compare("-y") == 0)
		{
			bIsYun = TRUE;
		}
	}

	g_GlobalDataBlock = new CGlobalDataBlock();	//创建全局数据区

	WriteInitGlobalData(); 

	g_MC = new CMCCtrl();
	g_server = new CServerSocket;

	if( g_server->Init(nPort) == -1 )
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR,__TFILE__,__LINE__,_T(" MCServiceRun 创建失败!端口号 %d",nPort));
		printf("创建失败\n");
		return 0;
	}

	g_MC->Start(bIsYun);

	g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("Enter MCServiceRun WaitRecData"));	
	g_server->WaitRecData(FALSE);
	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T(" OUT WaitRecData"));
	
	g_server->UnInit();
	g_MC->Stop();
	

	if (g_server)
		delete g_server;

	if (g_MC)
		delete g_MC;

	return(0); 
}

//普通方式控制台可结束
BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
	switch( fdwCtrlType ) 
	{ 
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT: 
	case CTRL_CLOSE_EVENT: 
		g_MC->Stop();
		g_server->UnInit();

		printf( "Ctrl-Close event\n\n" );
		return( FALSE ); 

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT: 

		printf( "Ctrl-Break event\n\n" );
		return FALSE; 

	case CTRL_LOGOFF_EVENT: 

		printf( "Ctrl-Logoff event\n\n" );
		return FALSE; 

	case CTRL_SHUTDOWN_EVENT: 

		printf( "Ctrl-Shutdown event\n\n" );
		return FALSE; 

	default: 
		return FALSE; 
	} 
} 

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // 设置入口地址 

int main(int argc, char* argv[])
{
	g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("Enter Main"));	
	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("szwWebMC"));
	setlocale(LC_ALL,"chs");   //设置区域选项
	
//	SetErrorMode(SEM_NOGPFAULTERRORBOX);  //屏蔽显示错误框
#ifndef _DEBUG 
	HINSTANCE histDbg = LoadLibrary(_T("BugReport.dll"));
   	if(histDbg == NULL)
   	{
   		::MessageBox(NULL,_T("装入BugReport出错！"),_T("请重新安装舟大师"),MB_OK|MB_ICONERROR);
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("load bugreport.dll failed,errcode:%d"), GetLastError());	
   		return 0;
   	}
#endif	

	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
	MCServiceRun(argc,argv);
	CloseHandle(hMutex);
	hMutex = NULL;
#ifndef _DEBUG
  	FreeLibrary(histDbg);
#endif
	
	return 0;
}

