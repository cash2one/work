///////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2007, ��Ѷ����Ϣ���޹�˾
//
// �汾��1.0
// �ļ�˵������־��ӡ��ʵ���ļ�
// �������ڣ�2007-4-4
// ���ߣ����ױ�
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2. 
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <stdarg.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include "Trace.h"
#include "common_define.h"
#include "CommFunc.h"
#include "ZipEncode/ZipEncode.h"
#include <psapi.h>
#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"ZipEnCode.lib")

char *ToMultiByte(LPCWSTR pszSource, int *dwRet)
{
	int nLanguage = CP_ACP;

	int nLength = WideCharToMultiByte(nLanguage, 0, pszSource, wcslen(pszSource), NULL, 0, NULL, FALSE);

	char *pBuffer = NULL;
	pBuffer = new char[nLength+1];

	if (!pBuffer)
	{
		return NULL;
	}

	WideCharToMultiByte(nLanguage, 0, pszSource, wcslen(pszSource), pBuffer, nLength, NULL, FALSE);
	pBuffer[nLength] = 0;

	if(dwRet)
	{
		*dwRet = nLength;
	}

	return pBuffer; 
}

CLogTrace::CLogTrace()
{
	m_hLogFile = NULL;
	m_hLogWnd = NULL;
	m_bFirstCreate = false;
}

bool CLogTrace::IsLogFileOpened()
{
	return (m_hLogFile != NULL);
}

 //��/������־�ļ�
void CLogTrace::OpenLogFile(const TCHAR * pszTraceFileName, HWND hWnd)
{
	TCHAR tszTmp[MAX_PATH] = {0};
	_stprintf(tszTmp, _T("%s\\data2\\mc.dat"), GetInstallPath());

	m_nOutLogInfo = GetPrivateProfileInt(_T("DEBUGLOG"),_T("Output"),1, tszTmp);
	m_nEncryptLog = GetPrivateProfileInt(_T("DEBUGLOG"),_T("Encrypt"),1, tszTmp);
	m_nOutLogLevel = GetPrivateProfileInt(_T("DEBUGLOG"),_T("OutLevel"),3,tszTmp);
	m_nSaveAllLog = GetPrivateProfileInt(_T("DEBUGLOG"), _T("SaveAll"), 0, tszTmp);

	if (!IsLogFileEnable()) return;

	ZeroMemory(m_szFullLogFileName, sizeof(m_szFullLogFileName) * sizeof(TCHAR));
	m_hLogFile = NULL;
	m_hLogWnd = NULL;
	m_bFirstCreate = false;
	int i = 0;

	//�Ϸ��Լ��
	if (!pszTraceFileName)
	{
		OutputDebugString(_T("Error:  pszTraceFileName is NULL\r\n"));
		return;
	}

	//modified by hpt 9-28������־�ļ�·��������ģ������logĿ¼��
	_sntprintf(m_szFullLogFileName, _TRUNCATE, _T("%s\\log\\%s"), GetInstallPath(), pszTraceFileName);

	//��¼���ھ��
	m_hLogWnd = hWnd;

}

///////////////////////////////////////////////////////////////////////////////
//
// �����������������캯������ʼ��������־�ļ�
// ���룺��־�ļ�������־���ھ��
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
CLogTrace::CLogTrace(const TCHAR * pszTraceFileName, HWND hWnd)
{
	TCHAR tszTmp[MAX_PATH] = {0};
	_stprintf(tszTmp, _T("%s\\data2\\mc.dat"), GetInstallPath());

	m_nOutLogInfo = GetPrivateProfileInt(_T("DEBUGLOG"),_T("Output"),1, tszTmp);
	m_nEncryptLog = GetPrivateProfileInt(_T("DEBUGLOG"),_T("Encrypt"),1, tszTmp);
	m_nOutLogLevel = GetPrivateProfileInt(_T("DEBUGLOG"),_T("OutLevel"),3,tszTmp);
	m_nSaveAllLog = GetPrivateProfileInt(_T("DEBUGLOG"), _T("SaveAll"), 0, tszTmp);

	if (!IsLogFileEnable()) return;

    ZeroMemory(m_szFullLogFileName, sizeof(m_szFullLogFileName) * sizeof(TCHAR));
    m_hLogFile = NULL;
    m_hLogWnd = NULL;
    m_bFirstCreate = false;
	int i = 0;
    
    //�Ϸ��Լ��
    if (!pszTraceFileName)
    {
        OutputDebugString(_T("Error:  pszTraceFileName is NULL\r\n"));
        return;
    }

    //modified by hpt 9-28������־�ļ�·��������ģ������logĿ¼��
	_sntprintf(m_szFullLogFileName, _TRUNCATE, _T("%s\\log\\%s"), GetInstallPath(), pszTraceFileName);

    //��¼���ھ��
    m_hLogWnd = hWnd;

}

