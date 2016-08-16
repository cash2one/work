
// Sumengine2.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Engine.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "EngineDoc.h"
#include "EngineView.h"
#include "Wininet.h"
#include "VolMute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSumengine2App

BEGIN_MESSAGE_MAP(CSumengine2App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSumengine2App::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CSumengine2App 构造

CSumengine2App::CSumengine2App()
{
	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Engine.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	memset(m_tszSuccQuickLookDir, 0, MAX_PATH * sizeof(TCHAR));
	memset(m_tszFailQuickLookDir, 0, MAX_PATH * sizeof(TCHAR));	
	m_nFlashRefCount = 0;
}

// 唯一的一个 CSumengine2App 对象
CSumengine2App theApp;
CLogTrace g_runlog(_T("Core\\runlog.log"), NULL);
CLogTrace g_CheckInputLog(_T("Core\\CheckInput.log"), NULL);
CLogTrace g_CheckCpuLog(_T("Core\\CheckCpu.log"), NULL);
LONGLONG g_lCurrentTickCount = 0;

IXMLRW g_configInfo;
CMessageShow g_msgbox;
CCommClient CSumengine2App::m_sCommClient;
COcrInput g_ocrInput;
//人工打码
CManmadeCode g_orcManmade;					
HANDLE g_hStepCtrlEvent = NULL;
T_CONFIG_PARAM g_cfgParam;
//对要过滤的弹框
vector<CString> g_vecWords;					
map<CStdString, XYPoint> g_mapObject;
vector<CStdString> g_vecSilentWeb;	
//对要设置setsilent的网站
map<CStdString, CStdString> g_mapWebIdToUrl;
//需要将远程打码结果转小写的网站ID
CString g_strMakeLowerWebId;

DWORD WINAPI ThreadStopProc(LPVOID lpParameter);
DWORD OnRecvData(TCHAR *Orgbuf, DWORD dwTotalLen);

std::map<CStdString, CExecMgr *> g_mapTaskExecMgr;
CLock g_lockStop;
//将要被删除的任务，DWORD表示将要被删除的时间
std::map<CStdString, DWORD> g_mapStopTaskMgr;
//根据任务ID找到其对应的执行管理对象
CExecMgr *GetTaskExecMgr(CStdString strTaskID);
//清除cookie
bool ClearCookie(bool bIsDeleteAll = false, CStdString strUrl = _T(""));

CProcessVolMute g_mute;
BOOL g_bSyncStepEnter = TRUE;

// CSumengine2App 初始化
BOOL CSumengine2App::InitInstance()
{	
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	//解析调用参数
	AnalysisDosParam(__argc, __targv);

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_EngineTYPE,
		RUNTIME_CLASS(CSumengine2Doc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CSumengine2View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生
	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

	//读取内核配置及网站map列表数据
	InitCfgSetting();    
	GetUrlMapData();
	
	if (m_tDosParam.m_iRunMode == rmNormal || m_tDosParam.m_iCommpPort == -1)
	{
		m_nCmdShow = SW_HIDE;
		pMainFrame->StartModeChkTimer(); //为正常模式启动显示内核窗口检查的Timer
	}
	else
	{
		m_nCmdShow = SW_MAXIMIZE;
	}
	
	//启动对内核激活状态的检测
	if (m_tDosParam.m_iRunMode == rmNormal || m_tDosParam.m_iRunMode == rmShow)
	{
		pMainFrame->StartEngActiveCheck();
	}

	g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("程序启动，mode:%d, port:%d,url:%s,RemoteFlag:%d,YunTaskFlag:%d"), 
		m_tDosParam.m_iRunMode, m_tDosParam.m_iCommpPort,m_tDosParam.m_strUrl,m_tDosParam.m_bIsRemoteCode,m_tDosParam.m_bIsYunTask);

	g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("程序启动:版本修改日期:%s#%s"), CString(__DATE__),CString(__TIME__));

	//65535：把内核当作浏览器使用，不与其它工具通信
	if (m_tDosParam.m_iRunMode != 65535 && m_tDosParam.m_iCommpPort != -1)       
	{
		//创建线程实时检测有没有要关闭的
		m_hStopThread = NULL;
		m_hStopThread = ::CreateThread(NULL, 0, &ThreadStopProc, 0, 0, NULL);
		m_sCommClient.Init(m_tDosParam.m_iCommpPort, OnRecvData);
	}

	//初始化DLL及相应接口
	if (!InitLibs())//BugReportHelp.dll,CodeInputDlg.dll
	{
		return FALSE;
	}
	//初始化远程打码接口
	if(!InitManmadeCode(m_tDosParam.m_bIsRemoteCode))
	{
		return false;
	}
	
	//设置主框架标题
	m_pMainWnd->SetWindowText(_T("Engine"));      

	//不加载默认视图
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
 	if (!ProcessShellCommand(cmdInfo))
 		return FALSE;

	//获取屏幕区域大小
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
	int nWidth = rt.right;
	int nHight = rt.bottom;

	if (m_nCmdShow == SW_HIDE)
	{	
		//由于云任务做了对用户行为限制，所以此处没必要再对内核进行限制了
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOP, 0, 0, nWidth, nHight, SWP_HIDEWINDOW);
	}	
	else
	{
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOP, 0, 0, nWidth, nHight, SWP_SHOWWINDOW);

		// 主窗口已初始化，因此显示它并对其进行更新
		pMainFrame->UpdateWindow();
	}

	m_updateFileObj.Init(AfxGetMainWnd()->m_hWnd, (m_tDosParam.m_iRunMode == rmNormal ? true : false));
	
	//直接打开网页(主要用于执行定时任务打开执行网页)
	if ((__argc > 3) && (m_tDosParam.m_iCommpPort == -1) && !m_tDosParam.m_strUrl.IsEmpty())
	{	
		//直接打开网页需要预先打开一个页面
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;

		CStdString str = m_tDosParam.m_strUrl;	
		m_pMainWnd->SendMessage(WM_MAINWND_NAVIGATE_URL, (WPARAM)str.GetBuffer(), 0);
	}

	//设置应用程序静音
	g_mute.SetMute(true);

	return TRUE;
}

