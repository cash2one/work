#include "stdafx.h"
#include "IXMLRW.h"

IXMLRW::IXMLRW()
{
	m_pXmlObj = NULL;
	m_hDll = NULL;
}

IXMLRW::~IXMLRW()
{	
	//dll申请的内存由dll负责释放
	DELETEMEMORY pfnDelete = (DELETEMEMORY)GetProcAddress(m_hDll, "DeleteMemory");

	if (pfnDelete && m_pXmlObj)
	{
		pfnDelete(m_pXmlObj);
	}
	if (NULL != m_hDll)
	{	
		//释放dll对应的内存空间也释放
		FreeLibrary(m_hDll);
	}
}

E_XMLRET IXMLRW::init(const CString& strFilePath)
{
	if (NULL == m_pXmlObj)
	{	
		CString strDllPath = CString(GetCurPath()) + _T("\\XMLRW.dll");
		m_hDll = LoadLibrary(strDllPath);

		if (!m_hDll)
		{
			return XRET_LOADDLLFAIL;
		}
		GETXMLOBJECT pfnGetObj = (GETXMLOBJECT)GetProcAddress(m_hDll, "GetXmlObj");
		
		if (!pfnGetObj)
		{
			return XRET_LOADDLLFAIL;
		}
		m_pXmlObj = pfnGetObj();

		if (!m_pXmlObj)
		{
			return XRET_LOADDLLFAIL;
		}
	}
	m_pXmlObj->InitialPath(strFilePath);

	return XRET_SUCCESS ;
}

TCHAR * IXMLRW::GetCurPath(void)
{

	static TCHAR s_szCurPath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, s_szCurPath, MAX_PATH);

	TCHAR *p = _tcsrchr(s_szCurPath, '\\');
	*p = 0;

	return s_szCurPath;

}

E_XMLRET IXMLRW::ReadString(const CString& strModule, const CString& strSection, const CString& strKey, CString& strValue, CString strDefault /*= _T("")*/)
{	
	if (NULL != m_pXmlObj)
	{
		return (m_pXmlObj->ReadString(strModule, strSection, strKey, strValue, strDefault));
	}
	else
	{
		strValue = strDefault;

		return XRET_INITFAIL;
	}
}

E_XMLRET IXMLRW::ReadSection(const CString& strModule, const CString& strSection, map<CString, CString>&mapRet)
{
	if (NULL != m_pXmlObj)
	{
		return (m_pXmlObj->ReadSection(strModule, strSection, mapRet));
	}
	else
	{
		return XRET_INITFAIL;
	}
}

E_XMLRET IXMLRW::ReadInt(const CString& strModule, const CString& strSection, const CString& strKey, int& iValue, int iDefault /*= 0*/)
{	
	if (NULL != m_pXmlObj)
	{
		return (m_pXmlObj->ReadInt(strModule, strSection, strKey, iValue, iDefault));
	}
	else
	{
		iValue = iDefault;

		return XRET_INITFAIL;
	}
}

E_XMLRET IXMLRW::WriteString(const CString& strModule, const CString& strSection, const CString& strKey, const CString& strValue)
{	
	if (NULL != m_pXmlObj)
	{
		return (m_pXmlObj->WriteString(strModule, strSection, strKey, strValue));
	}
	else
	{
		return XRET_INITFAIL;
	}
	
}

E_XMLRET IXMLRW::WriteInt(const CString& strModule, const CString& strSection, const CString& strKey, const int& iValue)
{	
	if (NULL != m_pXmlObj)
	{
		return (m_pXmlObj->WriteInt(strModule, strSection, strKey, iValue));
	}
	else
	{
		return XRET_INITFAIL;
	}
	
}

E_XMLRET IXMLRW::ReadXmlVer(CString& strVerion)
{	
	if (NULL != m_pXmlObj)
	{
		return(m_pXmlObj->ReadXmlVer(strVerion));
	}
	else
	{
		return XRET_INITFAIL;
	}
}

E_XMLRET IXMLRW::ReWriteXml(CString& strContent)
{	
	if (NULL != m_pXmlObj)
	{
		return(m_pXmlObj->ReWriteXml(strContent));
	}
	else
	{
		return XRET_INITFAIL;
	}
}

