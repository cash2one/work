// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#pragma  comment(lib,"ZipEnCode.lib")

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
//#define _AFXDLL
// Windows 头文件:
#include <afxwin.h>         // MFC 核心组件和标准组件
//#include <windows.h>
//MFC支持
//#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#include <winbase.h>


#include <atlbase.h>
#include <tchar.h>
#include <list>
#include <deque>
#include <map>
#include <vector>

#include "StdString.h"
#include "stringutils.h"
#include "trace.h"
#include "common_enum.h"
#include "common_define.h"
#include "common_stru.h"
#include "common_stru_exserver.h"
#include "SiteInfo.h"
#include "IGlobalDataBlock.h"
#include "IServerSocket.h"
#include  "IXMLRW.h"


extern IGlobalDataBlock* g_GlobalDataBlock;
extern IXMLRW *g_Config;
extern CLogTrace g_sysLog;

extern CStdString strCity ;		//城市变量
extern CStdString strProv ;     //省份变量



enum E_PPD_ERR_TYPE
{
	PPD_SUCCESS=0,
	PPD_NETCODELESSZERO_ERR,
	PPD_NOWEBDATA,
};

struct TParamItem
{
	CStdString Key;
	CStdString Value;
	TParamItem(CStdString& k, CStdString& v):Key(k), Value(v)
	{};
	TParamItem(TCHAR* k, TCHAR* v):Key(k), Value(v)
	{};

	TParamItem()
	{
	};

	
};
//end add 20080922


//add 20080923
struct TRapalceDataMsg
{
	std::vector<TParamItem> tParamItemTable;
	//未替换通配符的参数
	std::vector<CStdString> vNoRepleaceParam;

	DWORD dNameSize;
	DWORD dPsdSize;
	UINT64 nNetItemCode;
	CStdString strInfoID;
	CStdString strNetCode;
	E_Account_Style eUserType;
	E_Password_Style ePsdType;
	E_PPD_ERR_TYPE eErrType;//错误返回类型判断依据
	E_TASK_TYPE eTaskType;

	DWORD dwNetLanType;
	CStdString strWebName;
	CStdString strWebPass;
	CStdString strDefaultName;
	CStdString strDefaultPass;


	TRapalceDataMsg()
	{
		dNameSize = 8;  //默认帐号大小
		dPsdSize = 8;  //默认密码大小
		nNetItemCode = 0;
		strInfoID = _T("");

		eUserType = asNOT_INCLUDE_CHINESE;
		ePsdType = psNO_INCLUDE_CHINESE;
		eErrType = PPD_SUCCESS;//错误返回类型判断依据

		dwNetLanType = 0;
		eTaskType = ETTypeNone;
	};
};

enum E_PPD_TRIM_TYPE
{
	PPD_BBodyC_TYPE=0,
	PPD_BBodyE_TYPE,
	PPD_BBodyLen_TYPE,
	PPD_WWebsH_TYPE,
	PPD_BBodyCI_TYPE,
	PPD_BBodyP_TYPE,
	PPD_BBodySC_TYPE,
	PPD_BBodySCW_TYPE,
};

/* 输入:
strInStr: 输入的字符串
strOutStr: 输出的字符串
eType: 限制的类型,目前只有三种(E_LimtedLen(限制长度), E_CutAimStr(去掉目标字串), E_BothType(前二者同时都要做的))
strCutStr: 将要去掉的字串
nLimtLen: 限制的长度

返回值: 0成功,其它失败
*/
DWORD LimitedStr(const CStdString &strInStr, CStdString &strOutStr, 
                 E_PPD_TRIM_TYPE eType, CStdString strCutStr, int nLimtLen = 0);



