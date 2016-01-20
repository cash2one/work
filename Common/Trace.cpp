///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2007, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：日志打印类实现文件
// 生成日期：2007-4-4
// 作者：刘兆兵
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
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

 //打开/创建日志文件
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

	//合法性检测
	if (!pszTraceFileName)
	{
		OutputDebugString(_T("Error:  pszTraceFileName is NULL\r\n"));
		return;
	}

	//modified by hpt 9-28构造日志文件路径，放在模块所在log目录下
	_sntprintf(m_szFullLogFileName, _TRUNCATE, _T("%s\\log\\%s"), GetInstallPath(), pszTraceFileName);

	//记录窗口句柄
	m_hLogWnd = hWnd;

}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：构造函数，初始化，打开日志文件
// 输入：日志文件名，日志窗口句柄
// 输出：
// 返回值：
// 其它说明：
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
    
    //合法性检测
    if (!pszTraceFileName)
    {
        OutputDebugString(_T("Error:  pszTraceFileName is NULL\r\n"));
        return;
    }

    //modified by hpt 9-28构造日志文件路径，放在模块所在log目录下
	_sntprintf(m_szFullLogFileName, _TRUNCATE, _T("%s\\log\\%s"), GetInstallPath(), pszTraceFileName);

    //记录窗口句柄
    m_hLogWnd = hWnd;

}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：析构函数，关闭日志文件
// 输入：
// 输出：
// 返回值：
// 其它说明：
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
// 函数功能描述：标准日志打印
// 输入：日志级别、类型、源文件名、代码行，格式字串
// 输出：向窗口或文件写日志
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::Trace(BYTE cLevel, BYTE cType, const TCHAR * pszSourceFile,
           DWORD dwLine, LPCTSTR lpszFormat, ...)
{
	if (! IsLogFileEnable()) return;
    //指定类型级别以下的日志不打印；
    if ( m_nOutLogLevel < cType )
    {
        return;
    }

    //合法性检测
    if (!pszSourceFile || !lpszFormat)
    {
        OutputDebugString(_T("Error: pszSourceFile or lpszFormat is NULL\r\n"));
        return;
    }

    //读取参数
    va_list args;
    va_start(args, lpszFormat);

    CLocalLock localLock(&m_lock); //锁
    
    //打入字串
	m_szBuffer[0][0] = 0;
	int nBuf = -1;
	if(cType == LOGT_COMPRESS)      //对打印的日志进行压缩
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

    if (nBuf < 0) //出错
    {
		//有可能是字符串太长
        m_szBuffer[0][MAX_LEN_OF_LOG_LINE - 128] = 0;
    }
    
    //取系统日期、时间
    TCHAR szData[12], szTime[12];
    _tstrdate(szData);
    _tstrtime(szTime);

    //类型字串及颜色
	CString strType;
    DWORD dwColor = 0;

	//获取日志的类型文本及颜色
	GetLogType(strType,dwColor,cType);

    //源文件名缩短处理
    TCHAR * pszSourceShort = NULL;
    TCHAR szSourceShort[MAX_PATH];
    WORD wSourceNameLen = lstrlen(pszSourceFile);

    if (wSourceNameLen > LEN_SHORT_SOURCEFILE_NAME) //太长
    {
        wsprintf(szSourceShort, _T("...%s"),
            pszSourceFile + (wSourceNameLen - LEN_SHORT_SOURCEFILE_NAME));
        pszSourceShort = szSourceShort;
    }
    else
    {
        pszSourceShort = (TCHAR *)pszSourceFile;
    }

    //形成结果字串
    nBuf = _sntprintf(m_szBuffer[1], MAX_LEN_OF_LOG_LINE,
        _T("%s %s [%d]%s: %s at %d in %s\r\n"), szData, szTime, GetCurrentThreadId(), strType.GetBuffer(),
        m_szBuffer[0], dwLine, pszSourceShort);
    if (nBuf < 0) //出错
    {
        //有可能是字符串太长
        m_szBuffer[1][MAX_LEN_OF_LOG_LINE - 1] = 0;
    }

	strType.ReleaseBuffer();
    //打印
    TraceColor(cLevel, dwColor);
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：简单日志打印
// 输入：日志级别，格式字串
// 输出：向窗口或文件写日志，不含日期、时间、类型、文件名、代码行信息
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::TraceSim(BYTE cLevel, LPCTSTR lpszFormat, ...)
{
	if(IsLogFileEnable()==FALSE)return;
    //LOGLEVEL_HIGH级别以下的日志打印在Release版中直接返回
#ifndef _DEBUG
    if (cLevel > LOGL_LOW)
    {
        return;
    }
#endif
    
    //合法性检测
    if (!lpszFormat)
    {
        OutputDebugString(_T("Error: lpszFormat is NULL\r\n"));
        return;
    }
    
    //读取参数
    va_list args;
    va_start(args, lpszFormat);
    
    CLocalLock localLock(&m_lock); //锁
    
    //打入字串
    m_szBuffer[1][0] = 0;
    int nBuf = _sntprintf(m_szBuffer[1], MAX_LEN_OF_LOG_LINE, lpszFormat, args);
    
    va_end(args);
    
    if (nBuf < 0) //出错
    {
        //有可能是字符串太长
        m_szBuffer[1][MAX_LEN_OF_LOG_LINE - 1] = 0;
    }

    //打印
    TraceColor(cLevel, LOGC_PROMPT);
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：日志打印
// 输入：日志级别、颜色
// 输出：向窗口或文件写日志
// 返回值：
// 其它说明：
//
//2013-10-09 修改内容 : 添加加密日志信息部分 宋亚非
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::TraceColor(BYTE cLevel, DWORD dwColor)
{
	if(IsLogFileEnable()==FALSE)return;
    switch (cLevel)
    {
    case LOGL_TOP:
    case LOGL_MID:
        {
            if (m_hLogWnd) //写日志窗口
            {
                ::SendMessage(m_hLogWnd, TRACE_WND_MSG, 0, (LPARAM)dwColor);
            }
        }

    case LOGL_HIG:
    case LOGL_LOW:
        {
            static DWORD s_dwWriteCount = 0; //总共打印文件的次数
            DWORD dwWritedBytes = 0; //实际写入字节数

			if (NULL == m_hLogFile)
			{			
				CreatLogFile() ;  //创建或打开log文件
			}

			//读取文件开始标志 begin--
			int nFlag = 0 ;
			int nCharCount = 0 ;
			GetEncryptFlag(nFlag) ;
			if ( m_hLogFile )
			{
				//log文件为加密文件 但是此次写日志为明文 || log文件是明文 但是此次写日志需要加密
				//通过重命名备份源文件 并 创建新的文件进行日志打印
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

					CreatLogFile() ;  //创建新文件
				}
			}
			//--end

			if (m_hLogFile) //Log文件可用
            {
                //每写若干次检查一次大小                
                if (0 == s_dwWriteCount % MAX_CHK_SIZE_PER_TIMES)
                {
                    //获取文件大小
                    DWORD dwLogFileSize = GetFileSize(m_hLogFile, NULL);
                    if (INVALID_FILE_SIZE == dwLogFileSize)
                    {
                        OutputDebugString(_T("Error: run GetFileSize fail\r\n"));
                        CloseHandle(m_hLogFile);
                        m_hLogFile = NULL;
                    }
                    else if (dwLogFileSize > MAX_SIZE_OF_LOGFILE) //文件太大
                    {
                        CloseHandle(m_hLogFile);
                        m_hLogFile = NULL;
                        
						//暂时用来保存所有日志文件；
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
                        //新开一个文件
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

				//不管m_szBuffer是不是宽字符,都将它们转换成multiple string
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

				//加密日志信息 begin--
				if ( IsEncryptLog() )
				{
					pEncryptInfo = new char[nWriteStrLen * 2] ;
					memset(pEncryptInfo, 0, nWriteStrLen * 2) ;
					nWriteStrLen = m_base64Encrypt.Base64Encrypt(pEncryptInfo, pWriteStr, LOG_ENCRYPT_KEY) ;
					pEncryptInfo[++nWriteStrLen] = 0 ;  //作为分隔符
					pWriteStr = pEncryptInfo ;
				}
				//--end

				//写入日志文件  begin--
				if (0 == WriteFile(m_hLogFile,pWriteStr, nWriteStrLen, &dwWritedBytes, 0))
				{
					OutputDebugString(_T("Error: Write log file fail\r\n"));
					CloseHandle(m_hLogFile);
					m_hLogFile = NULL;
				}
				//--end

				//内存清理 begin--
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

				//每写一次日志就关闭掉日志文件，避免打印日志出现时序上的问题
				CloseHandle(m_hLogFile);
				m_hLogFile = NULL;

                s_dwWriteCount++;
            }
        }

    case LOGL_DBG:
    default:
        {
            //写Debug窗口
            OutputDebugString(m_szBuffer[1]);
        }
    }
}

bool CLogTrace::CreatLogFile()
{
	//打开日志文件
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
			//文件指针移到末尾
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
// 函数功能描述：文件开始写入加密标志
// 输入：
// 输出：
// 返回值：是否写入成功
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
bool CLogTrace::SetEncryptFlag()
{
	DWORD dwWritedBytes = 0 ;
	bool bRet = false ;

	//如果是写入加密内容 则在文件开始写入加密标记 begin--
	if ( IsEncryptLog() )
	{
		char *pEncryptFlag = new char[4] ;
		memset(pEncryptFlag, 0, 4) ;
		int nFlag = 1 ;
		memcpy(pEncryptFlag, &nFlag, sizeof(int) ) ; 

		//写入日志文件  begin--
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
// 函数功能描述：返回日志字串，用于日志窗口打印时取用
// 输入：
// 输出：
// 返回值：日志字串指针
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
const TCHAR * CLogTrace::GetLogBuf()
{
    return m_szBuffer[1];
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：设置日志窗口句柄
// 输入：日志窗口句柄
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
void CLogTrace::SetLogWnd(HWND hLogWnd)
{
    m_hLogWnd = hLogWnd;
}

bool CLogTrace::IsLogFileEnable()
{
	return (m_nOutLogInfo != 0);  //是否要创建日志文件
}

bool CLogTrace::IsEncryptLog()
{
	return (m_nEncryptLog != 0) ;  // 是否加密日志文件
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：对字符串进行压缩并编码
// 输入：strSource:要压缩的字符串
// 输出：strZipped:压缩之后的字符串
// 返回值：日志字串指针
// 其它说明：
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

	//传入数组的大小
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
//获取日志文本及打印颜色
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
