#include "StdAfx.h"
#include "ManmadeCode.h"

CManmadeCode::CManmadeCode(void)
{
	m_bType = false;//默认不需要人工打码
	m_pfuncRecognize = NULL;
	m_pfuncError = NULL;
	//m_hDLL = NULL;
}


CManmadeCode::~CManmadeCode(void)
{
	if (!m_hDLL)
	{
		FreeLibrary(m_hDLL);
	}
}

//初始化接口
bool CManmadeCode::Init(BOOL bIsRemoteCode)
{	
	IXMLRW xml;
	TCHAR tszTmp[MAX_PATH] = {0};
	_stprintf(tszTmp, _T("%s\\data2\\mc.dat"), GetInstallPath());

	CString tStrManual = _T("");
	xml.init(tszTmp);
	xml.ReadString(_T("MC"), _T("INPUTOCR"), _T("ManualInput"), tStrManual);

	m_bType = bIsRemoteCode;

	if (tStrManual[0] == '1')//ManualInput = 1;忽略远程打码
	{
		m_bIgnor = true;
		m_bType = false;//不需要远程打码
	}
	else
	{
		m_bIgnor = false;
	}

	CStdString strMsg = _T("");
	CStdString strDir = GetInstallPath();
	//MessageBox(NULL, _T("1232"), _T("212"), MB_OK);
	CStdString str = strDir + _T("\\bin\\SVCAR.dll");
	m_hDLL = LoadLibrary(strDir + _T("\\bin\\SVCAR.dll"));

	if (!m_hDLL)
	{
		strMsg.Format(_T("加载验证码识别模块失败! err:%d,dll路径：%s"), GetLastError(), (CString)str);
		MessageBox(NULL, strMsg, _T("程序错误"), MB_OK);
		return false;
	}

	if(m_hDLL)
	{
		m_pfuncRecognize = (CODE_RECOGNIZE)GetProcAddress(m_hDLL, "Recognize");
		m_pfuncWeRecognize = (CODE_WE_RECOGNIZE)GetProcAddress(m_hDLL, "CodeRecognize");
		m_pfuncError = (CODE_ERROR)GetProcAddress(m_hDLL, "ReportError");
	}

	if (!m_pfuncRecognize || !m_pfuncError || !m_pfuncWeRecognize)
	{
		strMsg.Format(_T("加载验证码识别模块函数失败! err:%d"), GetLastError());
		MessageBox(NULL, strMsg, _T("程序错误"), MB_OK);
		return false;
	}
	 
	return true;
}

bool CManmadeCode::CodeType()
{
	return m_bType;
}

bool CManmadeCode::IgnorCodeType()
{
	return m_bIgnor;
}

//对dll函数Recognize封装
LONG CManmadeCode::GetRecognize(CStdString strReg, LPTSTR strCode)
{
	ASSERT(m_pfuncRecognize != NULL);

	CString str(strReg.GetBuffer());
	return m_pfuncRecognize(str, strCode);
}

//对dll函数ReportError封装
LONG CManmadeCode::GetReportError(LONG nRes)
{
	ASSERT(m_pfuncError != NULL);
	return m_pfuncError(nRes);
}

//对dll函数WeRecognize封装,这部分是自己验证码识别模块
DWORD CManmadeCode::GetWeRecognize(CStdString strParam, CStdString strPath, CString &strResult)
{
	ASSERT(m_pfuncWeRecognize != NULL);

	CString strParam0(strParam.GetBuffer());
	CString strPath0(strPath.GetBuffer());
	return m_pfuncWeRecognize(strParam0, strPath0, strResult);
}

