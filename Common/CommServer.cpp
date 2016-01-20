
#include "stdafx.h"
#include "commserver.h"

CCommServer::CCommServer(void)
{
}

CCommServer::~CCommServer(void)
{
}

 
//初始化
int CCommServer::Init(int nPort, pfnDataHandler fn)
{
	WSADATA wsaData;
	int errid;

	if (!fn)
	{
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("回调函数不能为空！"));
		return -1;
	}

	m_pfnDataHandler = fn;


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

	if ( ::bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) 
	{
#ifdef _DEBUG
		::MessageBox(0,_T("舟大师运行时发现一个错误,\r\n请检查是否有第三方软件(如:防火墙)禁止了Mcproc.exe权限\r\n"),_T("\n运行时检测"),MB_OK|MB_ICONWARNING);

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
#ifdef _DEBUG
		::MessageBox(0,_T("舟大师运行时发现一个错误,\r\n请检查是否有第三方软件(如:防火墙)禁止了Mcproc.exe权限\r\n"),_T("\n运行时检测"),MB_OK|MB_ICONWARNING);
#endif
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("listen( m_socket, 1 ) == SOCKET_ERROR"));
		closesocket(m_socket);
		WSACleanup();

		return -1;
	}

	/*BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	if (setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOptVal, bOptLen) != SOCKET_ERROR) 
	{
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置发送KEEPALIVE, error:%d"), WSAGetLastError());
	}*/

	//设置发送超时(10秒）
	int iTimeOut = 1000*50;
	//int iTimeOut = 1000*100;
	if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char*) &iTimeOut,sizeof(int)) == SOCKET_ERROR)
	{
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置发送超时错误, error:%d"), WSAGetLastError());
	}

	//struct linger m_sLinger;
	//m_sLinger.l_onoff = 1; //在调用closesocket()时还有数据未发送完，允许等待
	//// 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭
	//m_sLinger.l_linger = 5; //设置等待时间为5秒
	//if(setsockopt( m_socket, SOL_SOCKET, SO_LINGER, (const char*)&m_sLinger, sizeof(struct linger)))
	//{
	//	g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置发送超时错误, error:%d"), WSAGetLastError());
	//}

	m_ClientSocket = INVALID_SOCKET ;
	m_bRun = TRUE;

	WaitRecData(TRUE);

	return 0;
}

//解除初始化
int CCommServer::UnInit()
{
	//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("-------------UnInit------------"));
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

int CCommServer::CloseClient()
{
	//g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("-------------CloseClient------------"));
	//优雅地关闭SOCKET的方式
	shutdown(m_ClientSocket,SD_SEND);
	closesocket(m_ClientSocket) ;
	m_ClientSocket = INVALID_SOCKET;
	return -1 ;
}

//等待客户端连接
int CCommServer::WaitClient()
{
	// 接受
	g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("等待客户端连接..." ));
	m_ClientSocket = INVALID_SOCKET;
	while ( m_ClientSocket == INVALID_SOCKET ) {
		m_ClientSocket = accept( m_socket, NULL, NULL );
	}
	g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("客户端连接成功！" ));
	return 0;
}
DWORD WINAPI ThreadRecv(PVOID param)
{
	CCommServer *Server = (CCommServer*)param;

WaitConnect:
	Server->WaitClient();
	int bytesRecv = SOCKET_ERROR;
	TCHAR recvbuf[8192] = {0};	//接收区
	//TCHAR recvbuf[1024] = {0};	//接收区
	DWORD dwPackteCount = 0;
	TCHAR *pDataBuff = NULL;	//缓冲区
	DWORD dwDataBuffLen = 0;	//缓冲区长度
	while(Server->m_bRun)
	{
		bytesRecv = recv( Server->m_ClientSocket, (char*)recvbuf, 8192, 0 );
		//bytesRecv = recv( Server->m_ClientSocket, (char*)recvbuf, 1024, 0 );
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("==========================>>>>>>>>>>>>WSAGetLastErrorWSAGetLastError err:%d"), WSAGetLastError());
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("%%%%%%%%%%%%%%%%%%%%%%%%%%bytesRecv = %d"),bytesRecv);
		if( bytesRecv == -1 || bytesRecv == 0)	//客户端断开连接 等待连接
		{
			if (bytesRecv == -1)
			{
				Server->m_pfnDataHandler(_T("DISCONNECT"),10);
				g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("客户端异常退出，结束4代所有事件"));
			}
			Server->m_ClientSocket = INVALID_SOCKET ;
			g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("客户端断开连接，退出任务 err:%d"), WSAGetLastError());
			break;   //连接断开，退出循环
		}

