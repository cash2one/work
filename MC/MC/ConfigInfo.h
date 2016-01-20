#pragma once

#include "IConfigInfo.h"


class CConfigInfo:public IConfigInfo
{
private:
	CStdString m_strFile;
public:
	CConfigInfo(CStdString strFile);
	~CConfigInfo(void);
	BOOL ReadString(CStdString strAppName,CStdString strKey,CStdString &strValue);
	BOOL ReadInteger(CStdString strAppName,CStdString strKey,INT &nValue);
	BOOL WriteString(CStdString strAppName,CStdString strKey,CStdString strValue);
	BOOL WriteInteger(CStdString strAppName,CStdString strKey,INT nValue);

};