int CSumengine2App::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	//if(m_hStopThread)
	AfxOleTerm(FALSE);
	return CWinApp::ExitInstance();
}

// CSumengine2App 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CSumengine2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSumengine2App 消息处理程序
void CSumengine2App::OnFileNew()
{
	CWinApp::OnFileNew();
}

int CSumengine2App::GetCurrentRunMode(void)
{
	return m_tDosParam.m_iRunMode;
}
BOOL CSumengine2App::GetYunTaskFlag(void)
{
	return m_tDosParam.m_bIsYunTask;
}

bool CSumengine2App::InitLibs(void)
{
	CStdString strDir = GetInstallPath();

	HINSTANCE histDbg = LoadLibrary(strDir + _T("\\bin\\BugReport.dll"));
	if(histDbg == NULL)
	{
		CStdString strMsg = _T("");
		strMsg.Format(_T("装入BugReport模块失败！ err:%d"), GetLastError());
		g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,CString(strMsg));
		MessageBox(NULL, strMsg, _T("错误"), MB_OK);
		return 0;
	}

	g_hStepCtrlEvent = CreateEvent(NULL, TRUE, TRUE, NULL);//这个事件用来点击内核工具栏停止按钮的
	if (g_hStepCtrlEvent == NULL)
	{
		g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("创建步骤控制信号失败！err:%d"), GetLastError());
		return 0;
	}
	//MessageBox(NULL, _T("fdsfd"), _T("fdsfdsf"), MB_OK);
 	return g_ocrInput.Init();
}

bool CSumengine2App::InitManmadeCode(BOOL bIsRemoteCode)        //初始化人工打验证码dll
{
	return g_orcManmade.Init(bIsRemoteCode);
}

bool ClearCookie(bool bIsDeleteAll, CStdString strUrl)
{
	BOOL bRet = false;
	HANDLE hEntry;
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;  
	DWORD dwEntrySize;

	g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("ClearCookie"));

	//delete the files
	dwEntrySize = 0;
	hEntry = FindFirstUrlCacheEntry(NULL, NULL, &dwEntrySize);
	lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
	hEntry = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwEntrySize);
	if (!hEntry)
	{
		goto cleanup;
	}

	do
	{
		if ((lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
		{	
			if (bIsDeleteAll)
			{
				DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
			}
			else
			{ 
				if (CStdString(lpCacheEntry->lpszSourceUrlName).find(strUrl) != -1)
				{
					DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
				}
			}
		}

		dwEntrySize = 0;
		FindNextUrlCacheEntry(hEntry, NULL, &dwEntrySize);
		delete [] lpCacheEntry; 
		lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
	}
	while (FindNextUrlCacheEntry(hEntry, lpCacheEntry, &dwEntrySize));

	bRet = true;

cleanup:
	if (lpCacheEntry)
	{
		delete [] lpCacheEntry; 
	}

	g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("ClearCookie over"));

	return bRet;
}

