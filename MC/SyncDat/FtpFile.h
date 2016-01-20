#pragma once
#include <windows.h>
#include <wininet.h>
#pragma comment(lib,"wininet.lib")

class CFtpFile
{
//private:
public:
	HINTERNET m_hHint;
	HINTERNET m_hConnect;
//private:
	TCHAR m_strIP[100];
	WORD m_dwPort;
	TCHAR m_strUser[100];
	TCHAR m_strPwd[100];
	CStdString m_strProx;
	BOOL m_isConnect;	//是否连接到服务器
	BOOL m_UserProx;
	int  m_Mode; // 主动被动模式，-1:初始化 0：被动，1：主动 记录上次的模式，下次直接可以用
public:
	CFtpFile(void);
	~CFtpFile(void);

	BOOL SetServerInfo(TCHAR *strIP,WORD dwPort,TCHAR *strUser,TCHAR *strPwd);
	BOOL SetServerProx(CStdString strProxy);
	BOOL Connect();
	BOOL DisConnect();
	BOOL ReConnect();
	BOOL GetFile(TCHAR *RemotePath,TCHAR *LocalPath);
	static DWORD WINAPI ConnectWork(LPVOID param); 
};
