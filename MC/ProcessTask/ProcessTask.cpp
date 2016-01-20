// Kernel4.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "ProcessTask.h"
#include "IThreadUnit.h"
#include "KernelProcessThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
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
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CKernel4App

BEGIN_MESSAGE_MAP(CKernel4App, CWinApp)
END_MESSAGE_MAP()


// CKernel4App ����

CKernel4App::CKernel4App()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CKernel4App ����

CKernel4App theApp;


// CKernel4App ��ʼ��

BOOL CKernel4App::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


extern "C" __declspec(dllexport) IThreadUnit * CreateIThread(IGlobalDataBlock* pGlobalDataBlock,
	IServerSocket *pSocket,
	IConfigInfo *pConfig)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_GlobalDataBlock = pGlobalDataBlock;
	g_Config = pConfig;
	g_ServerSocket  = pSocket;
	IThreadUnit * task = new CKernelProcessThread;	

	printf("4kernel Create\n");
	return task;
}