void CSumengine2App::ClearOldVerifyCodes(void)
{
	TCHAR tszPath[MAX_PATH] = {0};
	_sntprintf(tszPath, _TRUNCATE, _T("%s\\image\\code\\*.*"), GetInstallPath());

	SHFILEOPSTRUCT op;
	memset(&op, 0, sizeof(SHFILEOPSTRUCT));
	op.wFunc = FO_DELETE;
	op.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	op.pFrom = tszPath;            
	op.pTo = NULL;
	op.lpszProgressTitle = NULL;

	if (0 != SHFileOperation(&op))
	{
		g_runlog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("清除旧的验证码%s失败！ err: %d"), tszPath, GetLastError());
	}

	g_runlog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("--------------内核清理旧的验证码-------------"));

}

void CSumengine2App::InitQuickLookDirectory(void)
{	
	IXMLRW xml;

	TCHAR tszTmp[MAX_PATH] = {0};
	CString tStr = _T("");
	_stprintf(tszTmp, _T("%s\\data2\\mc.dat"), GetInstallPath());
	
	xml.init(tszTmp);
	xml.ReadString(_T("MC"), _T("RESULTPATH"), _T("savepath"), tStr);
	CTime time;
	time = CTime::GetCurrentTime();

	_sntprintf(m_tszSuccQuickLookDir, _TRUNCATE, _T("%s\\image\\result\\%04d%02d%02d\\%s\\succ"), \
		GetInstallPath(), time.GetYear(), time.GetMonth(), time.GetDay(), tStr);

	_sntprintf(m_tszFailQuickLookDir, _TRUNCATE, _T("%s\\image\\result\\%04d%02d%02d\\%s"), \
		GetInstallPath(), time.GetYear(), time.GetMonth(), time.GetDay(), tStr);

	SHCreateDirectory(NULL, m_tszSuccQuickLookDir);
	SHCreateDirectory(NULL, m_tszFailQuickLookDir);
}

TCHAR *CSumengine2App::GetSuccQuickLookDir(void)
{
	return m_tszSuccQuickLookDir;
}

TCHAR *CSumengine2App::GetFailQuickLookDir(void)
{
	return m_tszFailQuickLookDir;
}

CString CSumengine2App::GetFlashOcxFile(void)
{
	CString str = _T("");
	TCHAR szDir[MAX_PATH] = {0};
	if (SHGetSpecialFolderPath(NULL, szDir, CSIDL_SYSTEM, false))
	{
		//MessageBox(NULL, CString(szDir), _T("ok"), MB_OK);

		CFileFind finder;

		// build a string with wildcards
		CString strWildcard(szDir);
		strWildcard += _T("\\macromed\\flash\\flash*.ocx");

		// start working for files
		BOOL bWorking = finder.FindFile(strWildcard);

 		if(bWorking)
 		{
			finder.FindNextFile();
			str = finder.GetFilePath();		

			if (!PathFileExists(str))
				str = _T("");
		}

		finder.Close();
	}

	g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("flash组件路径：%s"), str);

	return str;
}