///////////////////////////////////////////////////////////////////////////////
//
// �������������������������ر���־�ļ�
// ���룺
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
CLogTrace::~CLogTrace()
{
	if(IsLogFileEnable()==FALSE)return;
    if (m_hLogFile)
    {
        Trace(LOGL_MID, LOGT_PROMPT, __TFILE__, __LINE__,
            _T("== Module Unloaded, Log end =="));
        TraceSim(LOGL_MID, _T("\r\n"));

        CloseHandle(m_hLogFile);
    }
	if (m_hLogWnd)
	{
		CloseHandle(m_hLogWnd);
	}
	
}

///////////////////////////////////////////////////////////////////////////////
//
// ����������������׼��־��ӡ
// ���룺��־�������͡�Դ�ļ����������У���ʽ�ִ�
// ������򴰿ڻ��ļ�д��־
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::Trace(BYTE cLevel, BYTE cType, const TCHAR * pszSourceFile,
           DWORD dwLine, LPCTSTR lpszFormat, ...)
{
	if (! IsLogFileEnable()) return;
    //ָ�����ͼ������µ���־����ӡ��
    if ( m_nOutLogLevel < cType )
    {
        return;
    }

    //�Ϸ��Լ��
    if (!pszSourceFile || !lpszFormat)
    {
        OutputDebugString(_T("Error: pszSourceFile or lpszFormat is NULL\r\n"));
        return;
    }

    //��ȡ����
    va_list args;
    va_start(args, lpszFormat);

    CLocalLock localLock(&m_lock); //��
    
    //�����ִ�
	m_szBuffer[0][0] = 0;
	int nBuf = -1;
	if(cType == LOGT_COMPRESS)      //�Դ�ӡ����־����ѹ��
	{
		TCHAR *tmp = new TCHAR[200000];
		memset(tmp,0,200000 * sizeof(TCHAR));
		int nBuf = _vsntprintf(tmp,200000-1,lpszFormat,args);
		CString strSource,strZipData;
		strSource.Format(_T("%s"),tmp);
		if (strSource.GetLength() < 100)
		{
			strZipData = strSource;
		}
		else
		{
			ZipEecodeStdString(strSource,strZipData);
		}
		_tcsncpy(m_szBuffer[0],strZipData.GetBuffer(strZipData.GetLength()),MAX_LEN_OF_LOG_LINE-128);
		strZipData.ReleaseBuffer();
		delete []tmp;
	}
	else
	{
		nBuf = _vsntprintf(m_szBuffer[0], MAX_LEN_OF_LOG_LINE - 128, lpszFormat, args);
	}
    va_end(args);		

    if (nBuf < 0) //����
    {
		//�п������ַ���̫��
        m_szBuffer[0][MAX_LEN_OF_LOG_LINE - 128] = 0;
    }
    
    //ȡϵͳ���ڡ�ʱ��
    TCHAR szData[12], szTime[12];
    _tstrdate(szData);
    _tstrtime(szTime);

    //�����ִ�����ɫ
	CString strType;
    DWORD dwColor = 0;

	//��ȡ��־�������ı�����ɫ
	GetLogType(strType,dwColor,cType);

    //Դ�ļ������̴���
    TCHAR * pszSourceShort = NULL;
    TCHAR szSourceShort[MAX_PATH];
    WORD wSourceNameLen = lstrlen(pszSourceFile);

    if (wSourceNameLen > LEN_SHORT_SOURCEFILE_NAME) //̫��
    {
        wsprintf(szSourceShort, _T("...%s"),
            pszSourceFile + (wSourceNameLen - LEN_SHORT_SOURCEFILE_NAME));
        pszSourceShort = szSourceShort;
    }
    else
    {
        pszSourceShort = (TCHAR *)pszSourceFile;
    }

    //�γɽ���ִ�
    nBuf = _sntprintf(m_szBuffer[1], MAX_LEN_OF_LOG_LINE,
        _T("%s %s [%d]%s: %s at %d in %s\r\n"), szData, szTime, GetCurrentThreadId(), strType.GetBuffer(),
        m_szBuffer[0], dwLine, pszSourceShort);
    if (nBuf < 0) //����
    {
        //�п������ַ���̫��
        m_szBuffer[1][MAX_LEN_OF_LOG_LINE - 1] = 0;
    }

	strType.ReleaseBuffer();
    //��ӡ
    TraceColor(cLevel, dwColor);
}

