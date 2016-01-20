#pragma once
#include <windows.h>

class IConfigInfo
{
public:
	virtual BOOL ReadString(CStdString strAppName,CStdString strKey,CStdString &strValue) = 0;
	virtual BOOL ReadInteger(CStdString strAppName,CStdString strKey,INT &nValue) = 0;
	virtual BOOL WriteString(CStdString strAppName,CStdString strKey,CStdString strValue) = 0;
	virtual BOOL WriteInteger(CStdString strAppName,CStdString strKey,INT nValue) = 0;

};