void CSumengine2App::InitCfgSetting(void)
{	
	CString strPath = CString(GetInstallPath()) + _T("\\data2\\engine.dat");
	//获取过滤字符串
	CString strFliterWord = _T("");
	CString strSilent = _T("");
	g_configInfo.init(strPath);

	//读取过滤字符
	g_configInfo.ReadString(_T("engine"), _T("silent"), _T("FLITERSTR"), strFliterWord, _T("FLITERSTR"));

	//新增验证码结果转小写网站ID记录
	g_configInfo.ReadString(_T("engine"),_T("ResultMakeUpper"), _T("WebID"), g_strMakeLowerWebId);

	g_configInfo.ReadString(_T("engine"), _T("silent"), _T("MASKTASKID"), strSilent);
	g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("strPath：%s---------strFliterWord = %s"), strPath, strSilent);

	g_configInfo.ReadInt(_T("engine"), _T("control"), _T("port"), g_cfgParam.nPort);
	g_configInfo.ReadInt(_T("engine"), _T("control"), _T("mode"), g_cfgParam.nMode);

	//整体控制参数
	g_configInfo.ReadInt(_T("engine"), _T("control"), _T("TimeoutRetry"), g_cfgParam.nTimeoutRetry, 2);
	g_configInfo.ReadInt(_T("engine"), _T("control"), _T("FailRetry"), g_cfgParam.nFailRetry, 3);
	g_configInfo.ReadInt(_T("engine"), _T("control"), _T("OcrMaxRetry"), g_cfgParam.nOcrMaxRetry, 3);
	g_configInfo.ReadInt(_T("engine"), _T("control"), _T("MaxTimesOfRestartFrom1thStep"), g_cfgParam.nMaxTimesOfRestartFrom1thStep, 3);

	//对cpu使用率进行控制
	g_configInfo.ReadInt(_T("engine"), _T("control"), _T("MaxCpuUseage"), g_cfgParam.nMaxCpuUseage, 80);


	//单步控制参数(默认最大超时时间90秒)
	g_configInfo.ReadInt(_T("engine"), _T("step"), _T("PTO"), g_cfgParam.nPageTimeout, 30000);
	g_configInfo.ReadInt(_T("engine"), _T("step"), _T("PTO_Retry"), g_cfgParam.nPageTimeoutRetry, 3);
	g_configInfo.ReadInt(_T("engine"), _T("step"), _T("CTO"), g_cfgParam.nClickTimeout, 2000);
	g_configInfo.ReadInt(_T("engine"), _T("step"), _T("CTO_Retry"), g_cfgParam.nClickTimeoutRetry, 5);
	
	FillVecSilentWeb(strFliterWord, CStdString(strSilent));

	IXMLRW iniFile;
	CString strTmpPath = CString(GetInstallPath()) + _T("\\data2\\engine.dat");
	iniFile.init(strTmpPath);

	map<CString, CString> mapInfo;
	iniFile.ReadSection(_T("engine"),_T("object_offset"), mapInfo);
	
	map<CString, CString>::iterator iter = mapInfo.begin();
	XYPoint pt;
	CStdString strSecond;
	for(; iter != mapInfo.end(); ++iter)
	{
		strSecond = CStdString(iter->second);
		if(StringToPoint(pt, strSecond))
		{
			g_mapObject[CStdString(iter->first)] = pt;
		}
	}
}

bool CSumengine2App::StringToPoint(XYPoint &pt, CStdString str)
{
	std::vector<CStdString> vResData;
	CStdStrUtils StringUtil;
	DWORD dwCount = StringUtil.SplitString(str, DELIMITER_DATA, vResData, true);

	if (vResData.size() != 2)
	{
		return false;
	}

	pt.x = _ttoi(vResData[0]);
	pt.y = _ttoi(vResData[1]);
	return true;
}

void CSumengine2App::FillVecSilentWeb(CString strFliterWord, CStdString strSilent)
{
	CString strSub;
	int pos;
	while(strFliterWord != _T(""))
	{
		pos = strFliterWord.Find(_T("#"));
		if(pos != -1)
		{
			strSub = strFliterWord.Left(pos);
			strFliterWord = strFliterWord.Mid(pos + 1);
			g_vecWords.push_back(strSub);
		}
		else if(strFliterWord != _T("") && pos == -1)
		{
			g_vecWords.push_back(strFliterWord);
			strFliterWord = _T("");
		}
	}
	vector<CString>::iterator iter = g_vecWords.begin();
	for(; iter != g_vecWords.end(); ++iter)
	{
		g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("g_vecWords[i] = %s"), *iter);
	}

	CStdString strSub0 = _T("");
	int nPos = -1;
	while(strSilent != _T(""))
	{
		nPos = strSilent.Find(_T("#"));
		if(nPos != -1)
		{
			strSub0 = strSilent.Left(nPos);
			strSilent = strSilent.Mid(nPos + 1);
			g_vecSilentWeb.push_back(strSub0);
		}
		else if(strSilent != _T("") && nPos == -1)
		{
			g_vecSilentWeb.push_back(strSilent);
			strSilent = _T("");
		}
	}

	vector<CStdString>::iterator iter0 = g_vecSilentWeb.begin();
	for(; iter0 != g_vecSilentWeb.end(); ++iter0)
	{
		g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("g_vecSilentWeb[i] = %s"), (CString)*iter0);
	}
}

