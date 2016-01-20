#include "stdafx.h"
#include "Station.h"
#include "stringutils.h"

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
    CStdString szDelimiter;
	CStdString szDelimiter1;
    CStdString szTemp;
	CStdString szTempItem;
    //	TCHAR TempTch[200];
    std::vector<CStdString> results;
    std::vector<CStdString> resultItem;
    szDelimiter=_T("(;0)");
    szDelimiter1=_T("(;1)");
    StringUtils StringUtil ;
    int nCount = StringUtil.SplitString(strData,szDelimiter,results,true)	;
    int intindex;
    if (2 >=nCount ) return 0;
    for(int i=2;i<nCount;i++)
    {		
        szTemp=results[i];
        resultItem.clear () ;
        ItemCount = StringUtil.SplitString(szTemp, szDelimiter1,resultItem,true)	;
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
	szDelimiter.Empty();
	szDelimiter1.Empty();
	szTemp.Empty();
	szTempItem.Empty();
	results.clear();
	resultItem.clear();

    return 1;
}


int CStation::GetItemFromString(const int iNscode,const CStdString strNscode,const CStdString strData,T_NetstationItemGroup &NetItem)
{

    //T_NetstationItemGroup *pNet = NULL;
    //_NEW(pNet,T_NetstationItemGroup);

    NetItem.dwNsCode =iNscode ;

    //NetItem.dwLastUpdateTime;  //需要从map中获取

    int ItemCount;
	DWORD dwCount = 0;
	CStdString szDelimiter;
	CStdString szDelimiter1;
	CStdString szTemp;
	CStdString szTempItem;
    TCHAR TempTch[300];
    std::vector<CStdString> results;
    std::vector<CStdString> resultItem;
    szDelimiter=_T("(;0)");
    szDelimiter1=_T("(;1)");
    StringUtils StringUtil ;
    int nSplitCount = StringUtil.SplitString(strData,szDelimiter,results,true);

	T_NetstationItem *pTemp = NULL;

    if (nSplitCount <2) 
    {
        NetItem.dwNetstationItemCount = 0;    
        return 0;
    }
    else
    {
		//容错,判断哪一个小用哪一个
		if ((nSplitCount - 1) > NetItem.dwNetstationItemCount)
		{
			dwCount = NetItem.dwNetstationItemCount;
		}
		else
		{
			dwCount = nSplitCount - 1;
		}

        if (NULL == NetItem.pNetstationItem)
        {
            g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,\
                _T("pNetStationItem pointer is NULL"));
            return 0;
        }

        pTemp = NetItem.pNetstationItem;

        for(int i=0;i<dwCount;i++)
        {		
            szTemp=results[i+2];
            resultItem.clear () ;
            ItemCount = StringUtil.SplitString(szTemp, szDelimiter1,resultItem,true)	;
			
			if (ItemCount > 0)
			{
				_tcscpy(TempTch , resultItem[1]);
	            
				_tcscpy(pTemp->szNSIparamater , resultItem[2]);
	            
				_tcscpy(pTemp->szNSIName , resultItem[3]);
				pTemp->dwNSICode = _ttol(TempTch);
			};

			pTemp++;
        }   

    }

	szDelimiter.Empty();
	szDelimiter1.Empty();
	szTemp.Empty();
	szTempItem.Empty();
	results.clear();
	resultItem.clear();

    return 1;
}


int CStation::GetAllStationLastUpdateALL(const CStdString strData,T_ServerNetStationInfoGroup &ServerLastupdateGroup)
{
    int ItemCount;
	CStdString szDelimiter;
	CStdString szDelimiter1;
	CStdString szTemp;

    TCHAR TempTch[30];
    std::vector<CStdString> results;
    std::vector<CStdString> resultItem;
    szDelimiter=_T("(;0)");
    szDelimiter1=_T("(;1)");
    StringUtils StringUtil ;
    int nCount = StringUtil.SplitString(strData,szDelimiter,results,true);
    int intindex;

    if (nCount < 2 ) 
    {
        ServerLastupdateGroup.dwNetCount = 0;    
        return 0;
    }
    else
    {

        ServerLastupdateGroup.dwNetCount =nCount -1 ; 

        for(intindex=0;intindex<nCount-1;intindex++)
        {		
            szTemp=results[intindex+2];
            resultItem.clear () ;
            ItemCount = StringUtil.SplitString(szTemp, szDelimiter1,resultItem,true)	;
			//增加容错处理
			if (ItemCount<3)
			{
				continue;
			}
            _tcscpy(TempTch , resultItem[0]);
            ServerLastupdateGroup.NetStationInfoGroup[intindex].dwNsCode = _ttol(TempTch);

            _tcscpy(TempTch , resultItem[1]);
            ServerLastupdateGroup.NetStationInfoGroup[intindex].dwLastUpdateTime = _ttol(TempTch);

            _tcscpy(TempTch , resultItem[2]);
            ServerLastupdateGroup.NetStationInfoGroup[intindex].dwItemLastUpdateTime = _ttol(TempTch);

            _tcscpy(TempTch , resultItem[3]);
            ServerLastupdateGroup.NetStationInfoGroup[intindex].dwVariableLastUpdateTime = _ttol(TempTch);
        }   

    }

	szDelimiter.Empty();
	szDelimiter1.Empty();
	szTemp.Empty();

	results.clear();
	resultItem.clear();

    return 1;
}
