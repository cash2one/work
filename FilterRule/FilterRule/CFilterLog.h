#pragma once
#include "afxwin.h"


// CCFilterLog �Ի���
enum E_LOG_TYPE
{
	E_LOG_USER,
	E_LOG_VER,
	E_LOG_MAC,
	E_POST_UPGRADE,
	E_FIRST_BOOT
};

class CCFilterLog : public CDialogEx
{
	DECLARE_DYNAMIC(CCFilterLog)

public:
	CCFilterLog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCFilterLog();

// �Ի�������
	enum { IDD = IDD_FILTERLOG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//��ʼ������
	void InitData();
private:
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnLoadlogxml();
	afx_msg void OnBnClickedBtnSavelog();
//	afx_msg void OnBnClickedBtnCancellog();
	afx_msg void OnCbnSelchangeCboCmptypelog();
	afx_msg void OnBnClickedRadUsername();
	afx_msg void OnBnClickedRadUsermac();
	afx_msg void OnBnClickedRadVersion();


protected:
	// �Ƚ�����
	CComboBox m_cboCmptype;
	// �ָ����ؼ�
	CComboBox m_cboSplit;
	// �Ƿ�Ҫ�ύ����
	CButton m_chkSubmitPhoto;


private:
	//��ȡ��ȡ��־�����ַ���
	E_LOG_TYPE GetLogType(const string& strType);
	//��������ѡ����ѡ��
	void SetRad(E_LOG_TYPE eType);
	//���ð汾�űȽ�����
	void SetCompareType(const string& strCmp,CComboBox* cbo);

	//ѡ��ָ����ͣ�
	string SelectSplit(int Index);

private:
	E_LOG_TYPE m_LogType;   //��ȡ��־����

public:
	afx_msg void OnBnClickedRadPostupgrade();
	afx_msg void OnBnClickedRadFirstboot();
	// ���������Ƚ�����
	CComboBox m_cboCmpPostType;
	// �Ƿ�����������
	CButton m_chkPostUpgrade;
	// �Ƿ񿪻�����
	CButton m_chkBootStart;
};
