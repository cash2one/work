#pragma once
#include "afxwin.h"


// CCFilterPost 对话框

class CCFilterPost : public CDialogEx
{
	DECLARE_DYNAMIC(CCFilterPost)

public:
	CCFilterPost(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCFilterPost();

// 对话框数据
	enum { IDD = IDD_FILTERINSTALL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedBtnLoadinstall();
	afx_msg void OnBnClickedBtnSaveinstall();
public:
	//初始化数据
	void InitData();

protected:
	// 主线版比较类型
	CComboBox m_cboMainCmp;
	// 定制版比较类型
	CComboBox m_CustmonCmp;

private:
	//获取属性和值
	bool GetXmlAttr(xml_node* node);
	//设置版本号比较类型
	void SetCompareType(const string& strCmp, CComboBox* cbo);
	//填写值到对应的框框；
	void SetValutToCtl(const string& strName, const string& strValue);

private:
	string m_strCurType;  //0为主线版，1是定制版

};
