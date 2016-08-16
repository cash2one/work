
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Engine.h"

#include "MainFrm.h"
#include "EngineDoc.h"
#include "EngineView.h"
#include "ChildFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::map<CStdString, CExecMgr *> g_mapTaskExecMgr;
extern std::map<CStdString, DWORD> g_mapStopTaskMgr;
extern CLock g_lockStop;

HHOOK g_hCBT = NULL ;	// 保存CBT钩子句柄
BOOL WINAPI SetHook(BOOL isInstall);

// CBT钩子消息处理过程
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_SOURCE, &CMainFrame::OnToolBarViewSource)
	ON_COMMAND(ID_GO_BACK, &CMainFrame::OnToolBarBack)
	ON_COMMAND(ID_GO_FORWARD, &CMainFrame::OnToolBarForward)
	ON_COMMAND(ID_REFRESH, &CMainFrame::OnToolBarRefresh)
	ON_COMMAND(ID_STOP, &CMainFrame::OnToolBarStop)
	ON_COMMAND(ID_STEPCTRL, &CMainFrame::OnToolBarStepCtrl)
	//ON_COMMAND(ID_VIEW_ALL, &CMainFrame::OnToolBarViewAll)
	ON_COMMAND(ID_VAGUE_LOC, &CMainFrame::OnToolBarVagueLocate)
	ON_COMMAND(IDOK, &CMainFrame::OnNewAddressEnter)
	ON_MESSAGE(WM_MAINWND_UPDATE_ADDRESS_URL, &CMainFrame::OnUpdateAddressURL)
	ON_MESSAGE(WM_MAINWND_NEW_FILE, &CMainFrame::OnMyNewFile)
	ON_MESSAGE(WM_MAINWND_CANCEL_VAGUE_LOC, &CMainFrame::OnCancelVagueLoc)
	ON_MESSAGE(WM_MAINWND_FAILED_PAUSE, &CMainFrame::OnFailedPause)
	ON_MESSAGE(WM_MAINWND_NAVIGATE_URL, &CMainFrame::OnShowURL)

	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码

	//模糊定位时将按钮设为按下状态
	m_bVagueLocPressed = false;
	m_bStepCtrlCanPause = true;
	_sntprintf(m_szShowFlagFile, _TRUNCATE, _T("%s\\show"), GetCurPath());

}

