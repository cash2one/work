// SaveThread.cpp: implementation of the CSaveThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UpdateThread.h"
#include "HttpUtils.h"
#include <stdlib.h>
#include <shlobj.h>
#include "DES.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUpdateThread::CUpdateThread()
:IThreadUnit(E_UPDATE_THREAD,0xFFFF)
{
	Flish = 0;
	bFtpDownload = TRUE;
	iServerPort = 0;
	bIsinit = FALSE;
}

DWORD CUpdateThread::CreateThread()
{
	return 0;
}

DWORD CUpdateThread::GetFastServer(char *Serv,char *Use,char *psd,int &port)
{
	TCHAR UserName[100] = {0};
	TCHAR PassWord[50] = {0};
	TCHAR Temp[100] = {0};
	int   ServerPort;
	DWORD Pin = 9999;
	CDES des;

	int count = 0;
	g_Config->ReadInt(_T("MC"), _T("UPDATE"), _T("count"), count);
	
	printf("获取服务器列表\n");
	TCHAR con[10] = {0};
	char ctemp[100] = {0};
	char Server[30] = {0};
	char User[100] = {0};
	char pwd[50] = {0};

	for(int i = 1;i <= count;i++)
	{
		ZeroMemory(Temp, sizeof(Temp));
		ZeroMemory(con, sizeof(con));
		wsprintf(con,_T("Code%d"),i);		
		CString strTmp = _T("");
		CString strValue = _T("");
		g_Config->ReadString(_T("MC"), _T("UPDATE"), con, strValue);		
		//strValue = _T("118.194.133.119:21:pengxiao:123456");
		des.Decrypt(strTmp,strValue);

		_stprintf(Temp,_T("%s"),strTmp.GetBuffer());

		memset(ctemp,0,100);
		TcharToChar(ctemp,Temp);
		memset(Server,0,30);
		memset(User,0,100);
		memset(pwd,0,50);
		GetDesCode(ctemp,Server,User,pwd,ServerPort);

		if(i == 1)
		{
			memset(Serv,0,30);
			memset(Use,0,100);
			memset(psd,0,50);
			strcpy(Serv,Server);
			strcpy(Use,User);
			strcpy(psd,pwd);
			port = ServerPort;
		}
		printf("IP = [%s] \n",Server);
	}
	return 1;
}

CUpdateThread::~CUpdateThread()
{
}

