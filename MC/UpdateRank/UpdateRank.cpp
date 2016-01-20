// GetUpdateRank.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "UpdateRank.h"
#include "UpdateRankDlg.h"
//#include "DataInfo.h"
  


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

// CGetUpdateRankApp

BEGIN_MESSAGE_MAP(CGetUpdateRankApp, CWinApp)
END_MESSAGE_MAP()


// CGetUpdateRankApp ����

CGetUpdateRankApp::CGetUpdateRankApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CGetUpdateRankApp ����

CGetUpdateRankApp theApp;

// CGetUpdateRankApp ��ʼ��

BOOL CGetUpdateRankApp::InitInstance()
{
	AfxEnableControlContainer();


	CWinApp::InitInstance();

	return TRUE;
}


DWORD WINAPI StartCapture(LPVOID lp)
{
	CoInitialize(NULL);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CUpdateRankDlg  photoDlg;
	photoDlg.DoModal();	

	CoUninitialize();

	return 0;
}

//��ʼ������Url
void InitDebugUrl()
{
	TCHAR szIniFile[MAX_PATH] = { 0 };
	TCHAR szInfo[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szIniFile, MAX_PATH);
	PathAppend(szIniFile, _T("..\\..\\"));
	PathAppend(szIniFile, _T("data2\\debugyundata.ini"));

	GetPrivateProfileString(_T("QuickPhoto"), _T("debugurl"), NULL, szInfo, MAX_PATH, szIniFile);

	g_strDebugUrl = szInfo;

	if (g_strDebugUrl.GetLength()>1)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��⵽������DebugUrl:%s,�������ģʽ"), g_strDebugUrl.GetBuffer());
		g_bIsDebug = TRUE;
	}
}

// ��ʼ����ʱʱ��
void InitDelayTime()
{
	TCHAR szFilePath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	PathAppend(szFilePath, _T("..\\..\\"));
	PathAppend(szFilePath, _T("data2\\mc.dat"));

	g_iDelayForKeyWord = (int)GetPrivateProfileInt(_T("QuickPhoto"), _T("delay_for_each_keyword"), -1, szFilePath);
	g_iDalayForSite    = (int)GetPrivateProfileInt(_T("QuickPhoto"), _T("delay_for_each_site"), -1, szFilePath);
	g_iThreadCount = (int)GetPrivateProfileInt(_T("PHOTOTHREAD"), _T("count"), -1, szFilePath);
	g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ʼ������:%d,%d,%d"), g_iDalayForSite, g_iDelayForKeyWord, g_iThreadCount);
}


HHOOK g_hCBT = NULL ;	// ����CBT���Ӿ��
BOOL WINAPI SetHook(BOOL isInstall);


// CBT������Ϣ�������
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);

