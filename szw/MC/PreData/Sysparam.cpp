#include "StdAfx.h"
#include ".\sysparam.h"
#include "shlobj.h"

CSysparam::CSysparam(void)
{
	
}

CSysparam::~CSysparam(void)
{
}


//功能:处理对替换值有特殊要求(除长度外,长度的在另一个地方处理)函数
int CSysparam::TrimFun(const CStdString &strIn, CStdString& strOut, E_PPD_TRIM_TYPE eType)
{
	if (strIn.IsEmpty())
	{
		strOut=_T("");
		return 1;
	}

	strOut.Empty();

	switch(eType)
	{
	case PPD_BBodyC_TYPE://处理%BBody-C%,限制联系方式的发布正文内容
		{
			LimitedStr(strIn, strOut, PPD_BBodyC_TYPE, _T(""), 0);
			break;
		}
	case PPD_BBodyCI_TYPE://处理%BBody-CI%,限制公司名称和联系方式
		{	
			LimitedStr(strOut, strOut, PPD_BBodyCI_TYPE, 
						_T(""), 0);
			break;
		}
	case PPD_BBodyE_TYPE://处理%BBody-E,限制回车符的发布正文内容
		{
			LimitedStr(strIn, strOut, PPD_BBodyE_TYPE, _T("\r\n"), 0);
			break;
		}
	case PPD_WWebsH_TYPE://处理%WWebs-H%,不带Http标头的网页地址
		{
			LimitedStr(strIn, strOut, PPD_WWebsH_TYPE, _T("http://"), 0);
			break;
		}
	case PPD_BBodyP_TYPE://处理%BBody-P,增加图片到正文
		{
			T_LocalGlobalData *pGlobalData = NULL;
			pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
				E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
			ASSERT(pGlobalData != NULL);

			std::vector<CStdString> result;
			StringUtils strUtil;
			int iCount = strUtil.SplitString(pGlobalData->TaskProductInfo.szPhototUrl,_T("(;7)"),result,false);
			if (iCount <= 0)
			{				
				break;
			}
			int PicIndex =  strUtil.GetRand(0,iCount);  //获取随机

			CStdString strTmp = _T("<img src =\"") + result[PicIndex] + _T("\">") + _T("\r\n\r\n")+ strIn;

			strOut = strTmp;
			break;
		}
	case PPD_BBodySC_TYPE: //处理%BBody-sc,增加支持联系人信息；
		{
			T_LocalGlobalData *pGlobalData = NULL;
			pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
				E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
			ASSERT(pGlobalData != NULL);

			CStdString strNameTmp = pGlobalData->aSystemInfo.userInfo.szXing;
			strNameTmp += pGlobalData->aSystemInfo.userInfo.szMing;
			CStdString strMobileTmp = pGlobalData->aSystemInfo.userInfo.szMobile;
			strNameTmp = _T("联系人：") + strNameTmp + _T("\r\n");
			strMobileTmp = _T("联系电话：") + strMobileTmp +_T("\r\n");
			strOut = strIn + _T("\r\n") + strNameTmp + strMobileTmp;
			break;
		}
	case PPD_BBodySCW_TYPE:
		{
			T_LocalGlobalData *pGlobalData = NULL;
			pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
				E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
			ASSERT(pGlobalData != NULL);

			CStdString strNameTmp = pGlobalData->aSystemInfo.userInfo.szXing;
			strNameTmp += pGlobalData->aSystemInfo.userInfo.szMing;
			strNameTmp = _T("联系人：") + strNameTmp + _T("\r\n");
			CStdString strMobileTmp = pGlobalData->aSystemInfo.userInfo.szMobile;			
			strMobileTmp = _T("联系电话：") + strMobileTmp +_T("\r\n");
			CStdString strUrlTmp = pGlobalData->aSystemInfo.systemInfo.szCompanURL;
			strUrlTmp = _T("公司网址：") +strUrlTmp + _T("\r\n");
			strOut = strIn + _T("\r\n") + strNameTmp + strMobileTmp + strUrlTmp;
			break;
		}
	default:
		{
			break;
		}
	}

	return 0;
}

