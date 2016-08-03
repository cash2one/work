#include "stdafx.h"
#include "unzipwebtaskdata.h"
#include "Stringutils.h"
#include "FileReadAndSave.h"
#include "Reg.h"

CUnZipWebTaskData::CUnZipWebTaskData(void)
{
}

CUnZipWebTaskData::~CUnZipWebTaskData(void)
{
}




DWORD CUnZipWebTaskData::TransInfoPostToTaskData(const CStdString &strSource, WEB_TASK_DATA_INFOPOST &stWebTaskData)
{

	//(;9)发布内容信息(;9)所有网站信息(;9)系统信息
	//(;9)配置信息(;9)帐号密码信息(;9)任务类型
	//(;9)服务器当前的分钟数(和2001-1-16的差) (;9)定时任务ID

	int dwFound,dwDim;
	DWORD dwCount;
	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};

	CStdString strData;
	CStdString strDelimiter;

	std::vector<CStdString> result;
	StringUtils StringUtil;

	CFileReadAndSave::UnZipDecodeStdString (strSource,strData);
	dwFound = StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,dwDim);
	dwCount = StringUtil.SplitString(strData,strDelimiter,result,true);

	if (dwCount >= 7)
	{
		stWebTaskData.strPostInfo = result[1];
		stWebTaskData.strWebInfo = result[2];
		stWebTaskData.strSystemInfo = result[3];
		stWebTaskData.strConfigInfo = result[4];
		stWebTaskData.strAccountPassword = result[5];
		
		stWebTaskData.strUserID =result[6];

		stWebTaskData.strServerMintus = result [ 7 ];
		if (dwCount >= 8)
		{
			stWebTaskData.strTimerTaskID = result[8];
		}
	}

	return 0;
}

/*
	@brief   保存产品图片与营业执照到本地
	@param   strProductImageName 产品图片路径
	@param   strTradeImageName 营业执照路径
	@return  
	*/
DWORD CUnZipWebTaskData::SetTaskImageToGlobal(const CStdString& strProductImageName, const CStdString& strTradeImageName)
{
	T_LocalGlobalData *pLocalGlobalData =	NULL;

	//先获取全局数据区的产品ID
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pLocalGlobalData != NULL)
	{
//		_tcsncpy(pLocalGlobalData->szProductImageName, strProductImageName.c_str(), MAX_FILE_PATH_LENGTH-1);
		_tcsncpy(pLocalGlobalData->szTradeImageName, strTradeImageName.c_str(), MAX_FILE_PATH_LENGTH-1);
	}
	
	return 0;
}

/****
***@brief  保存产品信息
***@param   strData产品信息字符串
***@return  
****/
DWORD CUnZipWebTaskData::SaveProductToLocal(const CStdString& strData)
{
	int dwFound,i,dwDim;
	DWORD dwCount;
	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
	CStdString strDelimiter;

	std::vector<CStdString> result;
	StringUtils StringUtil;
	T_LocalGlobalData *pLocalGlobalData =	NULL;

	//先获取全局数据区的产品ID
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pLocalGlobalData == NULL)
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 0;
	}

	dwFound = StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,dwDim);
	dwCount = StringUtil.SplitString(strData,strDelimiter,result,true);

	if (dwCount > 21)
	{
		i = 1 +  0 ;  //产品编号
		_tcsncpy(TempTch, result [ i ], MAX_ID_STRING_LENGTH-1);
		pLocalGlobalData->TaskProductInfo.dwProductID = _ttol(TempTch);
		
		i = 1 +  1 ; //产品名称
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductName, result [ i ], MAX_NAME_LENGTH-1);

		i = 1 +  2 ;  //产品价格
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductPrice, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  3 ;  //产品类型
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductType, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);
		
		i = 1 +  4 ;  //生产地址
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductAddress, result [ i ], MAX_NAME_LENGTH-1);
		
		i = 1 +  5 ;  //产品详细信息
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductDetail, result [ i ], MAX_PRODUCT_DETAIL_LENGTH-1);
		

		i = 1 +  6 ; //产品设置
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductSetup, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  7;  //产品规格
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductSpec, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);
		
		i = 1 +  8 ;  //产品批号
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductBatchno, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  9 ;  //产品型号
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductModel, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  10 ;  //产品数量
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductAmount, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  11 ;  //产品计量单位
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductUnit, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  12 ;  //生产厂家
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductFactory, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  13 ;  //产品品牌
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductBrand, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  14 ;  //(;3)产品海关码 
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szProductHGCode, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  15 ;  //产品所在栏目id
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szTradeId, result [ i ], MAX_ID_STRING_LENGTH-1);

		i = 1 +  16 ;  //产品分类名称
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szTradeName, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  17 ;  //材质
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szMaterial, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  18 ;  //保质/修期
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szDateToDate, result [ i ], MAX_PRODUCT_NORMAL_LENGTH-1);

		i = 1 +  19 ;  //补充说明
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szAddDetail, result [ i ], MAX_NAME_LENGTH-1);

		i = 1 +  20 ;  //产品图片下载地址
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szPhototUrl, result [ i ], MAX_URL_LENGTH-1);

		i = 1 +  21 ; //一级栏目名称（中文）
		_tcsncpy(pLocalGlobalData->TaskProductInfo.szCselFirName,result[ i ], MAX_MID_LENGTH-1);

		if (dwCount > 23)
		{
			i = 1 + 22; //二级栏目名称（中文）
			_tcsncpy(pLocalGlobalData->TaskProductInfo.szCselSecName, result[i], MAX_MID_LENGTH - 1);

			i = 1 + 23; //三级栏目名称（中文）
			_tcsncpy(pLocalGlobalData->TaskProductInfo.szCselThdName, result[i], MAX_MID_LENGTH - 1);
		}
		return 1;
	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("解析产品信息字符串失败:%s"), strData.c_str());
	}

	return 0;
}