DWORD CUpdateThread::TcharToChar(char *Dest,TCHAR *Src)
{
	int iLen = 2*wcslen(Src);
	char* chRtn = new char[iLen+1];
	wcstombs(chRtn,Src,iLen+1);
	sprintf(Dest,"%s",chRtn);
	delete chRtn;

	return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：Save线程消息分发
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CUpdateThread::DispatchMessage(T_Message *Msg)
{
    DWORD dwRet = 0;
	if (!bIsinit)
	{
		InitServer();
		bIsinit = TRUE;
	}

    switch (Msg->dwMsgParam)
    {
	case E_FTP_GET_FILE_FROM_SERVER:  //下载网站dat文件及其他单个文件
        {
			if (bFtpDownload)
			{
				StartDownload(E_FTP_GET_FILE_FROM_SERVER,Msg->dwData);
			}
			else
			{
				StartDownload(E_HTTP_GET_FILE_FROM_SERVER,Msg->dwData);
			}
			
        }
		break;
	case MSG_TIMER:
		{
			
		}
		break;
    default:
        break;
    }
    return 0;
}

DWORD CUpdateThread::StartDownload(DWORD dwMsgParam,DWORD dwData)
{
    switch (dwMsgParam)
    {
	case E_FTP_GET_FILE_FROM_SERVER:  //获取单个网站的基本信息
        {
			printf("E_FTP_GET_FILE_FROM_SERVER-----------------------\n");
			if(GetNetFileFromFtp(dwData) == RET_SUCCESS)
				Flish++;

			PostMessage(E_WEBSERVICE_THREAD,E_FTP_GET_FILE_FROM_SERVER_SAVED,dwData);
		}
		break;
	case E_HTTP_GET_FILE_FROM_SERVER:
		{
			printf("E_HTTP_GET_FILE_FROM_SERVER-----------------------\n");
			if(GetNetFileFromHttp(dwData) == RET_SUCCESS)
				Flish++;

			PostMessage(E_WEBSERVICE_THREAD,E_FTP_GET_FILE_FROM_SERVER_SAVED,dwData);
		}
		break;
    default:
        break;
    }

    return 0;
}

DWORD CUpdateThread::DefaultProc()
{

	return 0;
}
BOOL CUpdateThread::GetNetFileFromFtp(DWORD dwIndex)
{

	T_FtpFileName *pFtpFileName = NULL;
	pFtpFileName =	(T_FtpFileName *)g_GlobalDataBlock->ReadRealData(E_FtpFileName,dwIndex,E_WEBSERVICE_CLASS,eUseCommReadWithMulti);
	if(NULL == pFtpFileName)
	{
		g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("错误: GetNetFileFromFtp(), pFtpFileName为空"));
		return FALSE;

	}
	BOOL bRet = FALSE;
	TCHAR RemoteFile[MAX_PATH] = {0};
	TCHAR LocalFile[MAX_PATH]={0};

	{
		//_stprintf(RemoteFile,_T("/%d/%s"),pFtpFileName->dwFileType,pFtpFileName->szServerFileName);
		_stprintf(RemoteFile,_T("/%s"),pFtpFileName->szServerFileName);
		_tcscpy(LocalFile,pFtpFileName->szLocalFileName);
		
		for(int j=0;j<3;j++)
		{
			bRet = FtpFile.GetFile(RemoteFile,LocalFile);
			if(bRet==TRUE)
				break;
		
		}
	}

	if (!bRet)  //ftp下载失败时转http
	{
		bFtpDownload = FALSE;

		g_Config->ReadInt(_T("MC"), _T("SERVICE"), _T("HttpPort"), iServerPort);
		if (iServerPort == 0 || iServerPort == 21)
			iServerPort = 8072;

		bRet = GetNetFileFromHttp(dwIndex);
	}
    return bRet;

}

DWORD CUpdateThread::GetDesCode(char *code,char *ip,char *account,char *psd,int &port)
{
	int i = 0;
	char *p[4];
	char *buf = code;
	while((p[i]=strtok(buf,":"))!=NULL) 
	{
		//printf("str = [%s]\n",p[i]);
		if(i == 0)
		{
			sprintf(ip,"%s",p[i]);
		}
		if(i == 1)
		{
			char strport[10] = {0};
			sprintf(strport,"%s",p[i]);
			port = atoi(strport);
		}
		if(i == 2)
		{
			sprintf(account,"%s",p[i]);
		}
		if(i == 3)
		{
			sprintf(psd,"%s",p[i]);
		}
		i++;
		if(i == 4)
			break;
		buf=NULL; 
	}
	return 1;
}

BOOL CUpdateThread::GetNetFileFromHttp( DWORD dwIndex )
{
	T_FtpFileName *pFtpFileName = NULL;
	pFtpFileName =	(T_FtpFileName *)g_GlobalDataBlock->ReadRealData(E_FtpFileName,dwIndex,E_WEBSERVICE_CLASS,eUseCommReadWithMulti);
	

	if(NULL == pFtpFileName)
	{
		g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("错误: GetNetFileFromFtp(), pFtpFileName为空"));
		return FALSE;
	}
	
	TCHAR RemoteFile[MAX_PATH] = {0};
	TCHAR LocalFile[MAX_PATH]={0};

	_stprintf(RemoteFile,_T("http://%s:%d/%s"), m_strHttpIp.c_str(), iServerPort, pFtpFileName->szServerFileName);
	_tcscpy(LocalFile,pFtpFileName->szLocalFileName);
	
	return CHttpUtils::GetFile(RemoteFile,LocalFile);
}

