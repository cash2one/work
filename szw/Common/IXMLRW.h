/**********************************************
*  XMLRW接口使用说明:
*  1.使用时只需要CXmlBase.h 和 XMLRW.dll放在对目录即可
*  2.成功失败均有对应的返回码，可以打印排查问题
*    读取接口失败后会使用默认值
*************************************************/

#pragma once
#include "CXmlBase.h"

typedef CXmlBase* (*GETXMLOBJECT)();
typedef void(*DELETEMEMORY)(CXmlBase*);

class IXMLRW
{
public:
	IXMLRW();
	~IXMLRW();
public:
	//初始化 返回值:XRET_SUCCESS :成功 其他失败
	E_XMLRET init(const CString& strFilePath);

	//读取字符串型数据 返回值:XRET_SUCCESS :成功 其他失败使用默认值strDefault
	E_XMLRET ReadString(const CString& strModule, const CString& strSection, const CString& strKey, CString& strValue, CString strDefault = _T(""));

	//读取section值，用map存放key->value
	E_XMLRET ReadSection(const CString& strModule, const CString& strSection, map<CString, CString>&mapRet);

	//读取整型数据 返回值:XRET_SUCCESS :成功 其他失败使用默认值iDefault
	E_XMLRET ReadInt(const CString& strModule, const CString& strSection, const CString& strKey, int& iValue, int iDefault = 0);

	//写入字符串型数据 返回值:XRET_SUCCESS :成功 其他失败
	E_XMLRET WriteString(const CString& strModule, const CString& strSection, const CString& strKey, const CString& strValue);

	//写入整型数据 返回值:XRET_SUCCESS :成功 其他失败
	E_XMLRET WriteInt(const CString& strModule, const CString& strSection, const CString& strKey, const int& iValue);

	//获取XML文件版本，默认前4Byte作为文件版本号(字符串型，如:1.09) 返回值:XRET_SUCCESS :成功 其他失败
	E_XMLRET ReadXmlVer(CString& strVerion);

	//清空重写xml文件 返回值:XRET_SUCCESS :成功 其他失败
	E_XMLRET ReWriteXml(CString& strContent);
private:
	TCHAR *GetCurPath(void);
private:
	CXmlBase* m_pXmlObj;
	HMODULE   m_hDll;
};



