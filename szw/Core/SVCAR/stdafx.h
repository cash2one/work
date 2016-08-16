// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once



#define WIN32_LEAN_AND_MEAN	

#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <afxmt.h>
#include <afxinet.h>
#include "Trace.h"
#include "DES.h"
#include "cdef.h"
#include "Lock.h"
#include "yundamaAPI/yundamaAPI.h"
#include "IRecognize.h"
#include "UUColdRecognize.h"
#include "YDMRecognize.h"
#include "CRecogFactory.h"
#include "ICodeOCR.h"
#include "IXMLRW.h"

#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "yundamaAPI/yundamaAPI.lib")

// 指针空
#define  ERROR_NULL  -123456789

extern CLogTrace g_SvcarLog;
extern CLogTrace g_ResultLog;

// 识别类指针
extern IRecognize* g_pRecognize;

extern ICodeOCR* g_pCodeORC;

extern bool g_bCodeInit;

// 识别类型
extern int g_iType;

// 获取打码类型
int GetCodeType();

// 初始化打码
bool InitCodeORC();

extern CLock g_CodeInitLock;
extern CLock g_UUinitLock;
// TODO: reference additional headers your program requires here

typedef LONG(*ProcRecognize)(LPCTSTR strImgPath, LPTSTR strResult);
typedef LONG(*ProcReportError)(LONG nResultCode);
typedef DWORD(*ProcCodeRecongnize)(CString strRecegiseParm, CString strImgPath, CString& strResult);
typedef ICodeOCR* (*CreateCodeOCR)(const GUID &, DWORD);

