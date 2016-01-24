
// FilterRuleDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "CFilterLog.h"
#include "CFilterPost.h"

// CFilterRuleDlg �Ի���
class CFilterRuleDlg : public CDialogEx
{
// ����
public:
	CFilterRuleDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CFilterRuleDlg();
// �Ի�������
	enum { IDD = IDD_FILTERRULE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabFilterRule;
	afx_msg void OnTcnSelchangeTabFilter(NMHDR *pNMHDR, LRESULT *pResult);

private:
	//�ı�Tab
	void ChangeTab();


private:
	CCFilterLog*	m_Filterlog;	//������־�Ի���
	CCFilterPost*   m_FilterPost;   //�������ͶԻ���

	CWnd*	m_pCurrShow;     //��ǰ��ʾ
	
};
