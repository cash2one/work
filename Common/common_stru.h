#ifndef __COMMON_STRUCT_H__
#define __COMMON_STRUCT_H__

#include "common_enum.h"
#include "common_define.h"
#include "StdString.h"
#include "common_stru_exserver.h"
#include "SiteInfo.h"



const DWORD DATABASE_VALID = 0;       //���ݿ��������ʹ��

//GUIѹ������������
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

//�˽ṹ��ֻ���û�����������,���Ҳ�����ָ��
struct T_TaskNetItem
{
	DWORD   dwWebId;							//��վ���
	UINT64   dwNetStationItemID;					//��վ��Ŀ���

	TCHAR   szWebName[MAX_MID_LENGTH];         //��վ����
	TCHAR   szWebItemName[MAX_MID_LENGTH];         //��վ��Ŀ����

	std::vector<CStdString>   vInfoID;                           //��ϢID;
	int		dwTaskId;							//����ID

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


//��������
struct T_TaskData
{
    DWORD dwIndex;//�����ڶ��ı��
    DWORD dwWebID;//��վ���

	DWORD dwTotalStep;            //������һ����Ҫִ�еĲ������
	DWORD dwCurStep;              //�Ѿ�ִ���˶��ٲ���

    E_TASK_TYPE eTaskType;//���������(ע��,����,������)
	DWORD     dwPostType;
	E_Task_Level eTaskLevel;	//����ִ�е����ȼ���Ŀǰ��ʱ����������������ʱʹ�ã�
	
	CStdString szAccount;	//�û���վ�ʺ�
	CStdString szPassword;	//�û���վ����

	CStdString szConfirmMail; //���伤���õ��ʺţ�
	CStdString szMailPassword; //���伤���õ����룻

	CStdString strHost;
	CStdString strOCRParam;	//��֤�����

	CStdString strNotUpdateWeb;  //����Ҫ���µ�Ƕ����վ��

	T_TaskNetItem tTaskNetItem;
	T_Task_Data_Exser tTaskDataExser; //����׼����Ҫ����������
	T_TaskResultInfo  tTaskResultInfo;	//ExSer��������

	T_Site_Type  siteType;          //��վ���͡�һЩ����Ҫ��
	T_Limit      limitRefresh;      //��վˢ��Ҫ��

	std::vector<DWORD>   vRefreshIndex;     //��վˢ��ʱ  ˢ�µ�����
	BOOL    bIsRefreshAll;
	BOOL	bUpdateDataFlag;   //�Ƿ���Ҫ������վģ��

	BOOL    bIsSendExtra;         //����ʱ������������ʱ���Ƿ����߷��������ϲ���
	BOOL    bIsAddExtra;        //��ǰ�����Ƿ�����������

	BOOL    bIsSendPic;         //����ʱ��������ͼƬʱ���Ƿ����߷�����ͼƬ����
	BOOL	bIsAddPic;          //��ǰ�����Ƿ�������ͼƬ

	BOOL    bGetLogin;			//�Ƿ���Ҫ��ȡ��¼����
	BOOL	bAddExtraIsSucc;    //���������Ƿ�ɹ���

