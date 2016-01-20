
#include "stdafx.h"
#include "commserver.h"

CCommServer::CCommServer(void)
{
}

CCommServer::~CCommServer(void)
{
}

 
//��ʼ��
int CCommServer::Init(int nPort, pfnDataHandler fn)
{
	WSADATA wsaData;
	int errid;

	if (!fn)
	{
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("�ص���������Ϊ�գ�"));
		return -1;
	}

	m_pfnDataHandler = fn;


	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
	{
	    errid = WSAGetLastError();
	    g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("WSAStartup������, error:%d"), errid);
		printf("WSAStartup() ����:\n");
	}
	// ����socket
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_socket == INVALID_SOCKET ) {
		errid = WSAGetLastError();
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("socket������, error:%d"), errid);
		printf( "socket() ����: %ld\n", WSAGetLastError() );
		WSACleanup();
		return -1;
	}

	// ��socket
	sockaddr_in service;

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	service.sin_port = htons( nPort );

	if ( ::bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) 
	{
#ifdef _DEBUG
		::MessageBox(0,_T("�۴�ʦ����ʱ����һ������,\r\n�����Ƿ��е��������(��:����ǽ)��ֹ��Mcproc.exeȨ��\r\n"),_T("\n����ʱ���"),MB_OK|MB_ICONWARNING);

		CStdString strErro;
		strErro.Format(_T("bind �˿ڣ�%d ,������:%d "),nPort, GetLastError());
		::MessageBox(0,strErro.GetBuffer(0),_T("\n����ʱ���"),MB_OK|MB_ICONWARNING);
#endif
		closesocket(m_socket);
		WSACleanup();

		return -1;
	}

	// ����socket
	if ( listen( m_socket, 1 ) == SOCKET_ERROR )
	{
#ifdef _DEBUG
		::MessageBox(0,_T("�۴�ʦ����ʱ����һ������,\r\n�����Ƿ��е��������(��:����ǽ)��ֹ��Mcproc.exeȨ��\r\n"),_T("\n����ʱ���"),MB_OK|MB_ICONWARNING);
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
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("���÷���KEEPALIVE, error:%d"), WSAGetLastError());
	}*/

	//���÷��ͳ�ʱ(10�룩
	int iTimeOut = 1000*50;
	//int iTimeOut = 1000*100;
	if(setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char*) &iTimeOut,sizeof(int)) == SOCKET_ERROR)
	{
		g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("���÷��ͳ�ʱ����, error:%d"), WSAGetLastError());
	}

	//struct linger m_sLinger;
	//m_sLinger.l_onoff = 1; //�ڵ���closesocket()ʱ��������δ�����꣬����ȴ�
	//// ��m_sLinger.l_onoff=0;�����closesocket()��ǿ�ƹر�
	//m_sLinger.l_linger = 5; //���õȴ�ʱ��Ϊ5��
	//if(setsockopt( m_socket, SOL_SOCKET, SO_LINGER, (const char*)&m_sLinger, sizeof(struct linger)))
	//{
	//	g_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("���÷��ͳ�ʱ����, error:%d"), WSAGetLastError());
	//}

	m_ClientSocket = INVALID_SOCKET ;
	m_bRun = TRUE;

	WaitRecData(TRUE);

	return 0;
}

//�����ʼ��
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
		shutdown(m_ClientSocket, 2); //�Ͽ����ͽ��չ���
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
	//���ŵعر�SOCKET�ķ�ʽ
	shutdown(m_ClientSocket,SD_SEND);
	closesocket(m_ClientSocket) ;
	m_ClientSocket = INVALID_SOCKET;
	return -1 ;
}

