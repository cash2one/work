#pragma once


// CLevelDlg 对话框

class CLevelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLevelDlg)

public:
	CLevelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLevelDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_LEVEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	int   m_nLevel;
	CEdit m_editLevel;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
