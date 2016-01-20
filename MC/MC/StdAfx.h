// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__17886CA8_0316_4BB6_8F5C_5A352C013DF6__INCLUDED_)
#define AFX_STDAFX_H__17886CA8_0316_4BB6_8F5C_5A352C013DF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define _WIN32_WINNT 0x0500
#pragma warning(disable:4786)


//#define _CAIBIAN     //采编模式需打开此宏定义

#if !defined _DEBUG && !defined _CAIBIAN
#pragma comment(linker, "/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")//去除console窗口
#endif

#include <windows.h>
#include <winbase.h>
#include <atlbase.h>
#include <tchar.h>
#include <list>
#include <deque>
#include <map>
#include <vector>


#include "stdstring.h"
#include "stringutils.h"
#include "trace.h"
#include "common_enum.h"
#include "common_define.h"
#include "common_stru.h"
#include "common_stru_exserver.h"
#include "IServerSocket.h"
#include "IGlobalDataBlock.h"
#include "ServerSocket.h"
#include "IConfigInfo.h"

#include "McCtrl.h"
// TODO: reference additional headers your program requires here
#pragma  comment(lib,"ZipEnCode.lib")

#include <string.h>
#include <tchar.h>
#include <iostream>
using namespace std;
#define DbgPrint(A) OutputDebugString(_T(A));

extern CLogTrace g_log;
extern IGlobalDataBlock *g_GlobalDataBlock;
extern CMCCtrl *g_MC;
extern CServerSocket *g_server;
extern IConfigInfo *g_Config;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__17886CA8_0316_4BB6_8F5C_5A352C013DF6__INCLUDED_)