//功能:获取系统信息
int CSysparam::GetSysparam(std::vector<TParamItem>& tParamItemTable, 
				T_TaskData* pTaskData)
{
	CStdString strKey;
	CStdString strValue;

	CStdString strXing;
	CStdString strMing;

	T_LocalGlobalData *pGlobalData = NULL;
	pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
	ASSERT(pGlobalData != NULL);

	strKey.Empty();
	strValue.Empty();
	strKey=_T("ppass");
	strValue= pTaskData->szPassword;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	strKey.Empty();
	strValue.Empty();
	strKey=_T("nname");
	strValue=pTaskData->szAccount;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CComp%:公司名称.
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CComp");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCompanName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));
	if (pGlobalData->vCompany.size() <= 0)
	{
		pGlobalData->vCompany.push_back(strValue);
	}

	//%CCompshortname%:公司简称.
	std::vector<CStdString>  result;
	StringUtils strUtil;
	if (pGlobalData->vCompany.size()<5)
	{	
		strUtil.SplitString(pGlobalData->aSystemInfo.systemInfo.szCompanShortName,_T("(;3)"),result,true);
		for(int i=0; i<result.size(); i++)
		{
			const CStdString &strTmp = result[i];
			if (strTmp.length() > 0)
			{
				switch (i)
				{
				case 0:
					{
						strKey.Empty();
						strValue.Empty();
						strKey=_T("CCompshortname");
						strValue=strTmp;
						tParamItemTable.push_back(TParamItem(strKey, strValue));
						break;
					}
				case 1: case 2: case 3: case 4:
					{
						//添加多个公司名称
						pGlobalData->vCompany.push_back(result[i]);
					}

				}
			}
		}
	}
	else
	{
		strKey.Empty();
		strValue.Empty();
		strKey=_T("CCompshortname");
		strValue=pGlobalData->vCompany[1];
		tParamItemTable.push_back(TParamItem(strKey, strValue));
	}

	//%CInfo%:公司简介
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CInfo");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCCINFO;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PersonsCount%:公司规模
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PersonsCount");
	strValue=pGlobalData->aSystemInfo.systemInfo.szScale;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PersonsCount-N%:公司规模 仅人数
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PersonsCount-N");
	strValue=pGlobalData->aSystemInfo.systemInfo.szScale;
	strValue = GetNumberFromString(strValue);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CompanyKind%:企业性质
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CompanyKind");
	strValue=pGlobalData->aSystemInfo.systemInfo.szProperty;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MoneyInfo%:注册资金
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MoneyInfo");
	strValue=pGlobalData->aSystemInfo.systemInfo.szFund;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MoneyInfo-N%:注册资金(将汉字万转换成数字0000)
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MoneyInfo-N");
	strValue=pGlobalData->aSystemInfo.systemInfo.szFund;
	strValue = GetNumberFromString(strValue);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MoneyInfo-M%:注册资金(将去掉万字)
	strKey.Empty();
	strValue.Empty();
	strKey = _T("MoneyInfo-M");
	strValue = pGlobalData->aSystemInfo.systemInfo.szFund;
	strValue = GetNumberFromString(strValue);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CompanyIndustry%:公司所属行业
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CompanyTrade");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTrade;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Company place%:公司注册所在地
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegPlace");
	strValue=pGlobalData->aSystemInfo.systemInfo.szAddress;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegTime%:注册时间
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegTime");
	strValue=pGlobalData->aSystemInfo.systemInfo.szDate;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegTime%:注册时间 年
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegTime-Y");
	strValue=pGlobalData->aSystemInfo.systemInfo.szDate;
	strValue = GetDateFromString(strValue, D_YEAR);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegTime%:注册时间   月
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegTime-M");
	strValue=pGlobalData->aSystemInfo.systemInfo.szDate;
	strValue = GetDateFromString(strValue, D_MONTH);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegTime%:注册时间 日
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegTime-D");
	strValue=pGlobalData->aSystemInfo.systemInfo.szDate;
	strValue = GetDateFromString(strValue, D_DAY);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CPrincipal%%:法人代表
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CPrincipal");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCompanOwner;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ECinfo%:英文公司介绍
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ECinfo");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCCINFO;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%EComp%:英文公司名
	strKey.Empty();
	strValue.Empty();
	strKey=_T("EComp");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCompanName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	result.clear();
	strUtil.SplitString(pGlobalData->aSystemInfo.systemInfo.szBrand,_T("(;3)"),result,true);
	for(int i=1; i<result.size(); i++)
	{
		const CStdString &strTmp = result[i];
		if (strTmp.length() > 0)
		{
			if (i == 1)
			{
				//%FactoryBrand%:公司品牌
				strKey.Empty();
				strValue.Empty();
				strKey=_T("FactoryBrand");
				strValue=strTmp;
				tParamItemTable.push_back(TParamItem(strKey, strValue));
			}
			else
			{
				strKey.Empty();
				strValue.Empty();
				strKey.Format(_T("FactoryBrand%d"), i-1);
				strValue=strTmp;
				tParamItemTable.push_back(TParamItem(strKey, strValue));
			}
		}
	}
	

	//%CPhilosophy%:经营理念
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CPhilosophy");
	strValue=pGlobalData->aSystemInfo.systemInfo.szIdea;
	if (strValue.IsEmpty())
		strValue = _T("以人为本，质量第一");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CKouhao%:公司口号
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CKouhao");
	strValue=pGlobalData->aSystemInfo.systemInfo.szSlogan;
	if (strValue.IsEmpty())
		strValue = _T("顾客至上");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MainMarket%:主要市场
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MainMarket");
	strValue=pGlobalData->aSystemInfo.systemInfo.szMarket;
	if (strValue.IsEmpty())
		strValue = _T("中国");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CComanyFeature%:公司特点
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CComanyFeature");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTrait;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BankUserName%:银行开户名
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BankUserName");
	strValue=pGlobalData->aSystemInfo.systemInfo.szBrankPersonName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CComanyRegName%:工商注册号
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CComanyRegName");
	strValue=pGlobalData->aSystemInfo.systemInfo.szBussisNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BankName%:开户银行
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BankName");
	strValue=pGlobalData->aSystemInfo.systemInfo.szBrankName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BankAccount%:企业银行账号
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BankAccount");
	strValue=pGlobalData->aSystemInfo.systemInfo.szBrankNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%tradcert%:营业执照
	strKey.Empty();
	strValue.Empty();
	strKey=_T("tradcert");
	strValue=pGlobalData->szTradeImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ccomppic%:公司照片
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ccomppic");
	strValue=pGlobalData->szCompImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%sumpicAddr1%公司照片URL
	result.clear();
	strUtil.SplitString(pGlobalData->TaskProductInfo.szPhototUrl,_T("(;7)"),result,true);
	for (int i = 1; i<result.size(); i++)
	{
		const CStdString &strTmp = result[i];
		if (strTmp.length() > 0)
		{
			strKey.Empty();
			strValue.Empty();
			strKey.Format(_T("sumpicAddr%d"), i);
			strValue = result[i];
			tParamItemTable.push_back(TParamItem(strKey, strValue));
		}
	}


	//%OrgCodeCert%  组织机构代码证书
	strKey.Empty();
	strValue.Empty();
	strKey = _T("OrgCodeCert");
	strValue = pGlobalData->szOrgImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%TaxRegCert%  税务登记证
	strKey.Empty();
	strValue.Empty();
	strKey = _T("TaxRegCert");
	strValue = pGlobalData->szTaxImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%FactoryPdt%:主营产品
	result.clear();
	strUtil.SplitString(pGlobalData->aSystemInfo.systemInfo.szPdt,_T("(;3)"),result,true);
	for(int i=1; i<result.size(); i++)
	{
		const CStdString &strTmp = result[i];
		if (strTmp.length() > 0)
		{
			if (i == 1)
			{	
				strKey.Empty();
				strValue.Empty();
				strKey=_T("FactoryPdt");
				strValue=strTmp;
				tParamItemTable.push_back(TParamItem(strKey, strValue));
			}
			else
			{
				strKey.Empty();
				strValue.Empty();
				strKey.Format(_T("FactoryPdt%d"), i-1);
				strValue=strTmp;
				tParamItemTable.push_back(TParamItem(strKey, strValue));
			}
		}
	}

	//%ICPNo%:网站ICP备案
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ICPNo");
	strValue=pGlobalData->aSystemInfo.systemInfo.szICPBackup;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertStartTime%:证书起始日期
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertStartTime");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeStartDate;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertStartTime%:证书起始日期 年
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertStartTime-Y");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeStartDate;
	strValue = GetDateFromString(strValue, D_YEAR);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertStartTime%:证书起始日期 月
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertStartTime-M");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeStartDate;
	strValue = GetDateFromString(strValue, D_MONTH);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertStartTime%:证书起始日期 日
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertStartTime-D");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeStartDate;
	strValue = GetDateFromString(strValue, D_DAY);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertEndTime%:证书结束日期
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertEndTime");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeEndDate;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertEndTime%:证书结束日期 年
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertEndTime-Y");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeEndDate;
	strValue = GetDateFromString(strValue, D_YEAR);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertEndTime%:证书结束日期 月
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertEndTime-M");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeEndDate;
	strValue = GetDateFromString(strValue, D_MONTH);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertEndTime%:证书结束日期 日
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertEndTime-D");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeEndDate;
	strValue = GetDateFromString(strValue, D_DAY);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ProName%:名称
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ProName");
	strValue=pGlobalData->TaskProductInfo.szProductName;
	if(strValue.IsEmpty())
		strValue =_T("无");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Price%:价格
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Price");
	strValue=pGlobalData->TaskProductInfo.szProductPrice;
	if(strValue.IsEmpty())
		strValue =_T("0");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Price-NoZero%:价格（不能为0）
	strKey.Empty();
	strValue.Empty();
	strKey = _T("Price-NoZero");
	strValue = pGlobalData->TaskProductInfo.szProductPrice;
	if (strValue.IsEmpty() || strValue.GetAt(0) == '0')	
		strValue = _T("1");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtMeasure%:单位
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtMeasure");
	strValue=pGlobalData->TaskProductInfo.szProductUnit;
	if(strValue.IsEmpty())
		strValue = _T("套");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//产品的封装
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtPack");
	strValue=pGlobalData->TaskProductInfo.szProductSetup;
	if(strValue.IsEmpty())
		strValue =_T("无");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//产品品牌
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtBrand");
	strValue=pGlobalData->TaskProductInfo.szProductBrand;
	if(strValue.IsEmpty())
		strValue =_T("无");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtOrigin%:产地
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtOrigin");
	strValue=pGlobalData->TaskProductInfo.szProductAddress;
	if(strValue.IsEmpty())
		strValue =_T("无");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Type%:型号
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Type");
	strValue=pGlobalData->TaskProductInfo.szProductModel;
	if(strValue.IsEmpty())
		strValue =_T("NULL");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtSpecif%:规格尺寸
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtSpecif");
	strValue=pGlobalData->TaskProductInfo.szProductSpec;
	if(strValue.IsEmpty())
		strValue =_T("1*1");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Amount%:数量
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Amount");
	strValue=pGlobalData->TaskProductInfo.szProductAmount;
	if(strValue.IsEmpty()
		|| strValue.GetAt(0) == '0')
		strValue = _T("1");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%HGCode%:海关编码
	strKey.Empty();
	strValue.Empty();
	strKey=_T("HGCode");
	strValue=pGlobalData->TaskProductInfo.szProductHGCode;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtTextures%:材质
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtTextures");
	strValue=pGlobalData->TaskProductInfo.szMaterial;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ManuFacturer%:生产厂商
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ManuFacturer");
	strValue=pGlobalData->TaskProductInfo.szProductFactory;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%GuaranteePeriod%:保质/修期
	strKey.Empty();
	strValue.Empty();
	strKey=_T("GuaranteePeriod");
	strValue=pGlobalData->TaskProductInfo.szDateToDate;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtInfo%:产品介绍
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtInfo");
	strValue=pGlobalData->TaskProductInfo.szProductDetail;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtExtInfo%:产品补充说明
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtExtInfo");
	strValue=pGlobalData->TaskProductInfo.szAddDetail;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%sumpic%:产品图片
	result.clear();
	result = pGlobalData->vProductImageName;
	for(int i=0; i< result.size(); i++)
	{
		const CStdString &strTmp = result[i];
		if (strTmp.length() > 0)
		{
			if (i == 0)
			{
				//%sumpic%:产品图片
				strKey.Empty();
				strValue.Empty();
				strKey=_T("sumpic");
				strValue=strTmp;
				tParamItemTable.push_back(TParamItem(strKey, strValue));
			}
			else
			{
				strKey.Empty();
				strValue.Empty();
				strKey.Format(_T("sumpic%d"), i);
				strValue=strTmp;
				tParamItemTable.push_back(TParamItem(strKey, strValue));
			}
		}
	}
/*
	strKey.Empty();
	strValue.Empty();
	strKey=_T("sumpic");
	strValue=pGlobalData->szProductImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));*/

	//%CselFirstName%:一级中文栏目
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CselFirstName");
	strValue=pGlobalData->TaskProductInfo.szCselFirName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CselSecondName%:二级中文栏目
	strKey.Empty();
	strKey=_T("CselSecondName");
	if (_tcslen(pGlobalData->TaskProductInfo.szCselSecName) > 0)
	{
		strValue.Empty();
		strValue = pGlobalData->TaskProductInfo.szCselSecName;
	}
	
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CselThirdName%:三级中文栏目
	strKey.Empty();	
	strKey=_T("CselThirdName");
	//怕客户没有三级行业
	if (_tcslen(pGlobalData->TaskProductInfo.szCselThdName) > 0)
	{
		strValue.Empty();
		strValue = pGlobalData->TaskProductInfo.szCselThdName;
	}	
	
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Batch%:批号
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Batch");
	strValue=pGlobalData->TaskProductInfo.szProductBatchno;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PProv:省份
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PProv");
	strValue=pGlobalData->aSystemInfo.userInfo.szProvNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PProv:省份（中文）
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PProvName");
	strValue=pGlobalData->aSystemInfo.userInfo.szProvin;
	strProv = strValue;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CCity:城市
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CCity");
	strValue=pGlobalData->aSystemInfo.userInfo.szCitNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CCity:城市(中文）
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CCityName");
	strValue=pGlobalData->aSystemInfo.userInfo.szCity;
	strCity=strValue;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%AArea:地区
	strKey.Empty();
	strValue.Empty();
	strKey=_T("AArea");
	strValue=pGlobalData->aSystemInfo.userInfo.szAreaNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));
	
	//%AArea:地区(中午)
	strKey.Empty();
	strValue.Empty();
	strKey=_T("AAreaName");
	strValue=pGlobalData->aSystemInfo.userInfo.szArea;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%SSex%:性别
	strKey.Empty();
	strValue.Empty();
	strKey=_T("SSex");
	strValue=pGlobalData->aSystemInfo.userInfo.szSex;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CCountry%:国家
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CCountry");
	strValue=pGlobalData->aSystemInfo.userInfo.szCountry;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Expire:有效期
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Expire0");
	strValue=pGlobalData->TaskDetailInfo.szExpire;

	if (strValue.IsEmpty())
	{
		strValue=_T("0");
	}
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	//%ACode%:国内区号
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ACode");
	strValue=pGlobalData->aSystemInfo.userInfo.szACode;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ICode%:国际区号
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ICode");
	strValue=pGlobalData->aSystemInfo.userInfo.szICode;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RName%:联系人姓名
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RName");
	strValue=_T("联系人:");
	strValue=pGlobalData->aSystemInfo.userInfo.szXing;
	strValue += pGlobalData->aSystemInfo.userInfo.szMing;
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	//%Xing%:姓
	strKey.Empty();
	strKey=_T("Xing");
	strValue= pGlobalData->aSystemInfo.userInfo.szXing;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Ming%:名
	strKey.Empty();
	strKey=_T("Ming");
	strValue= pGlobalData->aSystemInfo.userInfo.szMing;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ZZip%:邮编
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ZZip");
	strValue=pGlobalData->aSystemInfo.userInfo.szZip;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PPID%:身份证
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PPID");
	strValue=pGlobalData->aSystemInfo.userInfo.szPersonNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%YYear%:出生年
	strKey.Empty();
	strValue.Empty();
	strKey=_T("YYear");
	strValue=pGlobalData->aSystemInfo.userInfo.szYear;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Month%:月
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Month");
	strValue=pGlobalData->aSystemInfo.userInfo.szMonth;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%DDay%:日
	strKey.Empty();
	strValue.Empty();
	strKey=_T("DDay");
	strValue=pGlobalData->aSystemInfo.userInfo.szDay;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%QQNo%:QQ号
	strKey.Empty();
	strValue.Empty();
	strKey=_T("QQNo");
	strValue = pGlobalData->aSystemInfo.userInfo.szQQ;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PerMsn%:MSN
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PerMsn");
	strValue = pGlobalData->aSystemInfo.userInfo.szMsn;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%TTel%:电话
	strKey.Empty();
	strValue.Empty();
	strKey=_T("TTel");
	strValue=pGlobalData->aSystemInfo.userInfo.szPhone;
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	//%FFax%:传真
	strKey.Empty();
	strValue.Empty();
	strKey=_T("FFax");
	strValue=pGlobalData->aSystemInfo.userInfo.szFax;;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%EMing%:英文联系人名firstname
	strKey.Empty();
	strValue.Empty();
	strKey=_T("EMing");
	strValue=pGlobalData->aSystemInfo.userInfo.szEName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%EXing%:英文联系人姓lastname
	strKey.Empty();
	strValue.Empty();
	strKey=_T("EXing");
	strValue=pGlobalData->aSystemInfo.userInfo.szEName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ERName%:英文联系人
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ERName");
	strValue= pGlobalData->aSystemInfo.userInfo.szEName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%AAddr%:街道
	strKey.Empty();
	strValue.Empty();
	strKey=_T("AAddr");
	strValue=pGlobalData->aSystemInfo.systemInfo.szAddress;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CCar%:职位
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CCar");
	strValue=pGlobalData->aSystemInfo.userInfo.szJob;
	if (strValue.IsEmpty())
		strValue = _T("经理");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//所在部门:%DeptName%
	strKey.Empty();
	strValue.Empty();
	strKey=_T("DeptName");
	strValue=pGlobalData->aSystemInfo.userInfo.szDept;
	if (strValue.IsEmpty())
		strValue = _T("市场部");
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	//%SSubj%:主题
	strKey.Empty();
	strValue.Empty();
	strKey=_T("SSubj");
	strValue=pGlobalData->TaskDetailInfo.szSubject;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%InfoK%:发布信息关键字
	result.clear();
	strUtil.SplitString(pGlobalData->TaskDetailInfo.szrKeyWords,_T("(;3)"),result,true);
	for(int i=1; i<result.size(); i++)
	{
		const CStdString &strTmp = result[i];
		if (strTmp.length() > 0)
		{
			if (i == 1)
			{
				//%InfoK%:关键词
				strKey.Empty();
				strValue.Empty();
				strKey=_T("InfoK");
				strValue=strTmp;
				tParamItemTable.push_back(TParamItem(strKey, strValue));
			}
			else
			{
				strKey.Empty();
				strValue.Empty();
				strKey.Format(_T("InfoK%d"), i-1);
				strValue=strTmp;
				tParamItemTable.push_back(TParamItem(strKey, strValue));
			}
		}
	}

	//%MMail%:电子邮件
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MMail");
	if (pTaskData->eTaskType == ETTypeMailActivate || pTaskData->eTaskType == ETypeAutoLogin)
	{
		strValue = pTaskData->szConfirmMail;
	}
	else
	{
		strValue=pGlobalData->aSystemInfo.userInfo.szMail;
	}
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%NNameMail%:(注册时)以邮箱作为账号名
	strKey.Empty();
	strValue.Empty();
	strKey=_T("NNameMail");
	strValue=pGlobalData->aSystemInfo.userInfo.szMail;
	if (pTaskData->eTaskType == ETypeAutoLogin)
	{
		//一键登录遇到帐号为邮箱时；
		if (pTaskData->szAccount.Find(_T("@")) != -1)
		{
			strValue = pTaskData->szAccount;
		}
	}
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MMailPass%:邮箱密码
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MMailPass");
	strValue=pTaskData->szMailPassword;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%HandSetNum%:手机号码
	strKey.Empty();
	strValue.Empty();
	strKey=_T("HandSetNum");
	strValue=pGlobalData->aSystemInfo.userInfo.szMobile;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%WWebs-H%:主页（不带http的）
	strKey.Empty();
	strValue.Empty();
	strKey=_T("WWebs-H");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCompanURL;
	strValue.Replace(_T("http://"), _T(""));
	strValue.Replace(_T("https://"), _T(""));
	tParamItemTable.push_back(TParamItem(strKey, strValue));
	
	//%WWebs%:主页
	strKey.Empty();
	strValue.Empty();
	strKey=_T("WWebs");
	//检测host是否带有http
	CStdString strTmp = pGlobalData->aSystemInfo.systemInfo.szCompanURL;
	strTmp.Replace(_T("http://"), _T(""));
	strTmp.Replace(_T("https://"), _T(""));
	strValue = _T("http://") + strTmp;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody%:正文
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody");
	strValue=pGlobalData->TaskDetailInfo.szContext;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-C%:限制联系方式的发布正文内容

	CStdString strTemp;
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-C");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyC_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-E:限制回车符的发布正文内容
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-E");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyE_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-:限制长度的发布正文内容
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyLen_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-CI%:限制公司名称和联系方式的发布正文内容
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-CI");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyCI_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-P%:发布正文内容包含图片
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-P");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyP_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-P%:发布正文内容包含联系人信息
	strKey.Empty();
	strValue.Empty();
	strKey = _T("BBody-SC");
	strTemp = pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodySC_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-P%:发布正文内容包含联系人信息及网址
	strKey.Empty();
	strValue.Empty();
	strKey = _T("BBody-SCW");
	strTemp = pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodySCW_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegisterOffice%:证书登记机关
	strKey.Empty();
	strValue.Empty();
	strKey = _T("RegisterOffice");
	strValue = pGlobalData->aSystemInfo.systemInfo.szCertReg;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ShopURL%  商铺地址
	strKey.Empty();
	strValue.Empty();
	strKey = _T("ShopURL");
	strValue = pGlobalData->szShopTrafficUrl;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ClickPdtPages% 产品页点击数
	strKey.Empty();
	strValue.Empty();
	strKey = _T("ClickPdtPages");
	strValue.Format(_T("%d"),pGlobalData->dwPdtClickCnt);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ClickOtherPages% 点击其它页个数
	strKey.Empty();
	strValue.Empty();
	strKey = _T("ClickOtherPages");
	strValue.Format(_T("%d"),pGlobalData->dwOtherClickCnt);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ProNum% 产品编号
	strKey.Empty();
	strValue.Empty();
	strKey = _T("ProNum");
	strValue.Format(_T("%d"), pGlobalData->TaskDetailInfo.dwNnumber);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

    return 0;
}


