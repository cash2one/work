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

	virtual BOOL Init(BOOL bDebug = FALSE) = 0;	//初始化
	virtual BOOL UnInit() = 0;  //反初始化

	virtual BOOL SetHtmlCode(DWORD dwCode) = 0;
	virtual BOOL SetProxy(BSTR strProxy,BSTR strCheck,BSTR strKey) = 0;

	virtual BOOL GetImage(BSTR URL,BSTR SavePath) = 0;	//获得图片,保存到指定的路径中
	virtual BOOL GetURL(BSTR URL,BSTR *pRet = NULL) = 0;	//Get方式打开一个URL，返回字符串


	virtual BOOL InsertForm(BSTR FormName,BSTR Action,BSTR Method,BSTR EncType=NULL) = 0; //插入FORM
	virtual BOOL InsertVar(BSTR FormName,BSTR VarName,BSTR VarVal,BSTR VarType=NULL) = 0; //插入变量
	virtual BOOL RemoveVar(BSTR FormName,BSTR VarName) = 0;				//删除变量
	virtual BOOL Submit(BSTR FormName,BSTR *pRet = NULL) = 0;	//Form进行提交，返回字符串

	virtual BOOL SetCookie(BSTR VarVal) = 0;	//设置Cookie
	virtual BOOL GetCookie(BSTR *pRet) = 0;	//获得Cookie值

	virtual BOOL SetReferer(BSTR VarVal) = 0; //设置Referrer值
	virtual BOOL GetReferer(BSTR *pRet) = 0; //获得Referrer值

	virtual BOOL GetAllResponseHeader(BSTR *pRet) = 0; //获得ResponseHeader头
	
	virtual BOOL ClearInfo() = 0; //清除一些初始信息，如cookie

	virtual BOOL SetAutoRedirect(BOOL bAuto) = 0;  //设置是否要自动跳转
};

#endif // !defined(AFX_IWEBOPER_H__028649C0_F20C_494C_87E2_0C44B25C8046__INCLUDED_)
