#include "StdAfx.h"
#include "YDMRecognize.h"

YDMRecognize g_ydmRecognize;

YDMRecognize::YDMRecognize()
{
	m_bInit = false;
	m_strUserName = new TCHAR[50];
	m_strPassword = new TCHAR[50];
	m_strSoftKey  = new TCHAR[256];

	ASSERT( m_strUserName != NULL && m_strPassword != NULL && m_strSoftKey != NULL);

	CString szEecDaTa = _T("");
	CString szDecData;
	TCHAR szFilePath[MAX_PATH] = {0};

	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	PathAppend(szFilePath, _T("..\\..\\"));
	PathAppend(szFilePath, _T("data2\\mc.dat"));

	IXMLRW xmlcfg;
	xmlcfg.init(szFilePath);

	int iSoftID = 0;
	xmlcfg.ReadInt(_T("MC"), _T("OCRDATA"), _T("id"),iSoftID);
	m_iSoftID = iSoftID;

	int iCodeType = 0;
	xmlcfg.ReadInt(_T("MC"), _T("OCRDATA"), _T("type"), iCodeType);
	m_nCodeType = iCodeType;

	xmlcfg.ReadString(_T("MC"), _T("OCRDATA"), _T("data"), szEecDaTa);
	//m_iSoftID = (int)GetPrivateProfileInt(_T("OCRDATA"), _T("id"), 0, szFilePath);
	//m_nCodeType = (int)GetPrivateProfileInt(_T("OCRDATA"), _T("type"), 0, szFilePath);

	//GetPrivateProfileString(_T("OCRDATA"), _T("data"), _T(""), szEecDaTa, sizeof(szEecDaTa), szFilePath);

	if(_tcslen(szEecDaTa) != 0)
	{
		CString strEncData = szEecDaTa;
		CDES des;
		des.Decrypt(szDecData, strEncData);
	}

	if(szDecData.GetLength() != 0)
	{
		TCHAR* strDecData = szDecData.GetBuffer(0);
		TCHAR *token;

		token = _tcstok(strDecData, _T(";"));

		if(token != NULL)
		{
			_tcscpy(m_strUserName, token);
		}

		token = _tcstok(NULL, _T(";"));

		if(token != NULL)
		{
			_tcscpy(m_strPassword, token);
		}

		token = _tcstok(NULL, _T(";"));

		if(token != NULL)
		{
			_tcscpy(m_strSoftKey, token);
		}
	}
}


YDMRecognize::~YDMRecognize()
{
	if(m_strUserName != NULL)
	{
		delete [] m_strUserName;
		m_strUserName = NULL;
	}
	if(m_strPassword != NULL)
	{
		delete [] m_strPassword;
		m_strPassword = NULL;
	}
	if(m_strSoftKey != NULL)
	{
		delete [] m_strSoftKey;
		m_strSoftKey = NULL;
	}
}


char* YDMRecognize::w2a(const wchar_t *pStr)
{
	char *pChar = NULL;
	int  iWlen = 0;

	if (pStr == NULL
		|| (iWlen = wcslen(pStr)) == 0)
	{
		return pChar;
	}

	int iLen = WideCharToMultiByte(CP_ACP, 0, pStr, iWlen, NULL, NULL, NULL, NULL);
	if (iLen > 0)
	{
		pChar = new char[iLen + 1];
		if (pChar != NULL)
		{
			memset(pChar, 0, iLen+1);
			WideCharToMultiByte(CP_ACP, 0, pStr, iWlen, pChar, iLen, NULL, NULL);
		}
	}
	return pChar;
}

wchar_t* YDMRecognize::a2w(const char* pStr)
{
	wchar_t *pChar = NULL;
	int  iWlen = 0;

	if (pStr == NULL
		|| (iWlen = strlen(pStr)) == 0)
	{
		return pChar;
	}

	int iLen = MultiByteToWideChar(CP_ACP, 0, pStr, iWlen, NULL, NULL);
	if (iLen > 0)
	{
		pChar = new wchar_t[iLen + 1];
		if (pChar != NULL)
		{
			MultiByteToWideChar(CP_ACP, 0, pStr, iWlen, pChar, iLen);
			pChar[iLen] = '\0';
		}
	}
	return pChar;
}

bool YDMRecognize::InitRecognize()
{
	bool bSuccess = true;
#ifdef _UNICODE
	// 初始化云打码
	char* softKey = w2a(m_strSoftKey);
	YDM_SetAppInfo(m_iSoftID, softKey);
	g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("初始化云打码"));
	// 登录
	char* userName = w2a(m_strUserName);
	char* passWord = w2a(m_strPassword);


	int loginRet = YDM_Login(userName, passWord);

	if(loginRet <= 0)
	{
		bSuccess = false;
		g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("登录失败,错误代码:%d"), loginRet);
	}
#else
	YDM_SetAppInfo(m_iSoftID, m_strSoftKey);

	int loginRet = YDM_Login(m_strUserName, m_strPassword);

	if(loginRet <= 0)
	{
		bSuccess = false;
		g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("登录失败,错误代码:%d"), loginRet);
	}
#endif
	// 设置30秒超时
	YDM_SetTimeOut(30);

	m_bInit = true;

	return bSuccess;
}

long YDMRecognize::Recognize(LPCTSTR strImgPath, LPTSTR strResult)
{
	if(!m_bInit)
	{
		InitRecognize();
	}
#ifdef _UNICODE
	char* pResult = new char[50];
	char* pPath = w2a(strImgPath);

	long ret = YDM_DecodeByPath(pPath, m_nCodeType, pResult);
	if(ret > 0)
	{
		wchar_t* wpResult = a2w(pResult);
		wcscpy(strResult, wpResult);
	}
	else
	{
		wcscpy(strResult, _T(""));
		g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("识别失败,错误码为：%d,文件名为:%s"), ret, strImgPath);
	}
	return ret;
#else
	long ret = YDM_DecodeByPath(strImgPath, m_nCodeType, strResult);
	return ret;
#endif
}

long YDMRecognize::ReportError(long nResultCode)
{
	long nCode = YDM_Report(nResultCode, false);
	g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("Report返回代码为：%d"), nCode);
	return nCode;
}