// LevelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Engine.h"
#include "LevelDlg.h"
#include "afxdialogex.h"


// CLevelDlg 对话框

IMPLEMENT_DYNAMIC(CLevelDlg, CDialogEx)

CLevelDlg::CLevelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLevelDlg::IDD, pParent)
{
	m_nLevel = 0;
}

CLevelDlg::~CLevelDlg()
{
}

void CLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LEVEL, m_editLevel);
}


BEGIN_MESSAGE_MAP(CLevelDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLevelDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLevelDlg 消息处理程序


BOOL CLevelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_nLevel = 0;
	SetDlgItemInt(IDC_EDIT_LEVEL, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CLevelDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bTrans = TRUE;
	BOOL bSign = TRUE;
	m_nLevel = GetDlgItemInt(IDC_EDIT_LEVEL, &bTrans, bSign);
	if(bTrans == FALSE)
	{
		m_nLevel = 0;
		SetDlgItemInt(IDC_EDIT_LEVEL, 0);
		::MessageBox(NULL, _T("所填的必须是一个正整数"), _T("error"), MB_OK);
		return;
	}
	if(m_nLevel < 0)
	{
		m_nLevel = 0;
		SetDlgItemInt(IDC_EDIT_LEVEL, 0);
		::MessageBox(NULL, _T("所填的必须是一个正整数"), _T("error"), MB_OK);
		return;
	}
	CDialogEx::OnOK();
}
