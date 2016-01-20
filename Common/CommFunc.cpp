#include "stdafx.h"
#include "CommFunc.h"
#include <shlobj.h>
#include <Windows.h>

TCHAR *GetCurPath(void)
{
	static TCHAR s_szCurPath[MAX_PATH] = {0};

	GetModuleFileName(NULL, s_szCurPath, MAX_PATH);

	TCHAR *p = _tcsrchr(s_szCurPath, '\\');
	*p = 0;

	return s_szCurPath;
}

TCHAR *GetProgPath(void)
{
	static TCHAR s_szProgPath[MAX_PATH] = {0};

	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 1, s_szProgPath);

	return s_szProgPath;
}

TCHAR *GetInstallPath(void)
{
	static TCHAR s_szCfgPath[MAX_PATH] = {0};

	GetModuleFileName(NULL, s_szCfgPath, MAX_PATH);

	TCHAR *p = _tcsrchr(s_szCfgPath, '\\');
	if (p != NULL)
	{
		*p = 0;
	}

	p = _tcsrchr(s_szCfgPath, '\\');
	if (p != NULL)
	{
		*p = 0;
	}

	return s_szCfgPath;
}

// 多字节转宽字节，长度不足时返回需要的长度
LPWSTR MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize, UINT codePage)
{
	DWORD dwMinSize;

 	dwMinSize = MultiByteToWideChar(codePage,NULL,lpcszStr,-1,NULL,0);
 	if(*dwSize < dwMinSize || lpwszStr == NULL)
 	{
		*dwSize = dwMinSize;
 		return NULL;
 	}
 
 	MultiByteToWideChar(codePage,NULL,lpcszStr,-1,lpwszStr,*dwSize);

	*dwSize = dwMinSize;
	return lpwszStr;
}

LPSTR WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize, UINT codePage)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(codePage,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
	if(*dwSize < dwMinSize)
	{
		*dwSize = dwMinSize;
		return NULL;
	}
	WideCharToMultiByte(codePage,NULL,lpcwszStr,-1,lpszStr,*dwSize,NULL,FALSE);
	*dwSize = dwMinSize;
	return lpszStr;
}

//比较两个BSTR类型的字符串
int CompareBSTR(_bstr_t bstr1, _bstr_t bstr2)
{
	if (bstr1 == bstr2)
		return 0;
	else if (bstr1 > bstr2)
		return 1;
	else
		return -1;
}

//比较两个BSTR类型的字符串（忽略大小写）
int CompareBSTRNoCase(_bstr_t bstr1, _bstr_t bstr2)
{
	CComBSTR comBstr1(bstr1.GetBSTR());
	CComBSTR comBstr2(bstr2.GetBSTR());

	comBstr1.ToLower();
	comBstr2.ToLower();

	if (comBstr1 == comBstr2)
		return 0;
	else if (comBstr1 > comBstr2)
		return 1;
	else
		return -1;

}


LONG GetGlobalIndexNum(void)
{
	static LONG lngBase = 0;

	return InterlockedIncrement(&lngBase);
}