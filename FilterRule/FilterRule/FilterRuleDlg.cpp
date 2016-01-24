
// FilterRuleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FilterRule.h"
#include "FilterRuleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFilterRuleDlg 对话框



CFilterRuleDlg::CFilterRuleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFilterRuleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CFilterRuleDlg::~CFilterRuleDlg()
{
	delete m_FilterPost;
	delete m_Filterlog;

}

void CFilterRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_FILTER, m_TabFilterRule);
}

BEGIN_MESSAGE_MAP(CFilterRuleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FILTER, &CFilterRuleDlg::OnTcnSelchangeTabFilter)
END_MESSAGE_MAP()


// CFilterRuleDlg 消息处理程序

BOOL CFilterRuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_TabFilterRule.InsertItem(0, _T("安装包推送设置"));
	m_TabFilterRule.InsertItem(1, _T("日志快照设置"));


	ChangeTab();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFilterRuleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFilterRuleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//切换Tab
void CFilterRuleDlg::OnTcnSelchangeTabFilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码

	ChangeTab();
	*pResult = 0;
}

//改变Tab
void CFilterRuleDlg::ChangeTab()
{
	int iSel = m_TabFilterRule.GetCurSel();
	if (-1 == iSel)
	{
		iSel = 0;
	}

	RECT rcTab;
	GetDlgItem(IDC_TAB_FILTER)->GetWindowRect(&rcTab);
	ScreenToClient(&rcTab);

	rcTab.top += 25;
	rcTab.bottom -= 5;
	rcTab.left += 5;
	rcTab.right -= 5;

	//隐藏上一个界面
	if (m_pCurrShow)
	{
		m_pCurrShow->UpdateData(TRUE);
		m_pCurrShow->ShowWindow(SW_HIDE);
	}

	m_pCurrShow = NULL;

	switch (iSel)
	{
	case 0:
		if (m_FilterPost == NULL)
		{			
			m_FilterPost = new CCFilterPost;
			m_FilterPost->Create(IDD_FILTERINSTALL_DLG, this);
			m_FilterPost->InitData();
		}
		m_pCurrShow = m_FilterPost;
		break;
	case 1:
		if (m_Filterlog == NULL)
		{
			m_Filterlog = new CCFilterLog;
			m_Filterlog->Create(IDD_FILTERLOG_DLG, this);
			m_Filterlog->InitData();
		}
		m_pCurrShow = m_Filterlog;
		break;
	default:
		break;
	}

	if (m_pCurrShow)
	{
		m_pCurrShow->MoveWindow(&rcTab);
		m_pCurrShow->ShowWindow(SW_SHOW);
		m_pCurrShow->UpdateData(FALSE);
	}
}

