
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CToolBar          m_wndToolBar;
	CReBar            m_wndReBar;
	CMenu			  *m_Menue;
	CStatusBar        m_wndStatusBar;

	CComboBoxEx m_wndAddress;
	bool m_bVagueLocPressed;
	bool m_bStepCtrlCanPause;
	TCHAR m_szShowFlagFile[MAX_PATH];

public:
	bool GetVagueLocState(void);

	//启动Timer 检测正常模式时是否需要显示内核窗口
	void StartModeChkTimer(void); 

	//启动定时器检测内核是否卡死
	void StartEngActiveCheck(void);

	//隐藏菜单栏和工具栏等
	void hideToolBar();

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnToolBarViewSource();
	afx_msg void OnToolBarBack();
	afx_msg void OnToolBarForward();
	afx_msg void OnToolBarRefresh();
	afx_msg void OnToolBarStop();
	afx_msg void OnToolBarStepCtrl();
	afx_msg void OnToolBarVagueLocate();
	afx_msg void OnNewAddressEnter();

	afx_msg LRESULT OnUpdateAddressURL(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowURL(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMyNewFile(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCancelVagueLoc(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFailedPause(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
};

