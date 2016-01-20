#include "stdafx.h"
#include "Station.h"
#include "..\common\stringutils.h"

CStation::CStation(void)
{
	nsDisplay = "2";
}

CStation::~CStation(void)
{
}
int CStation::GetDataFromString(CStdString strData)
{
	
	int ItemCount;
	CStdString szDelimiter,szDelimiter1;
	CStdString szTemp,szTempItem;
	std::vector<CStdString> results;
	std::vector<CStdString> resultItem;
	szDelimiter=_T("(;0)");
	szDelimiter1=_T("(;1)");
	StringUtils StrUtils ;
	int nCount = StrUtils.SplitString(strData,szDelimiter,results,true)	;
	int intindex;
	if (2 >=nCount ) return 0;
	for(int i=2;i<nCount;i++)
	{		
		szTemp=results[i];
		resultItem.clear () ;
		ItemCount = StrUtils.SplitString(szTemp, szDelimiter1,resultItem,true)	;
		// szTempItem = resultItem[1];
		intindex = i-2;
		//  0,,[NScode]
		//	1,,"      ,[NSname]"
		//	2,,"      ,[NSrstep]"
		//	3,,"      ,[NSraction]"
		//	4,,"      ,[NSrconst]"
		//	5,,"      ,[NSrkeyword]"
		//	6,,"      ,[NSrreffer]"
		//	7,,"      ,[NSrsucc]"
		//	8,,"      ,[NSpstep]"
		//	9,,"      ,[NSpaction]"
		//	10,,"      ,[NSpconst]"
		//	11,,"      ,[NSpkeyword]"
		//	12,,"      ,[NSpreffer]"
		//	13,,"      ,[NSpsucc]"
		//	14,,"      ,[NSdstep]"
		//	15,,"      ,[NSdaction]"
		//	16,,"      ,[NSdconst]"
		//	17,,"      ,[NSdreffer]"
		//	18,,"      ,[nsMemo]"
		//	19,,"      ,[NSsstep]"
		//	20,,"      ,[NSsaction]"
		//	21,,"      ,[NSsconst]"
		//	22,,"      ,[NSskeyword]"
		//	23,,"      ,[NSsreffer]"
		//	24,,"      ,[NSssucc]"
		//	25,,"      ,[nsDetailMark]"
		//	26,,"      ,[nsWebFilter]"
		//	27,,"      ,[nsValidUrlMark]"
		//	28,,"      ,[nsCompanyFiter]"
		//	29,,"      ,[nsNextPageMark]"
		//	30,,"      ,[NShost]"
		//	31,,"      ,[nsAdvanceSearchSymbol]"
		//	32,,"      ,[NSPublishDate]"
		//	33,,"      ,[NSClass]"
		//	34,,"      ,[NSAccount]"
		//	35,,"      ,[NSPass]"
		//	36,,"      ,[NSrFlag]"
		//	37,,"      ,[NSpFlag]"
		//	38,,"      ,[NSsFlag]"
		//	39,,"      ,[NSeFlag]"
		//	40,,"      ,[IsDemo]"
		//	41,,"      ,[nsEaction]"
		//	42,,"      ,[nsEconst]"
		//	43,,"      ,[nsEkeyword]"
		//	44,,"      ,[nsEreffer]"
		//	45,,"      ,[nsEstep]"
		//	46,,"      ,[nsEsucc]"
		//	47,,"      ,[nsERefreshDate]"
		//	48,,"      ,[nsLFlag]"
		//	49,,"      ,[nsLAction]"
		//	50,,"      ,[nsLConst]"
		//	51,,"      ,[nsLKeyword]"
		//	52,,"      ,[nsLreffer]"
		//	53,,"      ,[nsLstep]"
		//	54,,"      ,[nsLsucc]"
		//	55,,"      ,[nsLRefreshDate]"
		//	56,,"      ,[nsDeepDetailMark]"
		//	57,,"      ,[nsPRefreshDate]"
		//	58,,"      ,[nsSRefreshDate]"
		//	59,,"      ,[nsRRefreshDate]"
		//	60,,"      ,[nsPRePostKey]"
		//	61,,"      ,[nsRRePostKey]"
		//	62,,"      ,[nsoperator]"
		//	63,,"      ,[RegWebPath]"
		//	64,,"      ,[NsOnlyDown]"
		//	65,,"      ,[nsDisplay]"
		//	66,,"      ,[nstype]"
		//	67,,"      ,[nsonlypost]"
		//	68,,"      ,[nsarea]"
		//	69,,"      ,[nsScriptExplain]"
		//	70,,"      ,[nsWebRedirect]"
		//	71,,"      ,[nsMainHost]"
		//	72,,"      ,[nsLMRefreshDate]"
		//	73,,"      ,[nsBLRefreshDate]"
		//	74,,"      ,[nsNNTFileName]"
		//	75,,"      ,[nsImageProcessCode]"
		//	76,,"      ,[NsNeedFitLanmu]"
		//	77,,"      ,[nsTeleZone]"
		//	78,,"      ,[nsImageProcessRate]"
		//	79,,"      ,[nsIsValidForDemoOrTrial]"
		//	80,,"      ,[nsBaseInfoLastUpdate]"
		//	81,,"      ,[nsVariableLastUpdate]"
		//	82,,"      ,[nsItemLastUpdate]"
		//	83,,"      ,[nsVerifycodeLastUpdate]"
		//	84,,"      ,[nsEnabledUpdatedInvalid]"
		//	85,,"      ,[nsRowLastupdated]"
		//	86,,"      ,[nsRowLastupdatedBigint]"
		//	87,,"      ,[nsNetArea]"
		//	88,,"      ,[nsLMRefreshDateTemp]"
		//	89,,"      ,[nsBLRefreshDateTemp]"
		//	90,,"      ,[nsRowLastupdatedBigintTemp]"

		switch(intindex) {
			 case 0 : // 
					NScode = resultItem[1];
					break;
			 case 1 : // 
					NSname = resultItem[1];
					break;
			 case 2 : // 
					NSrstep = resultItem[1];
					break;
			 case 3 : // 
					NSraction = resultItem[1];
					break;
			 case 4 : // 
					NSrconst = resultItem[1];
					break;
			 case 5 : // 
					NSrkeyword = resultItem[1];
					break;
			 case 6 : // 
					NSrreffer = resultItem[1];
					break;
			 case 7 : // 
					NSrsucc = resultItem[1];
					break;
			 case 8 : // 
					NSpstep = resultItem[1];
					break;
			 case 9 : // 
					NSpaction = resultItem[1];
					break;
			 case 10 : // 
					NSpconst = resultItem[1];
					break;
			 case 11 : // 
					NSpkeyword = resultItem[1];
					break;
			 case 12 : // 
					NSpreffer = resultItem[1];
					break;
			 case 13 : // 
					NSpsucc = resultItem[1];
					break;
			 case 14 : // 
					NSdstep = resultItem[1];
					break;
			 case 15 : // 
					NSdaction = resultItem[1];
					break;
			 case 16 : // 
					NSdconst = resultItem[1];
					break;
			 case 17 : // 
					NSdreffer = resultItem[1];
					break;
			 case 18 : // 
					nsMemo = resultItem[1];
					break;
			 case 19 : // 
					NSsstep = resultItem[1];
					break;
			 case 20 : // 
					NSsaction = resultItem[1];
					break;
			 case 21 : // 
					NSsconst = resultItem[1];
					break;
			 case 22 : // 
					NSskeyword = resultItem[1];
					break;
			 case 23 : // 
					NSsreffer = resultItem[1];
					break;
			 case 24 : // 
					NSssucc = resultItem[1];
					break;
			 case 25 : // 
					nsDetailMark = resultItem[1];
					break;
			 case 26 : // 
					nsWebFilter = resultItem[1];
					break;
			 case 27 : // 
					nsValidUrlMark = resultItem[1];
					break;
			 case 28 : // 
					nsCompanyFiter = resultItem[1];
					break;
			 case 29 : // 
					nsNextPageMark = resultItem[1];
					break;
			 case 30 : // 
					NShost = resultItem[1];
					break;
			 case 31 : // 
					nsAdvanceSearchSymbol = resultItem[1];
					break;
			 case 32 : // 
					NSPublishDate = resultItem[1];
					break;
			 case 33 : // 
					NSClass = _ttol(resultItem[1]);
					break;
			 case 34 : // 
					//NSAccount = resultItem[1];
					break;
			 case 35 : // 
					//NSPass = resultItem[1];
					break;
			 case 36 : // 
					//NSrFlag = resultItem[1];
					break;
			 case 37 : // 
					//NSpFlag = resultItem[1];
					break;
			 case 38 : // 
					//NSsFlag = resultItem[1];
					break;
			 case 39 : // 
					//NSeFlag = resultItem[1];
					break;
			 case 40 : // 
					IsDemo = resultItem[1];
					break;
			 case 41 : // 
					nsEaction = resultItem[1];
					break;
			 case 42 : // 
					nsEconst = resultItem[1];
					break;
			 case 43 : // 
					nsEkeyword = resultItem[1];
					break;
			 case 44 : // 
					nsEreffer = resultItem[1];
					break;
			 case 45 : // 
					nsEstep = resultItem[1];
					break;
			 case 46 : // 
					nsEsucc = resultItem[1];
					break;
			 case 47 : // 
					nsERefreshDate = resultItem[1];
					break;
			 case 48 : // 
					//nsLFlag = resultItem[1];
					break;
			 case 49 : // 
					nsLAction = resultItem[1];
					break;
			 case 50 : // 
					nsLConst = resultItem[1];
					break;
			 case 51 : // 
					nsLKeyword = resultItem[1];
					break;
			 case 52 : // 
					nsLreffer = resultItem[1];
					break;
			 case 53 : // 
					nsLstep = resultItem[1];
					break;
			 case 54 : // 
					nsLsucc = resultItem[1];
					break;
			 case 55 : // 
					nsLRefreshDate = resultItem[1];
					break;
			 case 56 : // 
					nsDeepDetailMark = resultItem[1];
					break;
			 case 57 : // 
					nsPRefreshDate = resultItem[1];
					break;
			 case 58 : // 
					nsSRefreshDate = resultItem[1];
					break;
			 case 59 : // 
					nsRRefreshDate = resultItem[1];
					break;
			 case 60 : // 
					nsPRePostKey = resultItem[1];
					break;
			 case 61 : // 
					nsRRePostKey = resultItem[1];
					break;
			 case 62 : // 
					nsoperator = resultItem[1];
					break;
			 case 63 : // 
					RegWebPath = resultItem[1];
					break;
			 case 64 : // 
					NsOnlyDown = resultItem[1];
					break;
			 case 65 : // 
					nsDisplay = resultItem[1];
					break;
			 case 66 : // 
					nstype = resultItem[1];
					break;
			 case 67 : // 
					nsonlypost = resultItem[1];
					break;
			 case 68 : // 
					nsarea = resultItem[1];
					break;
			 case 69 : // 
					nsScriptExplain = resultItem[1];
					break;
			 case 70 : // 
					nsWebRedirect = resultItem[1];
					break;
			 case 71 : // 
					nsMainHost = resultItem[1];
					break;
			 case 72 : // 
					nsLMRefreshDate = resultItem[1];
					break;
			 case 73 : // 
					nsBLRefreshDate = resultItem[1];
					break;
			 case 74 : // 
					nsNNTFileName = resultItem[1];
					break;
			 case 75 : // 
					nsImageProcessCode = resultItem[1];
					break;
			 case 76 : // 
					NsNeedFitLanmu = resultItem[1];
					break;
			 case 77 : // 
					nsTeleZone = resultItem[1];
					break;
			 case 78 : // 
					nsImageProcessRate = resultItem[1];
					break;
			 case 79 : // 
					nsIsValidForDemoOrTrial = resultItem[1];
					break;
			 case 80 : // 
					nsBaseInfoLastUpdate = resultItem[1];
					break;
			 case 81 : // 
					nsVariableLastUpdate = resultItem[1];
					break;
			 case 82 : // 
					nsItemLastUpdate = resultItem[1];
					break;
			 case 83 : // 
					//nsVerifycodeLastUpdate = resultItem[1];
					break;
			 case 84 : // 
					//nsEnabledUpdatedInvalid = resultItem[1];
					break;
			 case 85 : // 
					//nsRowLastupdated = resultItem[1];
					break;
			 case 86 : // 
					//nsRowLastupdatedBigint = resultItem[1];
					break;
			 case 87 : // 
					nsNetArea = resultItem[1];
					break;
			 case 88 : // 
					//nsLMRefreshDateTemp = resultItem[1];
					break;
			 case 89 : // 
					//nsBLRefreshDateTemp = resultItem[1];
					break;
			 case 90 : // 
					//nsRowLastupdatedBigintTemp = resultItem[1];
					break;

			 default:
					break;
		}

		
		
	}

	return 1;
}


