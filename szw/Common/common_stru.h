#ifndef __COMMON_STRUCT_H__
#define __COMMON_STRUCT_H__

#include "common_enum.h"
#include "common_define.h"
#include "StdString.h"
#include "common_stru_exserver.h"
#include "SiteInfo.h"



const DWORD DATABASE_VALID = 0;       //数据库可以正常使用

//GUI压下来的数据流
struct T_GUIData
{
	DWORD dwFlag;
	DWORD dwLen;
	BYTE* pData;
	void* pReserve;
    
    T_GUIData()
    {
        dwFlag = 0;
        dwLen = 0;
        pData = NULL;
        pReserve = NULL;
    }
	~T_GUIData()
	{
		if(pData!=NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
};

//此结构中只能用基本数据类型,并且不能用指针
struct T_TaskNetItem
{
	DWORD   dwWebId;							//网站编号
	UINT64   dwNetStationItemID;					//网站栏目编号

	TCHAR   szWebName[MAX_MID_LENGTH];         //网站名称
	TCHAR   szWebItemName[MAX_MID_LENGTH];         //网站栏目名称

	std::vector<CStdString>   vInfoID;                           //信息ID;
	int		dwTaskId;							//任务ID

    T_TaskNetItem()
    {
        dwWebId = 0;
        dwNetStationItemID = 0;
		dwTaskId = -1;
		vInfoID.clear();

		ZeroMemory(szWebName, sizeof(szWebName));
		ZeroMemory(szWebItemName, sizeof(szWebItemName));
    }
};


//任务数据
struct T_TaskData
{
    DWORD dwIndex;//本地内定的编号
    DWORD dwWebID;//网站编号

	DWORD dwTotalStep;            //该任务一共需要执行的步骤次数
	DWORD dwCurStep;              //已经执行了多少步骤

	DWORD dwCompanyId;		//公司ID(与八方对接用的）
    E_TASK_TYPE eTaskType;//任务的类型(注册,发布,搜索等)
	DWORD     dwPostType;
	E_Task_Level eTaskLevel;	//任务执行的优先级别（目前暂时给分派搜索子任务时使用）
	
	CStdString szAccount;	//用户网站帐号
	CStdString szPassword;	//用户网站密码

	CStdString szConfirmMail; //邮箱激活用的帐号；
	CStdString szMailPassword; //邮箱激活用的密码；

	CStdString strHost;
	CStdString strOCRParam;	//验证码参数

	CStdString strNotUpdateWeb;  //不需要更新的嵌入网站；

	T_TaskNetItem tTaskNetItem;
	T_Task_Data_Exser tTaskDataExser; //数据准备需要产生的数据
	T_TaskResultInfo  tTaskResultInfo;	//ExSer返回数据

	T_Site_Type  siteType;          //网站类型、一些数据要求
	T_Limit      limitRefresh;      //网站刷新要求

	std::vector<DWORD>   vRefreshIndex;     //网站刷新时  刷新的索引
	BOOL    bIsRefreshAll;
	BOOL	bUpdateDataFlag;   //是否需要更新网站模板

	BOOL    bIsSendExtra;         //发布时遇到完善资料时，是否已走发完善资料步骤
	BOOL    bIsAddExtra;        //当前步骤是否是完善资料

	BOOL    bIsSendPic;         //发布时遇到完善图片时，是否已走发完善图片步骤
	BOOL	bIsAddPic;          //当前步骤是否是完善图片

	BOOL    bGetLogin;			//是否需要获取登录数据
	BOOL	bAddExtraIsSucc;    //完善资料是否成功；

	T_TaskData()
	{
		dwIndex = 0;
		dwWebID = 0;
		dwTotalStep = 0;
		dwCurStep = 0;

		dwCompanyId = 0;
		eTaskType = ETTypeNone;
		dwPostType = 0;
		eTaskLevel = ETOtherEmpty;
 
		bUpdateDataFlag=TRUE;
		bIsSendExtra = FALSE;
		bIsAddExtra = FALSE;
		bIsSendPic = FALSE;
		bIsAddPic = FALSE;
		bIsRefreshAll = FALSE;
		bGetLogin = FALSE;
		bAddExtraIsSucc = FALSE;

		szAccount.Empty();
		szPassword.Empty();
		strHost.Empty();
		szConfirmMail.Empty();
		szMailPassword.Empty();
		strOCRParam.Empty();
	}
};


//P2P返回的产品信息的结构
struct T_TaskProductInfo
{
	DWORD dwProductID;   //产品ID
	
	TCHAR szProductName[MAX_NAME_LENGTH];     //产品名称
	TCHAR szProductPrice[MAX_PRODUCT_NORMAL_LENGTH];     //产品价格
	TCHAR szProductType[MAX_PRODUCT_NORMAL_LENGTH];			//产品类型
	TCHAR szProductAddress[MAX_NAME_LENGTH];				//生产地址
	
	TCHAR szProductDetail[MAX_PRODUCT_DETAIL_LENGTH];		//产品详细信息
	TCHAR szProductSetup[MAX_PRODUCT_NORMAL_LENGTH];		//产品设置
	TCHAR szProductSpec[MAX_PRODUCT_NORMAL_LENGTH];			//产品规格

	TCHAR szProductBatchno[MAX_PRODUCT_NORMAL_LENGTH];		//产品批号
	TCHAR szProductModel[MAX_PRODUCT_NORMAL_LENGTH];		//产品型号
	TCHAR szProductAmount[MAX_PRODUCT_NORMAL_LENGTH];		//产品数量

	TCHAR szProductUnit[MAX_PRODUCT_NORMAL_LENGTH];		//产品计量单位
	TCHAR szProductFactory[MAX_PRODUCT_NORMAL_LENGTH];				//生产厂家
	TCHAR szProductBrand[MAX_PRODUCT_NORMAL_LENGTH];		//生产品牌
	TCHAR szProductHGCode[MAX_PRODUCT_NORMAL_LENGTH];		//产品海关码
	
	TCHAR szTradeId[MAX_ID_STRING_LENGTH];		//产品栏目id
	TCHAR szTradeName[MAX_MID_LENGTH];		//产品栏目名称
	TCHAR szMaterial[MAX_MID_LENGTH];		//材质
	TCHAR szDateToDate[MAX_MID_LENGTH];		//保质期
	TCHAR szAddDetail[MAX_NAME_LENGTH];		//补充说明
	TCHAR szPhototUrl[MAX_URL_LENGTH];		//产品图片下载地址

	TCHAR szCselFirName[MAX_MID_LENGTH];      //一级中文名栏目
	TCHAR szCselSecName[MAX_MID_LENGTH];		//二级中文名栏目
	TCHAR szCselThdName[MAX_MID_LENGTH];		//三级中文名栏目
	

    T_TaskProductInfo()
    {
		dwProductID = 0;

		ZeroMemory(szProductName, sizeof(szProductName));
		ZeroMemory(szProductPrice, sizeof(szProductPrice));
		ZeroMemory(szProductType, sizeof(szProductType));
		ZeroMemory(szProductAddress, sizeof(szProductAddress));
		ZeroMemory(szProductDetail, sizeof(szProductDetail));

		ZeroMemory(szProductSetup, sizeof(szProductSetup));
		ZeroMemory(szProductSpec, sizeof(szProductSpec));
		ZeroMemory(szProductBatchno, sizeof(szProductBatchno));
		ZeroMemory(szProductModel, sizeof(szProductModel));
		ZeroMemory(szProductAmount, sizeof(szProductAmount));

		
		ZeroMemory(szProductUnit, sizeof(szProductUnit));
		ZeroMemory(szProductFactory, sizeof(szProductFactory));
		ZeroMemory(szProductBrand, sizeof(szProductBrand));
		ZeroMemory(szProductHGCode, sizeof(szProductHGCode));


		ZeroMemory(szTradeId, sizeof(szTradeId));
		ZeroMemory(szTradeName, sizeof(szTradeName));
		ZeroMemory(szMaterial, sizeof(szMaterial));
		ZeroMemory(szDateToDate, sizeof(szDateToDate));
		ZeroMemory(szAddDetail, sizeof(szDateToDate));
		ZeroMemory(szPhototUrl, sizeof(szPhototUrl));

		ZeroMemory(szCselFirName,sizeof(szCselFirName));
		ZeroMemory(szCselSecName,sizeof(szCselSecName));
		ZeroMemory(szCselThdName,sizeof(szCselThdName));
    }
};

//此结构中只能用基本数据类型,并且不能用指针
struct T_TaskDetailInfo
{
	DWORD   dwNnumber;                                 //发布编号
	TCHAR   szSubject[MAX_NAME_LENGTH];					//主题
	TCHAR   szContext[MAX_CONTEXT_LENGTH];					//正文

	TCHAR   szExpire[MAX_MID_LENGTH];						//有效期
	DWORD   dwIsEnglish;									//是否英文信息,1为英文,0为中文 
	TCHAR   szrKeyWords[MAX_NAME_LENGTH];					//发布关键字  (以<@@@>分隔)
	TCHAR   szPagePhotoFlag[MAX_NAME_LENGTH];								//抓取快照用到的标记；

	

    T_TaskDetailInfo()
    {
		dwNnumber = 0;
		dwIsEnglish = 0;

		ZeroMemory(szSubject, sizeof(szSubject));
		ZeroMemory(szExpire, sizeof(szExpire));
		ZeroMemory(szContext, sizeof(szContext));
		ZeroMemory(szrKeyWords, sizeof(szrKeyWords));
		ZeroMemory(szPagePhotoFlag,sizeof(szPagePhotoFlag));
    }
};


//此结构中只能用基本数据类型,并且不能用指针
struct T_TaskListData
{
	DWORD     dwTaskType;					//任务类型
	DWORD     dwPostTask;					//任务的发布类型
	TCHAR     szUrl[MAX_URL_LENGTH];				    //URL地址
	DWORD     dwTaskItemCount;              //子任务数

	T_TaskNetItem TaskItem[MAX_TASK_ITEM_COUNT];// 子任务数据的
	T_TaskDetailInfo TaskDetailInfo ;

    T_TaskListData()
    {
        dwTaskType = 0;   
        dwPostTask = 0;    
        dwTaskItemCount = 0;   

		ZeroMemory(szUrl, sizeof(szUrl));
    }

};




struct T_SubjectAndFenlei
{
	//
	DWORD dwHistoryID;      //历史任务ID
	TCHAR szSubject[MAX_SUBJECT_LENGTH];     //主题
	TCHAR szFenLeiName[MAX_NAME_LENGTH];  //分类名称

	T_SubjectAndFenlei()
	{
		dwHistoryID = 0;
		ZeroMemory(szSubject, sizeof(szSubject));
		ZeroMemory(szFenLeiName, sizeof(szFenLeiName));
	}


};

struct T_CompNameAndRegID
{
	TCHAR szCompanyName[MAX_NAME_LENGTH];     //公司名称
	TCHAR szRegID[MAX_NAME_LENGTH];  //注册号ID

	T_CompNameAndRegID()
	{
        ZeroMemory(szCompanyName, sizeof(szCompanyName));
		ZeroMemory(szRegID, sizeof(szRegID));
	}
};



struct T_GuiHistoryTaskCount
{ 
	DWORD dwHistoryID;      //历史任务ID
	DWORD dwTaskType;      //任务类型
	DWORD dwCount;      //任务数量

    T_GuiHistoryTaskCount()
    {
        dwHistoryID = 0;
        dwTaskType = 0;
        dwCount = 0;
    }
};

struct T_UserInfo
{
	TCHAR szUserId[USER_ID_COUNT];            //用户id
	TCHAR szXing[MAX_LOW_LENGTH];			 //中文姓
	TCHAR szMing[MAX_LOW_LENGTH];               //中文名
	TCHAR szEName[MAX_ID_STRING_LENGTH];         //用户英文姓名

	TCHAR szSex[MAX_LOW_LENGTH];			     //性别
	TCHAR szPersonNumber[MAX_ID_STRING_LENGTH];   //身份证号码

	TCHAR szCountry[MAX_ID_STRING_LENGTH];                           //国家
	TCHAR szProvin[MAX_ID_STRING_LENGTH];                           //省份
	TCHAR szCity[MAX_ID_STRING_LENGTH];                           //城市
	TCHAR szArea[MAX_ID_STRING_LENGTH];                           //区、县
	TCHAR szZip[MAX_ID_STRING_LENGTH];                           //邮编

	TCHAR szYear[MAX_LOW_LENGTH];				 //出生年
	TCHAR szMonth[MAX_LOW_LENGTH];			   //出生月
	TCHAR szDay[MAX_LOW_LENGTH];			  //出生日

	TCHAR szMail[MAX_EMAIL_NAME_LENGTH];                           //公司邮箱   //域名推广也用到
	TCHAR szICode[MAX_LOW_LENGTH];                               //国际区号
	TCHAR szACode[MAX_LOW_LENGTH];                               //国内区号
	TCHAR szPhone[MAX_MID_LENGTH];                           //联系电话

	TCHAR szQQ[MAX_MID_LENGTH];                           //联系qq
	TCHAR szFax[MAX_MID_LENGTH];                           //传真
	TCHAR szMobile[MAX_MID_LENGTH];                       //手机号码
	TCHAR szMsn[MAX_MID_LENGTH];                       //MSN
	TCHAR szJob[MAX_MID_LENGTH];                       //职位

	TCHAR szDept[MAX_MID_LENGTH];                       //部门
	TCHAR szCountNumber[MAX_MID_LENGTH];                       //国家区域编号
	TCHAR szProvNumber[MAX_MID_LENGTH];                       //省份区域编号
	TCHAR szCitNumber[MAX_MID_LENGTH];                       //城市区域编号
	TCHAR szAreaNumber[MAX_MID_LENGTH];						//县级区域编号

	TCHAR szMailBak[MAX_EMAIL_NAME_LENGTH];					//备用邮箱

	T_UserInfo()
	{
		ZeroMemory(szUserId, sizeof(szUserId));
		ZeroMemory(szXing, sizeof(szXing));
		ZeroMemory(szMing, sizeof(szMing));
		ZeroMemory(szEName, sizeof(szEName));

		ZeroMemory(szSex, sizeof(szSex));
		ZeroMemory(szPersonNumber, sizeof(szPersonNumber));

		ZeroMemory(szCountry, sizeof(szCountry));
		ZeroMemory(szProvin, sizeof(szProvin));
		ZeroMemory(szCity, sizeof(szCity));
		ZeroMemory(szArea, sizeof(szArea));
		ZeroMemory(szZip, sizeof(szZip));

		ZeroMemory(szYear, sizeof(szYear));
		ZeroMemory(szMonth, sizeof(szMonth));
		ZeroMemory(szDay, sizeof(szDay));

		ZeroMemory(szMail, sizeof(szMail));
		ZeroMemory(szICode, sizeof(szICode));
		ZeroMemory(szACode, sizeof(szACode));
		ZeroMemory(szPhone, sizeof(szPhone));

		ZeroMemory(szQQ, sizeof(szQQ));
		ZeroMemory(szFax, sizeof(szFax));
		ZeroMemory(szMobile, sizeof(szMobile));
		ZeroMemory(szMsn, sizeof(szMsn));
		ZeroMemory(szJob, sizeof(szJob));

		ZeroMemory(szDept, sizeof(szDept));
		ZeroMemory(szCountNumber, sizeof(szCountNumber));
		ZeroMemory(szProvNumber, sizeof(szProvNumber));
		ZeroMemory(szCitNumber, sizeof(szCitNumber));
		ZeroMemory(szAreaNumber, sizeof(szAreaNumber));

		ZeroMemory(szMailBak, sizeof(szMailBak));
	}
};


//GUI ACTIVEX 传入的用户的系统设置信息的结构,用来向类systemInfo赋值的信息
struct T_CompanyInfo
{
    TCHAR szCompanOwner[MAX_MID_LENGTH];                           //公司联系人
	TCHAR szCompanName[MAX_MID_LENGTH];                           //公司名称
	TCHAR szCompanShortName[MAX_NAME_LENGTH];                           //公司简称
	TCHAR szCompanEName[MAX_MID_LENGTH];                           //公司英文名称

	TCHAR szCompanURL[MAX_URL_LENGTH];                           //公司网址    域名推广时也用到
	TCHAR szAddress[MAX_NAME_LENGTH];                           //地址
	TCHAR szScale[MAX_MID_LENGTH];                           //公司规模
	TCHAR szDate[MAX_MID_LENGTH];                           //公司注册时间
	TCHAR szProperty[MAX_MID_LENGTH];                           //企业性质

	TCHAR szTrade[MAX_MID_LENGTH];                           //公司所在行业
	TCHAR szFund[MAX_MID_LENGTH];                           //公司注册资金
    TCHAR szCCINFO[MAX_SUBJECT_LENGTH];					//  公司简介

	TCHAR szBrand[MAX_NAME_LENGTH];					//  公司品牌
	TCHAR szIdea[MAX_NAME_LENGTH];					//  经营理念
	TCHAR szSlogan[MAX_NAME_LENGTH];					//  公司口号
	TCHAR szMarket[MAX_NAME_LENGTH];					//  主要市场
	TCHAR szTrait[MAX_NAME_LENGTH];					//  公司特点

	TCHAR szBrankPersonName[MAX_MID_LENGTH];					//  银行开户名
	TCHAR szBussisNumber[MAX_MID_LENGTH];					//  工商注册号
	TCHAR szBrankName[MAX_MID_LENGTH];					//  开户银行
	TCHAR szBrankNumber[MAX_MID_LENGTH];					//  企业银行账号
	TCHAR szTradeURL[MAX_URL_LENGTH];					//  营业执照url
	TCHAR szPdt[MAX_NAME_LENGTH];					//  公司品牌
	TCHAR szCompImageURL[MAX_URL_LENGTH];					//  公司照片url

	TCHAR szTradeStartDate[MAX_ID_STRING_LENGTH];					//  营业执照起始日期
	TCHAR szTradeEndDate[MAX_ID_STRING_LENGTH];					//  营业执照结束日期


	TCHAR szICPBackup[MAX_NAME_LENGTH];					//  网站ICP备案  域名推广用到
	TCHAR szCertReg[MAX_MID_LENGTH];					//  证书登记机关

	TCHAR szOrgCodeURL[MAX_URL_LENGTH];					//组织代码证书url;
	TCHAR szTaxRegURL[MAX_URL_LENGTH];					//税务登记证url

    T_CompanyInfo()
    {
		ZeroMemory(szCompanOwner, sizeof(szCompanOwner));
		ZeroMemory(szCompanName, sizeof(szCompanName));
		ZeroMemory(szCompanShortName, sizeof(szCompanShortName));
		ZeroMemory(szCompanEName, sizeof(szCompanEName));
	
		ZeroMemory(szCompanURL, sizeof(szCompanURL));
		ZeroMemory(szAddress, sizeof(szAddress));
		ZeroMemory(szScale, sizeof(szScale));
		ZeroMemory(szDate, sizeof(szDate));
		ZeroMemory(szProperty, sizeof(szProperty));

		ZeroMemory(szTrade, sizeof(szTrade));
		ZeroMemory(szFund, sizeof(szFund));
		ZeroMemory(szCCINFO, sizeof(szCCINFO));

		ZeroMemory(szBrand, sizeof(szBrand));
		ZeroMemory(szIdea, sizeof(szIdea));
		ZeroMemory(szSlogan, sizeof(szSlogan));
		ZeroMemory(szMarket, sizeof(szMarket));
		ZeroMemory(szTrait, sizeof(szTrait));

		ZeroMemory(szBrankPersonName, sizeof(szBrankPersonName));
		ZeroMemory(szBussisNumber, sizeof(szBussisNumber));
		ZeroMemory(szBrankName, sizeof(szBrankName));
		ZeroMemory(szBrankNumber, sizeof(szBrankNumber));
		ZeroMemory(szTradeURL, sizeof(szTradeURL));
		ZeroMemory(szPdt, sizeof(szPdt));
		ZeroMemory(szCompImageURL, sizeof(szCompImageURL));

		ZeroMemory(szTradeStartDate, sizeof(szTradeStartDate));
		ZeroMemory(szTradeEndDate, sizeof(szTradeEndDate));


		ZeroMemory(szICPBackup, sizeof(szICPBackup));
		ZeroMemory(szCertReg, sizeof(szCertReg));
		ZeroMemory(szOrgCodeURL, sizeof(szOrgCodeURL));
		ZeroMemory(szTaxRegURL, sizeof(szTaxRegURL));
    }
};

struct T_AllSystemInfo
{
	T_CompanyInfo systemInfo;
	T_UserInfo   userInfo;
};



//任务结果的结构(通过AcitveX返回给GUI),所有的任务都采用同一个结构,用数组的方式多个结果返回给ActiveX层
struct T_GUIAllTaskItemResult
{
    TCHAR szNscode[15];				//网站编号
    DWORD dwNetItemID;				//网站栏目编号
    TCHAR szNetItemName[101];			//网站栏目名称
    DWORD dwResult;				//任务结果  // 任务结果的枚举,999 为成功,1000超时
    DWORD dwDriverTaskItemCount;	//派生出来的任务数

	T_GUIAllTaskItemResult()
	{
		dwNetItemID = 0; 
		dwResult = 0; 
		dwDriverTaskItemCount = 0; 

        ZeroMemory(szNscode, sizeof(szNscode));
		ZeroMemory(szNetItemName, sizeof(szNetItemName)); 
	}
};


//注册任务保存到数据库中的结构,函数传递数据,不要在大链表中存在
struct T_SavePostTaskResult
{
	CStdString szResult;		//任务结果
	CStdString szPType;			//任务Ptype
	CStdString szLocalFile;		//本地文件路径
	CStdString szSiteHost;		//网址
	CStdString szSiteName;		//网站名称
	CStdString szDisplayStyle;	//是否立显
	CStdString szUrlResult;		//Action
	CStdString szPstep;			//发布步骤 pstep
	CStdString szNetStationID;	//网站编号
	T_SavePostTaskResult()
	{
		szResult.Empty();
		szPType.Empty();
		szLocalFile.Empty();
		szSiteHost.Empty();
		szSiteName.Empty();
		szDisplayStyle.Empty();
		szUrlResult.Empty();
		szPstep.Empty();
		szNetStationID.Empty();
	}
	~T_SavePostTaskResult()
	{
		szResult.Empty();
		szPType.Empty();
		szLocalFile.Empty();
		szSiteHost.Empty();
		szSiteName.Empty();
		szDisplayStyle.Empty();
		szUrlResult.Empty();
		szPstep.Empty();
		szNetStationID.Empty();
	}

};

//网站栏目信息一条记录
//SELECT '(;1)' + cast(NSICode as varchar(15)) +'(;1)' + NSIparamater+'(;1)' +NSIName  as NSIResu  FROM NetStationItem where NSInscode=@intNetstationID AND NSInscode IN (SELECT NSCODE FROM NETSTATION WHERE NSCODE = @intNetstationID AND NSItemLastUpdate>@intLastUpdate)
struct T_NetstationItem
{
	DWORD dwNSICode;  
	TCHAR szNSIparamater[256];
	TCHAR szNSIName[256];

	T_NetstationItem()
	{
		dwNSICode = 0;
		ZeroMemory(szNSIparamater, sizeof(szNSIparamater));
		ZeroMemory(szNSIName, sizeof(szNSIName));
	}
};


//网站变量信息一条记录
//SELECT '(;1)' + cast(NSVtype as varchar(15))   + '(;1)' +cast(NSVCode as varchar(15))   + '(;1)'+cast(NSVrealvalue as varchar(300))   As NSVResu FROM NetStationVariable where NSVnscode=@intNetstationID AND NSVnscode IN (SELECT NSCODE FROM NETSTATION WHERE NSCODE = @intNetstationID AND NSVariableLastUpdate>@intLastUpdate)
struct T_NetstationVariable
{
	DWORD dwNSVCode;
	DWORD dwNSVtype;
	TCHAR szNSVrealvalue[256];
    
	T_NetstationVariable()
	{
		dwNSVCode = 0 ;
		dwNSVtype = 0 ;
		ZeroMemory(szNSVrealvalue, sizeof(szNSVrealvalue));
	}

};

//一个网站的所有栏目信息
struct T_NetstationItemGroup
{
    DWORD  dwNsCode;							//网站编号
    DWORD  dwLastUpdateTime;					//最后更新时间
    DWORD  dwNetstationItemCount;				//网站栏目数
    T_NetstationItem *pNetstationItem;	//网站栏目数据(这里根据网站栏目数动态分配) 此指针不需要在析构时释放

    T_NetstationItemGroup()
    {
        dwNsCode = 0;
        dwLastUpdateTime = 0;
        dwNetstationItemCount = 0;
        pNetstationItem = NULL;
    }

    
};

//一个网站的所有变量信息
struct T_NetstationVariableGroup
{
    DWORD  dwNsCode;							//网站编号
	DWORD  dwLastUpdateTime;					//最后更新时间
	DWORD  dwNetstationVariableCount;           //网站变量数
    T_NetstationVariable *pNetstationVariable;  //网站变量数据(这里根据网站变量数动态分配)(不需在析构释放)
    
    T_NetstationVariableGroup()
    {
        dwNsCode = 0;
        dwLastUpdateTime = 0;
        dwNetstationVariableCount = 0;
        pNetstationVariable = NULL;
    }
};


//一个网站的所有更新信息和任务成功失败的信息
struct T_NetstationNetStationInfo
{
    DWORD  dwNsCode;							//网站编号
    DWORD  dwLastUpdateTime;					//最后更新时间
    DWORD  dwItemLastUpdateTime;				//栏目最后更新时间
    DWORD  dwVariableLastUpdateTime;			//变量最后更新时间
    DWORD  dwNssuccedCount;						//成功任务的数量
    DWORD  dwNsFailCount;						//失败任务的数量

    T_NetstationNetStationInfo()
    {
        dwNsCode = -1;
        dwLastUpdateTime =0;
        dwItemLastUpdateTime =0; 
        dwVariableLastUpdateTime =0;
        dwNssuccedCount =0;
        dwNsFailCount =0;
    }
};


//一个网站的服务器的最后更新时间,所有更新信息
struct T_ServerNetStationInfo
{
        DWORD  dwNsCode;							//网站编号
        DWORD  dwLastUpdateTime;					//最后更新时间

        T_ServerNetStationInfo()
        {
            dwNsCode = -1;
            dwLastUpdateTime = -1;
        }
};



//一次从服务器获取的网站最后更新时间
struct T_ServerNetStationInfoGroup
{
        DWORD dwNetCount;                                                               //返回的网站数
        T_ServerNetStationInfo  NetStationInfoGroup[MAX_NET_COUNT_LASTUPDATED];     //网站的最后更新时间
        
        T_ServerNetStationInfoGroup()
        {
            dwNetCount = 0;
            ZeroMemory(NetStationInfoGroup, sizeof(NetStationInfoGroup));
        }

};



struct T_SaveToServerAccount
{
	DWORD dwOriginalClientID;          //帐号的所有者ID
	DWORD dwNetstationID;              //帐号所属的网站ID
    DWORD dwState ;                         //帐号的状态
    TCHAR szAccount[MAX_ACCOUNT_PASSWORD_LENGTH];	//用户网站帐号
	TCHAR szPassword[MAX_ACCOUNT_PASSWORD_LENGTH];	//用户网站密码
	TCHAR szConfirmMail[MAX_EMAIL_NAME_LENGTH];     //验证邮箱
	TCHAR szMailPassword[MAX_ACCOUNT_PASSWORD_LENGTH];     //验证邮箱密码
	DWORD dwCompanyId;					//公司ID 用于和八方资源网对接用的

    T_SaveToServerAccount()
    {
        dwOriginalClientID = 0;
        dwNetstationID = 0;
        dwState = 0;
		dwCompanyId = 0;

        ZeroMemory(szAccount, sizeof(szAccount));
		ZeroMemory(szPassword, sizeof(szPassword));
		ZeroMemory(szConfirmMail, sizeof(szConfirmMail));
		ZeroMemory(szMailPassword, sizeof(szMailPassword));
    }


};


//和FTP交互的数据组织类
struct T_FtpFileName
{
	DWORD dwFileID;					//文件的编号
	DWORD dwFileType;				//文件的类型编号
	DWORD dwReserve;				//保留的数据,例如用来判断是否是自动取栏目列表文件等的信息	
	TCHAR szLocalFileName[MAX_URL_LENGTH];	//本地文件名称(绝对)
	TCHAR szServerFileName[MAX_URL_LENGTH];	//服务器文件名称(相对)

	T_FtpFileName()
	{
		dwFileID = 0;
		dwFileType = 0;
		dwReserve = 0;

		ZeroMemory(szLocalFileName, sizeof(szLocalFileName));
		ZeroMemory(szServerFileName, sizeof(szServerFileName));
	}


};


//全局保存的当前任务的数据
struct T_LocalGlobalData
{
	DWORD dwCurrentMainControlVersion;//当前主控的版本
	DWORD dwWebMainTaskID; //web版本的任务ID

	T_AllSystemInfo   aSystemInfo;
	T_TaskProductInfo TaskProductInfo; //产品信息
	T_TaskDetailInfo TaskDetailInfo; //任务详细信息
	
	DWORD dwCurrentTaskItemCount;   //当前任务数,当前任务数目
	DWORD dwMigrationProductCount; //提取产品字段数量

	DWORD dwServerTickcount;           //服务器的tickcount
	DWORD dwStartTickcount;             // 和服务器的tickcount 相对应的本机tickcount
	DWORD dwServerMintus;                //和服务器的tickcount 相对应的服务器分钟数,用来判断用户是否升级已过期	

//	TCHAR szProductImageName[MAX_FILE_PATH_LENGTH];        //保存产品图片文件名
	TCHAR szTradeImageName[MAX_FILE_PATH_LENGTH];        //保存营业执照图片文件名
	TCHAR szCompImageName[MAX_FILE_PATH_LENGTH];        //保存照片图片文件名
	TCHAR szOrgImageName[MAX_FILE_PATH_LENGTH];			//保存组织机构代码图片文件名
	TCHAR szTaxImageName[MAX_FILE_PATH_LENGTH];			//税收登记证图片文件名

	TCHAR szMigrationCompanyField[MAX_PRODUCT_DETAIL_LENGTH];  //初始迁移公司字段；
	TCHAR szMigrationProductField[MAX_PRODUCT_DETAIL_LENGTH];  //初始迁移产品字段；
	TCHAR szMigrationProductNames[MAX_NET_COUNT_LASTUPDATED];  //要提取的产品标题；

	TCHAR szShopTrafficUrl[MAX_URL_LENGTH];	//刷新商铺流量地址；
	TCHAR szMac[MAX_ID_STRING_LENGTH];		//电脑的Mac地址
	DWORD dwPdtClickCnt; //产品点击数；
	DWORD dwOtherClickCnt; //其他页点击数；

	TCHAR szTimerTaskID[MAX_ID_STRING_LENGTH];      //定时任务ID
	DWORD dwTaskType;                               //任务类型（发布、注册等）
	TCHAR szUserID[MAX_ID_STRING_LENGTH];           //用户ID

	DWORD   dwRetry;                                        //验证码重试次数
	DWORD   dwShow;											//是否显示验证码  1显示  0不显示
	DWORD   dwShowTime;                                         //验证码显示时间
	DWORD   dwInputTime;									//验证码输入时间
	DWORD	dwOCRType;											//验证码识别类型：0表示不识别  1表示识别

	int     iMode;

	BOOL    bIsUpdate;              //是否更新网站dat文件
	BOOL    bIsYunTask;             //是否云任务

	std::vector<CStdString> vCompany;   //公司名
	std::vector<CStdString> vProductImageName;  ////保存产品图片文件名

	T_LocalGlobalData()
	{
        dwCurrentMainControlVersion = 0;//当前主控的版本
		dwWebMainTaskID = 0; //web版本的任务ID
        dwCurrentTaskItemCount = 0;   //当前任务数,当前任务数目
		dwMigrationProductCount = 1;

        dwServerTickcount = 0;           //服务器的tickcount
        dwStartTickcount = 0;             // 和服务器的tickcount 相对应的本机tickcount
        dwServerMintus = 0;                //和服务器的tickcount 相对应的服务器分钟数,用来判断用户是否升级已过期
        dwTaskType = 0;                  //任务的类型（发布、注册等）		

		dwRetry = 3;
		dwShow = 1;
		dwShowTime = 15;
		dwInputTime = 15;
		dwOCRType = 0;
		iMode = 0;

		dwPdtClickCnt = 0;
		dwOtherClickCnt = 0;

		bIsUpdate = TRUE;
		bIsYunTask = FALSE;
		vCompany.clear();
		vProductImageName.clear();

//		ZeroMemory(szProductImageName, sizeof(szProductImageName));
		ZeroMemory(szTradeImageName, sizeof(szTradeImageName));
		ZeroMemory(szCompImageName, sizeof(szCompImageName));
		ZeroMemory(szTimerTaskID, sizeof(szTimerTaskID));
		ZeroMemory(szMigrationProductField,sizeof(szMigrationProductField));
		ZeroMemory(szMigrationCompanyField,sizeof(szMigrationCompanyField));
		ZeroMemory(szMigrationProductNames,sizeof(szMigrationProductNames));
		ZeroMemory(szShopTrafficUrl, sizeof(szShopTrafficUrl));
		ZeroMemory(szOrgImageName, sizeof(szOrgImageName));
		ZeroMemory(szTaxImageName, sizeof(szTaxImageName));
		ZeroMemory(szUserID, sizeof(szUserID));
		ZeroMemory(szMac, sizeof(szMac));
	}
	
};

#endif