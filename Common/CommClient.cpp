#include "StdAfx.h"
#include "commclient.h"
#include "shlobj.h"
#include "Trace.h"


CLogTrace g_commLog(_T("Core\\Comm.log"),NULL);


CCommClient::CCommClient(void)
{
	m_hThread = NULL;
	m_socket = NULL;
	m_bStop = false;
}

CCommClient::~CCommClient(void)
{
	DisConnect();
}

//连接到服务端
int  CCommClient::Init(int iPort, pfnDataHandler fn)
{
	if (-1 == Connect(iPort) || -1 == SetDataHandler(fn))
		return -1;
		
	return CreateRecvThread();
}

int CCommClient::SetDataHandler(pfnDataHandler fn)
{
	if (!fn)
	{
		g_commLog.Trace(LOGL_TOP,LOGC_ERROR,__TFILE__, __LINE__,_T("回调函数不能为空！"));
		return -1;
	}

	m_pfnDataHandler = fn;

	return 0;
}

//连接到服务端
int  CCommClient::Connect(int iPort)
{
	WSADATA wsaData;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
		return -1;

	// 创建socket
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_socket == INVALID_SOCKET ) {
		WSACleanup();
		return -1;
	}
	sockaddr_in service;

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	service.sin_port = htons(iPort);

	g_commLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("Connect %d"), iPort);
	DWORD dwBeginTime = GetTickCount();
	while(connect(m_socket,(SOCKADDR*)&service,sizeof(service))==SOCKET_ERROR)
	{
		Sleep(60);
		if (GetTickCount() - dwBeginTime > 10000)
		{
			g_commLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("Connect timeout!"));
			return -1;
		}
	}

	g_commLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("Connect success"));

	return 0;
}

//与服务端断开连接
DWORD CCommClient::DisConnect(void)
{
    if(m_socket!=NULL)
	{
		g_commLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("closesocket"));
		closesocket(m_socket);
	}
	return 0;
}

int CCommClient::SendData(TCHAR *pData,int nLen)
{
	//防止多线程调用出错
	CLocalLock lock(&m_lockSend);

	if(m_socket == NULL)
	{
		return -1;
	}

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

	int nSendByte = send(m_socket,(char*)buff,dwLen,0);

	delete []buff;

	return nSendByte;
}

//发送数据到服务进程
int CCommClient::SendData(CStdString strData)
{
	int iLen = strData.GetLength();
	return SendData(strData.GetBuffer(), iLen + 1);

}

//创建接收线程
DWORD CCommClient::CreateRecvThread(void)
{	
	DWORD dwThreadID = 0;
	m_hThread = ::CreateThread(NULL, 0, 
		StaticThreadFunc, 
		this, 0, &dwThreadID);

	if (!m_hThread || !dwThreadID) //线程创建失败
	{
		g_commLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("创建接收线程失败"));
		return -1;
	}
	return 0;
}

//线程静态函数
DWORD WINAPI CCommClient::StaticThreadFunc(LPVOID lpParam)
{
	CCommClient *pChannel = (CCommClient*)lpParam;
	SOCKET ClientSocket = pChannel->m_socket;

	int bytesRecv = SOCKET_ERROR;
	TCHAR recvbuf[8192] = {0};	//接收区
	DWORD dwPackteCount = 0;
	TCHAR *pDataBuff = NULL;	//缓冲区
	DWORD dwDataBuffLen = 0;	//缓冲区长度

	if(ClientSocket==NULL)
	{
		return 0;
	}
	while(!pChannel->StopIsSet())
	{
		bytesRecv = recv( ClientSocket, (char*)recvbuf, 8192, 0 );

		if( bytesRecv <= 0 )	//服务端断开连接 等待连接
		{
			g_commLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("服务器断开连接 err:%d"), WSAGetLastError());
			
			pChannel->m_pfnDataHandler(_T("DISCONNECT"),10);


			ClientSocket = NULL;
			return 0;
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

			pChannel->m_pfnDataHandler(pData,dwPacketSize - 4);
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

			pChannel->m_pfnDataHandler(pData,dwPacketSize);
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
	g_commLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("检测到停止标记，接收线程退出！"));


	return 0;
}

//停止数据接收线程
void CCommClient::Stop(void)
{
	m_bStop = true;

	DisConnect();
}

bool CCommClient::StopIsSet(void)
{
	return m_bStop;
}