#pragma once

#include <vector>
#include <map>
#include "StdString.h"

class CIniFile
{
private:
	CStdString m_strFile;

public:
	CIniFile();
	~CIniFile(void);

	BOOL SetFilePath(const CStdString &strFilePath) ;
	CStdString GetCurPath() ;

	BOOL ReadString(CStdString strAppName,CStdString strKey,CStdString &strValue, CStdString strDefault = _T(""));
	BOOL ReadInteger(CStdString strAppName,CStdString strKey,INT &nValue,INT nDefault = 0);
	BOOL WriteString(CStdString strAppName,CStdString strKey,CStdString strValue);
	BOOL WriteInteger(CStdString strAppName,CStdString strKey,INT nValue);

	void GetSectionFromINI(const CStdString &strSection , std::map<CStdString,CStdString> &mapRet , std::vector<CStdString> &vecRet) ;
	void GetSectionFromINI(const CStdString &strSection , std::vector<CStdString> &vecRet) ;

	void GetAllSectionNames(std::vector<CStdString> &vecSectionNames) ;

};