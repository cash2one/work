//////////////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2013, ��Ѷ����Ϣ���޹�˾
// �汾��
// �ļ�˵����SOCKET������� ͨ���ص����������յ�������
// �������ڣ�
// ���ߣ�������
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2. 
//

#ifndef _CSERVERSOCKET__
#define _CSERVERSOCKET__

#include "IServerSocket.h"
#include "StdString.h"
#include "Trace.h"
#include "Lock.h"

#pragma comment(lib, "wsock32.lib")
 
typedef DWORD (*pfnDataHandler)(TCHAR *Orgbuf, DWORD dwTotalLen);

class CCommServer : public IServerSocket
{
public:
	CCommServer(void);
	~CCommServer(void);

	int Init(int nPort, pfnDataHandler fn);	//��ʼ��
	int UnInit();	//�����ʼ��
	int CloseClient() ;  //�ر�ClientSocket
	int SendData(DWORD dwLen,DWORD Flag,char *buff);
	int SendData(TCHAR *buff, int dwLen);
	int SendData(CStdString strData);
	bool  IsClientConnected() const;

	friend DWORD WINAPI ThreadRecv(PVOID param);

private:
	HANDLE m_hThread;
	BOOL m_bRun;
	SOCKET m_socket;
	SOCKET m_ClientSocket;
	pfnDataHandler m_pfnDataHandler;        //�յ����ݰ�֮��Ļص�����
	CLock m_lockSend;                     //�������ݵĻ�����

	int WaitClient(); //�ȴ��ͻ�������
	int WaitRecData(BOOL IsThread);	//��������
};
#endif