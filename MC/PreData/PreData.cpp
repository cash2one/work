// PreData.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "PreData.h"
#include "PrePareDataThread.h"

#include <iostream>


using namespace std;

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

// CPrePareDLLApp

BEGIN_MESSAGE_MAP(CPrePareDLLApp, CWinApp)
END_MESSAGE_MAP()


// CSEngineDriverApp ����

CPrePareDLLApp::CPrePareDLLApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSEngineDriverApp ����

CPrePareDLLApp theApp;


// CPrePareDLLApp ��ʼ��

BOOL CPrePareDLLApp::InitInstance()
{
	CWinApp::InitInstance();

//	g_sysLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("LOAD PreData.dll"));

	return TRUE;
}
extern "C" __declspec(dllexport) IThreadUnit* CreateIThread(IGlobalDataBlock* pFindDataByID,IServerSocket *pSocket,IConfigInfo *pConfig)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	g_GlobalDataBlock = pFindDataByID;
	g_Config = pConfig;
	IThreadUnit * task = new CPrePareDataThread();
	cout << "PPD Thread Create!" << endl;
	return task;
}