/*
	@brief   保存帐号密码等信息到内存
	@param   strData 帐号密码信息字符串 
	@return  
	*/
DWORD CUnZipWebTaskData::SaveAccountPasswordToLocal(const CStdString& strData)
{
	int intCount,dwDim,dwDim2;
	DWORD dwCount,dwItemCount;
	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};

	CStdString strDelimiter,strDelimiter2,szTemp;
	std::vector<CStdString> resultItem;
	std::vector<CStdString> resultNs;
	StringUtils StringUtil;

	if (strData.length() <= 0)
	{
		g_log.Trace(LOGL_HIG, LOGT_WARNING, __TFILE__, __LINE__,
			_T("网站帐号密码内容为空!"));
	}

	StringUtil.Get2DSplitCharAndLengthVBString(strData,strDelimiter,dwDim,strDelimiter2,dwDim2);
	dwCount = StringUtil.SplitString(strData,strDelimiter,resultNs,true);

	intCount = dwCount;
	if(dwCount > MAX_TASK_ITEM_COUNT)
	{
		intCount = MAX_TASK_ITEM_COUNT;
	}

	if (dwCount > 0 ) 
	{
		std::map<DWORD,T_SaveToServerAccount*> *pMapAccountPass = NULL;
		pMapAccountPass = 
			(std::map<DWORD,T_SaveToServerAccount*>  *)g_GlobalDataBlock->ReadRealData(
			E_MAP_SaveAccountPass,MAG_ACCOUNT_INDEX,E_Save_CLASS,eUseBlockWithOne);

		if (pMapAccountPass != NULL)
		{
			for(int i = 1; i < intCount+1 ; i++)
			{		
				szTemp=resultNs[i];
				resultItem.clear () ;
				dwItemCount = StringUtil.SplitString(szTemp, strDelimiter2,resultItem,true)	;

				if (dwItemCount >= 6)
				{
					//保存网站帐号密码等信息
					T_SaveToServerAccount *AccountData = new T_SaveToServerAccount;

					_tcsncpy(TempTch , resultItem[1], MAX_ID_STRING_LENGTH-1);
					AccountData->dwNetstationID  = _ttol(TempTch);

					_tcsncpy(AccountData->szAccount,resultItem[2], MAX_ACCOUNT_PASSWORD_LENGTH-1);

					_tcsncpy(AccountData->szPassword,resultItem[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);

					_tcsncpy(TempTch , resultItem[4], MAX_ID_STRING_LENGTH-1);
					AccountData->dwState = _ttol(TempTch);

					_tcsncpy(AccountData->szConfirmMail, resultItem[5], MAX_EMAIL_NAME_LENGTH-1);
					_tcsncpy(AccountData->szMailPassword, resultItem[6], MAX_ACCOUNT_PASSWORD_LENGTH-1);

					(*pMapAccountPass)[AccountData->dwNetstationID ] = AccountData;
				}
				else
				{
					g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
						_T("解析某网站帐号密码字符串失败:%s"), szTemp.c_str());
				}
			}
		}

		return 1;
	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("解析整体帐号密码字符串失败:%s"), strData.c_str());
	}

	return 0;
}

