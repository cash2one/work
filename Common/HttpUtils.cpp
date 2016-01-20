///////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2012, ��Ѷ����Ϣ���޹�˾
//
// �汾��1.0
// �ļ�˵����ͨ��HTTPЭ��������������Ľӿ���
// �������ڣ�2012-12-3
// ���ߣ�������
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpUtils.h"
#include <Wininet.h>

#pragma comment(lib, "Wininet.lib")

BOOL CHttpUtils::GetFile(TCHAR *pURL, TCHAR *pLocalName, int nRetry)
{
	CString strTargetDir = pLocalName;
	strTargetDir = strTargetDir.Mid(0, strTargetDir.ReverseFind('\\'));
	if (!PathFileExists(strTargetDir))
	{
		SHCreateDirectoryEx(NULL, strTargetDir, NULL);
	}

	try
	{
		for (int i = 0; i < nRetry; ++i)
		{
			if (InternetGetFile(pURL, pLocalName))
			{
				g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("���سɹ���%s  ����·����%s"), pURL, pLocalName);
				return TRUE;
			}

			g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("����ʧ�ܣ�%s  ����·����%s"), pURL, pLocalName);
		}
	}
	catch (...)
	{
		g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("�����ļ�ʱ�쳣��%s  ����·����%s"), pURL, pLocalName);
	}

	return FALSE;
}

BOOL CHttpUtils::InternetGetFile(TCHAR *szUrl, TCHAR *szFileName)
{
    DWORD dwFlags = 0;
	BOOL bRes = FALSE;
	HINTERNET hOpen = NULL;
	if (!InternetGetConnectedState(&dwFlags, 0))
	{
		g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("����δ����, err:%d"), GetLastError());
	}
	
	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
		hOpen = InternetOpen(_T("BIZEXPRESS_LIVEUPDATE_"), INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	else
		hOpen = InternetOpen(_T("BIZEXPRESS_LIVEUPDATE_"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if(!hOpen)
	{
		g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetOpen����, err:%d"), GetLastError());
		return bRes;
	}
	
	DWORD dwSize;
	TCHAR   szHead[] = _T("Accept: */*\r\n\r\n");
	VOID* szTemp[16384];             //16kb
	HINTERNET  hConnect = NULL;
	CFile file;
	CFileException e;

	if ( !(hConnect = InternetOpenUrl( hOpen, szUrl, szHead,
		 _tcslen(szHead), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
	{
		g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetOpenUrl����, err:%d"), GetLastError());
        goto end;
	}
		
	if  (!file.Open(szFileName, CFile::modeWrite | CFile::modeCreate, &e))
	{
		g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("���ļ�ʧ�ܣ�%d"), e.m_cause);
        goto end;	    
	}

	DWORD dwByteToRead = 0;
	DWORD dwSizeOfRq = 4;
	DWORD dwBytes = 0;

    if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
                  (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
	{
		dwByteToRead = 0;
	}
	
	do
	{
		memset(szTemp,0,sizeof(szTemp));
		if (!InternetReadFile (hConnect, szTemp, 16384,  &dwSize))
		{
			g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetReadFile����, err:%d"), GetLastError());
			break;
		}

		if (dwSize==0)
		{
			bRes = TRUE;
			break;
		}
		else
			file.Write(szTemp, dwSize);
		
	}while (TRUE);

end:
	if (NULL != hConnect)
		InternetCloseHandle(hConnect);

	if (NULL != hOpen)
		InternetCloseHandle(hOpen);

	return bRes;
}

CString CHttpUtils::GetSerRespInfo(TCHAR *szUrl)
{
    DWORD dwFlags = 0;
	BOOL bRes = FALSE;
	CString strRetValue = _T("");
	HINTERNET hOpen = NULL;

	if (!InternetGetConnectedState(&dwFlags, 0))
	{
		g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("����δ����, err:%d"), GetLastError());
	}
	
	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
		hOpen = InternetOpen(_T("BIZEXPRESS_LIVEUPDATE_"), INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	else
		hOpen = InternetOpen(_T("BIZEXPRESS_LIVEUPDATE_"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if(!hOpen)
	{
		g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetOpen����, err:%d"), GetLastError());
		return _T("$ERR$");
	}
	
	DWORD dwSize;
	TCHAR   szHead[] = _T("Accept: */*\r\n\r\n");
	VOID* szTemp[16384];             //16kb
	HINTERNET  hConnect = NULL;

	if ( !(hConnect = InternetOpenUrl( hOpen, szUrl, szHead,
		_tcslen(szHead), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
	{
		g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetOpenUrl����, err:%d"), GetLastError());
        goto end;
	}

	DWORD dwByteToRead = 0;
	DWORD dwSizeOfRq = 4;
	DWORD dwBytes = 0;

    if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
                  (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
	{
		dwByteToRead = 0;
	}
	
	do
	{
		memset(szTemp,0,sizeof(szTemp));
		if (!InternetReadFile (hConnect, szTemp, 16384,  &dwSize))
		{
			g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetReadFile����, err:%d"), GetLastError());
			break;
		}

		if (dwSize == 0)
		{
			bRes = TRUE;
			break;
		}
		else
		{
			strRetValue += (char*)szTemp;
		}
		
	}while (TRUE);

end:
	if (NULL != hConnect)
		InternetCloseHandle(hConnect);

	if (NULL != hOpen)
		InternetCloseHandle(hOpen);

	return bRes ? strRetValue : _T("$ERR$");
}
