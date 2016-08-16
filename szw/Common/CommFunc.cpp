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

char* CStringToUtf8Char(CString& str, int& chLength)
{
	char* pszMultiByte;
	int iSize = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	pszMultiByte = (char*)malloc((iSize + 1)/**sizeof(char)*/);
	memset(pszMultiByte, 0, iSize + 1);
	WideCharToMultiByte(CP_UTF8, 0, str, -1, pszMultiByte, iSize, NULL, NULL);
	chLength = iSize;
	return pszMultiByte;
}

//URL编码
CString URLEncode(CString sIn)
{

	int ilength = -1;
	char* pUrl = CStringToUtf8Char(sIn, ilength);
	CStringA strSrc(pUrl);

	CStringA sOut;
	const int nLen = strSrc.GetLength() + 1;

	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf = (LPBYTE)strSrc.GetBuffer(nLen);
	BYTE b = 0;

	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen * 3 - 2);//new BYTE [nLen  * 3];

	if (pOutBuf)
	{
		pInTmp = pInBuf;
		pOutTmp = pOutBuf;

		// do encoding
		while (*pInTmp)
		{
			if (isalnum(*pInTmp))
				*pOutTmp++ = *pInTmp;
			else
			if (isspace(*pInTmp))
				*pOutTmp++ = '+';
			else
			{
				*pOutTmp++ = '%';
				*pOutTmp++ = ToHex(*pInTmp >> 4);
				*pOutTmp++ = ToHex(*pInTmp % 16);
			}
			pInTmp++;
		}
		*pOutTmp = '\0';
		//sOut=pOutBuf;
		//delete [] pOutBuf;
		sOut.ReleaseBuffer();
	}
	strSrc.ReleaseBuffer();
	if (pUrl != NULL)
	{
		delete pUrl;
		pUrl = NULL;
	}
	return CString(sOut);
}

BYTE ToHex(const BYTE &x)
{
	return x > 9 ? x + 55 : x + 48;
}

char * WideToMulti(const wchar_t *pWide, DWORD dwCode)
{
	char *pChar = NULL;
	int  iWlen = 0;

	if (pWide == NULL
		|| (iWlen = wcslen(pWide)) == 0)
	{
		return pChar;
	}

	int iLen = WideCharToMultiByte(dwCode, 0, pWide, iWlen, NULL, NULL, NULL, NULL);
	if (iLen > 0)
	{
		pChar = new char[iLen + 1];
		if (pChar != NULL)
		{
			memset(pChar, 0, iLen + 1);
			WideCharToMultiByte(dwCode, 0, pWide, iWlen, pChar, iLen, NULL, NULL);
		}
	}

	return pChar;
}

char * WideToMulti2(const wchar_t *pWide, DWORD &iLen, DWORD dwCode)
{
	char *pChar = NULL;
	int  iWlen = 0;

	if (pWide == NULL
		|| (iWlen = wcslen(pWide)) == 0)
	{
		return pChar;
	}

	iLen = WideCharToMultiByte(dwCode, 0, pWide, iWlen, NULL, NULL, NULL, NULL);

	if (iLen > 0)
	{
		pChar = new char[iLen + 1];
		if (pChar != NULL)
		{
			memset(pChar, 0, iLen + 1);
			WideCharToMultiByte(dwCode, 0, pWide, iWlen, pChar, iLen, NULL, NULL);
		}
	}

	return pChar;
}

wchar_t * MultitoWide(const char *pMulti, DWORD dwCode /*= CP_ACP*/)
{
	wchar_t *pWide = NULL;
	int iAlen = 0;

	if (pMulti == NULL
		|| (iAlen = strlen(pMulti)) == 0)
	{
		return pWide;
	}

	int iLen = MultiByteToWideChar(dwCode, 0, pMulti, iAlen, NULL, NULL);
	if (iLen > 0)
	{
		pWide = new wchar_t[iLen + 1];
		if (pWide != NULL)
		{
			memset(pWide, 0, (iLen + 1)*sizeof(wchar_t));
			MultiByteToWideChar(dwCode, 0, pMulti, iAlen, pWide, iLen);
		}

	}

	return pWide;
}