/*
	@brief   保存系统信息到全局内存
	@param   系统信息字符串 
	@return  
*/
DWORD CUnZipWebTaskData::SaveSystemInfoToLocal( const CStdString& strData )
{
	int intValue = 0;
	DWORD dwFound = 0 ;
	T_LocalGlobalData *pLocalGlobalData = NULL;

	CStdString strDelimiter,strDelimiter2,strDelimiter3;
	std::vector<CStdString> result,result2, result3;
	StringUtils StringUtil;

	//先获取全局数据区的产品ID
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pLocalGlobalData == NULL)
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 0;
	}

	StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,intValue);
	dwFound = StringUtil.SplitString(strData,strDelimiter,result,true);

	if (dwFound > 1)
	{
		dwFound = StringUtil.Get1DSplitCharAndLengthVBString(result[1],strDelimiter2,intValue);
		dwFound = StringUtil.SplitString(result[1],strDelimiter2,result2,true);

		if (dwFound >= 24)	//公司信息
		{
			 //公司法人
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCompanOwner, result2[1], MAX_MID_LENGTH-1);

			//公司名称
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCompanName, result2[2], MAX_MID_LENGTH-1);

			//公司简称
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCompanShortName, result2[3], MAX_NAME_LENGTH-1);

			//公司英文名称
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCompanEName, result2[4], MAX_MID_LENGTH-1);

			//公司网址
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCompanURL, result2[5], MAX_URL_LENGTH-1);

			//公司地址
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szAddress, result2[6], MAX_NAME_LENGTH-1);

			//公司规模
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szScale, result2[7], MAX_MID_LENGTH-1);

			//公司Date
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szDate, result2[8], MAX_MID_LENGTH-1);

			//企业性质
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szProperty, result2[9], MAX_MID_LENGTH-1);

			//公司所在行业
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szTrade, result2[10], MAX_MID_LENGTH-1);

			//公司注册资金
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szFund, result2[11], MAX_MID_LENGTH-1);

			//公司简介
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCCINFO, result2[12], MAX_SUBJECT_LENGTH-1);

			//公司品牌
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szBrand, result2[13], MAX_NAME_LENGTH-1);

			//经营理念
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szIdea, result2[14], MAX_NAME_LENGTH-1);

			//公司口号
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szSlogan, result2[15], MAX_NAME_LENGTH-1);

			//主要市场
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szMarket, result2[16], MAX_NAME_LENGTH-1);

			//公司特点
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szTrait, result2[17], MAX_NAME_LENGTH-1);

			//银行开户名
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szBrankPersonName, result2[18], MAX_MID_LENGTH-1);

			//工商注册号
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szBussisNumber, result2[19], MAX_MID_LENGTH-1);

			//开户银行
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szBrankName, result2[20], MAX_MID_LENGTH-1);

			//企业银行账号
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szBrankNumber, result2[21], MAX_MID_LENGTH-1);

			//营业执照url
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL, result2[22], MAX_URL_LENGTH-1);

			//主营产品
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szPdt, result2[23], MAX_NAME_LENGTH-1);

			//公司照片url
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL, result2[24], MAX_URL_LENGTH-1);

			//时间如果为空，给默认值
			if (result2[25].length() == 0)
				result2[25] = _T("2011-01-01");
			if (result2[26].length() == 0)
				result2[26] = _T("2020-01-01");

			//营业执照起始日期
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szTradeStartDate, result2[25], MAX_ID_STRING_LENGTH-1);

			//营业执照结束日期
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szTradeEndDate, result2[26], MAX_ID_STRING_LENGTH-1);

			//证书登记机关
			if (dwFound >= 27)
			{
				_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCertReg, result2[27], MAX_MID_LENGTH - 1);
			}

			//组织机构代码证下载地址(;1)税务登记证下载地址
			if (dwFound >= 29)
			{
				_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szOrgCodeURL, result2[28], MAX_URL_LENGTH - 1);
				_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szTaxRegURL, result2[29], MAX_URL_LENGTH - 1);
			}
			
		}
		else
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("解析公司信息字符串失败:%s"), result[1].c_str());
		}

		dwFound = StringUtil.Get1DSplitCharAndLengthVBString(result[2],strDelimiter3,intValue);
		dwFound = StringUtil.SplitString(result[2],strDelimiter3,result3,true);

		if (dwFound >= 23)  //用户信息
		{
			//用户id
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szUserId, result3[1], USER_ID_COUNT-1);

			//用户中文姓
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szXing, result3[2], MAX_LOW_LENGTH-1);

			//用户中文名
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szMing, result3[3], MAX_LOW_LENGTH-1);

			//用户英文姓名
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szEName, result3[4], MAX_ID_STRING_LENGTH-1);

			//用户性别
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szSex, result3[5], MAX_LOW_LENGTH-1);

			//用户身份证
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szPersonNumber, result3[6], MAX_ID_STRING_LENGTH-1);

			//国家
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szCountry, result3[7], MAX_ID_STRING_LENGTH-1);

			//省份
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szProvin, result3[8], MAX_ID_STRING_LENGTH-1);

			//城市
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szCity, result3[9], MAX_ID_STRING_LENGTH-1);

			//区/县
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szArea, result3[10], MAX_ID_STRING_LENGTH-1);

			//邮编
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szZip, result3[11], MAX_ID_STRING_LENGTH-1);

			//出生年
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szYear, result3[12], MAX_LOW_LENGTH-1);

			//出生月
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szMonth, result3[13], MAX_LOW_LENGTH-1);

			//出生日
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szDay, result3[14], MAX_LOW_LENGTH-1);

			//邮箱
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szMail, result3[15], MAX_MID_LENGTH-1);

			//国际区号
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szICode, result3[16], MAX_LOW_LENGTH-1);

			//国内区号
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szACode, result3[17], MAX_LOW_LENGTH-1);

			//联系电话
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szPhone, result3[18], MAX_MID_LENGTH-1);

			//联系qq
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szQQ, result3[19], MAX_MID_LENGTH-1);

			//传真
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szFax, result3[20], MAX_MID_LENGTH-1);

			//手机号码
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szMobile, result3[21], MAX_MID_LENGTH-1);

			//msn
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szMsn, result3[22], MAX_MID_LENGTH-1);

			//职位
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szJob, result3[23], MAX_MID_LENGTH-1);
			
			//部门
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szDept, result3[24], MAX_MID_LENGTH-1);

			//国家编号
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szCountNumber, result3[25], MAX_MID_LENGTH-1);

			//省份编号
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szProvNumber, result3[26], MAX_MID_LENGTH-1);

			//城市编号
			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szCitNumber, result3[27], MAX_MID_LENGTH-1);

			if (dwFound >= 28)
			{
				//县级编号
				_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szAreaNumber, result3[28], MAX_MID_LENGTH-1);

				if (dwFound >= 29)
				{
					//备用邮箱
					_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szMailBak, result3[29], MAX_EMAIL_NAME_LENGTH - 1);
				}
			}
			

		}
		else
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("解析用户信息字符串失败:%s"), result[2].c_str());
		}

		return 1;
	}

	return 0;
}

