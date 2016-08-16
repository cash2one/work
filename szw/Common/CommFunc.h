#pragma once

#include <tchar.h>
#include <AtlBase.h>
#include <comutil.h>


TCHAR *GetCurPath(void);
TCHAR *GetProgPath(void);
TCHAR *GetInstallPath(void);
LPWSTR MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize, UINT codePage = CP_ACP);
LPSTR WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize, UINT codePage = CP_OEMCP);
int CompareBSTR(_bstr_t bstr1, _bstr_t bstr2);
int CompareBSTRNoCase(_bstr_t bstr1, _bstr_t bstr2);

/*
@brief  宽字符转多字节
@param  要转化的宽字符串
@return 返回多字节
*/
char * WideToMulti(const wchar_t *pWide, DWORD dwCode = CP_ACP);

/*
@brief  宽字符转多字节
@param  要转化的宽字符串
@return 返回多字节
*/
char * WideToMulti2(const wchar_t *pWide, DWORD &dwLen, DWORD dwCode = CP_ACP);

/*
@brief  多字节转宽字符
@param  要转化的多字符串
@return 返回宽字节
*/
wchar_t * MultitoWide(const char *pMulti, DWORD dwCode = CP_ACP);

//返回一个全局递增的数字（原子操作）
LONG GetGlobalIndexNum(void);

//转UTF8
char* CStringToUtf8Char(CString& str, int& chLength);
//URL编码
CString  URLEncode(CString sIn);

BYTE ToHex(const BYTE &x);
