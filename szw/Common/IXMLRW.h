/**********************************************
*  XMLRW�ӿ�ʹ��˵��:
*  1.ʹ��ʱֻ��ҪCXmlBase.h �� XMLRW.dll���ڶ�Ŀ¼����
*  2.�ɹ�ʧ�ܾ��ж�Ӧ�ķ����룬���Դ�ӡ�Ų�����
*    ��ȡ�ӿ�ʧ�ܺ��ʹ��Ĭ��ֵ
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
	//��ʼ�� ����ֵ:XRET_SUCCESS :�ɹ� ����ʧ��
	E_XMLRET init(const CString& strFilePath);

	//��ȡ�ַ��������� ����ֵ:XRET_SUCCESS :�ɹ� ����ʧ��ʹ��Ĭ��ֵstrDefault
	E_XMLRET ReadString(const CString& strModule, const CString& strSection, const CString& strKey, CString& strValue, CString strDefault = _T(""));

	//��ȡsectionֵ����map���key->value
	E_XMLRET ReadSection(const CString& strModule, const CString& strSection, map<CString, CString>&mapRet);

	//��ȡ�������� ����ֵ:XRET_SUCCESS :�ɹ� ����ʧ��ʹ��Ĭ��ֵiDefault
	E_XMLRET ReadInt(const CString& strModule, const CString& strSection, const CString& strKey, int& iValue, int iDefault = 0);

	//д���ַ��������� ����ֵ:XRET_SUCCESS :�ɹ� ����ʧ��
	E_XMLRET WriteString(const CString& strModule, const CString& strSection, const CString& strKey, const CString& strValue);

	//д���������� ����ֵ:XRET_SUCCESS :�ɹ� ����ʧ��
	E_XMLRET WriteInt(const CString& strModule, const CString& strSection, const CString& strKey, const int& iValue);

	//��ȡXML�ļ��汾��Ĭ��ǰ4Byte��Ϊ�ļ��汾��(�ַ����ͣ���:1.09) ����ֵ:XRET_SUCCESS :�ɹ� ����ʧ��
	E_XMLRET ReadXmlVer(CString& strVerion);

	//�����дxml�ļ� ����ֵ:XRET_SUCCESS :�ɹ� ����ʧ��
	E_XMLRET ReWriteXml(CString& strContent);
private:
	TCHAR *GetCurPath(void);
private:
	CXmlBase* m_pXmlObj;
	HMODULE   m_hDll;
};



