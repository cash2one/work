
// FilterRuleDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "CFilterLog.h"
#include "CFilterPost.h"

// CFilterRuleDlg 对话框
class CFilterRuleDlg : public CDialogEx
{
// 构造
public:
	CFilterRuleDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CFilterRuleDlg();
// 对话框数据
	enum { IDD = IDD_FILTERRULE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabFilterRule;
	afx_msg void OnTcnSelchangeTabFilter(NMHDR *pNMHDR, LRESULT *pResult);

private:
	//改变Tab
	void ChangeTab();


private:
	CCFilterLog*	m_Filterlog;	//过滤日志对话框
	CCFilterPost*   m_FilterPost;   //过滤推送对话框

	CWnd*	m_pCurrShow;     //当前显示
	
};
