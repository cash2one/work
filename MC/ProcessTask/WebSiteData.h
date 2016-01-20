#pragma once


#include "AbstractFileData.h"
#include "SiteInfo.h"


enum E_Value_Type
{
	etNSname = 1 ,  //��վ��
	etNShost ,  //��ַ
	etNStype ,  //��վ����
	etNsdata ,  //��վ����
	etNSlimit ,  //��վ����
	etNSImageProcessCode , //��֤��ʶ�����
};

struct T_NetstationBaseInfo  //��common_stru.h�ж���,�轫�˶����滻���еĶ���
{
	CStdString NScode; //��վ���
	CStdString NSname; //��վ����
	CStdString NShost ;
	CStdString NStype ;
	CStdString NSdata ;
	CStdString NSLimit ;
	CStdString	NSImageProcessCode ;
};


class CWebSiteData :public CAbstractFileData
{
public:
	CWebSiteData(void);
	~CWebSiteData(void);

	void Test();

	DWORD GetLastUpdateTime();               //��ȡ��վ������ʱ��
	PCHAR getNetStationValue(DWORD type);    //����type���ö�٣�����ò�ͬ��string����

	DWORD getNScode();                       //��ȡ��վ���
	CStdString getNSname() ;  //��ȡ��վ��
	CStdString GetNShost() ;    //��ȡ��վ��ַ
	void GetNStype(T_Site_Type &tSiteType) ;  //��ȡ��վ����
	CStdString GetNSdata() ;   //��ȡ��վ���� 
	void GetNSlimit(T_Limit &tLimit) ;  //��ȡ��վ����
	CStdString GetNSImageProcessCode() ;  //��ȡ��֤��ʶ�����
	

	PCHAR GetNSIparamater(UINT64 nNetItemId, int nStartPos);	//�����Ŀ����
	PCHAR GetNSIparamater_Post(UINT64 nNetItemId) ;  //��ȡ����ʱ��Ŀƥ�����
	PCHAR GetNSIparamater_Register(UINT64 nNetItemId) ; //��ȡע��ʱ��Ŀƥ�����

	UINT64 GetDefaultItemCode_Reg();					//���Ĭ��ITemCode
	UINT64 GetDefaultItemCode_Post();					//���Ĭ��ITemCode
	PCHAR GetNSVrealvalue(DWORD dwVType,DWORD dwVCode); //��ñ�������

	DWORD GetStandardByRealvalue(DWORD dwVType,PCHAR szRealValue);  //������վ��ʵֵ��ȡ��׼ֵ��
	
	DWORD GetNetBaseInfo(T_NetstationBaseInfo& t_NetstationBaseInfo);
};