#ifdef _UNICODE		
		bytesRecv /= 2;  //UNICODE下收到的字符数为字节数/2
		if(pDataBuff == NULL)
		{
			pDataBuff = new TCHAR[bytesRecv];
			wmemcpy(pDataBuff,recvbuf,bytesRecv);
			dwDataBuffLen = bytesRecv;
		}
		else
		{
			TCHAR *tmpBuff = new TCHAR[dwDataBuffLen];
			wmemcpy(tmpBuff,pDataBuff,dwDataBuffLen);

			delete []pDataBuff;

			pDataBuff = new TCHAR[dwDataBuffLen + bytesRecv];

			wmemcpy(pDataBuff,tmpBuff,dwDataBuffLen);
			delete []tmpBuff;

			wmemcpy(&(pDataBuff[dwDataBuffLen]),recvbuf,bytesRecv);
			dwDataBuffLen += bytesRecv;
		}

		//包处理
		if(dwDataBuffLen<sizeof(DWORD)*2)continue; //小于最小包头不做如下处理 

		DWORD dwPacketSize = 0; //包大小
		while(TRUE)
		{
			if(dwDataBuffLen == 0)break; //缓冲区没有数据
			memcpy(&dwPacketSize,pDataBuff,sizeof(DWORD));
			dwPacketSize /= 2;       //UNICODE下接收到包的实际长度为收到的字符数*2
			if(dwPacketSize > dwDataBuffLen)        
				break; //包长度大于缓冲区长度不做处理

			TCHAR *pData = new TCHAR[dwPacketSize];	//拼一个包
			wmemcpy(pData,pDataBuff + 4,dwPacketSize - 4);

			Server->m_pfnDataHandler(pData,dwPacketSize - 4);
			delete []pData;

			TCHAR *tmpBuff = new TCHAR[dwDataBuffLen];
			wmemcpy(tmpBuff,pDataBuff,dwDataBuffLen);

			delete []pDataBuff;
			pDataBuff = NULL;

			dwDataBuffLen = dwDataBuffLen - dwPacketSize;
			pDataBuff = new TCHAR[dwDataBuffLen];
			wmemcpy(pDataBuff,&(tmpBuff[dwPacketSize]),dwDataBuffLen);
			delete []tmpBuff;
			dwPackteCount ++ ; //包计数器
		} 

		printf("总共收到数据包 %d\n",dwPackteCount); //包计数器
		//Sleep(200);	//线程切换
	}
#else
		if(pDataBuff == NULL)
		{
			pDataBuff = new TCHAR[bytesRecv];
			memcpy(pDataBuff,recvbuf,bytesRecv);
			dwDataBuffLen = bytesRecv;
		}
		else
		{
			TCHAR *tmpBuff = new TCHAR[dwDataBuffLen];
			memcpy(tmpBuff,pDataBuff,dwDataBuffLen);

			delete []pDataBuff;

			pDataBuff = new TCHAR[dwDataBuffLen + bytesRecv];

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
			//if(dwDataBuffLen == 0)break; //缓冲区没有数据
			memcpy((WCHAR*)&dwPacketSize,pDataBuff,sizeof(DWORD));
			if(dwPacketSize > dwDataBuffLen)        
				break; //包长度大于缓冲区长度不做处理

			TCHAR *pData = new TCHAR[dwPacketSize];	//拼一个包
			memcpy(pData,pDataBuff + 4,dwPacketSize);

			Server->m_pfnDataHandler(pData,dwPacketSize);
			delete []pData;

			TCHAR *tmpBuff = new TCHAR[dwDataBuffLen];
			memcpy(tmpBuff,pDataBuff,dwDataBuffLen);

			delete []pDataBuff;
			pDataBuff = NULL;

			dwDataBuffLen = dwDataBuffLen - dwPacketSize;
			pDataBuff = new TCHAR[dwDataBuffLen];
			memcpy(pDataBuff,&(tmpBuff[dwPacketSize]),dwDataBuffLen);
			delete []tmpBuff;
			dwPackteCount ++ ; //包计数器
		} 

		printf("总共收到数据包 %d\n",dwPackteCount); //包计数器
		//Sleep(200);	//线程切换
	}
#endif

	if (Server->m_bRun)
		goto WaitConnect;

	return 0;
}
//接收数据

int CCommServer::WaitRecData(BOOL IsThread)
{
	if(IsThread == FALSE)
 		ThreadRecv(this); //普通方式运行（阻塞进程）
	else
		m_hThread = CreateThread(NULL,0,ThreadRecv,this,0,0); //线程方式运行（非阻塞进程）

	return 0;
}

int CCommServer::SendData(DWORD dwLen,DWORD dwFlag,char *pBuff)
{
    //防止多线程调用出错
    CLocalLock lock(&m_lockSend);

	DWORD dwTotalLen = dwLen + 8;
	if(m_ClientSocket != INVALID_SOCKET)
	{
		char *buffer = new char[dwTotalLen];
		memcpy(buffer,&dwTotalLen,sizeof(DWORD));
		memcpy(buffer + sizeof(DWORD),&dwFlag,sizeof(DWORD));
		memcpy(buffer + sizeof(DWORD)*2,pBuff,dwLen);
		int nRet = send(m_ClientSocket,buffer,dwTotalLen,0);	//发送数据

		delete []buffer;
		return nRet;
	}
	return -1;
}


//发送数据到服务进程
int CCommServer::SendData(TCHAR *pData,int nLen)
{
	//防止多线程调用出错
	CLocalLock lock(&m_lockSend);
	
	if(m_ClientSocket == INVALID_SOCKET )return -1;

	DWORD dwLen = (nLen + 1 + sizeof(DWORD)) * sizeof(TCHAR);
	TCHAR *buff=new TCHAR[dwLen];
	ZeroMemory(buff, dwLen * sizeof(TCHAR));

#ifdef _UNICODE
	memcpy_s((char*)buff, sizeof(DWORD), (char*)&dwLen, sizeof(DWORD));
	wmemcpy_s(&buff[sizeof(DWORD)], dwLen - sizeof(DWORD) - sizeof(TCHAR), pData, nLen);
#else
	memcpy(buff, sizeof(DWORD), &dwLen,sizeof(DWORD));
	memcpy(&buff[sizeof(DWORD)], dwLen - sizeof(DWORD) - sizeof(TCHAR), pData,nLen);
#endif

	int nSendByte = send(m_ClientSocket,(char*)buff,dwLen,0);

	delete []buff;

	return nSendByte;
}

int CCommServer::SendData(CStdString strData)
{
	char *pSend = NULL;
	DWORD dwLen = 0;
	int iLen = strData.GetLength();

	return SendData(strData.GetBuffer(), iLen + 1);
}

bool CCommServer::IsClientConnected() const
{
	if (m_ClientSocket != INVALID_SOCKET )
		return true;

	return false;
}
