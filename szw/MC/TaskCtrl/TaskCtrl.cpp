// TaskCtl.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "TaskCtrl.h"
#include "TaskCtrlThread.h"
#include "IThreadUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CTaskCtlApp

BEGIN_MESSAGE_MAP(CTaskCtlApp, CWinApp)
END_MESSAGE_MAP()


// CTaskCtlApp ����

CTaskCtlApp::CTaskCtlApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTaskCtlApp ����

CTaskCtlApp theApp;


// CTaskCtlApp ��ʼ��

BOOL CTaskCtlApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport) IThreadUnit * CreateIThread(IGlobalDataBlock* pGlobalDataBlock,
	IServerSocket *pSocket,
	IXMLRW *pConfig)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_GlobalDataBlock = pGlobalDataBlock;
	g_Socket = pSocket;
	g_Config = pConfig;
	IThreadUnit *pThread = new CTaskMgrThread();
	return pThread;
}