CMainFrame::~CMainFrame()
{
	//卸载CBT钩子
	if (NULL != g_hCBT)
		SetHook(FALSE);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	
	CImageList img;
	CString str;

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndReBar.Create(this))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	// set up toolbar properties
	m_wndToolBar.GetToolBarCtrl().SetButtonWidth(50, 150);
	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	img.Create(IDB_HOTTOOLBAR, 22, 0, RGB(255, 0, 255));
	m_wndToolBar.GetToolBarCtrl().SetHotImageList(&img);
	m_wndToolBar.GetToolBarCtrl().SetImageList(&img);
	img.Detach();
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);

	int iRunMode = 0;
	int iButtons = 4;        //前进、后退、停止、刷新

	//为不同模式显示模糊定位及查看源码菜单
	if (__argc > 2)
	{
		iRunMode = theApp.GetCurrentRunMode();

		if (iRunMode == rmShow || iRunMode == rmCaibian || iRunMode == rmTest)
		{
			iButtons += 1;  //步骤控制
		}

		if (iRunMode == rmCaibian|| iRunMode == rmTest)
		{
			iButtons += 2;  //源码、模糊定位
		}
	}

	m_wndToolBar.SetButtons(NULL, iButtons);
	//m_wndToolBar.SetHeight(20);

	// set up each toolbar button
	m_wndToolBar.SetButtonInfo(eBtnGoBack, ID_GO_BACK, TBSTYLE_BUTTON , eBtnGoBack);
	str.LoadString(IDS_BACK);
	m_wndToolBar.SetButtonText(eBtnGoBack, str);
	
	m_wndToolBar.SetButtonInfo(eBtnGoForward, ID_GO_FORWARD, TBSTYLE_BUTTON, eBtnGoForward);
	str.LoadString(IDS_FORWARD);
	m_wndToolBar.SetButtonText(eBtnGoForward, str);

	m_wndToolBar.SetButtonInfo(eBtnStop, ID_STOP, TBSTYLE_BUTTON, eBtnStop);
	str.LoadString(IDS_STOP);
	m_wndToolBar.SetButtonText(eBtnStop, str);

	m_wndToolBar.SetButtonInfo(eBtnRefresh, ID_REFRESH, TBSTYLE_BUTTON, eBtnRefresh);
	str.LoadString(IDS_REFRESH);
	m_wndToolBar.SetButtonText(eBtnRefresh, str);

	if (iRunMode == rmShow || iRunMode == rmCaibian || iRunMode == rmTest)
	{
		m_wndToolBar.SetButtonInfo(eBtnStepCtrl, ID_STEPCTRL, TBSTYLE_BUTTON, eBtnStepCtrlPause);
		str.LoadString(IDS_STEPCTRL_PAUSE);
		m_wndToolBar.SetButtonText(eBtnStepCtrl, str);
	}
	//采编模式显示模糊定位及查看源码菜单
	if (iRunMode == rmCaibian || iRunMode == rmTest)
	{
		m_wndToolBar.SetButtonInfo(eBtnViewSrc, ID_VIEW_SOURCE, TBBS_BUTTON, eBtnViewSrc);
		str.LoadString(IDS_VIEW_SOURCE);
		m_wndToolBar.SetButtonText(eBtnViewSrc, str);

		m_wndToolBar.SetButtonInfo(eBtnVagueLoc, ID_VAGUE_LOC, TBSTYLE_BUTTON, eBtnVagueLoc);
		str.LoadString(IDS_VAGUE_LOC);
		m_wndToolBar.SetButtonText(eBtnVagueLoc, str);
	}
	
	// set up toolbar button sizes
	CRect rectToolBar;
	m_wndToolBar.GetItemRect(0, &rectToolBar);
	m_wndToolBar.SetSizes(rectToolBar.Size(), CSize(30,20));
	m_wndReBar.AddBar(&m_wndToolBar,NULL, NULL, RBBS_GRIPPERALWAYS|RBBS_FIXEDBMP|RBBS_USECHEVRON);

	CRect rect;
	GetWindowRect(&rect);
	int iComboxWidth = rect.Width() - 100 - rectToolBar.Width() * iButtons;  

	// create a combo box for the address bar
	if (!m_wndAddress.Create(CBS_DROPDOWN | WS_CHILD, CRect(0, 0, iComboxWidth, rectToolBar.Height()), this, AFX_IDW_TOOLBAR + 1))
	{
		TRACE0("Failed to create combobox\n");
		return -1;      // fail to create
	}

	str.LoadString(IDS_ADDRESS);
	m_wndReBar.AddBar(&m_wndAddress, str, NULL, RBBS_FIXEDBMP);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果不需要工具提示，则将此移除
	//m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED);

	SetHook(true); //安装CBT钩子屏蔽网页的安全弹框

	//正常模式时，设置不在任务栏显示内核图标（主要针对显示步骤）
	if (iRunMode == rmNormal)
	{	
		//隐藏工具菜单栏
		hideToolBar();

		//设置窗口风格为无任务栏
		ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);  
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~FWS_ADDTOTITLE; //不在主框架标题后显示子框架的标题内容

	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	if (theApp.GetCurrentRunMode() == rmNormal)
	{
		cs.style &= ~WS_MAXIMIZEBOX; //禁止窗口最大化
		cs.style &= ~WS_MINIMIZEBOX; //禁止窗口最小化
		cs.style &= ~WS_THICKFRAME;//使窗口不能用鼠标改变大小
	}
	
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序
void CMainFrame::OnToolBarViewSource()
{
	CChildFrame::m_sbViewSrcFlag = !CChildFrame::m_sbViewSrcFlag;

	//修改“源码”按钮状态
	if (CChildFrame::m_sbViewSrcFlag)
	{
		m_wndToolBar.SetButtonStyle(eBtnViewSrc, TBBS_BUTTON | TBBS_PRESSED);
	}
	else
	{
		m_wndToolBar.SetButtonStyle(eBtnViewSrc, TBBS_BUTTON);
	}

	CSumengine2App *pMyApp = (CSumengine2App*)AfxGetApp();
	POSITION pos = pMyApp->GetFirstDocTemplatePosition();
	
	//显示所有子框架窗口上的源码窗口
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
					((CChildFrame*)pView->GetParentFrame())->SendMessage(WM_MAINWND_SPLITE_WINDOW);
				}
			}
		}
	}
}