DWORD CUnZipWebTaskData::TransRegistryToTaskData( const CStdString &strSource, WEB_TASK_DATA_REGISTRY &strRegistryData )
{
	//截取有效字符也在这里
	//解压缩也是在这里

	//(;9)所有网站信息(;9)系统信息(;9)配置信息
	//(;9)服务器当前的分钟数(和2001-1-16的差) (;9)定时任务ID

	int dwFound,dwDim;
	DWORD dwCount;

	CStdString strData;
	CStdString strDelimiter;

	std::vector<CStdString> result;
	StringUtils StringUtil;

	CFileReadAndSave::UnZipDecodeStdString (strSource,strData);
	dwFound = StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,dwDim);
	dwCount = StringUtil.SplitString(strData,strDelimiter,result,true);

	if (dwCount >= 4)
	{
		strRegistryData.strWebInfo = result[1];  
		strRegistryData.strSystemInfo = result[2];
		strRegistryData.strConfigInfo = result[3];
		strRegistryData.strServerMintus = result[4];

		if (dwCount >= 5)
		{
			strRegistryData.strTimerTaskID = result[5];
		}
	}

	return 0;
}

DWORD CUnZipWebTaskData::TransInfoModifyToTaskData( const CStdString &strSource, WEB_TASK_DATA_INFOMODIFY &stWebTaskData )
{
	//截取有效字符和解压缩是在这里

	//(;9)所有网站信息(;9)系统信息(;9)配置信息
	//(;9)帐号密码信息

	int dwFound, dwDim;
	DWORD dwCount;

	CStdString strData;
	CStdString strDelimiter;

	std::vector<CStdString> result;
	StringUtils stringUtil;

	CFileReadAndSave::UnZipDecodeStdString(strSource,strData);
	dwFound = stringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,dwDim);
	dwCount = stringUtil.SplitString(strData,strDelimiter,result,true);

	if (dwCount >= 4)
	{
		stWebTaskData.strWebInfo = result[1];
		stWebTaskData.strSystemInfo =result[2];
		stWebTaskData.strConfigInfo = result[3];
		stWebTaskData.strAccountPassword = result[4];
	}

	return 0;
}

DWORD CUnZipWebTaskData::TransRefreshAndDeleteToTaskData( const CStdString &strSource, WEB_TASK_DATA_REFRESHANDDELETE &strRefreshAndDelteData )
{
	//截取有效字符也在这里
	//解压缩也是在这里

	//(;9) 所有网站信息(;9)帐号密码信息(;9)配置信息
	//(;9)服务器当前的分钟数(和2001-1-16的差) (;9)定时任务ID

	int dwFound,dwDim;
	DWORD dwCount;

	CStdString strData;
	CStdString strDelimiter;

	std::vector<CStdString> result;
	StringUtils StringUtil;

	CFileReadAndSave::UnZipDecodeStdString (strSource,strData);
	dwFound = StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,dwDim);
	dwCount = StringUtil.SplitString(strData,strDelimiter,result,true);

	if (dwCount >= 4)
	{
		strRefreshAndDelteData.strWebInfo = result[1];
		strRefreshAndDelteData.strAccountPassword = result[2];
		strRefreshAndDelteData.strConfigInfo = result[3];
		strRefreshAndDelteData.strServerMintus = result[4];

		if (dwCount > 4)
		{
			strRefreshAndDelteData.strTimerTaskID = result[5];
		}
	}

	return 0;
}

/*
	@brief   保存网站信息到内存
	@param   网站信息字符串 
	@return  
	*/
