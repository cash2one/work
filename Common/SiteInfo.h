#pragma once

#include <vector>
#include "EnDeCode.h"
#include "StdString.h"
#include "enumdef.h"
/************************************************************************/
/*

文件说明：对网站信息的描述 包含网站类型，网站限制等

网站限制条件：

1、发布限制
单次发布条数
免费发布总条数
发布间隔时间
发布后信息位置


2、刷新限制
//刷新类型：可单刷，可全选，可多选
单次刷新条数
免费刷新条数
刷新之后信息位置：首部，尾部，不变
//单页总条数(发满的情况下)


3、删除限制
/..

作者：宋亚非
创建日期：6月25号
版本：1.0
*/
/************************************************************************/


//////////////////////////////////////////////////////////////////////////
/////////////////网站限制///////////////////////////////////////////////
enum E_NewPos
{
	npStart ,  //首部
	npEnd ,  //尾部
	npFixed ,  //不变
};

/////////////////IE限制///////////////////////////////////////////////
enum E_IeLimit
{
	IE6 = 0,
	IE7,
	IE8,
	IE9,
	IE10,
	IE11,
	IECount,
};

struct T_Limit : public CEnDeCode
{
	/*T_Post_Limit tPostLimit ;
	T_Refresh_Limit tRefreshLimit ;
	T_Delete_Limit tDeleteLimit ;
	T_Other_Limit tOtherLimit ;*/
	DWORD dwSinglePostCount ;  //产品每天可发布条数
	DWORD dwFreePostCount ;  //可发布总条数
	DWORD dwPostIntervalTime ;  //两次发布最小时间间隔
	E_NewPos eNewPosPost ;  //发布后信息位置
	E_IeLimit eIeEdtion;   //IE版本

	DWORD dwSingleRefreshCount ;  //每天可刷新次数
	DWORD dwFreeRefreshCount ;  //免费刷新总条数
	E_NewPos eNewPosRefresh ;  //刷新后信息位置

	T_Limit()
	{
		Clear() ;
	}

	void Clear()
	{
		dwSinglePostCount = -1 ;
		dwFreePostCount = -1 ;
		dwPostIntervalTime = -1 ;
		eNewPosPost = npStart ;
		eIeEdtion = IE8;

		dwSingleRefreshCount = -1 ;
		dwFreeRefreshCount = -1 ;
		eNewPosRefresh = npStart; 
	}

	CStdString Get()
	{
		CStdString strData , strTmp ;

		strTmp.Format( _T("%d") , dwSinglePostCount ) ;
		strData += strTmp + _T("+") ;

		strTmp.Format( _T("%d") , dwFreePostCount ) ;
		strData += strTmp + _T("+") ;

		strTmp.Format( _T("%d") , dwPostIntervalTime) ;
		strData += strTmp + _T("+") ;

		strTmp.Format( _T("%d") , eNewPosPost) ;
		strData += strTmp + _T("+");

		strTmp.Format( _T("%d") , dwSingleRefreshCount ) ;
		strData += strTmp + _T("+") ;

		strTmp.Format( _T("%d") , dwFreeRefreshCount ) ;
		strData += strTmp + _T("+") ;

		strTmp.Format( _T("%d") , eNewPosRefresh) ;
		strData += strTmp + _T("+");

		strTmp.Format(_T("%d"), eIeEdtion);
		strData += strTmp;

		return strData ;
	}

	void Set(CStdString &strData)
	{
		if ( strData.IsEmpty() )
		{
			return ;
		}

		std::vector<CStdString> vecData ;
		int nSize = SplitStringWithSeparator(strData , _T("+") , vecData) ;

		int i = 0 ;
		if ( i<nSize )
		{ 
			dwSinglePostCount = _ttoi( vecData[i++] ) ;
		}
		if ( i<nSize )
		{
			dwFreePostCount = _ttoi( vecData[i++] ) ;
		}
		if ( i<nSize )
		{
			dwPostIntervalTime = _ttoi( vecData[i++] ) ;
		}
		if ( i<nSize )
		{
			eNewPosPost = (E_NewPos)_ttoi( vecData[i++] ) ;
		}

		if ( i<nSize )
		{
			dwSingleRefreshCount = _ttoi( vecData[i++] ) ;
		}
		if ( i<nSize )
		{
			dwFreeRefreshCount = _ttoi( vecData[i++] ) ;
		}
		if ( i<nSize )
		{
			eNewPosRefresh = (E_NewPos)_ttoi( vecData[i++] ) ;
		}
		if (i < nSize)
		{
			eIeEdtion = (E_IeLimit)_ttoi(vecData[i++]);
		}
	}
};