void CMainFrame::OnToolBarBack()
{
	//处理工具栏按钮消息，将其转给View处理
	CChildFrame *pActiveFrame = (CChildFrame *)GetActiveFrame();
	CSumengine2View *pView = (CSumengine2View *)pActiveFrame->GetPageWnd();
	if (NULL != pView)
	    pView->SendMessage(WM_VIEW_GO_BACK);

}

void CMainFrame::OnToolBarForward()
{
	//处理工具栏按钮消息，将其转给View处理
	CChildFrame *pActiveFrame = (CChildFrame *)GetActiveFrame();
	CSumengine2View *pView = (CSumengine2View *)pActiveFrame->GetPageWnd();
	if (NULL != pView)
		pView->SendMessage(WM_VIEW_GO_FOWARD);
}

void CMainFrame::OnToolBarRefresh()
{
	//处理工具栏按钮消息，将其转给View处理
	CChildFrame *pActiveFrame = (CChildFrame *)GetActiveFrame();
	CSumengine2View *pView = (CSumengine2View *)pActiveFrame->GetPageWnd();
	if (NULL != pView)
		pView->SendMessage(WM_VIEW_REFRESH);
}

void CMainFrame::OnToolBarStop()
{
	//处理工具栏按钮消息，将其转给View处理
	CChildFrame *pActiveFrame = (CChildFrame *)GetActiveFrame();
	CSumengine2View *pView = (CSumengine2View *)pActiveFrame->GetPageWnd();
	if (NULL != pView)
		pView->SendMessage(WM_VIEW_STOP);
}

void CMainFrame::OnToolBarStepCtrl()
{
	//处理工具栏按钮消息，暂停/继续步骤的执行
	CString str = _T("");

	if (m_bStepCtrlCanPause)
	{
		//暂停
		m_bStepCtrlCanPause = false;
		m_wndToolBar.SetButtonInfo(eBtnStepCtrl, ID_STEPCTRL, TBSTYLE_BUTTON, eBtnStepCtrlContinue);
		str.LoadString(IDS_STEPCTRL_CONTINUE);
		m_wndToolBar.SetButtonText(eBtnStepCtrl, str);

		//设置步骤等待信号，所有步骤将暂停执行
		ResetEvent(g_hStepCtrlEvent);

		m_wndStatusBar.SetPaneText(ID_SEPARATOR, _T("步骤已暂停"));
	}
	else
	{
		//继续
		m_bStepCtrlCanPause = true;
		m_wndToolBar.SetButtonInfo(eBtnStepCtrl, ID_STEPCTRL, TBSTYLE_BUTTON, eBtnStepCtrlPause);
		str.LoadString(IDS_STEPCTRL_PAUSE);
		m_wndToolBar.SetButtonText(eBtnStepCtrl, str);

		//设置步骤继续信号
		SetEvent(g_hStepCtrlEvent);
	}
}

void CMainFrame::OnToolBarVagueLocate()//模糊查找是否按下
{
	if (m_bVagueLocPressed)
	{
		m_wndToolBar.SetButtonStyle(eBtnVagueLoc, TBBS_BUTTON);
		m_bVagueLocPressed = false;
	}
	else
	{
		m_wndToolBar.SetButtonStyle(eBtnVagueLoc, TBBS_BUTTON | TBBS_PRESSED);
		m_bVagueLocPressed = true;
	}
}

//响应地址栏的回车消息
void CMainFrame::OnNewAddressEnter()
{
	// gets called when an item is entered manually into the edit box portion
	// of the Address combo box.
	// navigate to the newly selected location and also add this address to the
	// list of addresses in the combo box.
	CString str;

    m_wndAddress.GetEditCtrl()->GetWindowText(str);

	CChildFrame *pActiveFrame = (CChildFrame*)GetActiveFrame();
	CSumengine2View *pActiveView = (CSumengine2View*)pActiveFrame->GetPageWnd();
    if (!pActiveView)
		return;

	pActiveView->Navigate2(str);

 	COMBOBOXEXITEM item; 
 	item.mask = CBEIF_TEXT;
 	item.iItem = -1;
 	item.pszText = (LPTSTR)(LPCTSTR)str;
 	m_wndAddress.InsertItem(&item);
}

LRESULT CMainFrame::OnUpdateAddressURL(WPARAM wParam, LPARAM lParam)
{
	CString strURL((LPCTSTR)wParam);

	m_wndAddress.SetWindowText(strURL);

	return 0;
}