DWORD CUnZipWebTaskData::SaveWebInfoToLocal( const CStdString &strData )
{
	std::map<DWORD,T_TaskNetItem*> *pMapStation = NULL;
	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pMapStationAndKey = NULL;
	T_LocalGlobalData *pLocalGlobalData = NULL;

	DWORD dwCount,dwCount1;
	DWORD dwTaskType;
	int   dwTaskID; //任务ID
	pMapStationAndKey = (std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,
		MAG_STATION_INDEX,E_Save_CLASS,eUseBlockWithOne);
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	std::vector<CStdString> resultItem, resultNsType;
	std::vector<CStdString> resultNs,resultWeb;
	StringUtils StringUtil;

	int dwDim;
	int dwDim2;
	int dwFound;
	int iCount = 0;

	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
	CStdString strDelimiter,strDelimiter5;
	CStdString strDelimiter2,strDelimiter3,strDelimiter4;
	resultNs.clear();
	dwFound = StringUtil.Get2DSplitCharAndLengthVBString(strData,strDelimiter,dwDim,strDelimiter2,dwDim2);
	dwCount = StringUtil.SplitString(strData,strDelimiter,resultNs,true);

	for (int i = 1; i<dwCount + 1; i++)
	{
		if (pMapStationAndKey != NULL)
		{
			pMapStation = new std::map<DWORD,T_TaskNetItem*>;
			resultNsType.clear();
			dwCount1 = StringUtil.SplitString(resultNs[i],strDelimiter2,resultNsType,true);
			if (dwCount1 >= 2)
			{
				if ( 0 == resultNsType[1].CompareNoCase(_T("Infopost")))     //发布
				{
					dwTaskType = ETTypePost;
				}
				else if ( 0 == resultNsType[1].CompareNoCase(_T("Registry")))   //注册
				{
					dwTaskType = ETTypeRegaccount;
				}
				else if ( 0 == resultNsType[1].CompareNoCase(_T("Inforefr")))    //刷新
				{
					dwTaskType = ETypeRefresh;
				}
				else if ( 0 == resultNsType[1].CompareNoCase(_T("InfoDele")))    //删除
				{
					dwTaskType = ETypeDelete;
				}
				else if ( 0 == resultNsType[1].CompareNoCase(_T("CompanyPopularity")))  //公司知名度
				{
					dwTaskType = ETypeCompanyPopular;
				}
				else if ( 0 == resultNsType[1].CompareNoCase(_T("MailRegistry")))   //邮箱注册
				{
					dwTaskType = ETypeRegEmail;
				}
				else if( 0 == resultNsType[1].CompareNoCase(_T("MailCount")))   //邮件查询
				{
					dwTaskType = ETypeMailCount;
				}
				else if ( 0 == resultNsType[1].CompareNoCase(_T("ReRegistry")))   //注册
				{
					dwTaskType = ETTypeReRegaccount;
				}
				else if ( 0 == resultNsType[1].CompareNoCase(_T("InfoMody")))   //修改资料
				{
					dwTaskType = ETTypeModify;
				}
				else if (0 == resultNsType[1].CompareNoCase(_T("MailActivate"))) //邮箱激活
				{
					dwTaskType = ETTypeMailActivate;
				}
				else
				{
					g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
						_T("获取任务类型失败！"));
				}
				
				//增加任务ID;
				if (dwCount1 >= 3)
				{
					_tcscpy(TempTch, resultNsType[3]);
					dwTaskID = _ttoi(TempTch);
				}
				else
				{
					dwTaskID = -1;
				}

				resultWeb.clear();
				DWORD dwFound1 = StringUtil.Get2DSplitCharAndLengthVBString(resultNsType[2],strDelimiter3,dwDim,strDelimiter4,dwDim2);
				DWORD dwCount2 = StringUtil.SplitString(resultNsType[2],strDelimiter3,resultWeb,true);

				if (pMapStation != NULL)
				{
					for(int i = 1; i < dwCount2 +1; ++i)
					{
						resultItem.clear();
						DWORD dwItemCount = StringUtil.SplitString(resultWeb[i],strDelimiter4,resultItem,true);
						switch (dwTaskType)
						{
						case ETTypePost:
						case ETTypeRegaccount:
						case ETTypeReRegaccount:
							if (dwItemCount >= 4)
							{
								//网站id
								_tcscpy(TempTch , resultItem[1]);
								std::map<DWORD,T_TaskNetItem*>::iterator iter = (pMapStation)->find(_ttol(TempTch));
								if (iter != (pMapStation)->end())
								{
									g_log.Trace(LOGL_HIG, LOGT_WARNING, __TFILE__, __LINE__,
										_T("检测到发布类型有重复网站ID:%s"), resultItem[1].c_str());
									continue;
								}

								T_TaskNetItem *pNetItem = new T_TaskNetItem;


								pNetItem->dwWebId = _ttol(TempTch);

								//网站栏目id
								_tcscpy(TempTch , resultItem[2]);
								pNetItem->dwNetStationItemID = _ttoi64(TempTch);

								//网站名称
								_tcsncpy(pNetItem->szWebName, resultItem[3].c_str(), MAX_MID_LENGTH-1);

								//网站栏目名称
								_tcsncpy(pNetItem->szWebItemName, resultItem[4].c_str(), MAX_MID_LENGTH-1);

								//任务ID
								pNetItem->dwTaskId = dwTaskID;

								(*pMapStation)[pNetItem->dwWebId] = pNetItem;

							}
							else
							{
								g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
									_T("解析网站信息字符串失败:%s"), resultNs[i].c_str());
							}
							break;
						case ETypeDelete:
							if (dwItemCount >=3)
							{
								//网站id
								_tcscpy(TempTch , resultItem[1]);
								std::map<DWORD,T_TaskNetItem*>::iterator iter = (pMapStation)->find(_ttol(TempTch));
								if (iter != (pMapStation)->end())
								{
									g_log.Trace(LOGL_HIG, LOGT_WARNING, __TFILE__, __LINE__,
										_T("检测到删除类型有重复网站ID:%s"), resultItem[1].c_str());
									continue;
								}
								
								T_TaskNetItem *pNetItem = new T_TaskNetItem;

								pNetItem->dwWebId = _ttol(TempTch);

								//网站名称
								_tcsncpy(pNetItem->szWebName, resultItem[2].c_str(), MAX_MID_LENGTH-1);

								//信息id
								dwFound = StringUtil.Get1DSplitCharAndLengthVBString(resultItem[3],strDelimiter5,dwDim);
								DWORD dwCnt = StringUtil.SplitString(resultItem[3],strDelimiter5,pNetItem->vInfoID,true);

								//任务ID
								pNetItem->dwTaskId = dwTaskID;

								(*pMapStation)[pNetItem->dwWebId] = pNetItem;
								
							}
							else
							{
								g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
									_T("解析网站信息字符串失败:%s"), resultNs[i].c_str());
							}
							break;

						case ETypeCompanyPopular:
						case ETypeRegEmail:
						case ETypeRefresh:
						case ETypeMailCount:
						case ETTypeModify:
						case ETTypeMailActivate:

							if (dwItemCount>=2)
							{
								//网站id
								_tcscpy(TempTch , resultItem[1]);
								std::map<DWORD,T_TaskNetItem*>::iterator iter = (pMapStation)->find(_ttol(TempTch));
								if (iter != (pMapStation)->end())
								{
									g_log.Trace(LOGL_HIG, LOGT_WARNING, __TFILE__, __LINE__,
										_T("检测到注册/刷新类型有重复网站ID:%s"), resultItem[1].c_str());
									continue;
								}							
							
								T_TaskNetItem *pNetItem = new T_TaskNetItem;

								pNetItem->dwWebId = _ttol(TempTch);

								//网站名称
								_tcsncpy(pNetItem->szWebName, resultItem[2].c_str(), MAX_MID_LENGTH-1);

								//任务ID
								pNetItem->dwTaskId = dwTaskID;

								(*pMapStation)[pNetItem->dwWebId] = pNetItem;
								

							}
							else
							{
								g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
									_T("解析网站信息字符串失败:%s"), resultNs[i].c_str());
							}
							break;
						}
					}
				}
				else
				{
					g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
						_T("pMapStation New 失败"));
				}

			}
			g_log.Trace(LOGL_HIG, LOGT_PROMPT, __TFILE__, __LINE__,
				_T("执行任务类型: %s ,数量为：%d ！"),resultNsType[1].c_str(),pMapStation->size());
			iCount += pMapStation->size();
			(*pMapStationAndKey)[dwTaskType] = pMapStation;
		}
		else 
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
						_T("读取网站数据失败"));
		}

	}

	if (pLocalGlobalData != NULL)
	{
		pLocalGlobalData->dwCurrentTaskItemCount = iCount;
		g_log.Trace(LOGL_HIG, LOGT_PROMPT, __TFILE__, __LINE__,
			_T("网站总数：%d "),iCount);
	}

	return 0;
}