///////////////////////////////////////////////////////////////////////////////
//
// ������������������־��ӡ
// ���룺��־���𣬸�ʽ�ִ�
// ������򴰿ڻ��ļ�д��־���������ڡ�ʱ�䡢���͡��ļ�������������Ϣ
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::TraceSim(BYTE cLevel, LPCTSTR lpszFormat, ...)
{
	if(IsLogFileEnable()==FALSE)return;
    //LOGLEVEL_HIGH�������µ���־��ӡ��Release����ֱ�ӷ���
#ifndef _DEBUG
    if (cLevel > LOGL_LOW)
    {
        return;
    }
#endif
    
    //�Ϸ��Լ��
    if (!lpszFormat)
    {
        OutputDebugString(_T("Error: lpszFormat is NULL\r\n"));
        return;
    }
    
    //��ȡ����
    va_list args;
    va_start(args, lpszFormat);
    
    CLocalLock localLock(&m_lock); //��
    
    //�����ִ�
    m_szBuffer[1][0] = 0;
    int nBuf = _sntprintf(m_szBuffer[1], MAX_LEN_OF_LOG_LINE, lpszFormat, args);
    
    va_end(args);
    
    if (nBuf < 0) //����
    {
        //�п������ַ���̫��
        m_szBuffer[1][MAX_LEN_OF_LOG_LINE - 1] = 0;
    }

    //��ӡ
    TraceColor(cLevel, LOGC_PROMPT);
}

