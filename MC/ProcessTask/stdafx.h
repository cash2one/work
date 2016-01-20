// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "common_enum.h"
#include "common_define.h"
#include "common_stru.h"
#include "stdstring.h"
#include "IServerSocket.h"
#include "IGlobalDataBlock.h"
#include "IConfigInfo.h"
#include "Trace.h"
#include "structdef.h"



extern CLogTrace g_log;
extern IGlobalDataBlock* g_GlobalDataBlock;
extern IConfigInfo *g_Config;
extern IServerSocket *g_ServerSocket;


// LPSTR WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize);
// LPWSTR MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize);


//add 20150601
struct CommonParam
{
	DWORD dwLoopPdtCnt;			//产品数量
	DWORD dwCurPdtCnt;			//当前产品数；
	DWORD dwTaskType;			//功能类型

	DWORD dwEmbedID;			//邮箱类型（126还是sina);
	
	int	  iFunCount;		//总步骤数
	CStdString strPreCompName;  //上一个公司名称
	CStdString strSendDataBak;  //备份发送给内核的数据，方便后期提取产品替换
	std::vector<CStdString> vPdtLists;  //一键搬家提取的产品标题；

	BOOL bIsObject;  //是否Object 网站
	CommonParam()
	{
		dwLoopPdtCnt = 1;
		dwCurPdtCnt = 0;
		dwTaskType = 0;
		iFunCount = 0;
		dwEmbedID = 0;
		strPreCompName = _T("");
		strSendDataBak = _T("");
		vPdtLists.clear();
		bIsObject = FALSE;
	};
};