//////////////////////////////////////////////////////////////////////////
/////////////////网站类型///////////////////////////////////////////////


//网站语言
enum E_Site_Language
{
	slSimplified ,  //简体
	slTraditional ,  //繁体
	slEnglish ,      //英语
};

//账号、密码长度限制
enum E_Length 
{
	alSix ,
	alEight ,
	alTen ,
	alTwelve ,
};

//账号状态
enum E_Account_State
{
	asEmailConfirm ,
	asInstant ,
	asNeedVerify ,
	asEmailRecieveAfterVerify ,
	asPhoneVerify ,
};

//账号表现形式
enum E_Account_Style
{
	asNOT_INCLUDE_CHINESE = 0,  //正常
	asFIRST_CHAR,    //以字符开头
	asFIRST_DIGITAL, // 以数字开头
	asFIRST_CHINESE, //以汉字开头
	asONLY_CHARACTER,//仅字母character
	asFIRST_VALUE_UNKNOWN,//始终放在最后
};

//密码组合形式
enum E_Password_Style
{
	psNO_INCLUDE_CHINESE,  //常、混合(不包含汉字)
	psALL_UPPER_CHART,     //纯大写字符
	psALL_LOWER_CHART,    //纯小写字符
	psALL_DIGITAL,        //纯数字
	psALL_CHINESE,      //纯汉字
	psUPPER_CHART_AND_DIGITAL,  //大写字符和数字
	psLOWER_CHART_AND_DIGITAL,  //小写字符和数字
	psUPPER_CHART_AND_CHINESE,  //大写字符和汉字
	psLOWER_CHART_AND_CHINESE,  //小写字符和汉字
	psDIGITAL_AND_CHINESE,    //  数字和汉字
	psALL_VALUE_UNKNOWN,//始终放在最后
};

//刷新类型
enum E_Refresh_Type
{
	rtSingle ,
	rtAll ,
	rtMutiple ,
};

//信息发布后显示方式
enum E_Display_AfterPost
{
	daInstant ,   //立显
	daAfterCheck ,  //审核后显示
};

//支持产品图片数量
enum E_ProductPic_Count
{
	pcZero ,
	pcOne  ,
	pcTwo ,
	pcThree ,
	pcFour ,
	pcFive ,
	pcSix ,
	pcSeven ,
	pcEight ,
	pcNine ,
};

//是否支持注册
enum E_Need_Register
{
	nrNotNeed = 0 ,  
	nrNeedRegister ,
	nrRegisterEmail,
};

//网站类型
struct T_Site_Type
{
	E_Site_Language eSiteLanguage ;  //网站语言
	E_Length eAccountLen ; //账号长度
	E_Length ePasswordLen ;  //密码长度
	E_Account_State eAccountState ; //账号状态
	E_Account_Style eAccountStyle ;  //账号形式
	E_Password_Style ePasswordStyle ; //密码形式
	E_Refresh_Type eRefreshType ; //刷新类型
	E_Display_AfterPost eDisAfterPost ; //发布后信息显示方式
	BOOL bIsSupportStore ;  //是否支持开网店
	BOOL bIsSupportBL ;   //是否支持营业执照
	E_ProductPic_Count eProductPicCount ;  //支持产品图片数量
	E_Need_Register eNeedRegister ;  //是否需要注册
	BOOL bKeywordQuery;//是否支持关键词排名查询
	BOOL bManualCode;//是否支持人工打码
	BOOL bCloudSchedule;//云调度
	BOOL bSelectAll;//是否支持全行业匹配
	BOOL bIsObject;//是不是Object网站

	//设置默认值
	T_Site_Type()
	{
		Clear() ;
	}

	void SetCloudSchedule(BOOL bCloud)
	{
		bCloudSchedule = bCloud;
	}

	void SetManualCode(BOOL bManual)
	{
		bManualCode = bManual;
	}

	void SetSingleExtension(BOOL bSingle)
	{
		bIsObject = bSingle;
	}

	void Clear()
	{
		eSiteLanguage = slSimplified ;
		eAccountLen = alEight ;
		ePasswordLen = alEight ;
		eAccountState = asInstant ;
		eAccountStyle = asNOT_INCLUDE_CHINESE ;
		ePasswordStyle = psNO_INCLUDE_CHINESE ;
		eRefreshType = rtAll ;
		eDisAfterPost = daAfterCheck ;
		bIsSupportStore = FALSE ;
		bIsSupportBL = FALSE ;
		eProductPicCount = pcOne ;
		eNeedRegister = nrNeedRegister ;
		bKeywordQuery = TRUE;
		bManualCode = FALSE;
		bSelectAll = FALSE;
		bIsObject = FALSE;
		bCloudSchedule = FALSE;
	}