DWORD OnRecvData(TCHAR *Orgbuf, DWORD dwTotalLen)
{	
	//处理收到的数据,上层压入数据的格式：任务ID,输入数据的编码数据
	CString strTmp(Orgbuf);
	CStdString strData = strTmp;

	CStdString strTaskID = _T("");
	CStdString strEncData = _T("");
 	CStdString strCmd = _T("");
	T_DATA_IN dataIn;
		
	std::vector<CStdString> vResData;
	CStdStrUtils StringUtil;
	DWORD dwCount = StringUtil.SplitString(strData, DELIMITER_DATA, vResData, true);

	if (vResData.size() >= 2)
	{
		strTaskID = vResData[0];
		strEncData = vResData[1];

		//解析编码数据，格式：请求命令,输入数据块
		dataIn.SetSource(strEncData);
	}
	else if (!strTmp.CompareNoCase(_T("DISCONNECT")) && theApp.GetCurrentRunMode() != rmBrowser && theApp.GetCurrentRunMode() != rmCaibian)
	{
		//非浏览模式时，连接断开后自动退出内核
		g_runlog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("非浏览模式收到连接断开消息，内核退出！"));
		
		//稍加延时等相关资源释放，再结束内核(0KBdump可能在此产生)
		Sleep(100);
		::ExitProcess(0);
	}
	else
	{
		g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("收到数据无效, %s！"), (CString)strData);	
		return 0 ;
	}

	CExecMgr *pTaskExecMgr = GetTaskExecMgr(strTaskID);
	if (NULL == pTaskExecMgr)
	{
		g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("获取任务管理对象失败，任务ID：%s， NsName：%s"), CString(strTaskID), CString(dataIn.strNsName));
		return 1;
	}

	if (_T("EXEC") == dataIn.strCmd || _T("GET") == dataIn.strCmd)
	{	
		//执行任务或采编数据
		pTaskExecMgr->PushBlockData(strEncData, dataIn.strNsName);
		g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("压入块数据, 任务ID：%s， NsName：%s"), CString(strTaskID), CString(dataIn.strNsName));

		return 0;
	}
	else if (_T("STOP") == dataIn.strCmd)
	{
		g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("收到停止任务命令, 任务ID：%s， NsName：%s"), CString(strTaskID), CString(dataIn.strNsName));
		//此处可以判断下界面是否已经卡死，如果已经卡死则退出内核窗口
		if (((GetTickCount() - g_lCurrentTickCount) > 30 * 1000) && (theApp.GetCurrentRunMode() == rmNormal || theApp.GetCurrentRunMode() == rmShow))
		{	
			g_runlog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("内核界面已经卡死超过30s,退出内核!"));

			//稍加延时等相关资源释放，再结束内核
			Sleep(100);
			::ExitProcess(0);
		}

		//释放掉互斥对象
		g_mapTaskExecMgr[strTaskID]->ReleaseConfilictMutex();

		DWORD stopTime = GetTickCount();
		g_lockStop.Lock();
		g_mapStopTaskMgr[strTaskID] = stopTime;
		g_lockStop.Unlock();

		g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("-------------网站%s收到Stop消息，网站ID:%s！--------------"), (CString)dataIn.strNsName, (CString)strTaskID);

		return 0;
	}

	g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("收到未定义的命令, %s！"), strCmd);	
	return 0;
}

//根据任务ID找到其对应的执行管理对象
CExecMgr *GetTaskExecMgr(CStdString strTaskID)
{	
	std::map<CStdString, CExecMgr *>::iterator itMgr = g_mapTaskExecMgr.find(strTaskID);
	if (itMgr == g_mapTaskExecMgr.end() || itMgr->second == NULL)
	{
		CExecMgr* pTaskMgr = new CExecMgr(&CSumengine2App::m_sCommClient, strTaskID);

		//如果检测到任务冲突
		if (pTaskMgr->IsMutexConfilict())
		{
			pTaskMgr->SendConfilictResultToMC();
			pTaskMgr->SetStopFlag();

			g_runlog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("收到任务ID:%s出现冲突，重新压回给主控！"), strTaskID);

			if (theApp.GetCurrentRunMode() != rmNormal && theApp.GetCurrentRunMode() != rmShow)
			{
				AfxMessageBox(_T("该网站正在运行,请稍后再试!"));
			}
			return NULL;
		}
		else
		{	
			g_mapTaskExecMgr[strTaskID] = pTaskMgr;

			//清空本网站缓存(modefy  by zhumingxing 20160520)
			if (theApp.GetCurrentRunMode() == rmNormal/* || theApp.GetCurrentRunMode() != rmShow*/)
			{
				ClearCookie(false, g_mapWebIdToUrl[strTaskID]);
			}
			else
			{
				ClearCookie(TRUE);
			}
		}
	}
	return g_mapTaskExecMgr[strTaskID];
}

