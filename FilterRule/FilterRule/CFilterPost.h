#pragma once
#include "afxwin.h"


// CCFilterPost �Ի���

class CCFilterPost : public CDialogEx
{
	DECLARE_DYNAMIC(CCFilterPost)

public:
	CCFilterPost(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCFilterPost();

// �Ի�������
	enum { IDD = IDD_FILTERINSTALL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedBtnLoadinstall();
	afx_msg void OnBnClickedBtnSaveinstall();
public:
	//��ʼ������
	void InitData();

protected:
	// ���߰�Ƚ�����
	CComboBox m_cboMainCmp;
	// ���ư�Ƚ�����
	CComboBox m_CustmonCmp;

private:
	//��ȡ���Ժ�ֵ
	bool GetXmlAttr(xml_node* node);
	//���ð汾�űȽ�����
	void SetCompareType(const string& strCmp, CComboBox* cbo);
	//��дֵ����Ӧ�Ŀ��
	void SetValutToCtl(const string& strName, const string& strValue);

private:
	string m_strCurType;  //0Ϊ���߰棬1�Ƕ��ư�

};
