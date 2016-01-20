// GetUpdateRank.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "UpdateRank.h"
#include "UpdateRankDlg.h"
//#include "DataInfo.h"
  


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CGetUpdateRankApp

BEGIN_MESSAGE_MAP(CGetUpdateRankApp, CWinApp)
END_MESSAGE_MAP()


// CGetUpdateRankApp 构造

CGetUpdateRankApp::CGetUpdateRankApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CGetUpdateRankApp 对象

CGetUpdateRankApp theApp;

// CGetUpdateRankApp 初始化

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

//初始化调试Url
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
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("检测到有配置DebugUrl:%s,进入调试模式"), g_strDebugUrl.GetBuffer());
		g_bIsDebug = TRUE;
	}
}

// 初始化延时时间
void InitDelayTime()
{
	TCHAR szFilePath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	PathAppend(szFilePath, _T("..\\..\\"));
	PathAppend(szFilePath, _T("data2\\mc.dat"));

	g_iDelayForKeyWord = (int)GetPrivateProfileInt(_T("QuickPhoto"), _T("delay_for_each_keyword"), -1, szFilePath);
	g_iDalayForSite    = (int)GetPrivateProfileInt(_T("QuickPhoto"), _T("delay_for_each_site"), -1, szFilePath);
	g_iThreadCount = (int)GetPrivateProfileInt(_T("PHOTOTHREAD"), _T("count"), -1, szFilePath);
	g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("初始化数据:%d,%d,%d"), g_iDalayForSite, g_iDelayForKeyWord, g_iThreadCount);
}


HHOOK g_hCBT = NULL ;	// 保存CBT钩子句柄
BOOL WINAPI SetHook(BOOL isInstall);


// CBT钩子消息处理过程
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);

extern "C" BOOL __declspec(dllexport) GetQuickPhoto(const CStdString &strWord, IServerSocket *pSocket)
{
	g_Socket = pSocket;

	InitializeCriticalSection(&critSection);
	InitializeCriticalSection(&critSendMsg);
	InitializeCriticalSection(&critSearchFlag);
	InitializeCriticalSection(&critCount);
	
//	MessageBox(NULL, _T("开始快照"), _T("hhh"), MB_OK);
	//解密加密字符串
	CStdString strInfo = _T("");
	CFileReadAndSave::UnZipDecodeStdString (strWord, strInfo);
	g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("收到主控传过来的抓取快照数据；"));

	strInfo.append(_T("(;8)"));
	//初始化调试URL
	InitDebugUrl();

	ParseAllKeyWordData(strInfo);


	for(int j=0; j<SEARCHFLAGCOUNT; j++)
	{
		bSearchFlag[j] = TRUE;
	}
	
	if (g_vAllKeyWords.size() > 0)
	{ 
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("快照总数：%d"), g_vAllKeyWords.size());
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("开始清除ie中搜索引擎缓存"));
		DeleteSearchCache();
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("清除ie中搜索引擎缓存结束"));

		SetHook(TRUE);

		InitDelayTime();

		int iCPUNums = 2;
		iCPUNums = GetCPUCoreNums();

		if (g_iThreadCount == -1)
		{
			if (iCPUNums <= 0) // 防止GetCPUCoreNums()返回-1的情况
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
			//调试模式只要一个线程
			g_iThreadCount = 1;
		}

		if (g_iThreadCount > g_vAllKeyWords.size())  //这里任务线程多于任务个数
			g_iThreadCount = g_vAllKeyWords.size();

		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("CPU核心数:%d,开启线程数:%d"), iCPUNums, g_iThreadCount);

		HANDLE *hThreads = new HANDLE[g_iThreadCount];
		for (int i=0; i<g_iThreadCount; i++)
		{
			DWORD dwThreadId;
			hThreads[i] = CreateThread(NULL, NULL, StartCapture, 0, 0, &dwThreadId);
		}

		DWORD dwTime = GetTickCount();
		WaitForMultipleObjects(g_iThreadCount, hThreads, TRUE, INFINITE);
		dwTime = GetTickCount()-dwTime;

		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("本次抓取快照花费时间,%.2f,秒"), dwTime/1000.0);

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
		_T("抓取快照结束，已发送结束字符串:%s"), strBack.c_str());

	return TRUE;
}



BOOL WINAPI SetHook ( BOOL isInstall ) 
{
	// 需要安装，且钩子不存在
	if ( isInstall && !g_hCBT )
	{
		// 设置全局钩子
		g_hCBT = SetWindowsHookEx ( WH_CBT, (HOOKPROC)CBTProc, 0, GetCurrentThreadId() ) ;
		if ( g_hCBT == NULL )
		{
			return FALSE ;
		}

		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("勾子设置成功"));
	}

	// 需要卸载，且钩子存在
	if ( !isInstall && g_hCBT )
	{
		// 卸载钩子
		BOOL ret = UnhookWindowsHookEx ( g_hCBT ) ;
		g_hCBT	= NULL ;
		return ret ;
	}

	return TRUE ;
}

//CBT钩子消息处理过程
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

		g_pageLog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获创建窗口消息，标题：%s"), strText);

		//监视类型为WS_POPUP且标题中含有过滤字符串的窗口创建
		if ((pCreateStruct->style & WS_POPUP) &&	
			(-1 != strText.Find(_T("安全警告")) || 
			-1 != strText.Find(_T("安全警报")) ||
			0 == strText.CompareNoCase(_T("Internet Explorer")) ||
			0 == strText.CompareNoCase(_T("Windows Internet Explorer")) ||
			strText == _T("网页错误") ||
			strText.Find(_T("来自网页的消息")) != -1||
			strText.Find(_T("MC.exe")) != -1
			))
		{
			g_pageLog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获到安全弹框提示，已屏蔽，标题：%s"), strText);
			return 1;

		}
		break ;

	}

	// 继续传递消息
	return CallNextHookEx ( g_hCBT, nCode, wParam, lParam ) ;
}