int CStation::GetBaseInfoFromString(const CStdString strNscode,const CStdString strData,T_NetstationBaseInfo * &pNetBase)
{
        
	GetDataFromString (strData);
        
        
    pNetBase->NScode = strNscode;	 //网站编号
    pNetBase->NSname = NSname;	 //网站名称
    pNetBase->NSrstep = NSrstep;	 //注册步骤
    pNetBase->NSraction = NSraction;	 //注册参数
    pNetBase->NSrconst = NSrconst;	 //
    pNetBase->NSrkeyword = NSrkeyword;	 //
    pNetBase->NSrreffer = NSrreffer;	 //
    pNetBase->NSrsucc = NSrsucc;	 //成功标记
    pNetBase->NSpstep = NSpstep;	 //发布步骤
    pNetBase->NSpaction = NSpaction;	 //发布参数
    pNetBase->NSpconst = NSpconst;	 //
    pNetBase->NSpkeyword = NSpkeyword;	 //
    pNetBase->NSpreffer = NSpreffer;	 //
    pNetBase->NSpsucc = NSpsucc;	 //成功标记
    pNetBase->NSdstep = NSdstep;	 //下载步骤
    pNetBase->NSdaction = NSdaction;	 //下载参数
    pNetBase->NSdconst = NSdconst;	 //												
    pNetBase->NSdreffer = NSdreffer;	 //												
    pNetBase->nsMemo = nsMemo;	 //												
    pNetBase->NSsstep = NSsstep;	 //搜索步骤												
    pNetBase->NSsaction = NSsaction;	 //搜索参数												
    pNetBase->NSsconst = NSsconst;	 //												
    pNetBase->NSskeyword = NSskeyword;	 //												
    pNetBase->NSsreffer = NSsreffer;	 //												
    pNetBase->NSssucc = NSssucc;	 //成功标记												
    pNetBase->nsDetailMark = nsDetailMark;	 //详细信息网址标记												
    pNetBase->nsWebFilter = nsWebFilter;	 //网页头尾过滤标记												
    pNetBase->nsValidUrlMark = nsValidUrlMark;	 //网址过滤参数												
    pNetBase->nsCompanyFiter = nsCompanyFiter;	 //附加信息过滤标记: 信息组分隔<@@	主题<@@	正文<@@	公司<@@	邮件<@@	电话<@@	传真<@@	区域<@@	地址<@@	联系人<@@	简介<@@	网址<@@	行业
    pNetBase->nsNextPageMark = nsNextPageMark;	 //下一页标记												
    pNetBase->NShost = NShost;	 //主机地址												
    pNetBase->nsAdvanceSearchSymbol = nsAdvanceSearchSymbol;	 //搜索转义符号												
    pNetBase->NSPublishDate = NSPublishDate;	 //
    pNetBase->NSClass = NSClass;	 //网站评级
    pNetBase->IsDemo = IsDemo;	 // 是否为演示版本网站
    pNetBase->nsEaction = nsEaction;	 //ID邮件参数
    pNetBase->nsEconst = nsEconst;	 //
    pNetBase->nsEkeyword = nsEkeyword;	 //
    pNetBase->nsEreffer = nsEreffer;	 //
    pNetBase->nsEstep = nsEstep;	 //ID邮件步骤
    pNetBase->nsEsucc = nsEsucc;	 //成功标记
    pNetBase->nsERefreshDate = nsERefreshDate;	 //
    pNetBase->nsLAction = nsLAction;	 //
    pNetBase->nsLConst = nsLConst;	 //
    pNetBase->nsLKeyword = nsLKeyword;	 //
    pNetBase->nsLreffer = nsLreffer;	 //
    pNetBase->nsLstep = nsLstep;	 //
    pNetBase->nsLsucc = nsLsucc;	 //登陆成功关键字
    pNetBase->nsLRefreshDate = nsLRefreshDate;	 //
    pNetBase->nsDeepDetailMark = nsDeepDetailMark;	 //szRelateUrlFilter
    pNetBase->nsPRefreshDate = nsPRefreshDate;	 //
    pNetBase->nsSRefreshDate = nsSRefreshDate;	 //
    pNetBase->nsRRefreshDate = nsRRefreshDate;	 //
    pNetBase->nsPRePostKey = nsPRePostKey;	 //重复发布关键字
    pNetBase->nsRRePostKey = nsRRePostKey;	 //重复注册关键字
    pNetBase->nsoperator = nsoperator;	 //
    pNetBase->RegWebPath = RegWebPath;	 //
    pNetBase->NsOnlyDown = NsOnlyDown;	 //仅搜索
    pNetBase->nsDisplay = nsDisplay;	 //是否立显标记
    pNetBase->nstype = nstype;	 //发布类型
    pNetBase->nsonlypost = nsonlypost;	 //仅发布
    pNetBase->nsarea = nsarea;	 //
    pNetBase->nsScriptExplain = nsScriptExplain;	 //ScriptUrl特征字符串
    pNetBase->nsWebRedirect = nsWebRedirect;	 //页面重新定向
    pNetBase->nsMainHost = nsMainHost;	 //主网站地址
    pNetBase->nsLMRefreshDate = nsLMRefreshDate;	 //
    pNetBase->nsBLRefreshDate = nsBLRefreshDate;	 //
    pNetBase->nsNNTFileName = nsNNTFileName;	 //模板文件名称
    pNetBase->nsImageProcessCode = nsImageProcessCode;	 //图像预处理方式
    pNetBase->NsNeedFitLanmu = NsNeedFitLanmu;	 //
    pNetBase->nsTeleZone = nsTeleZone;	 //
    pNetBase->nsImageProcessRate = nsImageProcessRate;	 //
    pNetBase->nsIsValidForDemoOrTrial = nsIsValidForDemoOrTrial;	 //
    pNetBase->nsBaseInfoLastUpdate = nsBaseInfoLastUpdate;	 //保存的网站基本信息的最后更新时间
    pNetBase->nsVariableLastUpdate = nsVariableLastUpdate;	 //保存的网站变量的最后更新时间
    pNetBase->nsItemLastUpdate = nsItemLastUpdate;	 //保存的网站栏目的最后更新时间
    pNetBase->nsEnabledUpdatedInvalid = nsEnabledUpdatedInvalid;	 //
    pNetBase->nsRowLastupdated = nsRowLastupdated;	 //
    pNetBase->nsRowLastupdatedBigint = nsRowLastupdatedBigint;	 //
    pNetBase->nsNetArea = nsNetArea;	 //所属区域
    pNetBase->nsLMRefreshDateTemp = nsLMRefreshDateTemp;	 //
    pNetBase->nsBLRefreshDateTemp = nsBLRefreshDateTemp;	 //
    pNetBase->nsRowLastupdatedBigintTemp = nsRowLastupdatedBigintTemp;	 //
    pNetBase->szFixCookies                 = szFixCookies;                	// String                   //固定的，只在指定步骤才使用的cookies
    pNetBase->ProgressData                 = ProgressData;                	// String                   //新增，记录session数据
    pNetBase->blnLocalDataIsNew            = blnLocalDataIsNew;           	// Boolean                  //true表示本地数据已经是最新的
    pNetBase->PExeNumber                    = PExeNumber;                   	                     //已存在的任务数(仅发布时使用)

    return 1;
}