//�ȴ��ͻ�������
int CCommServer::WaitClient()
{
	// ����
	g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("�ȴ��ͻ�������..." ));
	m_ClientSocket = INVALID_SOCKET;
	while ( m_ClientSocket == INVALID_SOCKET ) {
		m_ClientSocket = accept( m_socket, NULL, NULL );
	}
	g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("�ͻ������ӳɹ���" ));
	return 0;
}
DWORD WINAPI ThreadRecv(PVOID param)
{
	CCommServer *Server = (CCommServer*)param;

WaitConnect:
	Server->WaitClient();
	int bytesRecv = SOCKET_ERROR;
	TCHAR recvbuf[8192] = {0};	//������
	//TCHAR recvbuf[1024] = {0};	//������
	DWORD dwPackteCount = 0;
	TCHAR *pDataBuff = NULL;	//������
	DWORD dwDataBuffLen = 0;	//����������
	while(Server->m_bRun)
	{
		bytesRecv = recv( Server->m_ClientSocket, (char*)recvbuf, 8192, 0 );
		//bytesRecv = recv( Server->m_ClientSocket, (char*)recvbuf, 1024, 0 );
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("==========================>>>>>>>>>>>>WSAGetLastErrorWSAGetLastError err:%d"), WSAGetLastError());
		//g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("%%%%%%%%%%%%%%%%%%%%%%%%%%bytesRecv = %d"),bytesRecv);
		if( bytesRecv == -1 || bytesRecv == 0)	//�ͻ��˶Ͽ����� �ȴ�����
		{
			if (bytesRecv == -1)
			{
				Server->m_pfnDataHandler(_T("DISCONNECT"),10);
				g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("�ͻ����쳣�˳�������4�������¼�"));
			}
			Server->m_ClientSocket = INVALID_SOCKET ;
			g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("�ͻ��˶Ͽ����ӣ��˳����� err:%d"), WSAGetLastError());
			break;   //���ӶϿ����˳�ѭ��
		}

#ifdef _UNICODE		
		bytesRecv /= 2;  //UNICODE���յ����ַ���Ϊ�ֽ���/2
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

		//������
		if(dwDataBuffLen<sizeof(DWORD)*2)continue; //С����С��ͷ�������´��� 

		DWORD dwPacketSize = 0; //����С
		while(TRUE)
		{
			if(dwDataBuffLen == 0)break; //������û������
			memcpy(&dwPacketSize,pDataBuff,sizeof(DWORD));
			dwPacketSize /= 2;       //UNICODE�½��յ�����ʵ�ʳ���Ϊ�յ����ַ���*2
			if(dwPacketSize > dwDataBuffLen)        
				break; //�����ȴ��ڻ��������Ȳ�������

			TCHAR *pData = new TCHAR[dwPacketSize];	//ƴһ����
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
			dwPackteCount ++ ; //��������
		} 

		printf("�ܹ��յ����ݰ� %d\n",dwPackteCount); //��������
		//Sleep(200);	//�߳��л�
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

		//������
		if(dwDataBuffLen<sizeof(DWORD)*2)continue; //С����С��ͷ�������´��� 

		DWORD dwPacketSize = 0; //����С
		while(TRUE)
		{
			//if(dwDataBuffLen == 0)break; //������û������
			memcpy((WCHAR*)&dwPacketSize,pDataBuff,sizeof(DWORD));
			if(dwPacketSize > dwDataBuffLen)        
				break; //�����ȴ��ڻ��������Ȳ�������

			TCHAR *pData = new TCHAR[dwPacketSize];	//ƴһ����
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
			dwPackteCount ++ ; //��������
		} 

		printf("�ܹ��յ����ݰ� %d\n",dwPackteCount); //��������
		//Sleep(200);	//�߳��л�
	}
#endif

	if (Server->m_bRun)
		goto WaitConnect;

	return 0;
}
//��������

int CCommServer::WaitRecData(BOOL IsThread)
{
	if(IsThread == FALSE)
 		ThreadRecv(this); //��ͨ��ʽ���У��������̣�
	else
		m_hThread = CreateThread(NULL,0,ThreadRecv,this,0,0); //�̷߳�ʽ���У����������̣�

	return 0;
}

int CCommServer::SendData(DWORD dwLen,DWORD dwFlag,char *pBuff)
{
    //��ֹ���̵߳��ó���
    CLocalLock lock(&m_lockSend);

	DWORD dwTotalLen = dwLen + 8;
	if(m_ClientSocket != INVALID_SOCKET)
	{
		char *buffer = new char[dwTotalLen];
		memcpy(buffer,&dwTotalLen,sizeof(DWORD));
		memcpy(buffer + sizeof(DWORD),&dwFlag,sizeof(DWORD));
		memcpy(buffer + sizeof(DWORD)*2,pBuff,dwLen);
		int nRet = send(m_ClientSocket,buffer,dwTotalLen,0);	//��������

		delete []buffer;
		return nRet;
	}
	return -1;
}


//�������ݵ��������
int CCommServer::SendData(TCHAR *pData,int nLen)
{
	//��ֹ���̵߳��ó���
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
