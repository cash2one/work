// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <winbase.h>
#include <atlbase.h>
#include <tchar.h>
#include <list>
#include <deque>
#include <map>
#include <vector>
#include <atlstr.h>

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

extern CLogTrace g_tskLog;
extern IGlobalDataBlock *g_GlobalDataBlock;
extern IServerSocket *g_Socket;
extern IConfigInfo *g_Config;


// TODO: 在此处引用程序要求的附加头文件
//#define cjhtest   //测试专用
//#define cjhdebug