/*
@brief   保存单任务网站信息到内存
@param   网站信息字符串 
@return  
*/
DWORD CUnZipWebTaskData::SaveWebInfoToLocalOfOneTask( const CStdString &strData )
{
	std::map<DWORD,T_TaskNetItem*> *pMapStation = NULL;
	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pMapStationAndKey = NULL;
	T_LocalGlobalData *pLocalGlobalData = NULL;

	DWORD dwCount;

	pMapStationAndKey = (std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,
		MAG_STATION_INDEX,E_Save_CLASS,eUseBlockWithOne);
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	std::vector<CStdString> resultItem;
	std::vector<CStdString> resultNs;
	StringUtils StringUtil;

	int dwDim;
	int dwDim2;
	int dwFound;

	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
	CStdString strDelimiter;
	CStdString strDelimiter2;

	dwFound = StringUtil.Get2DSplitCharAndLengthVBString(strData,strDelimiter,dwDim,strDelimiter2,dwDim2);
	dwCount = StringUtil.SplitString(strData,strDelimiter,resultNs,true);

	if(dwCount > MAX_TASK_ITEM_COUNT)
	{
		dwCount = MAX_TASK_ITEM_COUNT;
	}

	if (pMapStationAndKey != NULL)
	{
		pMapStation = new std::map<DWORD,T_TaskNetItem*>;

		if(pMapStation != NULL)
		{
			for(int i=1; i<dwCount+1; i++)
			{		
				resultItem.clear () ;
				DWORD dwItemCount = StringUtil.SplitString(resultNs[i], strDelimiter2,resultItem,true)	;

				if (dwItemCount >= 2)
				{
					T_TaskNetItem *pNetItem = new T_TaskNetItem;

					//网站id
					_tcscpy(TempTch , resultItem[1]);
					pNetItem->dwWebId = _ttol(TempTch);

// 					//网站栏目id
// 					_tcscpy(TempTch , resultItem[2]);
// 					pNetItem->dwNetStationItemID = _ttoi64(TempTch);

					//网站名称
					_tcsncpy(pNetItem->szWebName, resultItem[2].c_str(), MAX_MID_LENGTH-1);

// 					//网站栏目名称
// 					_tcsncpy(pNetItem->szWebItemName, resultItem[4].c_str(), MAX_MID_LENGTH-1);

					(*pMapStation)[pNetItem->dwWebId] = pNetItem;
				}
				else
				{
					g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
						_T("解析网站信息字符串失败:%s"), resultNs[i].c_str());
				}
			}
		}
		else
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("pMapStation New 失败"));
		}
		(*pMapStationAndKey)[ETTypeModify] = pMapStation;

		if (pLocalGlobalData != NULL)
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStation->size();

		g_log.Trace(LOGL_HIG, LOGT_PROMPT, __TFILE__, __LINE__,
			_T("网站数量：%d "),pMapStation->size());

		return 1;
	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("读取网站数据失败"));
	}

	return 0;

}

