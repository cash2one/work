// MyMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "Engine.h"
#include "MyMessageBox.h"
#include "afxdialogex.h"


// CMyMessageBox dialog

IMPLEMENT_DYNAMIC(CMyMessageBox, CDialogEx)

CMyMessageBox::CMyMessageBox(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyMessageBox::IDD, pParent)
{
}

CMyMessageBox::CMyMessageBox(CStdString strTitle, CStdString strMsg, CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyMessageBox::IDD, pParent)
{
	m_strTitle = strTitle;
	m_strMsg = strMsg;
}

CMyMessageBox::~CMyMessageBox()
{
}

void CMyMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

CMessageShow::CMessageShow()
{
}

CMessageShow::~CMessageShow()
{
}

// CMyMessageBox message handlers
void CMessageShow::Show(CStdString strTitle, CStdString strMsg)
{
	CLocalLock lc(&m_lock);

	if (theApp.GetCurrentRunMode() == rmCaibian || theApp.GetCurrentRunMode() == rmTest)
	{
		CMyMessageBox msgbox(strTitle, strMsg);

		msgbox.DoModal();
	}
}


BEGIN_MESSAGE_MAP(CMyMessageBox, CDialogEx)
END_MESSAGE_MAP()


BOOL CMyMessageBox::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(m_strTitle);
	((CStatic*)GetDlgItem(IDC_STATIC_MSG))->SetWindowText(m_strMsg);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
