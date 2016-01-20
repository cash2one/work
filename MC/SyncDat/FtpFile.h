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
	BOOL m_isConnect;	//�Ƿ����ӵ�������
	BOOL m_UserProx;
	int  m_Mode; // ��������ģʽ��-1:��ʼ�� 0��������1������ ��¼�ϴε�ģʽ���´�ֱ�ӿ�����
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