LRESULT CMainFrame::OnShowURL(WPARAM wParam, LPARAM lParam)
{
	CString strURL((LPCTSTR)wParam);

	m_wndAddress.SetWindowText(strURL);

	//处理工具栏按钮消息，将其转给View处理
	CChildFrame *pActiveFrame = (CChildFrame *)GetActiveFrame();
	CSumengine2View *pView = (CSumengine2View *)pActiveFrame->GetPageWnd();
	if (NULL != pView)
		pView->SendMessage(WM_VIEW_NAVIGATE_URL, wParam, lParam);

	return 0;
}

bool CMainFrame::GetVagueLocState(void)
{
	return m_bVagueLocPressed;
}

LRESULT CMainFrame::OnMyNewFile(WPARAM wParam, LPARAM lParam)
{
	theApp.OnFileNew();

	return 0;
}

LRESULT CMainFrame::OnCancelVagueLoc(WPARAM wParam, LPARAM lParam)
{
	if (m_bVagueLocPressed)
	{
		m_wndToolBar.SetButtonStyle(eBtnVagueLoc, TBBS_BUTTON);
		m_bVagueLocPressed = false;
	}

	return 0;
}

LRESULT CMainFrame::OnFailedPause(WPARAM wParam, LPARAM lParam)
{
	//确保当前为可暂停（即正在执行）状态
	m_bStepCtrlCanPause = true;

	//暂停
	OnToolBarStepCtrl();


	return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message != 0)
	{
		return CMDIFrameWnd::PreTranslateMessage(pMsg);
	}
	else
	{
		//g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("-----------------：%d"), pMsg->message);
	}
	return TRUE;
}


//启动Timer 检测正常模式时是否需要显示内核窗口
void CMainFrame::StartModeChkTimer(void)
{
	SetTimer(TIMER_FRAMEWIN, 1000, NULL);
}

void CMainFrame::StartEngActiveCheck(void)
{
	SetTimer(TIMER_CHECK_ENGINE_ACTIVE, 1000, NULL);
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{	
	//此处及时更新最新窗口活动时间
	if (TIMER_CHECK_ENGINE_ACTIVE == nIDEvent)
	{
		g_lCurrentTickCount = GetTickCount();
	}
	else if (TIMER_FRAMEWIN == nIDEvent)
	{
		//正常模式下，若存在标记文件则显示内核窗口
		if (theApp.GetCurrentRunMode() == rmNormal)
		{
			static bool s_bIsShowing = false;

			RECT rt;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
			int nWidth = rt.right;
			int nHight = rt.bottom;

			if (PathFileExists(m_szShowFlagFile))
			{
				//将内核窗口移到屏幕区显示
				s_bIsShowing = true;

				BYTE balpha = 0;
				GetWndTransparentValue(AfxGetMainWnd()->m_hWnd, balpha);

				RECT rect;
				::GetWindowRect(AfxGetMainWnd()->m_hWnd, &rect);

				if (!::IsWindowVisible(m_hWnd) || ::IsIconic(m_hWnd) ||
					(rect.left == rect.right) || (rect.top == rect.bottom) || (rect.left == nWidth) || (rect.top == nHight))

				{
					setWndTransparent(AfxGetMainWnd()->m_hWnd, 255);
					::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOP, 0, 0, nWidth, nHight, SWP_SHOWWINDOW | SWP_NOZORDER);
				}
				else if (balpha == 1)
				{
					setWndTransparent(AfxGetMainWnd()->m_hWnd, 255);
					::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW | SWP_NOZORDER);
				}
			}
			else
			{
				if (s_bIsShowing)
				{
					s_bIsShowing = false;
					::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_BOTTOM, nWidth, nHight, nWidth, nHight, SWP_HIDEWINDOW);
				}
			}
		}

	}
	
	CMDIFrameWnd::OnTimer(nIDEvent);
}

//捕获文件选择对话框窗口过程(可能会导致文件选择框关不掉？去掉不影响。。。)
//WNDPROC g_pFiledlgProc;
//static LRESULT CALLBACK SelFileWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	//隐藏文件选择对话框
//	if (message == WM_WINDOWPOSCHANGING)
//	{
//		WINDOWPOS *pWndPos = (WINDOWPOS *)lParam;
//
//		if (pWndPos->flags&SWP_SHOWWINDOW)
//		{
//			pWndPos->flags &= 0xFFFFFFBF;
//		}
//	}
//	return CallWindowProc(g_pFiledlgProc, hWnd, message, wParam, lParam);
//}