//功能:系统信息测试函数
int CSysparam::GetSysparamForTest(std::vector<TParamItem>& tParamItemTable)
{
	CStdString strKeyTemp;
	CStdString strValueTemp;

	//%CompanyKind%:企业性质
	strKeyTemp=_T("CompanyKind");
	strValueTemp=_T("8");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CCity:城市
	strKeyTemp=_T("CCity");
	strValueTemp=_T("440300");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Prov%:省份(没用了,先保留)
	strKeyTemp=_T("Prov");
	strValueTemp=_T("4");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%PProv:省份
	strKeyTemp=_T("PProv");
	strValueTemp=_T("4");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	strKeyTemp=_T("AArea");
	strValueTemp=_T("1");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%OFtype:信息类型
	strKeyTemp=_T("OFtype");
	strValueTemp=_T("0");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%SSex%:性别
	strKeyTemp=_T("SSex");
	strValueTemp=_T("1");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CCountry%:国家
	strKeyTemp=_T("CCountry");
	strValueTemp=_T("0");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Expire:有效期
	strKeyTemp=_T("Expire");
	strValueTemp=_T("0");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%PersonsCount%:公司规模
	strKeyTemp=_T("PersonsCount");
	strValueTemp=_T("1");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%MoneyInfo%:注册资金
	strKeyTemp=_T("MoneyInfo");
	strValueTemp=_T("1");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CSel:标记选择的子栏目参数(后面转换)
	strKeyTemp=_T("CSel");
	strValueTemp=_T("0755");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	strKeyTemp=_T("CrLf");
	strValueTemp=_T("\r\n");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ACode%:国内区号
	strKeyTemp=_T("ACode");
	strValueTemp=_T("0755");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BACode%:国内区号
	strKeyTemp=_T("BACode");
	strValueTemp=_T("0755");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CInfo%:公司简介
	strKeyTemp=_T("CInfo");
	strValueTemp=_T("国家技术创新项目“60Mt综放工作面设备配套与技术研究”中，\
					ZFS35型电液程序控制放顶煤液压支架的液控单向阀和安全阀全部由国外引进，\
					立柱上的液控单向阀和安全阀损坏比较频繁，而且安全阀的阀芯在卸载时切削密\
					封而产生漏液使得立柱无法达到工作阻力。因此，兖州矿业（集团）\
					公司兴隆庄煤矿研制了国产大流量液控单向阀和安全阀，满足了生产的需要。");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%RName%:联系人姓名
	strKeyTemp=_T("RName");
	strValueTemp=_T("李青");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BRName%:联系人姓名
	strKeyTemp=_T("BRName");
	strValueTemp=_T("李青");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BERName%:联系人姓名
	strKeyTemp=_T("BERName");
	strValueTemp=_T("李青");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ZZip%:邮编
	strKeyTemp=_T("ZZip");
	strValueTemp=_T("518100");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BZZip%:邮编
	strKeyTemp=_T("BZZip");
	strValueTemp=_T("51800");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%PPID%:身份证
	strKeyTemp=_T("PPID");
	strValueTemp=_T("360371198007177972");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%YYear%:出生年
	strKeyTemp=_T("YYear");
	strValueTemp=_T("1980");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Month%:月
	strKeyTemp=_T("Month");
	strValueTemp=_T("07");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%DDay%:日
	strKeyTemp=_T("DDay");
	strValueTemp=_T("17");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%NewTradeBig%:公司所属大行业
	strKeyTemp=_T("NewTradeBig");
	strValueTemp=_T("22");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%NewTradeDetail%:公司所属小行业
	strKeyTemp=_T("NewTradeDetail");
	strValueTemp=_T("2235");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CompanyIndustry%:公司所属行业
	strKeyTemp=_T("CompanyIndustry");
	strValueTemp=_T("2");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%SEARCHKEY%:搜索信息关键字
	strKeyTemp=_T("SEARCHKEY");
	strValueTemp=_T("电脑");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%QQNo%:QQ号
	strKeyTemp=_T("QQNo");
	strValueTemp=_T("480120350");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%TTel%:电话
	strKeyTemp=_T("TTel");
	strValueTemp=_T("45102302");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BTTel%:电话
	strKeyTemp=_T("BTTel");
	strValueTemp=_T("45102302");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%FFax%:传真
	strKeyTemp=_T("FFax");
	strValueTemp=_T("45102302");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProName%:名称
	strKeyTemp=_T("ProName");
	strValueTemp=_T("阀门");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Price%:价格
	strKeyTemp=_T("Price");
	strValueTemp=_T("0");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProFactory%:生产商(没用了,先保留)
	strKeyTemp=_T("ProFactory");
	strValueTemp=_T("中国");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%ManuFacturer%:生产产商
	strKeyTemp=_T("ManuFacturer");
	strValueTemp=_T("中国");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProArea%:产地
	strKeyTemp=_T("ProArea");
	strValueTemp=_T("深圳");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Type%:型号
	strKeyTemp=_T("Type");
	strValueTemp=_T("1000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));		

	//%ProAmount%:产品的数量配符(没用了,先保留)
	strKeyTemp=_T("ProAmount");
	strValueTemp=_T("1000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%Amount%:数量
	strKeyTemp=_T("Amount");
	strValueTemp=_T("1000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProSetup%:封装(没用了,先保留)
	strKeyTemp=_T("ProSetup");
	strValueTemp=_T("各种");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%EncapSulation%:封装
	strKeyTemp=_T("EncapSulation");
	strValueTemp=_T("各种");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ECity%:英文市/县名称
	strKeyTemp=_T("ECity");
	strValueTemp=_T("shen zhen");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EAddr%:英文街道名称
	strKeyTemp=_T("EAddr");
	strValueTemp=_T("shenzhenshiwuhaidagongsi");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BEAddr%:英文街道名称
	strKeyTemp=_T("BEAddr");
	strValueTemp=_T("shenzhenshiwuhaidagongsi");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EMing%:英文联系人名firstname
	strKeyTemp=_T("EMing");
	strValueTemp=_T("qing");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EXing%:英文联系人姓lastname
	strKeyTemp=_T("EXing");
	strValueTemp=_T("li");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ERName%:英文联系人
	strKeyTemp=_T("ERName");
	strValueTemp=_T("qing li");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BERName%:英文联系人
	strKeyTemp=_T("BERName");
	strValueTemp=_T("qing li");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%WWebs%:主页
	strKeyTemp=_T("WWebs");
	strValueTemp=_T("http://www.wuhaidagongsi.com");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%WWebs%:主页
	strKeyTemp=_T("WWebs-H");
	strValueTemp=_T("www.wuhaidagongsi.com");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BWWebs%:主页
	strKeyTemp=_T("BWWebs");
	strValueTemp=_T("http://www.wuhaidagongsi.com");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%AAddr%:街道
	strKeyTemp=_T("AAddr");
	strValueTemp=_T("深南中路58栋205室");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BAAddr%:街道
	strKeyTemp=_T("BAAddr");
	strValueTemp=_T("深南中路58栋205室");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ECountry%:英文国家名称
	strKeyTemp=_T("ECountry");
	strValueTemp=_T("China");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%SSubj%:主题
	strKeyTemp=_T("SSubj");
	strValueTemp=_T("安全球阀");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody%:正文
	strKeyTemp=_T("BBody");
	strValueTemp=_T("我们供应安全球阀国家技术创新项目“60Mt综放工作面设备配套与技术研究”中，\
					ZFS35型电液程序控制放顶煤液压支架的液控单向阀和安全阀全部由国外引进，\
					立柱上的液控单向阀和安全阀损坏比较频繁，而且安全阀的阀芯在卸载时切削\
					密封而产生漏液使得立柱无法达到工作阻力。因此，\
					兖州矿业（集团）公司兴隆庄煤矿研制了国产大流量液控单向阀\
					和安全阀，满足了生产的需要。******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%InfoK%:发布信息关键字
	strKeyTemp=_T("InfoK");
	strValueTemp=_T("安全球阀");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%MMail%:电子邮件
	strKeyTemp=_T("MMail");
	strValueTemp=_T("yaohong562@163.com");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ICode%:国际区号
	strKeyTemp=_T("ICode");
	strValueTemp=_T("086");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BICode%:国际区号
	strKeyTemp=_T("BICode");
	strValueTemp=_T("086");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CComp%:公司名称.
	strKeyTemp=_T("CComp");
	strValueTemp=_T("shenzhenshiwuhaidajixiegongsi");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProBatchno%:批号(没用了,先保留)
	strKeyTemp=_T("ProBatchno");
	strValueTemp=_T("1000000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%Batch%:批号
	strKeyTemp=_T("Batch");
	strValueTemp=_T("1000000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ECinfo%:英文公司介绍
	strKeyTemp=_T("ECinfo");
	strValueTemp=_T("China has been working in the field of jet printing packaging code cause, \
					after many years of research and development, has now established non-contact and \
					contact Damaji Stenciling Unit two series of more than 10 varieties, \
					fully able to meet the different customers different requirements.");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EComp%:英文公司名
	strKeyTemp=_T("EComp");
	strValueTemp=_T("shenzhenshiwuhaidajixiegongsi");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%HGCode%:海关编码
	strKeyTemp=_T("HGCode");
	strValueTemp=_T("");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%HandSetNum%:手机号码
	strKeyTemp=_T("HandSetNum");
	strValueTemp=_T("13421582256");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CPrincipal%:法人代表名(中文)
	strKeyTemp=_T("CPrincipal");
	strValueTemp=_T("路海彬");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%MSTitle%:回复留言主题
	strKeyTemp=_T("MSTitle");
	strValueTemp=_T("REQRQRRWEQREW");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%MSBody%:回复留言正文
	strKeyTemp=_T("MSBody");
	strValueTemp=_T("erwfefsdfwefdsfdsffffffffffffdsfsdfsdfdsfdsfds");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//ImageLogo
	//ImageValue
	//%NName%:账号
	//%PPass%:密码
	//SpecialMark
	//RegPlace
	//RegTime
	//EProv
	//ECInfo
	//Image
	//ImageValue
	//EPrincipal
	//CRName
	//CAddr
	//DDhttp
	//DACookies

	//%VKey:变量.不通用项目,随栏目不同而不同//没用了
	strKeyTemp=_T("VKey");
	strValueTemp=_T("VKey");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody-C%:限制联系方式的发布正文内容
	strKeyTemp=_T("BBody-C");
	strValueTemp=_T("我们供应安全球阀国家技术创新项目“60Mt综放工作面设备配套与技术研究”中，\
					ZFS35型电液程序控制放顶煤液压支架的液控单向阀和安全阀全部由国外引进，\
					立柱上的液控单向阀和安全阀损坏比较频繁，而且安全阀的阀芯在卸载时切削\
					密封而产生漏液使得立柱无法达到工作阻力。因此，\
					兖州矿业（集团）公司兴隆庄煤矿研制了国产大流量液控单向阀\
					和安全阀，满足了生产的需要。******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody-E:限制回车符的发布正文内容
	strKeyTemp=_T("BBody-E");
	strValueTemp=_T("我们供应安全球阀国家技术创新项目“60Mt综放工作面设备配套与技术研究”中，\
					ZFS35型电液程序控制放顶煤液压支架的液控单向阀和安全阀全部由国外引进，\
					立柱上的液控单向阀和安全阀损坏比较频繁，而且安全阀的阀芯在卸载时切削\
					密封而产生漏液使得立柱无法达到工作阻力。因此，\
					兖州矿业（集团）公司兴隆庄煤矿研制了国产大流量液控单向阀\
					和安全阀，满足了生产的需要。******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody-:限制长度的发布正文内容
	strKeyTemp=_T("BBody-");
	strValueTemp=_T("我们供应安全球阀国家技术创新项目“60Mt综放工作面设备配套与技术研究”中，\
					ZFS35型电液程序控制放顶煤液压支架的液控单向阀和安全阀全部由国外引进，\
					立柱上的液控单向阀和安全阀损坏比较频繁，而且安全阀的阀芯在卸载时切削\
					密封而产生漏液使得立柱无法达到工作阻力。因此，\
					兖州矿业（集团）公司兴隆庄煤矿研制了国产大流量液控单向阀\
					和安全阀，满足了生产的需要。******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody-CI%:限制公司名称和联系方式的发布正文内容
	strKeyTemp=_T("BBody-CI");
	strValueTemp=_T("我们供应安全球阀国家技术创新项目“60Mt综放工作面设备配套与技术研究”中，\
					ZFS35型电液程序控制放顶煤液压支架的液控单向阀和安全阀全部由国外引进，\
					立柱上的液控单向阀和安全阀损坏比较频繁，而且安全阀的阀芯在卸载时切削\
					密封而产生漏液使得立柱无法达到工作阻力。因此，\
					兖州矿业（集团）公司兴隆庄煤矿研制了国产大流量液控单向阀\
					和安全阀，满足了生产的需要。******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Xing%:中文姓//注册表添加
	strKeyTemp=_T("Xing");
	strValueTemp=_T("李");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Ming%:中文名//注册表添加
	strKeyTemp=_T("青");
	strValueTemp=_T("0755");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EProv%:英文省份//可以
	strKeyTemp=_T("EProv");
	strValueTemp=_T("guangdong");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%IDEmail%:ID邮件的ID号//不要了
	strKeyTemp=_T("IDEmail");
	strValueTemp=_T("aaa@yahoo.com.cn");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProPicture%:产品的图片通配符//不要了
	strKeyTemp=_T("ProPicture");
	strValueTemp=_T("");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EPrincipal%:法人代表名(英文)//加
	strKeyTemp=_T("EPrincipal");
	strValueTemp=_T("qing li");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%RegisterOffice%:证书登记机关
	strKeyTemp = _T("RegisterOffice");
	strValueTemp = _T("登记管理局");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	return 0;
}

/*
	@brief  根据字符串中提取首段数字
	@param  strIn 要提取的字符串
	@return 返回数字字符串
	*/
CStdString CSysparam::GetNumberFromString( const CStdString &strIn )
{
	CStdString strNumber;

	for (int i=0; i<strIn.length(); i++)
	{
		char c = strIn.at(i);
		if (c >= '0' && c<= '9')
		{
			strNumber += c;
			continue;
		}
		break;
	}

	return strNumber;
}

/*
	@brief  根据日期字符串中提取相应的年月日 格式：2011-01-02
	@param  strIn 要提取的字符串
	@param  dType 类型
	@return 返回字符串
	*/
CStdString CSysparam::GetDateFromString( const CStdString &strIn, DATETIME dType )
{
	CStdString strDate;

	if (strIn.length() != 10)
		return strDate;

	if (dType == D_YEAR)
		strDate = strIn.Mid(0, 4);
	else if (dType == D_MONTH)
		strDate = strIn.Mid(5, 2);
	else
		strDate = strIn.Mid(8, 2);

	return strDate;
}
