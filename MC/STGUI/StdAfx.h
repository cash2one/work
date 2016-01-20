// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C23925D2_65F3_4B00_8B4F_21CA0EBCF500__INCLUDED_)
#define AFX_STDAFX_H__C23925D2_65F3_4B00_8B4F_21CA0EBCF500__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma  comment(lib,"ZipEnCode.lib")

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _USING_V110_SDK71_   //兼容XP时，定义（不兼容去掉）

#include <windows.h>

// TODO: reference additional headers your program requires here
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
#include "IConfigInfo.h"

extern CLogTrace g_log;

extern IGlobalDataBlock* g_GlobalDataBlock;

extern IServerSocket *g_ServerSocket;

extern IConfigInfo *g_Config;

//#define cjhdebug
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C23925D2_65F3_4B00_8B4F_21CA0EBCF500__INCLUDED_)