CSumengine2View *CSumengine2App::NewTaskView(CStdString strTaskID, CStdString strNsName, DWORD dwAmbientProperty, bool bIsTmpView)
{
	CLocalLock lock(&m_lockNewView);

	//先查找当前空闲（没有关联任务）的View，如有则直接返回
	CSumengine2App *pMyApp = (CSumengine2App*)AfxGetApp();
	POSITION pos = pMyApp->GetFirstDocTemplatePosition();

	while(NULL != pos)
	{
		CDocTemplate *pDocTemp = pMyApp->GetNextDocTemplate(pos);
		POSITION posDoc = pDocTemp->GetFirstDocPosition();
		while(NULL != posDoc)
		{
			CDocument *pDoc = pDocTemp->GetNextDoc(posDoc);
			POSITION posView = pDoc->GetFirstViewPosition();
			while (NULL != posView)
			{
				CView *pView = pDoc->GetNextView(posView);
				if (pView->IsKindOf(RUNTIME_CLASS(CSumengine2View)))
				{
					CStdString strTmpTaskID = ((CSumengine2View*)pView)->GetTaskID();
					if (strTmpTaskID == _T(""))
					{
						((CSumengine2View*)pView)->Init(strTaskID, strNsName, dwAmbientProperty);

 						if (bIsTmpView)
 						{
 							((CSumengine2View*)pView)->GetParentFrame()->ShowWindow(SW_MINIMIZE) ;  //最小化临时视图窗口
 						}

						return (CSumengine2View*)pView;
					}
				}
			}
		}
	}


	//没有空闲的就新建一个
	m_pMainWnd->SendMessage(WM_MAINWND_NEW_FILE);

	//查找当前空闲（没有关联任务）的View并返回
	pMyApp = (CSumengine2App*)AfxGetApp();
	pos = pMyApp->GetFirstDocTemplatePosition();

	while(NULL != pos)
	{
		CDocTemplate *pDocTemp = pMyApp->GetNextDocTemplate(pos);
		POSITION posDoc = pDocTemp->GetFirstDocPosition();
		while(NULL != posDoc)
		{
			CDocument *pDoc = pDocTemp->GetNextDoc(posDoc);
			POSITION posView = pDoc->GetFirstViewPosition();
			while (NULL != posView)
			{
				CView *pView = pDoc->GetNextView(posView);
				if (pView->IsKindOf(RUNTIME_CLASS(CSumengine2View)))
				{
					CStdString strTmpTaskID = ((CSumengine2View*)pView)->GetTaskID();
					if (strTmpTaskID == _T(""))
					{
						((CSumengine2View*)pView)->Init(strTaskID, strNsName, dwAmbientProperty);

 						if (bIsTmpView)
 						{
 							((CSumengine2View*)pView)->GetParentFrame()->ShowWindow(SW_MINIMIZE) ;  //最小化临时视图窗口
 						}

						return (CSumengine2View*)pView;
					}
				}
			}
		}
	}

	g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("创建任务窗口失败！ 任务ID：%s"), CString(strTaskID));
	return NULL;
}

void CSumengine2App::UploadFile(T_UPLOAD_FILE_PARAM &param)
{
	m_updateFileObj.PushTask(param);
}

bool CSumengine2App::GetUploadResult(CStdString strWebName, T_UPLOAD_FILE_RESULT &res)
{
	return m_updateFileObj.GetUploadResult(strWebName, res);
}