	T_TaskData()
	{
		dwIndex = 0;
		dwWebID = 0;
		dwTotalStep = 0;
		dwCurStep = 0;

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


//P2P���صĲ�Ʒ��Ϣ�Ľṹ
struct T_TaskProductInfo
{
	DWORD dwProductID;   //��ƷID
	
	TCHAR szProductName[MAX_NAME_LENGTH];     //��Ʒ����
	TCHAR szProductPrice[MAX_PRODUCT_NORMAL_LENGTH];     //��Ʒ�۸�
	TCHAR szProductType[MAX_PRODUCT_NORMAL_LENGTH];			//��Ʒ����
	TCHAR szProductAddress[MAX_NAME_LENGTH];				//������ַ
	
	TCHAR szProductDetail[MAX_PRODUCT_DETAIL_LENGTH];		//��Ʒ��ϸ��Ϣ
	TCHAR szProductSetup[MAX_PRODUCT_NORMAL_LENGTH];		//��Ʒ����
	TCHAR szProductSpec[MAX_PRODUCT_NORMAL_LENGTH];			//��Ʒ���

	TCHAR szProductBatchno[MAX_PRODUCT_NORMAL_LENGTH];		//��Ʒ����
	TCHAR szProductModel[MAX_PRODUCT_NORMAL_LENGTH];		//��Ʒ�ͺ�
	TCHAR szProductAmount[MAX_PRODUCT_NORMAL_LENGTH];		//��Ʒ����

	TCHAR szProductUnit[MAX_PRODUCT_NORMAL_LENGTH];		//��Ʒ������λ
	TCHAR szProductFactory[MAX_PRODUCT_NORMAL_LENGTH];				//��������
	TCHAR szProductBrand[MAX_PRODUCT_NORMAL_LENGTH];		//����Ʒ��
	TCHAR szProductHGCode[MAX_PRODUCT_NORMAL_LENGTH];		//��Ʒ������
	
	TCHAR szTradeId[MAX_ID_STRING_LENGTH];		//��Ʒ��Ŀid
	TCHAR szTradeName[MAX_MID_LENGTH];		//��Ʒ��Ŀ����
	TCHAR szMaterial[MAX_MID_LENGTH];		//����
	TCHAR szDateToDate[MAX_MID_LENGTH];		//������
	TCHAR szAddDetail[MAX_NAME_LENGTH];		//����˵��
	TCHAR szPhototUrl[MAX_URL_LENGTH];		//��ƷͼƬ���ص�ַ

	TCHAR szCselFirName[MAX_MID_LENGTH];      //һ����������Ŀ
	TCHAR szCselSecName[MAX_MID_LENGTH];		//������������Ŀ
	TCHAR szCselThdName[MAX_MID_LENGTH];		//������������Ŀ
	

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

//�˽ṹ��ֻ���û�����������,���Ҳ�����ָ��
struct T_TaskDetailInfo
{
	DWORD   dwNnumber;                                 //�������
	TCHAR   szSubject[MAX_NAME_LENGTH];					//����
	TCHAR   szContext[MAX_CONTEXT_LENGTH];					//����

	TCHAR   szExpire[MAX_MID_LENGTH];						//��Ч��
	DWORD   dwIsEnglish;									//�Ƿ�Ӣ����Ϣ,1ΪӢ��,0Ϊ���� 
	TCHAR   szrKeyWords[MAX_NAME_LENGTH];					//�����ؼ���  (��<@@@>�ָ�)
	TCHAR   szPagePhotoFlag[MAX_NAME_LENGTH];								//ץȡ�����õ��ı�ǣ�

	

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


//�˽ṹ��ֻ���û�����������,���Ҳ�����ָ��
struct T_TaskListData
{
	DWORD     dwTaskType;					//��������
	DWORD     dwPostTask;					//����ķ�������
	TCHAR     szUrl[MAX_URL_LENGTH];				    //URL��ַ
	DWORD     dwTaskItemCount;              //��������

	T_TaskNetItem TaskItem[MAX_TASK_ITEM_COUNT];// ���������ݵ�
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
	DWORD dwHistoryID;      //��ʷ����ID
	TCHAR szSubject[MAX_SUBJECT_LENGTH];     //����
	TCHAR szFenLeiName[MAX_NAME_LENGTH];  //��������

	T_SubjectAndFenlei()
	{
		dwHistoryID = 0;
		ZeroMemory(szSubject, sizeof(szSubject));
		ZeroMemory(szFenLeiName, sizeof(szFenLeiName));
	}


};

struct T_CompNameAndRegID
{
	TCHAR szCompanyName[MAX_NAME_LENGTH];     //��˾����
	TCHAR szRegID[MAX_NAME_LENGTH];  //ע���ID

	T_CompNameAndRegID()
	{
        ZeroMemory(szCompanyName, sizeof(szCompanyName));
		ZeroMemory(szRegID, sizeof(szRegID));
	}
};



struct T_GuiHistoryTaskCount
{ 
	DWORD dwHistoryID;      //��ʷ����ID
	DWORD dwTaskType;      //��������
	DWORD dwCount;      //��������

    T_GuiHistoryTaskCount()
    {
        dwHistoryID = 0;
        dwTaskType = 0;
        dwCount = 0;
    }
};

struct T_UserInfo
{
	TCHAR szUserId[USER_ID_COUNT];            //�û�id
	TCHAR szXing[MAX_LOW_LENGTH];			 //������
	TCHAR szMing[MAX_LOW_LENGTH];               //������
	TCHAR szEName[MAX_ID_STRING_LENGTH];         //�û�Ӣ������

	TCHAR szSex[MAX_LOW_LENGTH];			     //�Ա�
	TCHAR szPersonNumber[MAX_ID_STRING_LENGTH];   //���֤����

	TCHAR szCountry[MAX_ID_STRING_LENGTH];                           //����
	TCHAR szProvin[MAX_ID_STRING_LENGTH];                           //ʡ��
	TCHAR szCity[MAX_ID_STRING_LENGTH];                           //����
	TCHAR szArea[MAX_ID_STRING_LENGTH];                           //������
	TCHAR szZip[MAX_ID_STRING_LENGTH];                           //�ʱ�

	TCHAR szYear[MAX_LOW_LENGTH];				 //������
	TCHAR szMonth[MAX_LOW_LENGTH];			   //������
	TCHAR szDay[MAX_LOW_LENGTH];			  //������

	TCHAR szMail[MAX_EMAIL_NAME_LENGTH];                           //��˾����   //�����ƹ�Ҳ�õ�
	TCHAR szICode[MAX_LOW_LENGTH];                               //��������
	TCHAR szACode[MAX_LOW_LENGTH];                               //��������
	TCHAR szPhone[MAX_MID_LENGTH];                           //��ϵ�绰

	TCHAR szQQ[MAX_MID_LENGTH];                           //��ϵqq
	TCHAR szFax[MAX_MID_LENGTH];                           //����
	TCHAR szMobile[MAX_MID_LENGTH];                       //�ֻ�����
	TCHAR szMsn[MAX_MID_LENGTH];                       //MSN
	TCHAR szJob[MAX_MID_LENGTH];                       //ְλ

	TCHAR szDept[MAX_MID_LENGTH];                       //����
	TCHAR szCountNumber[MAX_MID_LENGTH];                       //����������
	TCHAR szProvNumber[MAX_MID_LENGTH];                       //ʡ��������
	TCHAR szCitNumber[MAX_MID_LENGTH];                       //����������
	TCHAR szAreaNumber[MAX_MID_LENGTH];						//�ؼ�������

	TCHAR szMailBak[MAX_EMAIL_NAME_LENGTH];					//��������

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


//GUI ACTIVEX ������û���ϵͳ������Ϣ�Ľṹ,��������systemInfo��ֵ����Ϣ
struct T_CompanyInfo
{
    TCHAR szCompanOwner[MAX_MID_LENGTH];                           //��˾��ϵ��
	TCHAR szCompanName[MAX_MID_LENGTH];                           //��˾����
	TCHAR szCompanShortName[MAX_NAME_LENGTH];                           //��˾���
	TCHAR szCompanEName[MAX_MID_LENGTH];                           //��˾Ӣ������

	TCHAR szCompanURL[MAX_URL_LENGTH];                           //��˾��ַ    �����ƹ�ʱҲ�õ�
	TCHAR szAddress[MAX_NAME_LENGTH];                           //��ַ
	TCHAR szScale[MAX_MID_LENGTH];                           //��˾��ģ
	TCHAR szDate[MAX_MID_LENGTH];                           //��˾ע��ʱ��
	TCHAR szProperty[MAX_MID_LENGTH];                           //��ҵ����

	TCHAR szTrade[MAX_MID_LENGTH];                           //��˾������ҵ
	TCHAR szFund[MAX_MID_LENGTH];                           //��˾ע���ʽ�
    TCHAR szCCINFO[MAX_SUBJECT_LENGTH];					//  ��˾���

	TCHAR szBrand[MAX_NAME_LENGTH];					//  ��˾Ʒ��
	TCHAR szIdea[MAX_NAME_LENGTH];					//  ��Ӫ����
	TCHAR szSlogan[MAX_NAME_LENGTH];					//  ��˾�ں�
	TCHAR szMarket[MAX_NAME_LENGTH];					//  ��Ҫ�г�
	TCHAR szTrait[MAX_NAME_LENGTH];					//  ��˾�ص�

	TCHAR szBrankPersonName[MAX_MID_LENGTH];					//  ���п�����
	TCHAR szBussisNumber[MAX_MID_LENGTH];					//  ����ע���
	TCHAR szBrankName[MAX_MID_LENGTH];					//  ��������
	TCHAR szBrankNumber[MAX_MID_LENGTH];					//  ��ҵ�����˺�
	TCHAR szTradeURL[MAX_URL_LENGTH];					//  Ӫҵִ��url
	TCHAR szPdt[MAX_NAME_LENGTH];					//  ��˾Ʒ��
	TCHAR szCompImageURL[MAX_URL_LENGTH];					//  ��˾��Ƭurl

	TCHAR szTradeStartDate[MAX_ID_STRING_LENGTH];					//  Ӫҵִ����ʼ����
	TCHAR szTradeEndDate[MAX_ID_STRING_LENGTH];					//  Ӫҵִ�ս�������


	TCHAR szICPBackup[MAX_NAME_LENGTH];					//  ��վICP����  �����ƹ��õ�
	TCHAR szCertReg[MAX_MID_LENGTH];					//  ֤��Ǽǻ���

	TCHAR szOrgCodeURL[MAX_URL_LENGTH];					//��֯����֤��url;
	TCHAR szTaxRegURL[MAX_URL_LENGTH];					//˰��Ǽ�֤url

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



//�������Ľṹ(ͨ��AcitveX���ظ�GUI),���е����񶼲���ͬһ���ṹ,������ķ�ʽ���������ظ�ActiveX��
struct T_GUIAllTaskItemResult
{
    TCHAR szNscode[15];				//��վ���
    DWORD dwNetItemID;				//��վ��Ŀ���
    TCHAR szNetItemName[101];			//��վ��Ŀ����
    DWORD dwResult;				//������  // ��������ö��,999 Ϊ�ɹ�,1000��ʱ
    DWORD dwDriverTaskItemCount;	//����������������

	T_GUIAllTaskItemResult()
	{
		dwNetItemID = 0; 
		dwResult = 0; 
		dwDriverTaskItemCount = 0; 

        ZeroMemory(szNscode, sizeof(szNscode));
		ZeroMemory(szNetItemName, sizeof(szNetItemName)); 
	}
};


//ע�����񱣴浽���ݿ��еĽṹ,������������,��Ҫ�ڴ������д���
struct T_SavePostTaskResult
{
	CStdString szResult;		//������
	CStdString szPType;			//����Ptype
	CStdString szLocalFile;		//�����ļ�·��
	CStdString szSiteHost;		//��ַ
	CStdString szSiteName;		//��վ����
	CStdString szDisplayStyle;	//�Ƿ�����
	CStdString szUrlResult;		//Action
	CStdString szPstep;			//�������� pstep
	CStdString szNetStationID;	//��վ���
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

//��վ��Ŀ��Ϣһ����¼
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


//��վ������Ϣһ����¼
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

//һ����վ��������Ŀ��Ϣ
struct T_NetstationItemGroup
{
    DWORD  dwNsCode;							//��վ���
    DWORD  dwLastUpdateTime;					//������ʱ��
    DWORD  dwNetstationItemCount;				//��վ��Ŀ��
    T_NetstationItem *pNetstationItem;	//��վ��Ŀ����(���������վ��Ŀ����̬����) ��ָ�벻��Ҫ������ʱ�ͷ�

    T_NetstationItemGroup()
    {
        dwNsCode = 0;
        dwLastUpdateTime = 0;
        dwNetstationItemCount = 0;
        pNetstationItem = NULL;
    }

    
};

//һ����վ�����б�����Ϣ
struct T_NetstationVariableGroup
{
    DWORD  dwNsCode;							//��վ���
	DWORD  dwLastUpdateTime;					//������ʱ��
	DWORD  dwNetstationVariableCount;           //��վ������
    T_NetstationVariable *pNetstationVariable;  //��վ��������(���������վ��������̬����)(�����������ͷ�)
    
    T_NetstationVariableGroup()
    {
        dwNsCode = 0;
        dwLastUpdateTime = 0;
        dwNetstationVariableCount = 0;
        pNetstationVariable = NULL;
    }
};


//һ����վ�����и�����Ϣ������ɹ�ʧ�ܵ���Ϣ
struct T_NetstationNetStationInfo
{
    DWORD  dwNsCode;							//��վ���
    DWORD  dwLastUpdateTime;					//������ʱ��
    DWORD  dwItemLastUpdateTime;				//��Ŀ������ʱ��
    DWORD  dwVariableLastUpdateTime;			//����������ʱ��
    DWORD  dwNssuccedCount;						//�ɹ����������
    DWORD  dwNsFailCount;						//ʧ�����������

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


//һ����վ�ķ�������������ʱ��,���и�����Ϣ
struct T_ServerNetStationInfo
{
        DWORD  dwNsCode;							//��վ���
        DWORD  dwLastUpdateTime;					//������ʱ��

        T_ServerNetStationInfo()
        {
            dwNsCode = -1;
            dwLastUpdateTime = -1;
        }
};



//һ�δӷ�������ȡ����վ������ʱ��
struct T_ServerNetStationInfoGroup
{
        DWORD dwNetCount;                                                               //���ص���վ��
        T_ServerNetStationInfo  NetStationInfoGroup[MAX_NET_COUNT_LASTUPDATED];     //��վ��������ʱ��
        
        T_ServerNetStationInfoGroup()
        {
            dwNetCount = 0;
            ZeroMemory(NetStationInfoGroup, sizeof(NetStationInfoGroup));
        }

};



struct T_SaveToServerAccount
{
	DWORD dwOriginalClientID;          //�ʺŵ�������ID
	DWORD dwNetstationID;              //�ʺ���������վID
    DWORD dwState ;                         //�ʺŵ�״̬
    TCHAR szAccount[MAX_ACCOUNT_PASSWORD_LENGTH];	//�û���վ�ʺ�
	TCHAR szPassword[MAX_ACCOUNT_PASSWORD_LENGTH];	//�û���վ����
	TCHAR szConfirmMail[MAX_EMAIL_NAME_LENGTH];     //��֤����
	TCHAR szMailPassword[MAX_ACCOUNT_PASSWORD_LENGTH];     //��֤��������

    T_SaveToServerAccount()
    {
        dwOriginalClientID = 0;
        dwNetstationID = 0;
        dwState = 0;

        ZeroMemory(szAccount, sizeof(szAccount));
		ZeroMemory(szPassword, sizeof(szPassword));
		ZeroMemory(szConfirmMail, sizeof(szConfirmMail));
		ZeroMemory(szMailPassword, sizeof(szMailPassword));
    }


};


//��FTP������������֯��
struct T_FtpFileName
{
	DWORD dwFileID;					//�ļ��ı��
	DWORD dwFileType;				//�ļ������ͱ��
	DWORD dwReserve;				//����������,���������ж��Ƿ����Զ�ȡ��Ŀ�б��ļ��ȵ���Ϣ	
	TCHAR szLocalFileName[MAX_URL_LENGTH];	//�����ļ�����(����)
	TCHAR szServerFileName[MAX_URL_LENGTH];	//�������ļ�����(���)

	T_FtpFileName()
	{
		dwFileID = 0;
		dwFileType = 0;
		dwReserve = 0;

		ZeroMemory(szLocalFileName, sizeof(szLocalFileName));
		ZeroMemory(szServerFileName, sizeof(szServerFileName));
	}


};


//ȫ�ֱ���ĵ�ǰ���������
struct T_LocalGlobalData
{
	DWORD dwCurrentMainControlVersion;//��ǰ���صİ汾
	DWORD dwWebMainTaskID; //web�汾������ID

	T_AllSystemInfo   aSystemInfo;
	T_TaskProductInfo TaskProductInfo; //��Ʒ��Ϣ
	T_TaskDetailInfo TaskDetailInfo; //������ϸ��Ϣ
	
	DWORD dwCurrentTaskItemCount;   //��ǰ������,��ǰ������Ŀ
	DWORD dwMigrationProductCount; //��ȡ��Ʒ�ֶ�����

	DWORD dwServerTickcount;           //��������tickcount
	DWORD dwStartTickcount;             // �ͷ�������tickcount ���Ӧ�ı���tickcount
	DWORD dwServerMintus;                //�ͷ�������tickcount ���Ӧ�ķ�����������,�����ж��û��Ƿ������ѹ���	

//	TCHAR szProductImageName[MAX_FILE_PATH_LENGTH];        //�����ƷͼƬ�ļ���
	TCHAR szTradeImageName[MAX_FILE_PATH_LENGTH];        //����Ӫҵִ��ͼƬ�ļ���
	TCHAR szCompImageName[MAX_FILE_PATH_LENGTH];        //������ƬͼƬ�ļ���
	TCHAR szOrgImageName[MAX_FILE_PATH_LENGTH];			//������֯��������ͼƬ�ļ���
	TCHAR szTaxImageName[MAX_FILE_PATH_LENGTH];			//˰�յǼ�֤ͼƬ�ļ���

	TCHAR szMigrationCompanyField[MAX_PRODUCT_DETAIL_LENGTH];  //��ʼǨ�ƹ�˾�ֶΣ�
	TCHAR szMigrationProductField[MAX_PRODUCT_DETAIL_LENGTH];  //��ʼǨ�Ʋ�Ʒ�ֶΣ�
	TCHAR szMigrationProductNames[MAX_NET_COUNT_LASTUPDATED];  //Ҫ��ȡ�Ĳ�Ʒ���⣻

	TCHAR szShopTrafficUrl[MAX_URL_LENGTH];	//ˢ������������ַ��
	DWORD dwPdtClickCnt; //��Ʒ�������
	DWORD dwOtherClickCnt; //����ҳ�������

	TCHAR szTimerTaskID[MAX_ID_STRING_LENGTH];      //��ʱ����ID
	DWORD dwTaskType;                               //�������ͣ�������ע��ȣ�
	DWORD dwPostType;                            //����ʱ������������

	DWORD   dwRetry;                                        //��֤�����Դ���
	DWORD   dwShow;											//�Ƿ���ʾ��֤��  1��ʾ  0����ʾ
	DWORD   dwShowTime;                                         //��֤����ʾʱ��
	DWORD   dwInputTime;									//��֤������ʱ��
	DWORD	dwOCRType;											//��֤��ʶ�����ͣ�0��ʾ��ʶ��  1��ʾʶ��

	int     iMode;

	BOOL    bIsUpdate;              //�Ƿ������վdat�ļ�
	BOOL    bIsYunTask;             //�Ƿ�������

	std::vector<CStdString> vCompany;   //��˾��
	std::vector<CStdString> vProductImageName;  ////�����ƷͼƬ�ļ���

	T_LocalGlobalData()
	{
        dwCurrentMainControlVersion = 0;//��ǰ���صİ汾
		dwWebMainTaskID = 0; //web�汾������ID
        dwCurrentTaskItemCount = 0;   //��ǰ������,��ǰ������Ŀ
		dwMigrationProductCount = 1;

        dwServerTickcount = 0;           //��������tickcount
        dwStartTickcount = 0;             // �ͷ�������tickcount ���Ӧ�ı���tickcount
        dwServerMintus = 0;                //�ͷ�������tickcount ���Ӧ�ķ�����������,�����ж��û��Ƿ������ѹ���
        dwTaskType = 0;                  //��������ͣ�������ע��ȣ�
		dwPostType = 0;                  //����ʱ������������

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
	}
	
};

#endif