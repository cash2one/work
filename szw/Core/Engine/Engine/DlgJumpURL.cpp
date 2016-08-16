// DlgJumpURL.cpp : implementation file
//

#include "stdafx.h"
#include "Sumengine2.h"
#include "DlgJumpURL.h"
#include "afxdialogex.h"


// CDlgJumpURL dialog

IMPLEMENT_DYNAMIC(CDlgJumpURL, CDialogEx)

CDlgJumpURL::CDlgJumpURL(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgJumpURL::IDD, pParent)
{
}

CDlgJumpURL::~CDlgJumpURL()
{
}

void CDlgJumpURL::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgJumpURL, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgJumpURL::OnBnClickedOk)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgJumpURL message handlers


BOOL CDlgJumpURL::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgJumpURL::OnBnClickedOk()
{
 	// TODO: Add your control notification handler code here
 	CString strURL = _T("");
 	((CEdit*)GetDlgItem(IDC_EDIT_URL))->GetWindowText(strURL);
 	//m_pParentWnd->SendMessage(WM_VIEW_SET_JUMP_URL, (WPARAM)((LPCTSTR)strURL));
 
	m_strURL = strURL;

 	CDialogEx::OnOK();
}


void CDlgJumpURL::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	((CEdit*)GetDlgItem(IDC_EDIT_URL))->SetFocus();

	// TODO: Add your message handler code here
}

CStdString CDlgJumpURL::GetURL(void)
{
	return m_strURL;
}