#pragma once 

#include <Windows.h>
#include "StdString.h"
#define WM_CODEDATA_IN WM_USER+103
#define WM_CODEDATA_BACK WM_USER+203

#define PUSHCODEDATA "PushCodeData"
#define SETOCRINPUTFOCUS "SetOCRInputFocus"
#define SETINPUTFOCUSEVENT "ReleaseInputFocusMutex"
#define GETCODEDATABACK "GetCodeDataBack"

//���ں˴��͹�������֤������	
/*struct T_CodeData_IN
{
	HWND hView ;  //�ں�view���ھ��
	long lgTimeWait ;  //�ȴ������ʱ��
	CString strPicPath ;  //��֤��ͼƬ·��
	CString strAutoResult ;  //�Զ�ʶ���� 
	CString strNsCode ;  //��վID
	CString strNsName ; //��վ��
	int nCodeType ;  //���ͣ��ֻ��� ��֤���� ��֤��� Ϊ�Ժ���չ
	bool bEnableRefresh; //�Ƿ�֧��ˢ��  ����������2������4���ĵ���

	T_CodeData_IN()
	{
		hView = NULL;
		lgTimeWait = 0;
		strPicPath = _T("");
		strAutoResult = _T("");
		strNsCode = _T("");
		strNsName = _T("");
		nCodeType = 1; //Ĭ������Ϊ��֤��ͼƬ
		bEnableRefresh = true;  //Ĭ������ˢ��
	}
};*/

struct T_CodeData_IN
{
	bool bEnableRefresh; //�Ƿ�֧��ˢ��  ����������2������4���ĵ���
	int nCodeType ;  //���ͣ��ֻ��� ��֤���� ��֤��� Ϊ�Ժ���չ
	HWND hView ;  //�ں�view���ھ��
	long lgTimeWait ;  //�ȴ������ʱ��
	CString strPicPath ;  //��֤��ͼƬ·��
	CString strAutoResult ;  //�Զ�ʶ���� 
	CString strNsCode ;  //��վID
	CString strNsName ; //��վ��
	
	T_CodeData_IN()
	{
		bEnableRefresh	= true;  //Ĭ������ˢ��
		nCodeType		= 1; //Ĭ������Ϊ��֤��ͼƬ
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
		nCodeType		= tempCodeDataIn.nCodeType;  //1:��ʾ��֤�� 0������ʾ��֤��
		hView			= tempCodeDataIn.hView ;  //�������ս��
		lgTimeWait		= tempCodeDataIn.lgTimeWait ; //��վID
		strPicPath		= tempCodeDataIn.strPicPath ; //ˢ��ͼƬ�ı���·��
		strAutoResult	= tempCodeDataIn.strAutoResult ; //ˢ��ͼƬ�ı���·��
		strNsCode		= tempCodeDataIn.strNsCode ; //ˢ��ͼƬ�ı���·��
		strNsName		= tempCodeDataIn.strNsName ; //ˢ��ͼƬ�ı���·��
	}

	T_CodeData_IN & operator=(const T_CodeData_IN &tempCodeDataIn)
	{
		if(this == &tempCodeDataIn)
		{
			return *this;
		}

		bEnableRefresh	= tempCodeDataIn.bEnableRefresh;
		nCodeType		= tempCodeDataIn.nCodeType;  //1:��ʾ��֤�� 0������ʾ��֤��
		hView			= tempCodeDataIn.hView ;  //�������ս��
		lgTimeWait		= tempCodeDataIn.lgTimeWait ; //��վID
		strPicPath		= tempCodeDataIn.strPicPath ; //ˢ��ͼƬ�ı���·��
		strAutoResult	= tempCodeDataIn.strAutoResult ; //ˢ��ͼƬ�ı���·��
		strNsCode		= tempCodeDataIn.strNsCode ; //ˢ��ͼƬ�ı���·��
		strNsName		= tempCodeDataIn.strNsName ; //ˢ��ͼƬ�ı���·��

		return *this;
	}
};

//���ظ����ں˵�ʶ����
/*struct T_CodeData_Back
{
	CString strNsCode ; //��վID
	BOOL bIsRefresh ;  //�Ƿ���ˢ��
	CString strResult ;  //�������ս��
	CString strPicPath ; //ˢ��ͼƬ�ı���·��
	int nCodeInputMode;  //1:��ʾ��֤�� 0������ʾ��֤��

	T_CodeData_Back()
	{
		bIsRefresh = FALSE ;
	}
};*/

struct T_CodeData_Back
{
	BOOL bIsRefresh ;  //�Ƿ���ˢ��
	int nCodeInputMode;  //1:��ʾ��֤�� 0������ʾ��֤��
	CString strNsCode ; //��վID
	CString strResult ;  //�������ս��
	CString strPicPath ; //ˢ��ͼƬ�ı���·��
	
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
		nCodeInputMode	= tempCodeDataBack.nCodeInputMode;  //1:��ʾ��֤�� 0������ʾ��֤��
		strNsCode		= tempCodeDataBack.strNsCode ; //��վID
		strResult		= tempCodeDataBack.strResult ;  //�������ս��
		strPicPath		= tempCodeDataBack.strPicPath ; //ˢ��ͼƬ�ı���·��
	}

	T_CodeData_Back & operator=(const T_CodeData_Back &tempCodeDataBack)
	{
		if(this == &tempCodeDataBack)
		{
			return *this;
		}

		bIsRefresh		= tempCodeDataBack.bIsRefresh;
		nCodeInputMode	= tempCodeDataBack.nCodeInputMode;  //1:��ʾ��֤�� 0������ʾ��֤��
		strNsCode		= tempCodeDataBack.strNsCode ; //��վID
		strResult		= tempCodeDataBack.strResult ;  //�������ս��
		strPicPath		= tempCodeDataBack.strPicPath ; //ˢ��ͼƬ�ı���·��

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
typedef BOOL (*SET_OCR_INPUTFOCUS)(void) ;   //�������뽹��
typedef BOOL (*RELEASE_INPUTFOCUS_MUTEX)(void) ; //�����ϴ��ļ������¼�
typedef BOOL (*GET_CODEDATA_BACK)(CString , T_CodeData_Back &) ;  //��ȡ��֤�뵯�򷵻ص�����

typedef LONG (*CODE_RECOGNIZE)(LPCTSTR strImgPath, LPTSTR strCode); 
typedef LONG (*CODE_ERROR)(LONG nResultCode); 
typedef DWORD (*CODE_WE_RECOGNIZE)(CString strRecegiseParm, CString strImgPath, CString &strResult);