DWORD CUpdateThread::GetFastServerBak( char *Serv,char *Use,char *psd,int &port )
{
	TCHAR UserName[100] = {0};
	TCHAR PassWord[50] = {0};
	TCHAR Temp[100] = {0};
	int   ServerPort;
	DWORD Pin = 9999;
	CDES des;

	int count = 0;
	g_Config->ReadInt(_T("MC"), _T("UPDATEBAK"), _T("count"), count);

	printf("获取服务器列表\n");
	TCHAR con[10] = {0};
	char ctemp[100] = {0};
	char Server[30] = {0};
	char User[100] = {0};
	char pwd[50] = {0};

	for(int i = 1;i <= count;i++)
	{
		ZeroMemory(Temp, sizeof(Temp));
		ZeroMemory(con, sizeof(con));
		wsprintf(con,_T("Code%d"),i);
		CString strTmp = _T("");
		CString strValue = _T("");
		g_Config->ReadString(_T("MC"),_T("UPDATEBAK"),con,strValue);
		//strValue = _T("118.194.133.119:21:pengxiao:123456");
		des.Decrypt(strTmp,strValue);

		_stprintf(Temp,_T("%s"),strTmp.GetBuffer());

		memset(ctemp,0,100);
		TcharToChar(ctemp,Temp);
		memset(Server,0,30);
		memset(User,0,100);
		memset(pwd,0,50);
		GetDesCode(ctemp,Server,User,pwd,ServerPort);

		if(i == 1)
		{
			memset(Serv,0,30);
			memset(Use,0,100);
			memset(psd,0,50);
			strcpy(Serv,Server);
			strcpy(Use,User);
			strcpy(psd,pwd);
			port = ServerPort;
		}
		printf("IP = [%s] \n",Server);
	}
	return 1;

}

//初始化服务器地址及密码等
BOOL CUpdateThread::InitServer()
{
	char Server[30] = {0};
	char User[100] = {0};
	char pwd[50] = {0};
	int  iCnt = 0;

	GetFastServer(Server, User, pwd, iServerPort);

Start:
	iCnt++; // 切换服务器递增1
	TCHAR FastIP[30] = {0};
	int nChar = MultiByteToWideChar(CP_ACP, 0, Server, -1, FastIP, 30);
	TCHAR strServer[50]={0};
	TCHAR strUser[100]={0};
	TCHAR strPwd[50]={0};
	MultiByteToWideChar(CP_ACP, 0, Server, -1, strServer, 30);
	MultiByteToWideChar(CP_ACP, 0, User, -1, strUser, 100);
	MultiByteToWideChar(CP_ACP, 0, pwd, -1, strPwd, 50);		

	int iHttpDown = 0;
	g_Config->ReadInt(_T("MC"), _T("SERVICE"), _T("HttpDown"), iHttpDown);

	if (iHttpDown == 0)
	{
		FtpFile.SetServerInfo(strServer,iServerPort,strUser,strPwd);
		if (!FtpFile.Connect())
		{
			if (iCnt == 1)
			{
				GetFastServerBak(Server, User, pwd, iServerPort);      //备用服务器
			}
			bFtpDownload = FALSE;
		}
	}
	else
	{
		bFtpDownload = FALSE;
	}

	m_strHttpIp = strServer;
	if (!bFtpDownload)
	{
		g_Config->ReadInt(_T("MC"), _T("SERVICE"), _T("HttpPort"), iServerPort);
		if (iServerPort == 0 || iServerPort == 21)
			iServerPort = 8072;

		g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("本连接为http连接,服务器ser:%s ,port:%d"), strServer, iServerPort);
	}
	else
	{
		g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("本连接为ftp连接,服务器ser:%s ,port:%d"), strServer, iServerPort);
	}

	return TRUE;
}

