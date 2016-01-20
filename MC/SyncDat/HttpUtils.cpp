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
// 1. ���ڣ�2013-6-28
//   ���ߣ�laihao
//   �޸����ݣ���mfc��Ϣȥ������׼��
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpUtils.h"
#include <Wininet.h>

#pragma comment(lib, "Wininet.lib")

BOOL CHttpUtils::GetFile( const TCHAR *pURL, const TCHAR *pLocalName, int nRetry /*= 3*/ )
{
	try
	{
		for (int i = 0; i < nRetry; ++i)
		{
			if (InternetGetFile(pURL, pLocalName))
			{
				//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("���سɹ���%s  ����·����%s"), pURL, pLocalName);
				return TRUE;
			}

			//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("����ʧ�ܣ�%s  ����·����%s"), pURL, pLocalName);
		}
	}
	catch (...)
	{
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("�����ļ�ʱ�쳣��%s  ����·����%s"), pURL, pLocalName);
	}

	return FALSE;
}

BOOL CHttpUtils::InternetGetFile(const TCHAR *pURL, const TCHAR *pLocalName)
{
    DWORD dwFlags = 0;
	BOOL bRes = FALSE;
	HINTERNET hOpen = NULL;

	if (!InternetGetConnectedState(&dwFlags, 0))
	{
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("����δ����, err:%d"), GetLastError());
	}
	
	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
		hOpen = InternetOpen(_T("BIZEXPRESS_LIVEUPDATE_"), INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	else
		hOpen = InternetOpen(_T("BIZEXPRESS_LIVEUPDATE_"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if(!hOpen)
	{
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetOpen����, err:%d"), GetLastError());
		return bRes;
	}
	
	DWORD dwSize = 0;
	TCHAR   szHead[] = _T("Accept: */*\r\n\r\n");
	BYTE   szTemp[16384];             //16kb
	HINTERNET  hConnect = NULL;

	if ( !(hConnect = InternetOpenUrl( hOpen, pURL, szHead,
		 _tcslen(szHead), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
	{
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetOpenUrl����, err:%d"), GetLastError());
        goto end;
	}
		

	FILE *pFile = _wfopen(pLocalName, _T("wb+"));
	if  (pFile == NULL)
	{
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("���ļ�%sʧ��"), pLocalName);
        goto end;	    
	}

	do
	{
		memset(szTemp,0,sizeof(szTemp));
		if (!InternetReadFile (hConnect, (void*)szTemp, sizeof(szTemp),  &dwSize))
		{
			//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("InternetReadFile����, err:%d"), GetLastError());
			break;
		}

		if (dwSize==0)
		{
			bRes = TRUE;
			break;
		}
		else
		{
			dwSize = fwrite((void*)szTemp, sizeof(BYTE), dwSize, pFile);
		}
		
	}while (TRUE);

	fclose(pFile);

end:
	if (NULL != hConnect)
		InternetCloseHandle(hConnect);

	if (NULL != hOpen)
		InternetCloseHandle(hOpen);

	return bRes;
}
