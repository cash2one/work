// stdafx.cpp : source file that includes just the standard includes
//	McProc.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
IGlobalDataBlock *g_GlobalDataBlock = NULL;
CServerSocket *g_server;
CLogTrace g_log(_T("MC.log"), NULL,TRUE);
CMCCtrl *g_MC = NULL;
IXMLRW *g_Config = NULL;