int CStation::GetVariableFromString(const int iNscode,const CStdString strNscode,const CStdString strData,T_NetstationVariableGroup *&pNet)
{
    pNet->dwNsCode =iNscode ;
    //pNet->dwLastUpdateTime;  //需要从map中获取
    int ItemCount;

	CStdString szDelimiter,szDelimiter1;
	CStdString szTemp,szTempItem;
	TCHAR TempTch[MAX_ID_STRING_LENGTH];
	std::vector<CStdString> results;
	std::vector<CStdString> resultItem;
	szDelimiter=_T("(;0)");
	szDelimiter1=_T("(;1)");
	StringUtils StrUtils ;
	int nCount = StrUtils.SplitString(strData,szDelimiter,results,true);
	int intindex;

	if (2 >=nCount ) 
        {
            pNet->dwNetstationVariableCount = 0;    
            return 0;
        }
        else
        {
	        
            pNet->dwNetstationVariableCount =nCount - 2 ; 
            
            T_NetstationVariable *pTemp = NULL;
            
            pTemp = pNet->pNetstationVariable ;
            
            if (NULL == pTemp) 
            {
                //容错
                return 0;
            }
            for(int i=2;i<nCount;i++)
	        {		
		        szTemp=results[i];
		        resultItem.clear () ;
		        ItemCount = StrUtils.SplitString(szTemp, szDelimiter1,resultItem,true)	;

                        intindex = i-2;
	                _tcscpy(TempTch , resultItem[0]);
                        pTemp[intindex].dwNSVtype = _ttol(TempTch);

	                _tcscpy(TempTch , resultItem[1]);
                        pTemp[intindex].dwNSVCode = _ttol(TempTch);

	                _tcscpy(pTemp[intindex].szNSVrealvalue, resultItem[2]);
            }   
            
        }

        return 1;
}


