#pragma once
#include "stdafx.h"
#include "WebSiteData.h"
#include "FileReadAndSave.h"
#include "SiteInfo.h"

class CNetData
{
public:
	CNetData(void);
	~CNetData(void);


	//������վid���������ͻ�ȡ��վ����
	int GetNetData(T_NetstationBaseInfo& tNetstationBaseInfo);

	PCHAR GetNSIparamater_Post(UINT64 nNetItemId) ;  //��ȡ����ʱ��Ŀƥ�����
	PCHAR GetNSIparamater_Register(UINT64 nNetItemId) ; //��ȡע��ʱ��Ŀƥ�����

	//��ȡNSVrealvalue����
	int GetNSVrealvalue(int nNSVtype, CStdString strNSVcode, CStdString& strNSVrealvalue);


	DWORD GetNetLastUpdateTime();

	//����վģ���ļ�
	DWORD OpenWebSiteData(DWORD dwNetId);
	
	DWORD CloseWebSiteData();

	UINT64 GetItemCode_Reg();
	UINT64 GetItemCode_Post();

	/*
	@brief  ȡ����վ����Ҫ��
	@param [out] tSiteType ������վ��������
	*/
	void GetNStype(T_Site_Type &tSiteType);

	/*
	@brief  ȡ����վˢ�ºͷ���Ҫ��
	@param [out] tLimit ����վ��վˢ�ºͷ�������
	*/
	void GetNSlimit(T_Limit &tLimit);

	/*
	@brief  ���ֽ�תunicode
	@param  startpos ���ֽ�
	@param [out] str uncoide
	*/
	DWORD ToMByteToWChar(IN PCHAR startPos,OUT CStdString &str);

	/*
	@brief ������վ����ʵֵ��ȡ��׼����ֵ
	@param nsType ���� 
	@param realValue ��վ��ʵֵ
	@return ��׼����ֵ
	*/
	DWORD GetStandardValue(int nsType,CStdString &realValue);

private:
	CWebSiteData m_webSiteData;	
};
