// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Kernel4.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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