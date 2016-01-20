#pragma once 

#include <Windows.h>
#include "StdString.h"
#define WM_CODEDATA_IN WM_USER+103
#define WM_CODEDATA_BACK WM_USER+203

#define PUSHCODEDATA "PushCodeData"
#define SETOCRINPUTFOCUS "SetOCRInputFocus"
#define SETINPUTFOCUSEVENT "ReleaseInputFocusMutex"
#define GETCODEDATABACK "GetCodeDataBack"

//有内核传送过来的验证码数据	
/*struct T_CodeData_IN
{
	HWND hView ;  //内核view窗口句柄
	long lgTimeWait ;  //等待输入的时间
	CString strPicPath ;  //验证码图片路径
	CString strAutoResult ;  //自动识别结果 
	CString strNsCode ;  //网站ID
	CString strNsName ; //网站名
	int nCodeType ;  //类型：手机号 验证短信 验证码等 为以后扩展
	bool bEnableRefresh; //是否支持刷新  用于区别是2代还是4代的调用

	T_CodeData_IN()
	{
		hView = NULL;
		lgTimeWait = 0;
		strPicPath = _T("");
		strAutoResult = _T("");
		strNsCode = _T("");
		strNsName = _T("");
		nCodeType = 1; //默认类型为验证码图片
		bEnableRefresh = true;  //默认允许刷新
	}
};*/

struct T_CodeData_IN
{
	bool bEnableRefresh; //是否支持刷新  用于区别是2代还是4代的调用
	int nCodeType ;  //类型：手机号 验证短信 验证码等 为以后扩展
	HWND hView ;  //内核view窗口句柄
	long lgTimeWait ;  //等待输入的时间
	CString strPicPath ;  //验证码图片路径
	CString strAutoResult ;  //自动识别结果 
	CString strNsCode ;  //网站ID
	CString strNsName ; //网站名
	
	T_CodeData_IN()
	{
		bEnableRefresh	= true;  //默认允许刷新
		nCodeType		= 1; //默认类型为验证码图片
		hView			= NULL;
		lgTimeWait		= 0;
		strPicPath		= _T("");
		strAutoResult	= _T("");
		strNsCode		= _T("");
		strNsName		= _T("");
	}

	T_CodeData_IN(const T_CodeData_IN &tempCodeDataIn)
	{
		bEnableRefresh	= tempCodeDataIn.bEnableRefresh;
		nCodeType		= tempCodeDataIn.nCodeType;  //1:显示验证码 0：不显示验证码
		hView			= tempCodeDataIn.hView ;  //返回最终结果
		lgTimeWait		= tempCodeDataIn.lgTimeWait ; //网站ID
		strPicPath		= tempCodeDataIn.strPicPath ; //刷新图片的保存路径
		strAutoResult	= tempCodeDataIn.strAutoResult ; //刷新图片的保存路径
		strNsCode		= tempCodeDataIn.strNsCode ; //刷新图片的保存路径
		strNsName		= tempCodeDataIn.strNsName ; //刷新图片的保存路径
	}

	T_CodeData_IN & operator=(const T_CodeData_IN &tempCodeDataIn)
	{
		if(this == &tempCodeDataIn)
		{
			return *this;
		}

		bEnableRefresh	= tempCodeDataIn.bEnableRefresh;
		nCodeType		= tempCodeDataIn.nCodeType;  //1:显示验证码 0：不显示验证码
		hView			= tempCodeDataIn.hView ;  //返回最终结果
		lgTimeWait		= tempCodeDataIn.lgTimeWait ; //网站ID
		strPicPath		= tempCodeDataIn.strPicPath ; //刷新图片的保存路径
		strAutoResult	= tempCodeDataIn.strAutoResult ; //刷新图片的保存路径
		strNsCode		= tempCodeDataIn.strNsCode ; //刷新图片的保存路径
		strNsName		= tempCodeDataIn.strNsName ; //刷新图片的保存路径

		return *this;
	}
};

//返回给你内核的识别结果
/*struct T_CodeData_Back
{
	CString strNsCode ; //网站ID
	BOOL bIsRefresh ;  //是否是刷新
	CString strResult ;  //返回最终结果
	CString strPicPath ; //刷新图片的保存路径
	int nCodeInputMode;  //1:显示验证码 0：不显示验证码

	T_CodeData_Back()
	{
		bIsRefresh = FALSE ;
	}
};*/

struct T_CodeData_Back
{
	BOOL bIsRefresh ;  //是否是刷新
	int nCodeInputMode;  //1:显示验证码 0：不显示验证码
	CString strNsCode ; //网站ID
	CString strResult ;  //返回最终结果
	CString strPicPath ; //刷新图片的保存路径
	
	T_CodeData_Back()
	{
		bIsRefresh = FALSE ;
		strNsCode = _T("");
		strResult = _T("");
		strPicPath = _T("");
	}

	T_CodeData_Back(const T_CodeData_Back &tempCodeDataBack)
	{
		bIsRefresh		= tempCodeDataBack.bIsRefresh;
		nCodeInputMode	= tempCodeDataBack.nCodeInputMode;  //1:显示验证码 0：不显示验证码
		strNsCode		= tempCodeDataBack.strNsCode ; //网站ID
		strResult		= tempCodeDataBack.strResult ;  //返回最终结果
		strPicPath		= tempCodeDataBack.strPicPath ; //刷新图片的保存路径
	}

	T_CodeData_Back & operator=(const T_CodeData_Back &tempCodeDataBack)
	{
		if(this == &tempCodeDataBack)
		{
			return *this;
		}

		bIsRefresh		= tempCodeDataBack.bIsRefresh;
		nCodeInputMode	= tempCodeDataBack.nCodeInputMode;  //1:显示验证码 0：不显示验证码
		strNsCode		= tempCodeDataBack.strNsCode ; //网站ID
		strResult		= tempCodeDataBack.strResult ;  //返回最终结果
		strPicPath		= tempCodeDataBack.strPicPath ; //刷新图片的保存路径

		return *this;
	}
};

struct T_DisCode_Config
{
	int m_nFailedTimer ;
	int m_nSuccessTimer ;

	T_DisCode_Config()
	{
		m_nFailedTimer = 60 ;
		m_nSuccessTimer = 15 ;
	}
};

typedef BOOL (*PUSH_OCR_DATA)(T_CodeData_IN &); 
typedef BOOL (*SET_OCR_INPUTFOCUS)(void) ;   //设置输入焦点
typedef BOOL (*RELEASE_INPUTFOCUS_MUTEX)(void) ; //设置上传文件互斥事件
typedef BOOL (*GET_CODEDATA_BACK)(CString , T_CodeData_Back &) ;  //获取验证码弹框返回的数据

typedef LONG (*CODE_RECOGNIZE)(LPCTSTR strImgPath, LPTSTR strCode); 
typedef LONG (*CODE_ERROR)(LONG nResultCode); 
typedef DWORD (*CODE_WE_RECOGNIZE)(CString strRecegiseParm, CString strImgPath, CString &strResult);


