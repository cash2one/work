//////////////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2013, ��Ѷ����Ϣ���޹�˾
// �汾��
// �ļ�˵����SOCKET�ͻ����� ͨ���ص����������յ������ݣ����������ط���ANSI�ַ�����
// �������ڣ�
// ���ߣ�������
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2. 
//

#pragma once
#include "Lock.h"

typedef DWORD (*pfnDataHandler)(char *Orgbuf, DWORD dwTotalLen);

class CCommClient
{
public:
	CCommClient(void);
	~CCommClient(void);

	int Init(int iPort, pfnDataHandler fn);
	int SendData(TCHAR *pData,int nLen); //�������ݵ��������
	int SendData(CStdString strData);

	void Stop(void);   //ֹͣ���ݽ����߳�
	bool StopIsSet(void);  //����Ƿ�������ֹͣ���

private:
	SOCKET m_socket;
	HANDLE m_hThread;
	bool m_bStop;

	pfnDataHandler m_pfnDataHandler;        //�յ����ݰ�֮��Ļص�����
// 	pfnDataHandler *m_pfnDataHandler;        //�յ����ݰ�֮��Ļص�����
	CLock m_lockSend;                     //�������ݵĻ�����

private:
	int Connect(int iPort);
	DWORD DisConnect(void);
	DWORD CreateRecvThread(void);
	int SetDataHandler(pfnDataHandler fn);

	static DWORD WINAPI StaticThreadFunc(LPVOID lpParam);
};
