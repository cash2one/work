// stdafx.cpp : source file that includes just the standard includes
//	SaveModule.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
CLogTrace g_log(_T("STGUI.log"), NULL);

IServerSocket *g_ServerSocket = NULL;
IGlobalDataBlock *g_GlobalDataBlock = NULL;
IConfigInfo *g_Config = NULL;
