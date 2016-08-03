#include "StdAfx.h"
#include "serversocket.h"
#include "FileReadAndSave.h"

CServerSocket::CServerSocket(void)
{
    m_ProcessData.SetPtrOfServer(this);
}

CServerSocket::~CServerSocket(void)
{
}
 
//初始化
int CServerSocket::Init(int nPort)
{
	WSADATA wsaData;
	int errid;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
	{
	    errid = WSAGetLastError();
	    g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("WSAStartup，错误, error:%d"), errid);
		printf("WSAStartup() 错误:\n");
	}
	// 创建socket
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_socket == INVALID_SOCKET ) {
		errid = WSAGetLastError();
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("socket，错误, error:%d"), errid);
		printf( "socket() 错误: %ld\n", WSAGetLastError() );
		WSACleanup();
		return -1;
	}

	// 绑定socket
	sockaddr_in service;

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	service.sin_port = htons( nPort );

	if ( ::bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {

		::MessageBox(0,_T("舟大师运行时发现一个错误,\r\n请检查是否有第三方软件(如:防火墙)禁止了Mcproc.exe权限\r\n"),_T("\n运行时检测"),MB_OK|MB_ICONWARNING);
#ifdef _DEBUG
		CStdString strErro;
		strErro.Format(_T("bind 端口：%d ,错误码:%d "),nPort, GetLastError());
		::MessageBox(0,strErro.GetBuffer(0),_T("\n运行时检测"),MB_OK|MB_ICONWARNING);
#endif
		closesocket(m_socket);
		WSACleanup();
		return -1;
	}

	// 监听socket
	if ( listen( m_socket, 1 ) == SOCKET_ERROR )
	{
		::MessageBox(0,_T("舟大师运行时发现一个错误,\r\n请检查是否有第三方软件(如:防火墙)禁止了Mcproc.exe权限\r\n"),_T("\n运行时检测"),MB_OK|MB_ICONWARNING);
		closesocket(m_socket);
		WSACleanup();

		return -1;
	}
	m_ClientSocket = INVALID_SOCKET;
	m_bRun = TRUE;
	return 0;
}

//解除初始化
int CServerSocket::UnInit()
{
	if(m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	if (m_ClientSocket != INVALID_SOCKET)
	{
		shutdown(m_ClientSocket, 2); //断开发送接收功能
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;
	}
	
	WSACleanup();
	m_bRun = FALSE;

	return -1;
}

//等待客户端连接
int CServerSocket::WaitClient()
{
	// 接受
	g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("等待客户端连接..." ));
	m_ClientSocket = INVALID_SOCKET ;
	while ( m_ClientSocket == INVALID_SOCKET) {
		m_ClientSocket = accept( m_socket, NULL, NULL );

	}
	g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("客户端连接成功%d"), m_ClientSocket);
	return 0;
}
DWORD WINAPI ThreadRecv(PVOID param)
{
	CServerSocket *Server = (CServerSocket*)param;

	int bytesRecv = SOCKET_ERROR;
	char recvbuf[8192] = {0};	//接收区
	DWORD dwPackteCount = 0;
	char *pDataBuff = NULL;	//缓冲区
	DWORD dwDataBuffLen = 0;	//缓冲区长度

	Server->WaitClient();

	while(Server->m_bRun)
	{
		bytesRecv = recv( Server->m_ClientSocket, recvbuf, 8192, 0 );
		g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("收到控件发过来的数据,数据大小：%d"), bytesRecv);

		if( bytesRecv == -1 || bytesRecv == 0)	//客户端断开连接
		{
			Server->m_ClientSocket = INVALID_SOCKET ;
			g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("客户端断开连接，退出任务"));

			return 0;	//退出程序
		}

		if(pDataBuff == NULL)
		{
			pDataBuff = new char[bytesRecv];
			memcpy(pDataBuff,recvbuf,bytesRecv);
			dwDataBuffLen = bytesRecv;
		}
		else
		{
			char *tmpBuff = new char[dwDataBuffLen];
			memcpy(tmpBuff,pDataBuff,dwDataBuffLen);

			delete []pDataBuff;

			pDataBuff = new char[dwDataBuffLen + bytesRecv];

			memcpy(pDataBuff,tmpBuff,dwDataBuffLen);
			delete []tmpBuff;

			memcpy(&(pDataBuff[dwDataBuffLen]),recvbuf,bytesRecv);
			dwDataBuffLen += bytesRecv;
		}

		//包处理
		if(dwDataBuffLen<sizeof(DWORD)*2)continue; //小于最小包头不做如下处理 

		DWORD dwPacketSize = 0; //包大小
		while(TRUE)
		{
			if(dwDataBuffLen == 0)break; //缓冲区没有数据
			memcpy(&dwPacketSize,pDataBuff,sizeof(DWORD));
			if(dwPacketSize > dwDataBuffLen) 
				break; //包长度大于缓冲区长度不做处理
			char *pData = new char[dwPacketSize];	//拼一个包
			memcpy(pData,pDataBuff,dwPacketSize);

			Server->m_ProcessData.CovertBufData(pData,dwPacketSize);
			delete []pData;

			char *tmpBuff = new char[dwDataBuffLen];
			memcpy(tmpBuff,pDataBuff,dwDataBuffLen);

			delete []pDataBuff;
			dwDataBuffLen = dwDataBuffLen - dwPacketSize;
			pDataBuff = new char[dwDataBuffLen];
			memcpy(pDataBuff,&(tmpBuff[dwPacketSize]),dwDataBuffLen);
			delete []tmpBuff;
			dwPackteCount ++ ; //包计数器
		} 

		printf("总共收到数据包 %d\n",dwPackteCount); //包计数器
		Sleep(200);	//线程切换
	}
	return 0;
}
//接收数据

int CServerSocket::WaitRecData(BOOL IsThread)
{
	if(IsThread == FALSE)
 		ThreadRecv(this); //普通方式运行（阻塞进程）
	else
	{
		m_hThread = CreateThread(NULL,0,ThreadRecv,this,0,0); //线程方式运行（非阻塞进程）
		TCHAR cmd[100]={0};
		do
		{
			_tscanf_s(_T("%s"),cmd);
		}
		while(_tcsicmp(_T("exit"),cmd)!=0);
	}

	return 0;
}

int CServerSocket::SendData(DWORD dwLen,DWORD dwFlag,char *pBuff)
{
	//web版本的处理
	if (E_GET_EXCUTE_TASK_RESULT == dwFlag)
	{
		//只处理返回任务结果的数据,其他返回不处理
		return SendData( dwLen,pBuff);
	}

	return -1;
}


int CServerSocket::SendData(DWORD dwLen,char *pBuff)
{
	DWORD dwTotalLen = dwLen + 4;
	
	if ( 0==dwLen)
	{
		return 0;
	}

	if(m_ClientSocket != INVALID_SOCKET)
	{
		//先转换为窄字符

		DWORD dwLength = 0;
		
		char *buffer = new char[dwTotalLen];
		memset(buffer,0,dwTotalLen);		
		
		char *pBufferDest = buffer + 4;

		dwLength = dwLen;

		CFileReadAndSave::WCharToMByte((LPCWSTR)pBuff ,pBufferDest,&dwLength);

		//传送的字节数变了
		dwTotalLen = dwLength + 4;
		memcpy(buffer,&dwTotalLen,sizeof(DWORD));
		//已经在固定位置了,不需要拷贝内存了
		int nRet = send(m_ClientSocket,buffer,dwTotalLen,0);	//发送数据

		delete []buffer;
		return nRet;
	}

	return -1;
}



