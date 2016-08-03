#include "stdafx.h"
#include ".\configinfo.h"

CConfigInfo::CConfigInfo(CStdString strFile)
{
	m_strFile = strFile;

}

CConfigInfo::~CConfigInfo(void)
{

}



BOOL CConfigInfo::ReadString(CStdString strAppName,CStdString strKey,CStdString &strValue)
{
	TCHAR retVal[255] = {0};
	GetPrivateProfileString(strAppName,strKey,_T(""),retVal,255,m_strFile);
	strValue = CStdString(retVal);

	return TRUE;
}
BOOL CConfigInfo::ReadInteger(CStdString strAppName,CStdString strKey,INT &nValue)
{
	nValue = GetPrivateProfileInt(strAppName,strKey,0,m_strFile);
	return TRUE;
}
BOOL CConfigInfo::WriteString(CStdString strAppName,CStdString strKey,CStdString strValue)
{
	WritePrivateProfileString(strAppName,strKey,strValue,m_strFile);
	return TRUE;
}
BOOL CConfigInfo::WriteInteger(CStdString strAppName,CStdString strKey,INT nValue)
{
	TCHAR strValue[255] = {0};
	_stprintf(strValue,_T("%d"),nValue);
	WritePrivateProfileString(strAppName,strKey,strValue,m_strFile);
	return TRUE;
}