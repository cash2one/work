#pragma once
#include <map>
using namespace std;

//���巵���룬�Ա�������ų�����
typedef enum _EM_XMLRET_
{
	XRET_SUCCESS  = 0,			//�ɹ�
	XRET_INITFAIL,				//��ʼ��ʧ��,������δ���ж����ʼ��
	XRET_FILENOTEXIST,			//�ļ�������
	XRET_NOTSUMSZWXML,			//������Ч��������xml
	XRET_NOTFINDNODE,			//δ�ҵ���Ӧ�ڵ�
	XRET_OPENFILEFAIL,			//���ļ�ʧ��
	XRET_NOTSUMSZWXMLCONTENT,	//������Ч��������XML����
	XRET_LOADDLLFAIL,			//����dllʧ��
}E_XMLRET;

class CXmlBase
{
public:
	CXmlBase();
	virtual ~CXmlBase();

public:
	//��ʼ��XML�ļ�·��
	virtual E_XMLRET InitialPath(const CString& strFilePath) = 0;

	//��ȡ�ַ���������
	virtual E_XMLRET ReadString(const CString& strModule, const CString& strSection, const CString& strKey, CString& strValue, CString strDefault = _T("")) = 0;

	//��ȡsection,����key->value��map���
	virtual E_XMLRET ReadSection(const CString& strModule, const CString& strSection,map<CString,CString>&mapRet) = 0;

	//��ȡ��������
	virtual E_XMLRET ReadInt(const CString& strModule, const CString& strSection, const CString& strKey, int& iValue, int iDefault = 0) = 0;

	//д���ַ���������
	virtual E_XMLRET WriteString(const CString& strModule, const CString& strSection, const CString& strKey, const CString& strValue) = 0;

	//д����������
	virtual E_XMLRET WriteInt(const CString& strModule, const CString& strSection, const CString& strKey, const int& iValue) = 0;

	//��ȡXML�ļ��汾��Ĭ��ǰ4Byte��Ϊ�ļ��汾��(�ַ����ͣ���:1.09)
	virtual E_XMLRET ReadXmlVer(CString& strVerion) = 0;

	//�����дxml�ļ�(��Ҫ�ж�XML����Ч��)
	virtual E_XMLRET ReWriteXml(CString& strContent) = 0;
};
