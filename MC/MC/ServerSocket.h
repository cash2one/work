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
	int WaitClient(); //等待客户端连接
    CProcessData m_ProcessData;
public:
	CServerSocket(void);
	~CServerSocket(void);
	int Init(int nPort);	//初始化
	int UnInit();	//解除初始化
	int WaitRecData(BOOL IsThread);	//接收数据
	int SendData(DWORD dwLen,DWORD Flag,char *buff);
	int SendData(DWORD dwLen,char *buff);  // web 版本直接发送字符串,需要转码为窄字符?
	friend DWORD WINAPI ThreadRecv(PVOID param);
};
#endif