/*
	@brief   保存发布信息到内存
	@param   网站信息字符串 
	@return  
	*/
DWORD CUnZipWebTaskData::SavePostInfoToLocal( const CStdString &strData )
{
	std::vector<CStdString> resultDetail;
	std::vector<CStdString> resultNs;
	StringUtils StringUtil;

	int dwDim;
	int dwFound;
	DWORD dwCount;

	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
	CStdString strDelimiter;

	if (strData.length() <= 0)
	{
		g_log.Trace(LOGL_HIG, LOGT_WARNING, __TFILE__, __LINE__,
			_T("发布内容为空!"));
	}

	dwFound = StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,dwDim);
	dwCount = StringUtil.SplitString(strData,strDelimiter,resultNs,true);

	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pLocalGlobalData == NULL)
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 0;
	}

	if (dwCount >= 1)
	{
		//处理任务详细信息的
		dwFound = StringUtil.Get1DSplitCharAndLengthVBString(resultNs[1],strDelimiter,dwDim);
		dwCount = StringUtil.SplitString(resultNs[1],strDelimiter,resultDetail,true);

		if (dwCount >= 7)
		{
			//发布编号
			_tcscpy(TempTch , resultDetail[1]);
			pLocalGlobalData->TaskDetailInfo.dwNnumber = _ttol(TempTch);

			//发布主题
			_tcsncpy(pLocalGlobalData->TaskDetailInfo.szSubject, resultDetail[2], MAX_NAME_LENGTH-1);

			//发布内容
			_tcsncpy(pLocalGlobalData->TaskDetailInfo.szContext, resultDetail[3], MAX_CONTEXT_LENGTH-1);

			//有效期
			_tcsncpy(pLocalGlobalData->TaskDetailInfo.szExpire, resultDetail[4], MAX_MID_LENGTH-1);

			//是否英文信息
			_tcscpy(TempTch , resultDetail[5]);
			pLocalGlobalData->TaskDetailInfo.dwIsEnglish = _ttol(TempTch);

			//发布关键词
			_tcsncpy(pLocalGlobalData->TaskDetailInfo.szrKeyWords, resultDetail[6], MAX_NAME_LENGTH-1);

			//产品信息
			const CStdString &strProduct = resultDetail[7];
			SaveProductToLocal(strProduct);

			//抓取快照用到的标记
			if (dwCount > 7)
			{
				_tcsncpy(pLocalGlobalData->TaskDetailInfo.szPagePhotoFlag,resultDetail[8],MAX_NAME_LENGTH-1);
			}
			

			return 1;
		}
		else
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("解析发布内容字符串失败:%s!"), resultNs[1].c_str());
		}
	}

	return 0;
}

/*
	@brief   保存配置信息
	@param   配置信息字符串
	@return  
	*/