	CStdString Get()
	{
		CStdString strData , strTmp  ;

		strTmp.Format(_T("%d") , eSiteLanguage) ;
		strData += strTmp ;
		
		strTmp.Format(_T("%d") , eAccountLen) ;
		strData +=  strTmp ;
		
		strTmp.Format(_T("%d") , ePasswordLen) ;
		strData +=  strTmp ;
		
		strTmp.Format(_T("%d") , eAccountState) ;
		strData +=  strTmp ;
		
		strTmp.Format(_T("%d") , eAccountStyle) ;
		strData += strTmp ;

		strTmp.Format(_T("%d") , ePasswordStyle) ;
		strData +=  strTmp ;

		strTmp.Format( _T("%d") , eRefreshType) ;
		strData += strTmp ;

		strTmp.Format( _T("%d") , eDisAfterPost) ;
		strData += strTmp ;

		strTmp.Format( _T("%d") , bIsSupportStore) ;
		strData += strTmp ;

		strTmp.Format( _T("%d") , bIsSupportBL) ;
		strData += strTmp ;

		strTmp.Format( _T("%d") , eProductPicCount) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), eNeedRegister) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bKeywordQuery) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bSelectAll) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bManualCode) ;
		strData += strTmp ;

		strTmp.Format(_T("%d"), bIsObject);
		strData += strTmp;

		strTmp.Format(_T("%d"), bCloudSchedule);
		strData += strTmp;
		
		return strData ;
	}

	void Set(CStdString &strType)
	{
		int nLen = strType.GetLength() ;
		if ( nLen == 0)
		{
			return ;
		}

		int i = 0 ;
		if ( i < nLen )
		{
			eSiteLanguage = (E_Site_Language)_ttoi( strType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			eAccountLen = (E_Length)_ttoi( strType.Mid(i++ ,1) ) ;
		}
		if ( i < nLen)
		{
			ePasswordLen = (E_Length)_ttoi( strType.Mid(i++ ,1) ) ;
		}
		if ( i < nLen)
		{
			eAccountState = (E_Account_State)_ttoi( strType.Mid(i++ ,1) ) ;
		}
		if (  i < nLen )
		{
			eAccountStyle = (E_Account_Style)_ttoi( strType.Mid(i++ ,1) ) ;
		}
		if ( i < nLen )
		{
			ePasswordStyle = (E_Password_Style)_ttoi( strType.Mid(i++ ,1) ) ;
		}
		if ( i < nLen )
		{
			eRefreshType = (E_Refresh_Type)_ttoi( strType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			eDisAfterPost = (E_Display_AfterPost)_ttoi( strType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			bIsSupportStore = _ttoi( strType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			bIsSupportBL = _ttoi( strType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			eProductPicCount = (E_ProductPic_Count)_ttoi( strType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			eNeedRegister = (E_Need_Register)_ttoi( strType.Mid(i++, 1) ) ;
		}
		if ( i < nLen )
		{
			bKeywordQuery = _ttoi( strType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			bSelectAll = _ttoi( strType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			bManualCode = _ttoi( strType.Mid(i++ , 1) ) ;
		}
		if (i < nLen)
		{
			bIsObject = _ttoi(strType.Mid(i++, 1));
		}
		if (i < nLen)
		{
			bCloudSchedule = _ttoi(strType.Mid(i++, 1));
		}
	}
};

//////////////////////////////////////////////////////////////////////////
/////////////////发布类型///////////////////////////////////////////////
struct T_Post_Type : public CEnDeCode
{
	BOOL bProductPost ;  //产品发布
	BOOL bSupplyPost ;  //供应发布
	BOOL bPurchasePost ;  //采购发布
	BOOL bSupportRefresh ; //支持刷新
	BOOL bDomainPromote ; //支持域名推广
	BOOL bSupportDelete ; //支持删除
	BOOL bCompany;        //公司知名度
	BOOL bEmailQuery;     //邮箱查询
	BOOL bMicroBlog;      //微博发布
	BOOL bOnceRemove;     //一键搬家
	BOOL bChangeData;     //修改资料
	BOOL bExtraProduct;   //提取产品

	T_Post_Type()
	{
		Clear() ;
	}

	void Clear()
	{
		bProductPost = FALSE ;
		bSupplyPost = TRUE ;
		bPurchasePost = FALSE ;
		bSupportRefresh = FALSE ;
		bDomainPromote = FALSE ;
		bSupportDelete = FALSE ;
		bCompany = FALSE;
		bEmailQuery = FALSE;
		bMicroBlog = FALSE;
		bOnceRemove = FALSE;
		bChangeData = FALSE;
		bExtraProduct = FALSE;
	}

	CStdString Get()
	{
		CStdString strData , strTmp ;

		strTmp.Format(_T("%d") , bProductPost) ;
		strData += strTmp ;

		strTmp.Format(_T("%d") , bSupplyPost) ;
		strData += strTmp ;

		strTmp.Format(_T("%d") , bPurchasePost) ;
		strData += strTmp ;

		strTmp.Format( _T("%d") , bSupportRefresh) ;
		strData += strTmp ;

		strTmp.Format( _T("%d") , bDomainPromote) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bSupportDelete) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bCompany) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bEmailQuery) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bMicroBlog) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bOnceRemove) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bChangeData) ;
		strData += strTmp ;

		strTmp.Format( _T("%d"), bExtraProduct) ;
		strData += strTmp ;

		return strData ;
	}

	void Set(CStdString &strPostType)
	{
		if ( strPostType.IsEmpty() )
		{
			return ;
		}

		int nLen = strPostType.GetLength() ;
		int i = 0 ;
		if ( i < nLen )
		{
			bProductPost = _ttoi( strPostType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			bSupplyPost = _ttoi( strPostType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			bPurchasePost = _ttoi( strPostType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			bSupportRefresh = _ttoi( strPostType.Mid(i++ , 1) ) ;
		}
		if ( i < nLen )
		{
			bDomainPromote = _ttoi( strPostType.Mid(i++ , 1) ) ;
		}
		if ( i <nLen )
		{
			bSupportDelete = _ttoi( strPostType.Mid(i++, 1) ) ;
		}
		if ( i <nLen )
		{
			bCompany = _ttoi( strPostType.Mid(i++, 1) ) ;
		}
		if ( i <nLen )
		{
			bEmailQuery = _ttoi( strPostType.Mid(i++, 1) ) ;
		}
		if ( i <nLen )
		{
			bMicroBlog = _ttoi( strPostType.Mid(i++, 1) ) ;
		}
		if ( i <nLen )
		{
			bOnceRemove = _ttoi( strPostType.Mid(i++, 1) ) ;
		}
		if ( i <nLen )
		{
			bChangeData = _ttoi( strPostType.Mid(i++, 1) ) ;
		}
		if ( i <nLen )
		{
			bExtraProduct = _ttoi( strPostType.Mid(i++, 1) ) ;
		}
	}
};


//////////////////////////////////////////////////////////////////////////
/////////////////网站信息(汇总)///////////////////////////////////////////////
//数据库网站信息
struct T_Station_Info : public CEnDeCode
{
	CStdString strID ;
	CStdString strName ;
	CStdString strHost ;
	CStdString strSiteState ;  //网站状态
	CStdString strConst ;  //网站数据
	CStdString strEditor ;  //采编
	CStdString strSiteClass ;  //网站星级
	CStdString strMemo ;   //网站备注
	CStdString strLastModify ; //网站最后更新时间
	CStdString strImagePrecessCode;//验证码参数

	DWORD dwBaiduWeight ;  //百度权重
	DWORD dwBaiduRecord ;  //百度收录数
	DWORD dwGooglePR ;  //Google PR指数
	DWORD dwGoogleRecord ;  //Google收录指数

	DWORD dwAverageDailyIP;  //IP
	DWORD dwAverageDailyPV;  //PV

//	CStdString strAccount ;
//	CStdString strPass ;
	T_Site_Type tSiteType ;
	T_Post_Type tPostType ;
	T_Limit tLimit ;

	E_Support_Type eSupportType ; //网站支持的软件版本类型

	T_Station_Info()
	{
		Clear() ;
	}

	void Clear()
	{
		strID.Empty() ;
		strName.Empty() ;
		strHost = _T("http://") ;
		strSiteState.Empty() ;
		strConst.Empty() ;
		strEditor.Empty() ;
		strEditor.Empty() ;
		strMemo.Empty() ;
		strImagePrecessCode.Empty();

		dwBaiduWeight = 0 ;
		dwBaiduRecord = 0 ;
		dwGooglePR = 0 ;
		dwGoogleRecord = 0 ;
		dwAverageDailyIP = 0;  //IP
		dwAverageDailyPV = 0;  //PV
		strSiteClass = _T("5") ;

		tSiteType.Clear() ;
		tPostType.Clear() ;
		tLimit.Clear() ;

		eSupportType = estExpressUltimate ;
	}

	CStdString GetBaiduRecord()
	{
		CStdString strData ;
		strData.Format(_T("%d") , dwBaiduRecord) ;
		
		return strData ;
	}

	CStdString GetBaiduWeight()
	{
		CStdString strData ;
		strData.Format(_T("%d") , dwBaiduWeight) ;

		return strData ;
	}

	CStdString GetGooglePR()
	{
		CStdString strData ;
		strData.Format(_T("%d") , dwGooglePR) ;

		return strData ;
	}

	CStdString GetGoogleRecord()
	{
		CStdString strData ;
		strData.Format(_T("%d") , dwGoogleRecord) ;

		return strData ;
	}

	CStdString GetIP()
	{
		CStdString strData ;
		strData.Format(_T("%d") , dwAverageDailyIP) ;

		return strData ;
	}

	CStdString GetPV()
	{
		CStdString strData ;
		strData.Format(_T("%d") , dwAverageDailyPV) ;

		return strData ;
	}

	CStdString GetSupportType()
	{
		CStdString strSupportType ;
		strSupportType.Format( _T("%d"), eSupportType) ;

		return strSupportType ;
	}

	CStdString Get()
	{
		CStdString strData ;

		strData += strID + _T(",") ;
		strData += strName + _T(",") ;
		strData += strHost + _T(",") ;
		strData += strSiteState + _T(",") ;
		strData += strConst + _T(",") ;
		strData += strEditor + _T(",") ;
		strData += strSiteClass + _T(",") ;
		strData += strMemo + _T(",") ;
		strData += strLastModify + _T(",") ;

		strData += GetBaiduWeight() + _T(",") ;
		strData += GetBaiduRecord() + _T(",") ;
		strData += GetGooglePR() + _T(",") ;
		strData += GetGoogleRecord() + _T(",") ;

		strData += tSiteType.Get() + _T(",") ;
		strData += tPostType.Get() + _T(",") ;
		strData += tLimit.Get() + _T(",") ;

		strData += GetSupportType() + _T(",") ;

		strData += GetIP() + _T(",") ;
		strData += GetPV() + _T(",") ;

		//strData += strImagePrecessCode + _T(",");
		return Encode(strData) ;
	}

	void Set(CStdString &strData)
	{
		std::vector<CStdString> vecStationInfo ;
		int nSize = SplitStringWithSeparator(Decode(strData) , _T(",") , vecStationInfo) ;

		int i = 0 ;
		if ( i< nSize)
		{
			strID = vecStationInfo[i++] ;
		}
		if ( i< nSize)
		{
			strName = vecStationInfo[i++] ;
		}
		if ( i< nSize)
		{
			strHost = vecStationInfo[i++] ;
		}
		if ( i< nSize)
		{
			strSiteState = vecStationInfo[i++] ;
		}
		if ( i< nSize)
		{
			strConst = vecStationInfo[i++] ;
		}
		if ( i< nSize)
		{
			strEditor = vecStationInfo[i++] ;
		}
		if ( i< nSize)
		{
			strSiteClass = vecStationInfo[i++] ;
		}
		if ( i< nSize)
		{
			strMemo = vecStationInfo[i++] ;
		}
		if ( i< nSize)
		{
			strLastModify = vecStationInfo[i++] ;
		}

		if ( i< nSize)
		{
			dwBaiduWeight = _ttoi(vecStationInfo[i++]) ;
		}
		if ( i< nSize)
		{
			dwBaiduRecord = _ttoi(vecStationInfo[i++]) ;
		}
		if ( i< nSize)
		{
			dwGooglePR = _ttoi(vecStationInfo[i++]) ;
		}
		if ( i< nSize)
		{
			dwGoogleRecord = _ttoi(vecStationInfo[i++]) ;
		}
		if ( i < nSize )
		{
			tSiteType.Set(vecStationInfo[i++]) ;
		}
		if ( i < nSize )
		{
			tPostType.Set(vecStationInfo[i++]) ;
		}
		if ( i < nSize )
		{
			tLimit.Set(vecStationInfo[i++]) ;
		}
		if ( i < nSize )
		{
			eSupportType = (E_Support_Type)_ttoi( vecStationInfo[i++] ) ;
		}
		if ( i< nSize)
		{
			dwAverageDailyIP = _ttoi(vecStationInfo[i++]) ;
		}
		if ( i< nSize)
		{
			dwAverageDailyPV = _ttoi(vecStationInfo[i++]) ;
		}
		/*if (i < nSize)
		{
			strImagePrecessCode = _ttoi(vecStationInfo[i++]);
		}*/
	}
};

