// PrepareData.h: interface for the CPrepareData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREPAREDATA_H__1E8CFFAD_E858_4328_B34A_2058C35518D9__INCLUDED_)
#define AFX_PREPAREDATA_H__1E8CFFAD_E858_4328_B34A_2058C35518D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//类功能内定编号: 9

#include "StdString.h"
#include "common_stru.h"
#include "common_stru_exserver.h"
#include "StringUtils.h"
#include "common_enum.h"
#include "IThreadUnit.h"
#include "Sysparam.h"
#include "NetData.h"
#include "ZBase64.h"
#include "DES.h"

//任务数据的前期准备工作
class CPrepareData
{
public:
	CPrepareData();
	virtual ~CPrepareData();

	//处理数据
	DWORD ProcessData(const int& nNetCode, const E_TASK_TYPE& eTaskType, 
						TRapalceDataMsg& tCompanyInfo, T_TaskData* tTaskData,DWORD dwIndex, DWORD dwPostType = -1);
	
	DWORD GetValueFromDB(T_NetstationBaseInfo& tNetStationData);

	//替换通配符
	DWORD ReplaceParam(const CStdString& strIn, CStdString& strOut, 
				const int& nKey, TRapalceDataMsg& tCompanyInfo);
	BOOL Equate(CStdString& src, CStdString& equate,
				TRapalceDataMsg& tCompanyInfo);
	//处理没有替换的%通配符
	void ReplacePercent(CStdString& strOut,std::vector<CStdString> &vNoRepalace);

	//替换内核处理的通配符
	void ReplaceSumengineParam(CStdString& strData);

	//根据通配符取得最大最小限制
	void GetMaxMin(const CStdString &strIn, int &iMax, int &iMin );

	//对网站动参变量进行处理
	BOOL DealVarible(CStdString &strIn, 
				CStdString &strOut, std::vector<TParamItem>& tSearchdata, DWORD nNetLanType);
	//得到某种时间格式的当前时间字符串。
    DWORD GetCurrentTimeByFormats(const CStdString strTimeFormat, 
				CStdString& strTimeOut);

	//检查注册用到的帐户信息，如有必要，做相应修改
	DWORD CheckRegCount(CStdString &strInAndOut, DWORD dLen, E_Account_Style eFirstFlag);
	
	//检查字符串第一位是不是想要的类型，不是，自动修改
	DWORD CheckFirstChar(CStdString &strInAndOut, E_Account_Style eFlag);

    void AddDefaultSubStr(const CStdString& strIn, const CStdString& strDelimiter, 
				std::vector<CStdString>& strOut);
   

    BOOL IsAimStr(CStdString str);
    BOOL IsChar(TCHAR ch);
    int KeyToVal(CStdString& temp);

	/*
		@breif 解析网站dat文件里的加密数据
		@param strCipher  加密数据
		@param strPlain  解密出来的数据
	*/
	void DecodeStationData(const CStdString &strCipher , CStdString &strPlain) ;

	/*
		@breif 根据通配符取得宽和高
		@param strTongPei  通配符
		@param [out] iWidth  宽
		@param [out] iHeight 高
	*/
	void GetWidthAndHeight(const CStdString &strTongPei, int &iWidth, int &iHeight);

	/*
		@breif 改变图片大小
		@param strTongPei  通配符
		@param [out] strImage  新图片路径
		@return TRUE为成功
	*/
	BOOL GetResizeImage(const CStdString &strTongPei, CStdString &strImage,DWORD dwFlag = 0,int iIndex = 0);

	//功能:处理对Body等值有长度要求的函数
	int TrimLenFun(CStdString strIn, CStdString& strOut, int nTrimLen,
					E_PPD_TRIM_TYPE eType);

	//初始化国家对照
	int InitCountry();

	//初始化省份名称
	int InitProvValue();

	//初始化性别
	void InitSex();

	//初始化公司性质
	void InitCompanyKind();

	//初始化有效期
	void InitExpire();

	//初始化公司人数
	void InitPersonsCount();

	//初始化注册资金
	void InitMoneyInfo();

	//void GetFullTime(int& nYear, int& nMouth, int& nDay, int nType, int nNum);
	int GetDateNum(int nMouth, int nYear);

	//获取参数默认值
	int GetParamValue(int nValueId, int nParamType, int nLanType,CStdString& strParamValue);

	//获取中文参数默认值
	int GetCParamValue(int nValueId, int nParamType, CStdString& strParamValue);
	//获取英文参数默认值
	int GetEParamValue(int nValueId, int nParamType, CStdString& strParamValue);

	//加载所有参数
	void LoadAllParam();

private:
	/*
	@brief 图片转换
	@param strImagePath; 要转换的图片地址；
	@param dwType;    转换类型 (1.jpg,2.png,3.bmp,4.gif)
	*/
	BOOL ImageConvert(CStdString &strSrcPath, DWORD dwType);

	/*
	@brief DES解密验证码参数
	@param iNetID  网站ID
	@param strEncryptData 加密数据
	@return 解密后的数据
	*/
	CStdString DesImageCode(const int &iNetID, CStdString& strEncryptData);

public:
	CNetData m_NetData;
private:
	E_TASK_TYPE m_eCurrentTaskType;

	CZBase64 m_Base64 ;

	CDES des;
	//国家
	std::map<int,CStdString> m_strCCountry;
	std::map<int,CStdString> m_strECountry;

	//省份
	std::map<int,CStdString> m_CProvValue;
	std::map<int,CStdString> m_EProvValue;

	//性别
	std::map<int,CStdString> m_strCSex;
	std::map<int,CStdString> m_strESex;

	//公司性质
	std::map<int,CStdString> m_strCCompanyKind;
	std::map<int,CStdString> m_strECompanyKind;

	//有效期
	std::map<int,CStdString> m_strCExpire;
	std::map<int,CStdString> m_strEExpire;

	//公司人数
	std::map<int,CStdString> m_strCPersonsCount;
	std::map<int,CStdString> m_strEPersonsCount;

	//注册资金
	std::map<int,CStdString> m_strCMoneyInfo;
	std::map<int,CStdString> m_strEMoneyInfo;

	//所有参数
	std::vector<CStdString> m_vstrParam;

	static const CStdString PARAM;

	CStdString m_strPagePhotoFlag; //抓取快照的标记
};


#endif // !defined(AFX_PREPAREDATA_H__1E8CFFAD_E858_4328_B34A_2058C35518D9__INCLUDED_)
