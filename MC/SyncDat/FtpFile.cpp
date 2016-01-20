#include "StdAfx.h"
#include ".\ftpfile.h"

CFtpFile::CFtpFile(void)
{
	m_UserProx = FALSE;

	m_strIP[0] = _T('\0');
	m_dwPort = 21;
	m_strUser[0] = _T('\0');
	m_strPwd[0] = _T('\0');
	m_strProx.Empty();
	//m_Mode  =  -1 ;//未定

    //modify by hpt 2013-3-5 
	//通过配置开关决定是主动连接FTP还被动连接FTP，
	//解决部分客户因网络设置导致被动模式下载文件失败的问题
	g_Config->ReadInteger(_T("SERVICE"), _T("FtpMode"), m_Mode);
	if (m_Mode != 0)
		m_Mode = 1;
}

CFtpFile::~CFtpFile(void)
{
	InternetCloseHandle(m_hHint);
}

BOOL CFtpFile::SetServerProx(CStdString strProxy)
{
	m_strProx.Format(_T("socks=%s"),strProxy);
	//m_strProx = _T("socks=198.18.68.220:1080");
	m_UserProx = TRUE;
	return TRUE;
}
BOOL CFtpFile::SetServerInfo(TCHAR *strIP,WORD dwPort,TCHAR *strUser,TCHAR *strPwd)
{
	

	m_dwPort = dwPort;
	_tcsncpy(m_strIP,strIP,100);
	_tcsncpy(m_strUser,strUser,100);
	_tcsncpy(m_strPwd,strPwd,100);

	return TRUE;
}
DWORD WINAPI CFtpFile::ConnectWork(LPVOID Param)
{
	CFtpFile *p = (CFtpFile *)Param;
	TCHAR LogBuff[300]={0};
	for(int i=0;i<3;i++)
	{
		if(-1 == p->m_Mode ||0 == p->m_Mode)
		{
			///被动模式连接
			g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("被动模式连接， start InternetConnect"));
			p->m_hConnect =
				InternetConnect(
				p->m_hHint,
				p->m_strIP,
				p->m_dwPort,
				p->m_strUser,
				p->m_strPwd,
				INTERNET_SERVICE_FTP,
				INTERNET_FLAG_PASSIVE,
				0);
			if(p->m_hConnect == NULL) //INTERNET_FLAG_PASSIVE
			{
				DWORD dwErr = GetLastError();
				p->m_isConnect = FALSE;
				_stprintf(LogBuff,_T("InternetConnect 被动连接失败 %s err = %d"),p->m_strIP,dwErr);
				g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, LogBuff);
				if(dwErr== 6||dwErr ==12002)
				{
					g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("连接失败退出 dwerr =6 or 12002"));
					return 1;
				}
			}
			else
			{
				p->m_Mode = 0;
				p->m_isConnect = TRUE;
				_stprintf(LogBuff,_T("FTP被动连接成功 %s"),p->m_strIP);
				g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, LogBuff);
				return 0;
			}

		}
		if(-1 == p->m_Mode ||1 == p->m_Mode)
		{
			///主动模式连接
			p->m_hConnect =
				InternetConnect(
				p->m_hHint,
				p->m_strIP,
				p->m_dwPort,
				p->m_strUser,
				p->m_strPwd,
				INTERNET_SERVICE_FTP,
				0,
				0);
			if(p->m_hConnect == NULL)
			{
				p->m_isConnect = FALSE;
				DWORD dwErr = GetLastError();
				_stprintf(LogBuff,_T("InternetConnect 主动连接失败 %s err = %d"),p->m_strIP,dwErr);
     			g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, LogBuff);
 				if(dwErr== 6||dwErr ==12002)
 				{
					g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("连接失败退出 dwerr =6 or 12002"));
					return 1;
 				}
			}
			else
			{
				p->m_isConnect = TRUE;
				_stprintf(LogBuff,_T("FTP主动连接成功 %s"),p->m_strIP);
				g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, LogBuff);
				return 0;
			}

		}
	}
	return 1;

}
BOOL CFtpFile::Connect()
{
	HANDLE hThread;
	DWORD dwThreadId;
	DWORD dwExitCode;
	
	
	if(m_UserProx == TRUE)
	{
		m_hHint = InternetOpen(NULL,INTERNET_OPEN_TYPE_PROXY,m_strProx.c_str(),NULL,INTERNET_FLAG_ASYNC);
	}
	else
	{
		m_hHint = InternetOpen(NULL,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,INTERNET_FLAG_ASYNC);
	}

	long timeout1 = 60000;
	long timeout2 = 120000;

	InternetSetOption(m_hHint,INTERNET_OPTION_CONNECT_TIMEOUT,(LPVOID)&timeout1,4);
	InternetSetOption(m_hHint,INTERNET_OPTION_SEND_TIMEOUT,(LPVOID)&timeout1,4);
	InternetSetOption(m_hHint,INTERNET_OPTION_RECEIVE_TIMEOUT,(LPVOID)&timeout2,4);
	InternetSetOption(m_hHint,INTERNET_OPTION_DATA_SEND_TIMEOUT,(LPVOID)&timeout1,4);
	InternetSetOption(m_hHint,INTERNET_OPTION_DATA_RECEIVE_TIMEOUT,(LPVOID)&timeout2,4);
	//wininet的超时设置是无效的，所以下面用线程来实现
	hThread = CreateThread(
		NULL,
		0,
		ConnectWork,
		this,
		0,
		&dwThreadId);
	//90秒都连接不上，就认为连不上
	if(WaitForSingleObject(hThread,90*1000)==WAIT_TIMEOUT)
	{
		if(m_hHint)
		{
			InternetCloseHandle(m_hHint);
			g_Datalog.Trace(LOGL_TOP,LOGT_WARNING, __TFILE__,__LINE__,_T("WaitTimeout 10s time not connect ftp"));
		}
		WaitForSingleObject(hThread, INFINITE);
		g_Datalog.Trace(LOGL_TOP,LOGT_WARNING, __TFILE__,__LINE__,_T("WaitThread Terminate"));
		m_hHint = NULL;
		return FALSE;
	}
	//g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("FTP连接完成"));
	dwExitCode = 0 ;
	GetExitCodeThread(hThread, &dwExitCode);
	g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("FTP连接完成 exitcode = %d"), dwExitCode);
	CloseHandle(hThread);
	if(dwExitCode)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CFtpFile::DisConnect()
{
	if(m_hConnect!=NULL)
	{
		InternetCloseHandle(m_hConnect);
		m_hConnect = NULL;
	}
	m_isConnect  = FALSE;
	return TRUE;
}

