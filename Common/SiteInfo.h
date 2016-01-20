#pragma once

#include <vector>
#include "EnDeCode.h"
#include "StdString.h"
#include "enumdef.h"
/************************************************************************/
/*

�ļ�˵��������վ��Ϣ������ ������վ���ͣ���վ���Ƶ�

��վ����������

1����������
���η�������
��ѷ���������
�������ʱ��
��������Ϣλ��


2��ˢ������
//ˢ�����ͣ��ɵ�ˢ����ȫѡ���ɶ�ѡ
����ˢ������
���ˢ������
ˢ��֮����Ϣλ�ã��ײ���β��������
//��ҳ������(�����������)


3��ɾ������
/..

���ߣ����Ƿ�
�������ڣ�6��25��
�汾��1.0
*/
/************************************************************************/


//////////////////////////////////////////////////////////////////////////
/////////////////��վ����///////////////////////////////////////////////
enum E_NewPos
{
	npStart ,  //�ײ�
	npEnd ,  //β��
	npFixed ,  //����
};

/////////////////IE����///////////////////////////////////////////////
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
	DWORD dwSinglePostCount ;  //��Ʒÿ��ɷ�������
	DWORD dwFreePostCount ;  //�ɷ���������
	DWORD dwPostIntervalTime ;  //���η�����Сʱ����
	E_NewPos eNewPosPost ;  //��������Ϣλ��
	E_IeLimit eIeEdtion;   //IE�汾

	DWORD dwSingleRefreshCount ;  //ÿ���ˢ�´���
	DWORD dwFreeRefreshCount ;  //���ˢ��������
	E_NewPos eNewPosRefresh ;  //ˢ�º���Ϣλ��

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
/////////////////��վ����///////////////////////////////////////////////


//��վ����
enum E_Site_Language
{
	slSimplified ,  //����
	slTraditional ,  //����
	slEnglish ,      //Ӣ��
};

//�˺š����볤������
enum E_Length 
{
	alSix ,
	alEight ,
	alTen ,
	alTwelve ,
};

//�˺�״̬
enum E_Account_State
{
	asEmailConfirm ,
	asInstant ,
	asNeedVerify ,
	asEmailRecieveAfterVerify ,
	asPhoneVerify ,
};

//�˺ű�����ʽ
enum E_Account_Style
{
	asNOT_INCLUDE_CHINESE = 0,  //����
	asFIRST_CHAR,    //���ַ���ͷ
	asFIRST_DIGITAL, // �����ֿ�ͷ
	asFIRST_CHINESE, //�Ժ��ֿ�ͷ
	asONLY_CHARACTER,//����ĸcharacter
	asFIRST_VALUE_UNKNOWN,//ʼ�շ������
};

//���������ʽ
enum E_Password_Style
{
	psNO_INCLUDE_CHINESE,  //�������(����������)
	psALL_UPPER_CHART,     //����д�ַ�
	psALL_LOWER_CHART,    //��Сд�ַ�
	psALL_DIGITAL,        //������
	psALL_CHINESE,      //������
	psUPPER_CHART_AND_DIGITAL,  //��д�ַ�������
	psLOWER_CHART_AND_DIGITAL,  //Сд�ַ�������
	psUPPER_CHART_AND_CHINESE,  //��д�ַ��ͺ���
	psLOWER_CHART_AND_CHINESE,  //Сд�ַ��ͺ���
	psDIGITAL_AND_CHINESE,    //  ���ֺͺ���
	psALL_VALUE_UNKNOWN,//ʼ�շ������
};

//ˢ������
enum E_Refresh_Type
{
	rtSingle ,
	rtAll ,
	rtMutiple ,
};

//��Ϣ��������ʾ��ʽ
enum E_Display_AfterPost
{
	daInstant ,   //����
	daAfterCheck ,  //��˺���ʾ
};

//֧�ֲ�ƷͼƬ����
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

//�Ƿ�֧��ע��
enum E_Need_Register
{
	nrNotNeed = 0 ,  
	nrNeedRegister ,
	nrRegisterEmail,
};

//��վ����
struct T_Site_Type
{
	E_Site_Language eSiteLanguage ;  //��վ����
	E_Length eAccountLen ; //�˺ų���
	E_Length ePasswordLen ;  //���볤��
	E_Account_State eAccountState ; //�˺�״̬
	E_Account_Style eAccountStyle ;  //�˺���ʽ
	E_Password_Style ePasswordStyle ; //������ʽ
	E_Refresh_Type eRefreshType ; //ˢ������
	E_Display_AfterPost eDisAfterPost ; //��������Ϣ��ʾ��ʽ
	BOOL bIsSupportStore ;  //�Ƿ�֧�ֿ�����
	BOOL bIsSupportBL ;   //�Ƿ�֧��Ӫҵִ��
	E_ProductPic_Count eProductPicCount ;  //֧�ֲ�ƷͼƬ����
	E_Need_Register eNeedRegister ;  //�Ƿ���Ҫע��
	BOOL bKeywordQuery;//�Ƿ�֧�ֹؼ���������ѯ
	BOOL bManualCode;//�Ƿ�֧���˹�����
	BOOL bCloudSchedule;//�Ƶ���
	BOOL bSelectAll;//�Ƿ�֧��ȫ��ҵƥ��
	BOOL bIsObject;//�ǲ���Object��վ

	//����Ĭ��ֵ
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
/////////////////��������///////////////////////////////////////////////
struct T_Post_Type : public CEnDeCode
{
	BOOL bProductPost ;  //��Ʒ����
	BOOL bSupplyPost ;  //��Ӧ����
	BOOL bPurchasePost ;  //�ɹ�����
	BOOL bSupportRefresh ; //֧��ˢ��
	BOOL bDomainPromote ; //֧�������ƹ�
	BOOL bSupportDelete ; //֧��ɾ��
	BOOL bCompany;        //��˾֪����
	BOOL bEmailQuery;     //�����ѯ
	BOOL bMicroBlog;      //΢������
	BOOL bOnceRemove;     //һ�����
	BOOL bChangeData;     //�޸�����
	BOOL bExtraProduct;   //��ȡ��Ʒ

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
/////////////////��վ��Ϣ(����)///////////////////////////////////////////////
//���ݿ���վ��Ϣ
struct T_Station_Info : public CEnDeCode
{
	CStdString strID ;
	CStdString strName ;
	CStdString strHost ;
	CStdString strSiteState ;  //��վ״̬
	CStdString strConst ;  //��վ����
	CStdString strEditor ;  //�ɱ�
	CStdString strSiteClass ;  //��վ�Ǽ�
	CStdString strMemo ;   //��վ��ע
	CStdString strLastModify ; //��վ������ʱ��
	CStdString strImagePrecessCode;//��֤�����

	DWORD dwBaiduWeight ;  //�ٶ�Ȩ��
	DWORD dwBaiduRecord ;  //�ٶ���¼��
	DWORD dwGooglePR ;  //Google PRָ��
	DWORD dwGoogleRecord ;  //Google��¼ָ��

	DWORD dwAverageDailyIP;  //IP
	DWORD dwAverageDailyPV;  //PV

//	CStdString strAccount ;
//	CStdString strPass ;
	T_Site_Type tSiteType ;
	T_Post_Type tPostType ;
	T_Limit tLimit ;

	E_Support_Type eSupportType ; //��վ֧�ֵ�����汾����

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

