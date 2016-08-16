#pragma once

#include "Lock.h"
#include "resource.h"
// CMyMessageBox dialog

class CMessageShow
{
public:
	CMessageShow();
	~CMessageShow();

	void Show(CStdString strTitle, CStdString strMsg);

protected:
private:
	CLock m_lock;
};

class CMyMessageBox : public CDialogEx
{
	DECLARE_DYNAMIC(CMyMessageBox)

public:
	CMyMessageBox(CWnd* pParent = NULL);   // standard constructor
	CMyMessageBox(CStdString strTitle, CStdString strMsg, CWnd* pParent = NULL);   // standard constructor

	virtual ~CMyMessageBox();


// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CStdString m_strTitle;
	CStdString m_strMsg;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
