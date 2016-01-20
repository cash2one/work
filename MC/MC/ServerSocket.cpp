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
 
//��ʼ��
int CServerSocket::Init(int nPort)
{
	WSADATA wsaData;
	int errid;
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

	if ( ::bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {

		::MessageBox(0,_T("�۴�ʦ����ʱ����һ������,\r\n�����Ƿ��е��������(��:����ǽ)��ֹ��Mcproc.exeȨ��\r\n"),_T("\n����ʱ���"),MB_OK|MB_ICONWARNING);
#ifdef _DEBUG
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
		::MessageBox(0,_T("�۴�ʦ����ʱ����һ������,\r\n�����Ƿ��е��������(��:����ǽ)��ֹ��Mcproc.exeȨ��\r\n"),_T("\n����ʱ���"),MB_OK|MB_ICONWARNING);
		closesocket(m_socket);
		WSACleanup();

		return -1;
	}
	m_ClientSocket = INVALID_SOCKET;
	m_bRun = TRUE;
	return 0;
}

//�����ʼ��
int CServerSocket::UnInit()
{
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

//�ȴ��ͻ�������
int CServerSocket::WaitClient()
{
	// ����
	g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("�ȴ��ͻ�������..." ));
	m_ClientSocket = INVALID_SOCKET ;
	while ( m_ClientSocket == INVALID_SOCKET) {
		m_ClientSocket = accept( m_socket, NULL, NULL );

	}
	g_log.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__, __LINE__,_T("�ͻ������ӳɹ�%d"), m_ClientSocket);
	return 0;
}
DWORD WINAPI ThreadRecv(PVOID param)
{
	CServerSocket *Server = (CServerSocket*)param;

	int bytesRecv = SOCKET_ERROR;
	char recvbuf[8192] = {0};	//������
	DWORD dwPackteCount = 0;
	char *pDataBuff = NULL;	//������
	DWORD dwDataBuffLen = 0;	//����������

	Server->WaitClient();

	while(Server->m_bRun)
	{
		bytesRecv = recv( Server->m_ClientSocket, recvbuf, 8192, 0 );
		g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("�յ��ؼ�������������,���ݴ�С��%d"), bytesRecv);

		if( bytesRecv == -1 || bytesRecv == 0)	//�ͻ��˶Ͽ�����
		{
			Server->m_ClientSocket = INVALID_SOCKET ;
			g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("�ͻ��˶Ͽ����ӣ��˳�����"));

			return 0;	//�˳�����
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

		//������
		if(dwDataBuffLen<sizeof(DWORD)*2)continue; //С����С��ͷ�������´��� 

		DWORD dwPacketSize = 0; //����С
		while(TRUE)
		{
			if(dwDataBuffLen == 0)break; //������û������
			memcpy(&dwPacketSize,pDataBuff,sizeof(DWORD));
			if(dwPacketSize > dwDataBuffLen) 
				break; //�����ȴ��ڻ��������Ȳ�������
			char *pData = new char[dwPacketSize];	//ƴһ����
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
			dwPackteCount ++ ; //��������
		} 

		printf("�ܹ��յ����ݰ� %d\n",dwPackteCount); //��������
		Sleep(200);	//�߳��л�
	}
	return 0;
}
//��������

int CServerSocket::WaitRecData(BOOL IsThread)
{
	if(IsThread == FALSE)
 		ThreadRecv(this); //��ͨ��ʽ���У��������̣�
	else
	{
		m_hThread = CreateThread(NULL,0,ThreadRecv,this,0,0); //�̷߳�ʽ���У����������̣�
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
	//web�汾�Ĵ���
	if (E_GET_EXCUTE_TASK_RESULT == dwFlag)
	{
		//ֻ������������������,�������ز�����
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
		//��ת��Ϊխ�ַ�

		DWORD dwLength = 0;
		
		char *buffer = new char[dwTotalLen];
		memset(buffer,0,dwTotalLen);		
		
		char *pBufferDest = buffer + 4;

		dwLength = dwLen;

		CFileReadAndSave::WCharToMByte((LPCWSTR)pBuff ,pBufferDest,&dwLength);

		//���͵��ֽ�������
		dwTotalLen = dwLength + 4;
		memcpy(buffer,&dwTotalLen,sizeof(DWORD));
		//�Ѿ��ڹ̶�λ����,����Ҫ�����ڴ���
		int nRet = send(m_ClientSocket,buffer,dwTotalLen,0);	//��������

		delete []buffer;
		return nRet;
	}

	return -1;
}



