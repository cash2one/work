// stdafx.cpp : 只包括标准包含文件的源文件
// Kernel4.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


IGlobalDataBlock* g_GlobalDataBlock = NULL;
IConfigInfo *g_Config = NULL;
IServerSocket *g_ServerSocket = NULL;
CLogTrace g_log(_T("ProcessTask.log"), NULL);


//
//
//LPWSTR MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize)
//{
//	DWORD dwMinSize;
//
//	dwMinSize = MultiByteToWideChar(CP_ACP,NULL,lpcszStr,-1,NULL,0);
//	if(*dwSize < dwMinSize)
//	{
//		return NULL;
//	}
//
//	MultiByteToWideChar(CP_ACP,NULL,lpcszStr,-1,lpwszStr,*dwSize);
//
//	*dwSize = dwMinSize;
//	return lpwszStr;
//}
//
//LPSTR WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize)
//{
//	DWORD dwMinSize;
//	dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
//	if(*dwSize < dwMinSize)
//	{
//		return NULL;
//	}
//	WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,*dwSize,NULL,FALSE);
//	*dwSize = dwMinSize;
//	return lpszStr;
//}