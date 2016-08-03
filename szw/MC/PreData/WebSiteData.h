#pragma once


#include "AbstractFileData.h"
#include "SiteInfo.h"


enum E_Value_Type
{
	etNSname = 1 ,  //网站名
	etNShost ,  //网址
	etNStype ,  //网站类型
	etNsdata ,  //网站数据
	etNSlimit ,  //网站限制
	etNSImageProcessCode , //验证码识别参数
};

struct T_NetstationBaseInfo  //在common_stru.h中定义,需将此定义替换其中的定义
{
	CStdString NScode; //网站编号
	CStdString NSname; //网站名称
	CStdString NShost ;
	CStdString NStype ;
	CStdString NSdata ;
	CStdString NSLimit ;
	CStdString NSImageProcessCode ;
	CStdString NsEmbedCode; //嵌入网站ID；
};


class CWebSiteData :public CAbstractFileData
{
public:
	CWebSiteData(void);
	~CWebSiteData(void);

	void Test();

	DWORD GetLastUpdateTime();               //获取网站最后更新时间
	PCHAR getNetStationValue(DWORD type);    //根据type这个枚举，来获得不同的string数据

	DWORD getNScode();                       //获取网站编号
	CStdString getNSname() ;  //获取网站名
	CStdString GetNShost() ;    //获取网站地址
	void GetNStype(T_Site_Type &tSiteType) ;  //获取网站类型
	CStdString GetNSdata() ;   //获取网站数据 
	void GetNSlimit(T_Limit &tLimit) ;  //获取网站限制
	CStdString GetNSImageProcessCode() ;  //获取验证码识别参数
	

	PCHAR GetNSIparamater(UINT64 nNetItemId, int nStartPos);	//获得栏目参数
	PCHAR GetNSIparamater_Post(UINT64 nNetItemId) ;  //获取发布时栏目匹配参数
	PCHAR GetNSIparamater_Register(UINT64 nNetItemId) ; //获取注册时栏目匹配参数

	UINT64 GetDefaultItemCode_Reg();					//获得默认ITemCode
	UINT64 GetDefaultItemCode_Post();					//获得默认ITemCode
	PCHAR GetNSVrealvalue(DWORD dwVType,DWORD dwVCode); //获得变量参数

	DWORD GetStandardByRealvalue(DWORD dwVType,PCHAR szRealValue);  //根据网站真实值获取标准值；
	
	DWORD GetNetBaseInfo(T_NetstationBaseInfo& t_NetstationBaseInfo);
};




