// IWebOper.h: interface for the IWebOper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IWEBOPER_H__028649C0_F20C_494C_87E2_0C44B25C8046__INCLUDED_)
#define AFX_IWEBOPER_H__028649C0_F20C_494C_87E2_0C44B25C8046__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IWebOper  
{
public:

	virtual BOOL Init(BOOL bDebug = FALSE) = 0;	//��ʼ��
	virtual BOOL UnInit() = 0;  //����ʼ��

	virtual BOOL SetHtmlCode(DWORD dwCode) = 0;
	virtual BOOL SetProxy(BSTR strProxy,BSTR strCheck,BSTR strKey) = 0;

	virtual BOOL GetImage(BSTR URL,BSTR SavePath) = 0;	//���ͼƬ,���浽ָ����·����
	virtual BOOL GetURL(BSTR URL,BSTR *pRet = NULL) = 0;	//Get��ʽ��һ��URL�������ַ���


	virtual BOOL InsertForm(BSTR FormName,BSTR Action,BSTR Method,BSTR EncType=NULL) = 0; //����FORM
	virtual BOOL InsertVar(BSTR FormName,BSTR VarName,BSTR VarVal,BSTR VarType=NULL) = 0; //�������
	virtual BOOL RemoveVar(BSTR FormName,BSTR VarName) = 0;				//ɾ������
	virtual BOOL Submit(BSTR FormName,BSTR *pRet = NULL) = 0;	//Form�����ύ�������ַ���

	virtual BOOL SetCookie(BSTR VarVal) = 0;	//����Cookie
	virtual BOOL GetCookie(BSTR *pRet) = 0;	//���Cookieֵ

	virtual BOOL SetReferer(BSTR VarVal) = 0; //����Referrerֵ
	virtual BOOL GetReferer(BSTR *pRet) = 0; //���Referrerֵ

	virtual BOOL GetAllResponseHeader(BSTR *pRet) = 0; //���ResponseHeaderͷ
	
	virtual BOOL ClearInfo() = 0; //���һЩ��ʼ��Ϣ����cookie

	virtual BOOL SetAutoRedirect(BOOL bAuto) = 0;  //�����Ƿ�Ҫ�Զ���ת
};

#endif // !defined(AFX_IWEBOPER_H__028649C0_F20C_494C_87E2_0C44B25C8046__INCLUDED_)
