
#include "stdafx.h"
#include "IniFile.h"

CIniFile::CIniFile()
{
}

CIniFile::~CIniFile(void)
{
}

BOOL CIniFile::SetFilePath(const CStdString &strFilePath)
{
	if ( !PathFileExists(strFilePath) )
	{
		return FALSE ;
	}
	m_strFile = strFilePath ;

	return TRUE ;
}

CStdString CIniFile::GetCurPath()
{
	TCHAR FileName[1024];   
	::GetModuleFileName(NULL,FileName,1023);   

	CStdString strPathname = _T("");   
	strPathname = FileName;   

	int nPos = strPathname.ReverseFind('\\');   
	strPathname= strPathname.Left(nPos);

	return strPathname;
}


BOOL CIniFile::ReadString(CStdString strAppName,CStdString strKey,CStdString &strValue, CStdString strDefault)
{
	TCHAR retVal[255] = {0};
	GetPrivateProfileString(strAppName,strKey,strDefault,retVal,255,m_strFile);
	strValue = CStdString(retVal);

	return TRUE;
}
BOOL CIniFile::ReadInteger(CStdString strAppName,CStdString strKey,INT &nValue,INT nDefault)
{
	nValue = GetPrivateProfileInt(strAppName,strKey,nDefault,m_strFile);
	return TRUE;
}
BOOL CIniFile::WriteString(CStdString strAppName,CStdString strKey,CStdString strValue)
{
	WritePrivateProfileString(strAppName,strKey,strValue,m_strFile);
	return TRUE;
}
BOOL CIniFile::WriteInteger(CStdString strAppName,CStdString strKey,INT nValue)
{
	TCHAR strValue[255] = {0};
	_stprintf(strValue,_T("%d"),nValue);
	WritePrivateProfileString(strAppName,strKey,strValue,m_strFile);
	return TRUE;
}

void CIniFile::GetSectionFromINI(const CStdString &strSection , std::map<CStdString,CStdString> &mapRet , std::vector<CStdString> &vecRet)
{
//	vecRet.clear() ;
	//TCHAR tcOperTypes[4048] ;
	TCHAR tcOperTypes[8192] ;
	memset(tcOperTypes , L'/0' , sizeof(tcOperTypes)) ;

	int nRetCount = ::GetPrivateProfileSection(strSection , tcOperTypes ,sizeof(tcOperTypes) , m_strFile) ;

	CStdString strKeyValue = _T("") ;
	for (int i=0 ; i<nRetCount ; ++i)
	{
		if (tcOperTypes[i] != L'\0')
		{
			strKeyValue += tcOperTypes[i] ;
		}
		else
		{
			int nPos = strKeyValue.Find(_T("=")) ;
			if (nPos != -1)
			{
				vecRet.push_back( strKeyValue.Left(nPos) ) ;
				mapRet[strKeyValue.Left(nPos)] = strKeyValue.Right(strKeyValue.GetLength()-nPos-1) ;
			}
			else
			{
				vecRet.push_back(strKeyValue) ;
			}

			strKeyValue.Empty() ;
		}
	}
}

void CIniFile::GetSectionFromINI(const CStdString &strSection , std::vector<CStdString> &vecRet)
{
	vecRet.clear() ;
	//TCHAR tcOperTypes[4048] ;
	TCHAR tcOperTypes[8192] ;
	memset(tcOperTypes , L'/0' , sizeof(tcOperTypes)) ;

	int nRetCount = ::GetPrivateProfileSection(strSection , tcOperTypes ,sizeof(tcOperTypes) , m_strFile) ;

	CStdString strKeyValue = _T("") ;
	for (int i=0 ; i<nRetCount ; ++i)
	{
		if (tcOperTypes[i] != L'\0')
		{
			strKeyValue += tcOperTypes[i] ;
		}
		else
		{
			int nPos = strKeyValue.Find(_T("=")) ;
			if (nPos != -1)
			{
				vecRet.push_back( strKeyValue.Left(nPos) ) ;
			}
			else
			{
				vecRet.push_back(strKeyValue) ;
			}

			strKeyValue.Empty() ;
		}
	}
}

void CIniFile::GetAllSectionNames(std::vector<CStdString> &vecSectionNames)
{
	TCHAR tcSection[500] ;
	memset(tcSection , L'/0' , sizeof(tcSection)) ;
	int nSectionCount = GetPrivateProfileSectionNames( tcSection , sizeof(tcSection) , m_strFile) ;

	CStdString strSectionName = _T("") ;
	for (int i=0 ; i<nSectionCount ; ++i)
	{
		if ( tcSection[i] != L'\0' )
		{
			strSectionName += tcSection[i] ;
		}
		else
		{
			vecSectionNames.push_back(strSectionName) ;
			strSectionName.Empty() ;
		}
	}
}