// SaveThread.h: interface for the CSaveThread class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_SAVETHREAD_H__64960593_7EA6_4892_9D30_5F9B9D76032D__INCLUDED_)
#define AFX_SAVETHREAD_H__64960593_7EA6_4892_9D30_5F9B9D76032D__INCLUDED_

#if _MSC_VER > 1000
#pragma once 

#endif // _MSC_VER > 1000

#include "IThreadUnit.h"
#include "IServerSocket.h"
#include "reg.h"
#include "ping.h"
#include "FtpFile.h"


class CPing;

class CUpdateThread  :public IThreadUnit
{
public:
	CUpdateThread();
	virtual ~CUpdateThread();


public:
	CFtpFile FtpFile;
	CPing    Ping;
	//消息分发
    DWORD DispatchMessage(T_Message *Msg);
	DWORD StartDownload(DWORD dwMsgParam,DWORD dwData);
	DWORD DefaultProc();

	DWORD CreateThread();

    DWORD DestoryThread()
    {
        return 0;
    }

private:

	DWORD GetFastServer(char *Serv,char *Use,char *psd,int &port);
	DWORD GetFastServerBak(char *Serv,char *Use,char *psd,int &port);
	DWORD TcharToChar(char *Dest,TCHAR *Src);


	BOOL GetNetFileFromFtp(DWORD dwIndex);
	BOOL GetNetFileFromHttp(DWORD dwIndex);

	DWORD GetDesCode(char *code,char *ip,char *account,char *psd,int &port);

	BOOL InitServer();

private:

	DWORD Flish;
	BOOL  bFtpDownload;
	CStdString  m_strHttpIp;
	int   iServerPort;
	BOOL  bIsinit;     //是否初始化帐号密码

};

#endif // !defined(AFX_SAVETHREAD_H__64960593_7EA6_4892_9D30_5F9B9D76032D__INCLUDED_)
