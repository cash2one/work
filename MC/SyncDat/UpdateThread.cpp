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
	g_Config->ReadInteger(_T("UPDATE"),_T("count"),count);
	
	printf("��ȡ�������б�\n");
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
		CStdString str = _T("");
		CString strTmp = _T("");
		CString strValue = _T("");
		g_Config->ReadString(_T("UPDATE"),con,str);
		strValue = str;
		//strValue = _T("118.194.133.119:21:pengxiao:123456");
		des.Decrypt(strTmp,strValue);

		_stprintf(Temp,_T("%s"),strTmp.GetBuffer());

		memset(ctemp,0,100);
		TcharToChar(ctemp,Temp);

// 		char p = 'A';
// 		for(int j = 0;j<50;j++)
// 		{
// 			ctemp[j] = ctemp[j] ^ p;
// 		}

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
/*		DWORD ret;*/

// 		ret = Ping.PingHost(Server);

// 		if((Pin > ret) && (ret >= 0))
// 		{
// 			Pin = ret;
// 			memset(Serv,0,30);
// 			memset(Use,0,100);
// 			memset(psd,0,50);
// 			strcpy(Serv,Server);
// 			strcpy(Use,User);
// 			strcpy(psd,pwd);
// 			port = ServerPort;
// 
// 			return 10;
// 		}
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
// ��������������Save�߳���Ϣ�ַ�
// ���룺
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
DWORD CUpdateThread::DispatchMessage(T_Message *Msg)
{
    DWORD dwRet = 0;


    switch (Msg->dwMsgParam)
    {
	case E_FTP_GET_FILE_FROM_SERVER:  //������վdat�ļ������������ļ�
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
	if (!bIsinit)
	{
		InitServer();
		bIsinit =TRUE;
	}

    switch (dwMsgParam)
    {
	case E_FTP_GET_FILE_FROM_SERVER:  //��ȡ������վ�Ļ�����Ϣ
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
		g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("����: GetNetFileFromFtp(), pFtpFileNameΪ��"));
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

	if (!bRet)  //ftp����ʧ��ʱתhttp
	{
		bFtpDownload = FALSE;

		g_Config->ReadInteger(_T("SERVICE"), _T("HttpPort"), iServerPort);
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
		g_Datalog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("����: GetNetFileFromFtp(), pFtpFileNameΪ��"));
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
	g_Config->ReadInteger(_T("UPDATEBAK"),_T("count"),count);

	printf("��ȡ�������б�\n");
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
		CStdString str = _T("");
		CString strTmp = _T("");
		CString strValue = _T("");
		g_Config->ReadString(_T("UPDATEBAK"),con,str);
		strValue = str;
		//strValue = _T("118.194.133.119:21:pengxiao:123456");
		des.Decrypt(strTmp,strValue);

		_stprintf(Temp,_T("%s"),strTmp.GetBuffer());

		memset(ctemp,0,100);
		TcharToChar(ctemp,Temp);

		// 		char p = 'A';
		// 		for(int j = 0;j<50;j++)
		// 		{
		// 			ctemp[j] = ctemp[j] ^ p;
		// 		}

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
		/*DWORD ret;

		if(count == 1)
		{
			ret = 10;
		}
		else
		{
			ret = Ping.PingHost(Server);
		}

		if((Pin > ret) && (ret >= 0))
		{
			Pin = ret;
			memset(Serv,0,30);
			memset(Use,0,100);
			memset(psd,0,50);
			strcpy(Serv,Server);
			strcpy(Use,User);
			strcpy(psd,pwd);
			port = ServerPort;
		}*/
	}
	return 1;

}

//��ʼ����������ַ�������
BOOL CUpdateThread::InitServer()
{
	char Server[30] = {0};
	char User[100] = {0};
	char pwd[50] = {0};
	int  iCnt = 0;

	GetFastServer(Server, User, pwd, iServerPort);

Start:
	iCnt++; // �л�����������1
	TCHAR FastIP[30] = {0};
	int nChar = MultiByteToWideChar(CP_ACP, 0, Server, -1, FastIP, 30);

	//	g_Config->WriteString(_T("SERVICE"),_T("Current"),FastIP);

	TCHAR strServer[50]={0};
	TCHAR strUser[100]={0};
	TCHAR strPwd[50]={0};
	MultiByteToWideChar(CP_ACP, 0, Server, -1, strServer, 30);
	MultiByteToWideChar(CP_ACP, 0, User, -1, strUser, 100);
	MultiByteToWideChar(CP_ACP, 0, pwd, -1, strPwd, 50);		

	int iHttpDown = 0;
	g_Config->ReadInteger(_T("SERVICE"), _T("HttpDown"), iHttpDown);

	if (iHttpDown == 0)
	{
		FtpFile.SetServerInfo(strServer,iServerPort,strUser,strPwd);
		if (!FtpFile.Connect())
		{
			if (iCnt == 1)
			{
				GetFastServerBak(Server, User, pwd, iServerPort);      //���÷�����
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
		g_Config->ReadInteger(_T("SERVICE"), _T("HttpPort"), iServerPort);
		if (iServerPort == 0 || iServerPort == 21)
			iServerPort = 8072;

		g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("������Ϊhttp����,������ser:%s ,port:%d"), strServer, iServerPort);
	}
	else
	{
		g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("������Ϊftp����,������ser:%s ,port:%d"), strServer, iServerPort);
	}

	return TRUE;
}