DWORD CUnZipWebTaskData::SaveConfigInfoToLocal( const CStdString &strData )
{
	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pLocalGlobalData != NULL)
	{
		if (strData.GetLength() <= 0)
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("上层传输配置信息字符串失败:使用默认值!"));
			pLocalGlobalData->dwRetry = 0;
			pLocalGlobalData->dwShow = 1;
			pLocalGlobalData->dwShowTime = 60;
			pLocalGlobalData->dwInputTime = 60;
			pLocalGlobalData->dwOCRType = 0;

			return 1;
		}

		TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
		CStdString strDelimiter;
		StringUtils StringUtil;
		std::vector<CStdString> resultConfig;

		int iDim;
		DWORD dwCount;

		StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,iDim);
		dwCount = StringUtil.SplitString(strData,strDelimiter,resultConfig,true);

		if (dwCount >= 4)
		{
			_tcscpy(TempTch , resultConfig[1]);
			pLocalGlobalData->dwRetry = _ttol(TempTch);

			_tcscpy(TempTch , resultConfig[2]);
			pLocalGlobalData->dwShow = _ttol(TempTch);

			_tcscpy(TempTch , resultConfig[3]);
			pLocalGlobalData->dwShowTime = _ttol(TempTch);

			_tcscpy(TempTch , resultConfig[4]);
			pLocalGlobalData->dwInputTime = _ttol(TempTch);
			if (dwCount >= 5)
			{
				_tcscpy(TempTch, resultConfig[5]);
				pLocalGlobalData->dwOCRType = _ttol(TempTch);
			}

			return 1;
		}
		else
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("解析配置信息字符串失败:%s!"), strData.c_str());
		}
	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("读取全局数据失败"));
	}

	return 0;
}

DWORD CUnZipWebTaskData::TransDomainPostToTaskData( const CStdString &strSource, WEB_TASK_DATA_DOMAINPOST &stWebTaskData )
{
	TCHAR cTemp[20] = {0};
	StringUtils StringUtil;
	std::vector<CStdString> result;

	CStdString strData;
	CFileReadAndSave::UnZipDecodeStdString (strSource,strData);

	StringUtil.SplitString(strData,_T("(;9)"),result,true);

	if (result.size() > 3)
	{
		stWebTaskData.strWebInfo = result[1];  
		stWebTaskData.strTaskInfo = result[2];
		stWebTaskData.strConfigInfo = result[3];

		return 0;
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
			_T("域名推广数据格式有误..."));
	}

	return 1;
}

/*
	@brief   保存域名推广任务信息
	@param   strData 域名推广任务信息字符串 
	@return  
	*/
DWORD CUnZipWebTaskData::SaveDomainInfoToLocal( const CStdString& strData )
{
	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pLocalGlobalData != NULL)
	{
		TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
		CStdString strDelimiter;
		StringUtils StringUtil;
		std::vector<CStdString> resultDomain;

		int iDim;
		DWORD dwCount;

		StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,iDim);
		dwCount = StringUtil.SplitString(strData,strDelimiter,resultDomain,true);

		if (dwCount >= 3)
		{
			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCompanURL, resultDomain[1], MAX_URL_LENGTH-1);

			_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szICPBackup, resultDomain[2], MAX_NAME_LENGTH-1);

			_tcsncpy(pLocalGlobalData->aSystemInfo.userInfo.szMail, resultDomain[3], MAX_EMAIL_NAME_LENGTH-1);

			return 1;
		}
		else
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("解析域名推广任务信息字符串失败:%s!"), strData.c_str());
		}
	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("读取全局数据失败"));
	}

	return 0;
}

DWORD CUnZipWebTaskData::TransWeboPostToTaskData( const CStdString &strSource, WEB_TASK_DATA_WEBOPOST &stWebTaskData )
{
	TCHAR cTemp[20] = {0};
	StringUtils StringUtil;
	std::vector<CStdString> result;

	CStdString strData;
	CFileReadAndSave::UnZipDecodeStdString (strSource,strData);

	StringUtil.SplitString(strData,_T("(;9)"),result,true);

	if (result.size() > 4)
	{
		stWebTaskData.strWebInfo = result[1];  
		stWebTaskData.strTaskInfo = result[2];
		stWebTaskData.strAccountPassword = result[3];
		stWebTaskData.strConfigInfo = result[4];

		return 0;
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
			_T("微博推广数据格式有误..."));
	}

	return 1;
}

DWORD CUnZipWebTaskData::SaveWeboInfoToLocal( const CStdString& strData )
{
	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pLocalGlobalData != NULL)
	{
		CStdString strDelimiter;
		StringUtils StringUtil;
		std::vector<CStdString> resultWebo;

		int iDim;
		DWORD dwCount;

		StringUtil.Get1DSplitCharAndLengthVBString(strData,strDelimiter,iDim);
		dwCount = StringUtil.SplitString(strData,strDelimiter,resultWebo,true);

		if (dwCount >= 2)
		{
			_tcsncpy(pLocalGlobalData->TaskDetailInfo.szSubject, resultWebo[1], MAX_NAME_LENGTH-1);

			_tcsncpy(pLocalGlobalData->TaskDetailInfo.szContext, resultWebo[2], MAX_CONTEXT_LENGTH-1);

			if (dwCount >=3)
			{
				_tcsncpy(pLocalGlobalData->TaskDetailInfo.szPagePhotoFlag,resultWebo[3],MAX_NAME_LENGTH-1);
			}

			return 1;
		}
		else
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("解析微博推广任务信息字符串失败:%s!"), strData.c_str());
		}
	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("读取全局数据失败"));
	}

	return 0;
}



