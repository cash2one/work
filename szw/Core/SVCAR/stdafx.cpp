// stdafx.cpp : source file that includes just the standard includes
// SVCAR.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CLogTrace g_SvcarLog(_T("SVCAR.log"), NULL);
CLogTrace g_ResultLog(_T("SVCAR_RESULT.log"), NULL);

IRecognize* g_pRecognize = NULL;

ICodeOCR* g_pCodeORC = NULL;
bool g_bCodeInit = false;

int g_iType = 0;

CLock g_CodeInitLock;
CLock g_UUinitLock;

int GetCodeType()
{
	TCHAR szFilePath[MAX_PATH] = {0};

	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	PathAppend(szFilePath, _T("..\\..\\"));
	PathAppend(szFilePath, _T("data2\\mc.dat"));

	int itype = 0;
	IXMLRW xmlcfg;
	xmlcfg.init(szFilePath);
	xmlcfg.ReadInt(_T("MC"), _T("OCRDATA"), _T("codetype"), itype);

	return itype;
	//return (int)GetPrivateProfileInt(_T("OCRDATA"), _T("codetype"), 0, szFilePath);
}

bool InitCodeORC()
{
	g_CodeInitLock.Lock();
	if (!g_bCodeInit)
	{
		HINSTANCE histOCR = LoadLibrary(_T("CodeOCR.dll"));
		if (histOCR == NULL)
		{
			g_SvcarLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("加载CodeOCR.dll失败,LastErrorCode:%d"),GetLastError());
			return false;
		}
		CreateCodeOCR procCodeORC;
		procCodeORC = (CreateCodeOCR)GetProcAddress(histOCR, "CreateCodeOCR");
		if (procCodeORC == NULL)
		{
			g_SvcarLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("获取函数地址失败"));
			return false;
		}
		GUID abc;
		g_pCodeORC = procCodeORC(abc, 5);
		if (g_pCodeORC == NULL)
		{
			g_SvcarLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("调用CreateCodeOCR函数地址失败"));
			return false;
		}
		TCHAR szFilePath[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		PathAppend(szFilePath, _T("..\\..\\"));
		PathAppend(szFilePath, _T("mod"));
		g_pCodeORC->SetModFilePath(szFilePath);
		g_bCodeInit = true;
	}
	g_CodeInitLock.Unlock();
	return true;
}

BOOL APIENTRY DllMain( 
	HANDLE hModule, 
	DWORD  ul_reason_for_call, 
	LPVOID lpReserved
	)
{
	switch( ul_reason_for_call ) 
	{
	case DLL_PROCESS_ATTACH:
		g_iType = GetCodeType();
		g_pRecognize = CRecogFactory::Create(g_iType);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		if(g_pRecognize != NULL)
		{
			delete g_pRecognize;
			g_pRecognize = NULL;
		}
		if (g_pCodeORC != NULL)
		{
			g_pCodeORC->Release();
			g_pCodeORC = NULL;
		}
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


// strImgPath 为图片的绝对路径 strResult为识别结果
extern "C" LONG __declspec(dllexport) Recognize(LPCTSTR strImgPath, LPTSTR strResult)
{	
	CLocalLock lock(&g_UUinitLock);

	if(g_pRecognize != NULL)
	{
		return g_pRecognize->Recognize(strImgPath, strResult);
	}
	return ERROR_NULL;
}

// nResultCode 参数为函数Recognize的返回值
extern "C" LONG __declspec(dllexport) ReportError(LONG nResultCode)
{	
	CLocalLock lock(&g_UUinitLock);

	if(g_pRecognize != NULL)
	{
		return g_pRecognize->ReportError(nResultCode);
	}
	return ERROR_NULL;
}

// strRecegiseParm 自动、参数或特殊识别所用到的参数列表 strImgPath 为图片的绝对路径 strResult为识别结果
// 返回值DWORD 识别成功返回1失败返回0
extern "C" DWORD __declspec(dllexport) CodeRecognize(CString strRecegiseParm, CString strImgPath, CString& strResult)
{
	DWORD dwRet = 0;
	if (InitCodeORC())
	{
		if (g_pCodeORC != NULL)
		{
			g_pCodeORC->OpenImageFile(strImgPath);

			CString strPreProcess;

			dwRet = g_pCodeORC->Recognising(strRecegiseParm, strResult, strPreProcess);
		}
	}
	if (dwRet == 0)
	{
		strResult = _T("");
	}
	return dwRet;
}