BOOL CFtpFile::ReConnect()
{
	DisConnect();
	return Connect();
}

BOOL CFtpFile::GetFile(TCHAR *RemotePath,TCHAR *LocalPath)
{
	
	if(!(m_isConnect && m_hConnect))
	{
		g_Datalog.Trace(LOGL_TOP,LOGT_WARNING, __TFILE__,__LINE__, _T("下载文件:%s失败，ftp未连接"),RemotePath);
		return FALSE;
	}
	BOOL bRet = FtpGetFile(m_hConnect, RemotePath,LocalPath , FALSE, 0, 1, 0);
	TCHAR buff[300]={0};	
	if(bRet == FALSE)
	{
		DWORD dwErr = GetLastError();
		switch(dwErr)
		{
		case ERROR_INTERNET_CONNECTION_ABORTED:	//网络中断
		case ERROR_INTERNET_CONNECTION_RESET:	//连接已经重置
		case ERROR_FILE_NOT_FOUND:			//文件未找到也要重新连接
			{
				_stprintf(buff,_T("FTP下载，连接已经重置 err=%d %s ,%s"),dwErr,RemotePath, LocalPath);
				g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, buff);
				ReConnect();	//失败就重新连接一次网络
			}
			break;

		default:
			{
				DWORD dwLastError = 0;
				DWORD dwLen = 512;
				TCHAR szErrMsg[512] = {0};
				InternetGetLastResponseInfo(&dwLastError, szErrMsg, &dwLen);
				_stprintf(buff,_T("FTP下载错误 err=%d, lasterr=%d :%s %s, %s"),dwErr, dwLastError, szErrMsg, RemotePath,LocalPath);
				g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, buff);

				ReConnect();
			}
			break;
		}

	}
	else
	{
		_stprintf(buff,_T("下载文件成功 %ls, %s"),RemotePath,LocalPath);
		g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, buff);
	}

	return bRet;
}