int CStation::GetItemFromString(const int iNscode,const CStdString strNscode,const CStdString strData,T_NetstationItemGroup * &pNet)
{
        
    pNet->dwNsCode =iNscode ;
    
    //pNet->dwLastUpdateTime;  //需要从map中获取
    
    int ItemCount;

	CStdString szDelimiter,szDelimiter1;
	CStdString szTemp,szTempItem;
	TCHAR TempTch[MAX_ID_STRING_LENGTH];
	std::vector<CStdString> results;
	std::vector<CStdString> resultItem;
	szDelimiter=_T("(;0)");
	szDelimiter1=_T("(;1)");
	StringUtils StrUtils ;
	int nCount = StrUtils.SplitString(strData,szDelimiter,results,true);
	int intindex;

	if (2 >=nCount ) 
        {
            pNet->dwNetstationItemCount = 0;    
                return 0;
        }
        else
        {
	        
            pNet->dwNetstationItemCount =nCount -2 ; 
            T_NetstationItem *pTemp = NULL;

            pTemp = pNet->pNetstationItem ;

            if (NULL == pTemp) 
            {
                //容错
                return 0;
            }
                for(int i=2;i<nCount;i++)
	        {		
		        szTemp=results[i];
		        resultItem.clear () ;
		        ItemCount = StrUtils.SplitString(szTemp, szDelimiter1,resultItem,true)	;

                        intindex = i-2;
	                _tcscpy(TempTch , resultItem[0]);
	                _tcscpy(pTemp[intindex].szNSIparamater , resultItem[1]);
	                _tcscpy(pTemp[intindex].szNSIName , resultItem[2]);
                        pTemp[intindex].dwNSICode = _ttol(TempTch);
                }   
                
        }



        return 1;
}


