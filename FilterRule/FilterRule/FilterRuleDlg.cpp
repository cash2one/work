
// FilterRuleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FilterRule.h"
#include "FilterRuleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFilterRuleDlg �Ի���



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


// CFilterRuleDlg ��Ϣ�������

BOOL CFilterRuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_TabFilterRule.InsertItem(0, _T("��װ����������"));
	m_TabFilterRule.InsertItem(1, _T("��־��������"));


	ChangeTab();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFilterRuleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFilterRuleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//�л�Tab
void CFilterRuleDlg::OnTcnSelchangeTabFilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	ChangeTab();
	*pResult = 0;
}

//�ı�Tab
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

	//������һ������
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