void CSumengine2App::GetUrlMapData(void)
{
	CString strPath = CString(GetInstallPath()) + _T("\\6\\SiteList.txt");

	if (!PathFileExists(strPath.GetBuffer()))
	{
		return;
	}
	else
	{
		//获取SiteList.txt中网站ID对应的URL路径
		CStdioFile file;
		if (!file.Open(strPath,CFile::modeRead | CFile::typeBinary))
		{
			return;
		}

		char* pContent = new char[file.GetLength() + 1];
		memset(pContent, 0, (file.GetLength() + 1));

		file.Read(pContent, file.GetLength());
		file.Close();

		CString strContent(pContent);
		delete pContent;
		pContent = NULL;

		vector<CStdString>vecLineData;
		vecLineData.clear();

		CStdStrUtils Utils;
		Utils.SplitString(CStdString(strContent), _T("\r\n"), vecLineData, true);
		
		for (int i = 0; i < vecLineData.size(); i++)
		{
			CStdString strLine = vecLineData[i];
			//只获取四代网站，四代网站与二代网站用两个换行符隔开,此处表示四代网站列表已经取完
			if (strLine.IsEmpty())
			{
				return;
			}

			vector<CStdString>vecSpliterData;
			vecSpliterData.clear();
			Utils.SplitString(strLine, _T(","), vecSpliterData, false);

			//最新sitelist.txt版本
			if (vecSpliterData.size() == 2)
			{
				vecSpliterData[1].Replace(_T("http://"), _T(""));
				vecSpliterData[1].Replace(_T("https://"), _T(""));

				g_mapWebIdToUrl[vecSpliterData[0]] = vecSpliterData[1];
			}
			//兼容sitelist.txt旧版本
			else if (vecSpliterData.size() == 4)
			{   
				vecSpliterData[2].Replace(_T("http://"), _T(""));
				vecSpliterData[2].Replace(_T("https://"), _T(""));

				g_mapWebIdToUrl[vecSpliterData[0]] = vecSpliterData[2];
			}
		}
	}
}

void CSumengine2App::AnalysisDosParam(int iArgCount, wchar_t** arg)
{
	if (iArgCount == 1)
	{
		m_tDosParam.m_iCommpPort = g_cfgParam.nPort;
		m_tDosParam.m_iRunMode = g_cfgParam.nMode;

		return;
	}

	for (int i = 1; i < iArgCount; ++i)
	{
		CString strParam = CString(arg[i]);

		//找到端口号参数
		if (strParam.Find(_T("-p")) != -1)
		{
			strParam.Replace(_T("-p"), _T(""));
			m_tDosParam.m_iCommpPort = _ttoi(strParam);
			continue;
		}

		//找到运行模式参数
		if (strParam.Find(_T("-m")) != -1)
		{
			strParam.Replace(_T("-m"), _T(""));
			m_tDosParam.m_iRunMode = _ttoi(strParam);
			continue;
		}

		//找到URL参数
		if (strParam.Find(_T("-u")) != -1)
		{

			strParam.Replace(_T("-u"), _T(""));
			m_tDosParam.m_strUrl = strParam;
			continue;
		}

		//找到远程打码标识
		if (!strParam.CompareNoCase(_T("-f")))
		{
			m_tDosParam.m_bIsRemoteCode = TRUE;
			continue;
		}
		//找到云推广任务标记
		if (!strParam.CompareNoCase(_T("-y")))
		{
			m_tDosParam.m_bIsYunTask = TRUE;
			continue;
		}
	}
}

DWORD WINAPI ThreadStopProc(LPVOID lpParameter)//实时检测是否有退出的任务
{
	std::map<CStdString, DWORD>::iterator		iter;
	std::map<CStdString, CExecMgr*>::iterator	iter0;
	CStdString strTaskID;
	DWORD dwBeginTime;
	DWORD dwEndTime;

	while(1)
	{
		Sleep(77);

		g_lockStop.Lock();
		iter  = g_mapStopTaskMgr.begin();
		for(; iter != g_mapStopTaskMgr.end(); ++iter)
		{
			strTaskID = iter->first;
			for(iter0 = g_mapTaskExecMgr.begin(); iter0 != g_mapTaskExecMgr.end(); ++iter0)//判断任务strTaskID是不是在g_mapTaskExecMgr中存在
			{
				if (strTaskID == iter0->first)
				{
						CExecMgr* pMgr = iter0->second;
						if(pMgr)
						{
							pMgr->SetStopFlag();
							g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("已设置任务停止标记，strTaskName = %s,strTaskID = %s,"), (CString)pMgr->GetTaskWebName(), (CString)strTaskID);
							g_mapTaskExecMgr.erase(iter0);
							g_mapStopTaskMgr.erase(iter);
							iter = g_mapStopTaskMgr.begin();
							break;
						}
				}
			}
		}
		g_lockStop.Unlock();
	}

	return 0;
}