extern "C" BOOL __declspec(dllexport) GetQuickPhoto(const CStdString &strWord, IServerSocket *pSocket)
{
	g_Socket = pSocket;

	InitializeCriticalSection(&critSection);
	InitializeCriticalSection(&critSendMsg);
	InitializeCriticalSection(&critSearchFlag);
	InitializeCriticalSection(&critCount);
	
//	MessageBox(NULL, _T("��ʼ����"), _T("hhh"), MB_OK);
	//���ܼ����ַ���
	CStdString strInfo = _T("");
	CFileReadAndSave::UnZipDecodeStdString (strWord, strInfo);
	g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("�յ����ش�������ץȡ�������ݣ�"));

	strInfo.append(_T("(;8)"));
	//��ʼ������URL
	InitDebugUrl();

	ParseAllKeyWordData(strInfo);


	for(int j=0; j<SEARCHFLAGCOUNT; j++)
	{
		bSearchFlag[j] = TRUE;
	}
	
	if (g_vAllKeyWords.size() > 0)
	{ 
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("����������%d"), g_vAllKeyWords.size());
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ʼ���ie���������滺��"));
		DeleteSearchCache();
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("���ie���������滺�����"));

		SetHook(TRUE);

		InitDelayTime();

		int iCPUNums = 2;
		iCPUNums = GetCPUCoreNums();

		if (g_iThreadCount == -1)
		{
			if (iCPUNums <= 0) // ��ֹGetCPUCoreNums()����-1�����
			{
				g_iThreadCount = 2;
			}
			else
			{
				g_iThreadCount = iCPUNums + 1;
			}
		}

		if (g_bIsDebug)
		{
			//����ģʽֻҪһ���߳�
			g_iThreadCount = 1;
		}

		if (g_iThreadCount > g_vAllKeyWords.size())  //���������̶߳����������
			g_iThreadCount = g_vAllKeyWords.size();

		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("CPU������:%d,�����߳���:%d"), iCPUNums, g_iThreadCount);

		HANDLE *hThreads = new HANDLE[g_iThreadCount];
		for (int i=0; i<g_iThreadCount; i++)
		{
			DWORD dwThreadId;
			hThreads[i] = CreateThread(NULL, NULL, StartCapture, 0, 0, &dwThreadId);
		}

		DWORD dwTime = GetTickCount();
		WaitForMultipleObjects(g_iThreadCount, hThreads, TRUE, INFINITE);
		dwTime = GetTickCount()-dwTime;

		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("����ץȡ���ջ���ʱ��,%.2f,��"), dwTime/1000.0);

		for (int i=0; i<g_iThreadCount; i++)
			CloseHandle(hThreads[i]);

		delete []hThreads;

		SetHook(FALSE);
	}

	
	DeleteCriticalSection(&critSearchFlag);
	DeleteCriticalSection(&critSendMsg);
	DeleteCriticalSection(&critSection);
	DeleteCriticalSection(&critCount);


	CStdString strBack;

	strBack.Format(_T("BackResult(;0)AllTaskComplete(;0)(;0)"));

	BYTE *pByData = (BYTE *)strBack.c_str();
	g_Socket->SendData(strBack.size() * 2,E_GET_EXCUTE_TASK_RESULT,(char*)pByData);

	g_pageLog.Trace(LOGL_HIG, LOGT_PROMPT, __TFILE__, __LINE__,
		_T("ץȡ���ս������ѷ��ͽ����ַ���:%s"), strBack.c_str());

	return TRUE;
}



BOOL WINAPI SetHook ( BOOL isInstall ) 
{
	// ��Ҫ��װ���ҹ��Ӳ�����
	if ( isInstall && !g_hCBT )
	{
		// ����ȫ�ֹ���
		g_hCBT = SetWindowsHookEx ( WH_CBT, (HOOKPROC)CBTProc, 0, GetCurrentThreadId() ) ;
		if ( g_hCBT == NULL )
		{
			return FALSE ;
		}

		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("�������óɹ�"));
	}

	// ��Ҫж�أ��ҹ��Ӵ���
	if ( !isInstall && g_hCBT )
	{
		// ж�ع���
		BOOL ret = UnhookWindowsHookEx ( g_hCBT ) ;
		g_hCBT	= NULL ;
		return ret ;
	}

	return TRUE ;
}

//CBT������Ϣ�������
LRESULT CALLBACK CBTProc ( int nCode, WPARAM wParam, LPARAM lParam )
{
	CBT_CREATEWND *pWndParam = NULL ;
	CREATESTRUCT *pCreateStruct = NULL ;
	CString strText ;

	switch ( nCode )
	{
	case HCBT_CREATEWND:	
		pWndParam = (CBT_CREATEWND*)lParam;
		pCreateStruct = pWndParam->lpcs;
		strText = pCreateStruct->lpszName;

		g_pageLog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("���񴴽�������Ϣ�����⣺%s"), strText);

		//��������ΪWS_POPUP�ұ����к��й����ַ����Ĵ��ڴ���
		if ((pCreateStruct->style & WS_POPUP) &&	
			(-1 != strText.Find(_T("��ȫ����")) || 
			-1 != strText.Find(_T("��ȫ����")) ||
			0 == strText.CompareNoCase(_T("Internet Explorer")) ||
			0 == strText.CompareNoCase(_T("Windows Internet Explorer")) ||
			strText == _T("��ҳ����") ||
			strText.Find(_T("������ҳ����Ϣ")) != -1||
			strText.Find(_T("MC.exe")) != -1
			))
		{
			g_pageLog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("���񵽰�ȫ������ʾ�������Σ����⣺%s"), strText);
			return 1;

		}
		break ;

	}

	// ����������Ϣ
	return CallNextHookEx ( g_hCBT, nCode, wParam, lParam ) ;
}


