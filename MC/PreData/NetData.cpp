#include "StdAfx.h"
#include ".\netdata.h"
#include "FileReadAndSave.h"


CNetData::CNetData(void)
{

}

CNetData::~CNetData(void)
{

}

//����:�򿪱��ذ�װĿ¼�µ��ļ���Ĭ�� A0.bin.src��
//����:file,�ļ���
//���:
//����ֵ:
//����˵��:
DWORD CNetData::OpenWebSiteData(DWORD dwNetId)
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path,MAX_PATH);
	PathAppend(path, _T("..\\..\\"));
	
	swprintf(path,_T("%sData\\%d.dat"),path,dwNetId);
	

	return m_webSiteData.Open(path);
}


//����:�رձ��ذ�װĿ¼�µ��ļ���Ĭ�� A0.bin.src��
//����:file,�ļ���
//���:
//����ֵ:
//����˵��:

DWORD CNetData::CloseWebSiteData()
{
	m_webSiteData.Close();
	return 0;
}



//����:������վid���������ͻ�ȡ��վ����
//����:dwNetId,��վ���;eTaskType,��������
//���:tNetstationBaseInfo,��վ����
//����ֵ:-1,δ֪����������;-2,�򿪼�¼��ʧ��;1,û�и���վ����;0,�ɹ�
//����˵��:
int CNetData::GetNetData(T_NetstationBaseInfo& tNetstationBaseInfo)
{
	return m_webSiteData.GetNetBaseInfo(tNetstationBaseInfo);;
}


//����:��ȡNSVrealvalue����
//����:nNSVnscode;nNSVtype;strNSVcode
//���:strNSVrealvalue
//����ֵ:-1,�򿪼�¼��ʧ��;1,ȡ�ò���ֵ
//����˵��:
int CNetData::GetNSVrealvalue(int nNSVtype, CStdString strNSVcode, 
							  CStdString& strNSVrealvalue)
{

	PCHAR cNSVrealValue = m_webSiteData.GetNSVrealvalue(nNSVtype,_ttoi(strNSVcode.c_str()));
	if (cNSVrealValue == NULL)
	{
		return 0;
	}
	
	m_webSiteData.ToMByteToWChar(cNSVrealValue , strNSVrealvalue);

	if (strNSVrealvalue.GetLength()>0)
	{
		return 1;
	}

	return 0;
}


UINT64 CNetData::GetItemCode_Reg()
{
	return m_webSiteData.GetDefaultItemCode_Reg();
}


///////////////////////////////////////////////////////////////////////////////
//
// ����������������ȡ�ͻ����ϵ�������ʱ��
// ���룺
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////

DWORD CNetData::GetNetLastUpdateTime()
{
	DWORD lastUpdataTime =  m_webSiteData.GetLastUpdateTime();
	return lastUpdataTime;
}

void CNetData::GetNStype( T_Site_Type &tSiteType )
{
	m_webSiteData.GetNStype(tSiteType);
}

PCHAR CNetData::GetNSIparamater_Post( UINT64 nNetItemId)
{
	return m_webSiteData.GetNSIparamater_Post(nNetItemId);
}

PCHAR CNetData::GetNSIparamater_Register( UINT64 nNetItemId )
{
	return m_webSiteData.GetNSIparamater_Register(nNetItemId);
}

UINT64 CNetData::GetItemCode_Post()
{
	return  m_webSiteData.GetDefaultItemCode_Post();
}

DWORD CNetData::ToMByteToWChar( IN PCHAR startPos,OUT CStdString &str )
{
	return m_webSiteData.ToMByteToWChar(startPos, str);
}

/*
	@brief  ȡ����վˢ�ºͷ���Ҫ��
	@param [out] tLimit ����վ��վˢ�ºͷ�������
	*/
void CNetData::GetNSlimit( T_Limit &tLimit )
{
	m_webSiteData.GetNSlimit(tLimit);
}

/*
@brief ������վ����ʵֵ��ȡ��׼����ֵ
@param nsType ���� 
@param realValue ��վ��ʵֵ
@return ��׼����ֵ
*/
DWORD CNetData::GetStandardValue( int nsType,CStdString& realValue )
{
	PCHAR szValue = m_webSiteData.WCharToMbyte(realValue); //ת���ֽ�
	if (szValue == NULL)
	{
		return -1;
	}
	DWORD dwResult = m_webSiteData.GetStandardByRealvalue(nsType,szValue);

	delete szValue;

	return dwResult;
}