//CBT钩子消息处理过程
LRESULT CALLBACK CBTProc ( int nCode, WPARAM wParam, LPARAM lParam )
{
	CBT_CREATEWND *pWndParam = NULL ;
	CREATESTRUCT *pCreateStruct = NULL ;
	CString strText ;
	CString strParentTitle = _T("");

	HWND hAboutToActive = NULL;

	switch ( nCode )
	{

		case HCBT_CREATEWND:	
			pWndParam = (CBT_CREATEWND*)lParam;
			pCreateStruct = pWndParam->lpcs;
			strText = pCreateStruct->lpszName;

			//针对jQuery弹框
			if (pCreateStruct->hwndParent )
			{
				::GetWindowText(pCreateStruct->hwndParent, strParentTitle.GetBuffer(MAX_PATH), MAX_PATH);
				strParentTitle.ReleaseBuffer(MAX_PATH);
			
				if (strText == _T("") && strParentTitle != _T(""))
				{
					strText = strParentTitle;
					g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获到jQuery弹框提示，标题：%s"), strParentTitle);
				}
			}

			if(strText == _T("") && strParentTitle == _T(""))
			{
				break;
			}
			//监视类型为WS_POPUP且标题中含有过滤字符串的窗口创建
			std::vector<CString>::iterator iter;
			bool bInclude = false;
			CString strTmp;
		
			for(iter = g_vecWords.begin(); iter != g_vecWords.end(); ++iter)
			{
				strTmp = (CString)(*iter);
				if(-1 != strText.Find(strTmp) || -1 != strParentTitle.Find(strTmp))
				{
					bInclude = true;
					goto FindSuss;
				}
			}
	FindSuss:
			if (bInclude)
			{
				//采编允许弹框，否则屏蔽弹框
				if (rmNormal != theApp.GetCurrentRunMode() && rmShow != theApp.GetCurrentRunMode())
				{
					break;
				}
				else
				{
					return 1;
				}
			}
			if ((CString(pCreateStruct->lpszName).Find(_T("打开(&O)")) != -1) || (CString(pCreateStruct->lpszName).Find(_T("保存(&S)")) != -1))
			{	
				if (rmNormal == theApp.GetCurrentRunMode() || rmShow == theApp.GetCurrentRunMode())
				{	
					//捕获窗口过程
					//g_pFiledlgProc = (WNDPROC)SetWindowLong(pCreateStruct->hwndParent, GWL_WNDPROC, (LONG)SelFileWndProc);

					//设置透明
					setWndTransparent(pCreateStruct->hwndParent, 0);
				}
			}
			if (!strParentTitle.CompareNoCase(_T("发布图片 - 久久信息网 -- 网页对话框")))
			{
				setWndTransparent(pCreateStruct->hwndParent, 0);
			}
			break;
	}
	// 继续传递消息
	return CallNextHookEx ( g_hCBT, nCode, wParam, lParam ) ;
}


BOOL WINAPI SetHook ( BOOL isInstall ) 
{
	// 需要安装，且钩子不存在
	if ( isInstall && !g_hCBT )
	{
		// 设置全局钩子
		g_hCBT = SetWindowsHookEx ( WH_CBT, (HOOKPROC)CBTProc, 0, GetCurrentThreadId() ) ;
		if ( g_hCBT == NULL )
			return FALSE ;
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

void CMainFrame::OnClose()
{
	//停止接收数据
 	theApp.m_sCommClient.Stop();
 	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{	
	if (rmNormal == theApp.GetCurrentRunMode())
	{
		g_ocrInput.FocusOcrInput();
	}
	else
	{
		return CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	}
}

void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (theApp.GetCurrentRunMode() == rmNormal)
	{
		return;
	}

	CMDIFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}

void CMainFrame::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (theApp.GetCurrentRunMode() == rmNormal)
	{
		return;
	}

	CMDIFrameWnd::OnNcRButtonDown(nHitTest, point);
}


void CMainFrame::hideToolBar()
{
	m_wndToolBar.ShowWindow(SW_HIDE);
	m_wndStatusBar.ShowWindow(SW_HIDE);
	m_wndAddress.ShowWindow(SW_HIDE);
	SetMenu(NULL);
}

