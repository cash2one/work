#pragma once
#include <map>
using namespace std;

//定义返回码，以便调用者排除问题
typedef enum _EM_XMLRET_
{
	XRET_SUCCESS  = 0,			//成功
	XRET_INITFAIL,				//初始化失败,或者是未进行对象初始化
	XRET_FILENOTEXIST,			//文件不存在
	XRET_NOTSUMSZWXML,			//不是有效的商舟网xml
	XRET_NOTFINDNODE,			//未找到对应节点
	XRET_OPENFILEFAIL,			//打开文件失败
	XRET_NOTSUMSZWXMLCONTENT,	//不是有效的商舟网XML内容
	XRET_LOADDLLFAIL,			//加载dll失败
}E_XMLRET;

class CXmlBase
{
public:
	CXmlBase();
	virtual ~CXmlBase();

public:
	//初始化XML文件路径
	virtual E_XMLRET InitialPath(const CString& strFilePath) = 0;

	//读取字符串型数据
	virtual E_XMLRET ReadString(const CString& strModule, const CString& strSection, const CString& strKey, CString& strValue, CString strDefault = _T("")) = 0;

	//读取section,遍历key->value用map存放
	virtual E_XMLRET ReadSection(const CString& strModule, const CString& strSection,map<CString,CString>&mapRet) = 0;

	//读取整型数据
	virtual E_XMLRET ReadInt(const CString& strModule, const CString& strSection, const CString& strKey, int& iValue, int iDefault = 0) = 0;

	//写入字符串型数据
	virtual E_XMLRET WriteString(const CString& strModule, const CString& strSection, const CString& strKey, const CString& strValue) = 0;

	//写入整型数据
	virtual E_XMLRET WriteInt(const CString& strModule, const CString& strSection, const CString& strKey, const int& iValue) = 0;

	//获取XML文件版本，默认前4Byte作为文件版本号(字符串型，如:1.09)
	virtual E_XMLRET ReadXmlVer(CString& strVerion) = 0;

	//清空重写xml文件(需要判断XML的有效性)
	virtual E_XMLRET ReWriteXml(CString& strContent) = 0;
};
