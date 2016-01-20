#ifndef _CSERVERSOCKET__
#define _CSERVERSOCKET__

#include "IServerSocket.h"
#include "ProcessData.h"

#pragma comment(lib, "wsock32.lib")
 

class CServerSocket : public IServerSocket
{
private:
	HANDLE m_hThread;
	BOOL m_bRun;
	SOCKET m_socket;
	SOCKET m_ClientSocket;
	int WaitClient(); //�ȴ��ͻ�������
    CProcessData m_ProcessData;
public:
	CServerSocket(void);
	~CServerSocket(void);
	int Init(int nPort);	//��ʼ��
	int UnInit();	//�����ʼ��
	int WaitRecData(BOOL IsThread);	//��������
	int SendData(DWORD dwLen,DWORD Flag,char *buff);
	int SendData(DWORD dwLen,char *buff);  // web �汾ֱ�ӷ����ַ���,��Ҫת��Ϊխ�ַ�?
	friend DWORD WINAPI ThreadRecv(PVOID param);
};
#endif