///////////////////////////////////////////////////////////////////////////////
//
// ����������������־��ӡ
// ���룺��־������ɫ
// ������򴰿ڻ��ļ�д��־
// ����ֵ��
// ����˵����
//
//2013-10-09 �޸����� : ��Ӽ�����־��Ϣ���� ���Ƿ�
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::TraceColor(BYTE cLevel, DWORD dwColor)
{
	if(IsLogFileEnable()==FALSE)return;
    switch (cLevel)
    {
    case LOGL_TOP:
    case LOGL_MID:
        {
            if (m_hLogWnd) //д��־����
            {
                ::SendMessage(m_hLogWnd, TRACE_WND_MSG, 0, (LPARAM)dwColor);
            }
        }

    case LOGL_HIG:
    case LOGL_LOW:
        {
            static DWORD s_dwWriteCount = 0; //�ܹ���ӡ�ļ��Ĵ���
            DWORD dwWritedBytes = 0; //ʵ��д���ֽ���

			if (NULL == m_hLogFile)
			{			
				CreatLogFile() ;  //�������log�ļ�
			}

			//��ȡ�ļ���ʼ��־ begin--
			int nFlag = 0 ;
			int nCharCount = 0 ;
			GetEncryptFlag(nFlag) ;
			if ( m_hLogFile )
			{
				//log�ļ�Ϊ�����ļ� ���Ǵ˴�д��־Ϊ���� || log�ļ������� ���Ǵ˴�д��־��Ҫ����
				//ͨ������������Դ�ļ� �� �����µ��ļ�������־��ӡ
				if ( nFlag == 1 && !IsEncryptLog() || nFlag != 1 && IsEncryptLog() )
				{
					CloseHandle(m_hLogFile) ;
					m_hLogFile = NULL ;

					TCHAR szExtension[] = _T(".old") ;
					int nNewPathLen = lstrlen(m_szFullLogFileName) + lstrlen(szExtension) + 1;
					TCHAR *pszNewPath = new TCHAR[nNewPathLen] ;
					memset(pszNewPath, 0, nNewPathLen*sizeof(TCHAR)) ;
					lstrcat(pszNewPath, m_szFullLogFileName) ;
					lstrcat(pszNewPath, szExtension) ;
					MoveFileEx(m_szFullLogFileName, pszNewPath, MOVEFILE_REPLACE_EXISTING) ;

					if ( pszNewPath != NULL )
					{
						delete []pszNewPath ;
						pszNewPath = NULL ;
					}

					CreatLogFile() ;  //�������ļ�
				}
			}
			//--end

			if (m_hLogFile) //Log�ļ�����
            {
                //ÿд���ɴμ��һ�δ�С                
                if (0 == s_dwWriteCount % MAX_CHK_SIZE_PER_TIMES)
                {
                    //��ȡ�ļ���С
                    DWORD dwLogFileSize = GetFileSize(m_hLogFile, NULL);
                    if (INVALID_FILE_SIZE == dwLogFileSize)
                    {
                        OutputDebugString(_T("Error: run GetFileSize fail\r\n"));
                        CloseHandle(m_hLogFile);
                        m_hLogFile = NULL;
                    }
                    else if (dwLogFileSize > MAX_SIZE_OF_LOGFILE) //�ļ�̫��
                    {
                        CloseHandle(m_hLogFile);
                        m_hLogFile = NULL;
                        
						//��ʱ��������������־�ļ���
						if (m_nSaveAllLog != 0)
						{
							time_t tim;
							time(&tim);
							struct tm *t;

							t = localtime(&tim);

							CString strDate;
							strDate.Format(_T("%s.%02d%02d%02d"), m_szFullLogFileName, t->tm_hour, t->tm_min, t->tm_sec);

							CopyFile(m_szFullLogFileName, strDate.GetBuffer(), FALSE);
						}
                        //�¿�һ���ļ�
                        m_hLogFile = CreateFile(m_szFullLogFileName, GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE, 0, TRUNCATE_EXISTING,//CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);
                        if (INVALID_HANDLE_VALUE == m_hLogFile)
                        {
                            m_hLogFile = NULL;
                            OutputDebugString(_T("Error: Create log file fail\r\n"));
                        }

                    }
                }

				//����m_szBuffer�ǲ��ǿ��ַ�,��������ת����multiple string
				char *pWriteStr = NULL ;
				char *pEncryptInfo = NULL ;
				int nWriteStrLen = 0 ;

				char * pMultipleStr = NULL ;

#ifndef _MBCS
				pMultipleStr = ToMultiByte(m_szBuffer[1], &nWriteStrLen);
				pWriteStr = pMultipleStr ;
#else
				pWriteStr = m_szBuffer[1] ;
				nWriteStrLen = lstrlen( m_szBuffer[1] ) ;
#endif

				//������־��Ϣ begin--
				if ( IsEncryptLog() )
				{
					pEncryptInfo = new char[nWriteStrLen * 2] ;
					memset(pEncryptInfo, 0, nWriteStrLen * 2) ;
					nWriteStrLen = m_base64Encrypt.Base64Encrypt(pEncryptInfo, pWriteStr, LOG_ENCRYPT_KEY) ;
					pEncryptInfo[++nWriteStrLen] = 0 ;  //��Ϊ�ָ���
					pWriteStr = pEncryptInfo ;
				}
				//--end

				//д����־�ļ�  begin--
				if (0 == WriteFile(m_hLogFile,pWriteStr, nWriteStrLen, &dwWritedBytes, 0))
				{
					OutputDebugString(_T("Error: Write log file fail\r\n"));
					CloseHandle(m_hLogFile);
					m_hLogFile = NULL;
				}
				//--end

				//�ڴ����� begin--
				if ( pMultipleStr )
				{
					delete[] pMultipleStr;
					pMultipleStr = NULL;
				}
				if ( pEncryptInfo )
				{
					delete []pEncryptInfo ;
					pEncryptInfo = NULL ;
				}
				//--end

				//ÿдһ����־�͹رյ���־�ļ��������ӡ��־����ʱ���ϵ�����
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;

                s_dwWriteCount++;
            }
        }

    case LOGL_DBG:
    default:
        {
            //дDebug����
            OutputDebugString(m_szBuffer[1]);
        }
    }
}

bool CLogTrace::CreatLogFile()
{
	//����־�ļ�
	m_hLogFile = CreateFile(m_szFullLogFileName, GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | 
		FILE_FLAG_WRITE_THROUGH, 0);
	if (INVALID_HANDLE_VALUE == m_hLogFile)
	{	
		m_hLogFile = NULL;
		OutputDebugString(_T("Error: Open log file fail\r\n"));
	}
	else
	{
		if ( ERROR_ALREADY_EXISTS != GetLastError() )
		{
			//�ļ�ָ���Ƶ�ĩβ
			SetFilePointer(m_hLogFile, 0, 0, FILE_END);

			if ( !SetEncryptFlag() )
			{
				OutputDebugString(_T("Error: Write Encrypt Flag fail !!\r\n"));
			}
		}
	}

	return true ;
}

///////////////////////////////////////////////////////////////////////////////
//
// ���������������ļ���ʼд����ܱ�־
// ���룺
// �����
// ����ֵ���Ƿ�д��ɹ�
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
bool CLogTrace::SetEncryptFlag()
{
	DWORD dwWritedBytes = 0 ;
	bool bRet = false ;

	//�����д��������� �����ļ���ʼд����ܱ�� begin--
	if ( IsEncryptLog() )
	{
		char *pEncryptFlag = new char[4] ;
		memset(pEncryptFlag, 0, 4) ;
		int nFlag = 1 ;
		memcpy(pEncryptFlag, &nFlag, sizeof(int) ) ; 

		//д����־�ļ�  begin--
		if ( 0 == WriteFile(m_hLogFile,pEncryptFlag, sizeof(int), &dwWritedBytes, 0) )
		{
			OutputDebugString(_T("Error: Write log file fail\r\n"));
			CloseHandle(m_hLogFile);
			m_hLogFile = NULL;
			bRet = false ;
		}
		else
		{
			bRet = true ;
		}

		if ( pEncryptFlag )
		{
			delete []pEncryptFlag ;
			pEncryptFlag = NULL ;
		}
		//--end
	}
	//--end

	return bRet ;
}

bool CLogTrace::GetEncryptFlag(int &nFlag)
{
	nFlag = 0 ;
	bool bRet = false ;

	char *pFlag = new char[4] ;
	memset(pFlag, 0, sizeof(int)) ;

	DWORD dwBytesToRead = 0 ;
	DWORD dwWantRead = sizeof(int) ;
	SetFilePointer(m_hLogFile, 0, 0, FILE_BEGIN) ;
	if ( !ReadFile(m_hLogFile, pFlag, dwWantRead, &dwBytesToRead, NULL) )
	{
		int nError = GetLastError() ;
		OutputDebugString(_T("Error: Read log file fail\r\n"));
		CloseHandle(m_hLogFile);
		m_hLogFile = NULL;
		bRet = false ;
	}
	else
	{
		bRet = true ;
		memcpy(&nFlag, pFlag, sizeof(int)) ;
	}

	if ( pFlag )
	{
		delete []pFlag ;
		pFlag = NULL ;
	}

	SetFilePointer(m_hLogFile, 0, 0, FILE_END) ;

	return bRet ;
}

///////////////////////////////////////////////////////////////////////////////
//
// ��������������������־�ִ���������־���ڴ�ӡʱȡ��
// ���룺
// �����
// ����ֵ����־�ִ�ָ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
const TCHAR * CLogTrace::GetLogBuf()
{
    return m_szBuffer[1];
}

///////////////////////////////////////////////////////////////////////////////
//
// ��������������������־���ھ��
// ���룺��־���ھ��
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::SetLogWnd(HWND hLogWnd)
{
    m_hLogWnd = hLogWnd;
}

bool CLogTrace::IsLogFileEnable()
{
	return (m_nOutLogInfo != 0);  //�Ƿ�Ҫ������־�ļ�
}

bool CLogTrace::IsEncryptLog()
{
	return (m_nEncryptLog != 0) ;  // �Ƿ������־�ļ�
}

///////////////////////////////////////////////////////////////////////////////
//
// �����������������ַ�������ѹ��������
// ���룺strSource:Ҫѹ�����ַ���
// �����strZipped:ѹ��֮����ַ���
// ����ֵ����־�ִ�ָ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::ZipEecodeStdString( const CString &strSource, CString & strZipped )
{
	int dwSourceCount = 0,dwZippedLen = 0,dwMaxZippedLen = 0;
	ZipEncode ZipFunction;
	int dwReturn = 0;

	dwSourceCount = strSource.GetLength();

	TCHAR * SourceChar = NULL;
	_NEWA(SourceChar,TCHAR,dwSourceCount + 1);
	if (NULL == SourceChar)
	{
		return ;
	}
	_tcscpy(SourceChar , strSource); 
	SourceChar[dwSourceCount] = 0;
	dwMaxZippedLen = ZipFunction.GetMaxZipBytesCount(dwSourceCount);

	//��������Ĵ�С
	dwZippedLen = dwMaxZippedLen;

	TCHAR * RetChar = NULL;
	_NEWA(RetChar,TCHAR,dwMaxZippedLen + 1);
	if (NULL == RetChar)
	{
		if(SourceChar)
		{
			delete[] SourceChar;
			SourceChar = NULL;
		}
		return ;
	}
	memset(RetChar, 0, (dwMaxZippedLen + 1) * sizeof(TCHAR));

	dwReturn = ZipFunction.EnocdeANDZipString(SourceChar,dwSourceCount,&RetChar,&dwZippedLen);

	if (0 != dwReturn)
	{
		RetChar[0] = 0;
	}

	RetChar[dwZippedLen] = 0;

	strZipped = RetChar;


	if(NULL != SourceChar)
	{
		delete[] SourceChar;
		SourceChar = NULL;
	}
	if(NULL != RetChar)
	{
		delete[] RetChar;
		RetChar = NULL;
	}
}
//��ȡ��־�ı�����ӡ��ɫ
void CLogTrace::GetLogType( CString &strType, DWORD &dwColor,BYTE cType)
{
	switch (cType)
	{
	case LOGT_ERROR:
		{
			strType = _T("Error");
			dwColor = LOGC_ERROR;
		}
		break;

	case LOGT_WARNING:
		{
			strType = _T("Warning");
			dwColor = LOGC_WARNING;
		}
		break;

	case LOGT_PROMPT:
		{
			strType = _T("Prompt");
			dwColor = LOGC_PROMPT;
		}
		break;
	case LOGT_DEBUG:
		{
			strType = _T("Debug");
			dwColor = LOGC_DEBUG;
		}
		break;
	case LOGT_COMPRESS:
		{
			strType = _T("Compress");
			dwColor = LOGC_PROMPT;
		}
		break;
	default:
		{
			strType = _T("Invalid");
			dwColor = LOGC_ERROR;
		}

		break;
	}
}
