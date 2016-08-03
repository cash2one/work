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


	//根据网站id和任务类型获取网站数据
	int GetNetData(T_NetstationBaseInfo& tNetstationBaseInfo);

	PCHAR GetNSIparamater_Post(UINT64 nNetItemId) ;  //获取发布时栏目匹配参数
	PCHAR GetNSIparamater_Register(UINT64 nNetItemId) ; //获取注册时栏目匹配参数

	//获取NSVrealvalue参数
	int GetNSVrealvalue(int nNSVtype, CStdString strNSVcode, CStdString& strNSVrealvalue);


	DWORD GetNetLastUpdateTime();

	//打开网站模板文件
	DWORD OpenWebSiteData(DWORD dwNetId);
	
	DWORD CloseWebSiteData();

	UINT64 GetItemCode_Reg();
	UINT64 GetItemCode_Post();

	/*
	@brief  取得网站类型要求
	@param [out] tSiteType 保存网站类型数据
	*/
	void GetNStype(T_Site_Type &tSiteType);

	/*
	@brief  取得网站刷新和发布要求
	@param [out] tLimit 保存站网站刷新和发布数据
	*/
	void GetNSlimit(T_Limit &tLimit);

	/*
	@brief  多字节转unicode
	@param  startpos 多字节
	@param [out] str uncoide
	*/
	DWORD ToMByteToWChar(IN PCHAR startPos,OUT CStdString &str);

	/*
	@brief 根据网站的真实值获取标准变量值
	@param nsType 类型 
	@param realValue 网站真实值
	@return 标准变量值
	*/
	DWORD GetStandardValue(int nsType,CStdString &realValue);

private:
	CWebSiteData m_webSiteData;	
};
