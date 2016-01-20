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

//����һ��ȫ�ֵ��������֣�ԭ�Ӳ�����
LONG GetGlobalIndexNum(void);