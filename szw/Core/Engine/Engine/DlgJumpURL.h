#pragma once


// CDlgJumpURL dialog

class CDlgJumpURL : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgJumpURL)

public:
	CDlgJumpURL(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgJumpURL();

	CStdString GetURL(void);
// Dialog Data
	enum { IDD = IDD_DIALOG_JUMP_URL };

private:
	CWnd *m_pParent;
	CStdString m_strURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