int CStation::GetAllStationLastUpdateALL(const CStdString strData,T_ServerNetStationInfoGroup *ServerLastupdateGroup)
{
    int ItemCount;
	CStdString szDelimiter,szDelimiter1;
	CStdString szTemp,szTempItem;
	TCHAR TempTch[MAX_ID_STRING_LENGTH];
	std::vector<CStdString> results;
	std::vector<CStdString> resultItem;
	szDelimiter=_T("(;0)");
	szDelimiter1=_T("(;1)");
	StringUtils StrUtils ;
	int nCount = StrUtils.SplitString(strData,szDelimiter,results,true);
	int intindex;

	if (2 >=nCount ) 
        {
            ServerLastupdateGroup->dwNetCount = 0;    
                return 0;
        }
        else
        {
	        
                ServerLastupdateGroup->dwNetCount =nCount -2 ; 

                for(int i=2;i<nCount;i++)
	        {		
		        szTemp=results[i];
		        resultItem.clear () ;
		        ItemCount = StrUtils.SplitString(szTemp, szDelimiter1,resultItem,true)	;

                        intindex = i-2;
	                _tcscpy(TempTch , resultItem[0]);
                        ServerLastupdateGroup->NetStationInfoGroup[intindex].dwNsCode = _ttol(TempTch);

	                _tcscpy(TempTch , resultItem[1]);
                        ServerLastupdateGroup->NetStationInfoGroup[intindex].dwLastUpdateTime = _ttol(TempTch);

	                _tcscpy(TempTch , resultItem[2]);
                        ServerLastupdateGroup->NetStationInfoGroup[intindex].dwItemLastUpdateTime = _ttol(TempTch);

	                _tcscpy(TempTch , resultItem[3]);
                        ServerLastupdateGroup->NetStationInfoGroup[intindex].dwVariableLastUpdateTime = _ttol(TempTch);
                }   
                
        }



        return 1;
}
