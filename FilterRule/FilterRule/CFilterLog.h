#pragma once
#include "afxwin.h"


// CCFilterLog 对话框
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
	CCFilterLog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCFilterLog();

// 对话框数据
	enum { IDD = IDD_FILTERLOG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	//初始化数据
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
	// 比较类型
	CComboBox m_cboCmptype;
	// 分隔符控件
	CComboBox m_cboSplit;
	// 是否要提交快照
	CButton m_chkSubmitPhoto;


private:
	//获取提取日志类型字符串
	E_LOG_TYPE GetLogType(const string& strType);
	//根据类型选定单选框
	void SetRad(E_LOG_TYPE eType);
	//设置版本号比较类型
	void SetCompareType(const string& strCmp,CComboBox* cbo);

	//选择分割类型；
	string SelectSplit(int Index);

private:
	E_LOG_TYPE m_LogType;   //提取日志类型

public:
	afx_msg void OnBnClickedRadPostupgrade();
	afx_msg void OnBnClickedRadFirstboot();
	// 推送升级比较类型
	CComboBox m_cboCmpPostType;
	// 是否有推送升级
	CButton m_chkPostUpgrade;
	// 是否开机启动
	CButton m_chkBootStart;
};
