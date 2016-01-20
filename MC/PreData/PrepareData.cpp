// PrepareData.cpp: implementation of the CPrepareData class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PrepareData.h"
#include "Reg.h"
#include <time.h>
#include <atlconv.h>
#include <atlimage.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//
/*CLogTrace g_PrepareData(_T("PreData.log"), NULL);*/
CLogTrace g_unknownParamLog(_T("UnknownParam.log"), NULL);
CLogTrace g_ReplaceParamLog(_T("ReplaceParam.log"), NULL);

#define PrepareData 1
#define ReplaceParamLog 1

const CStdString CPrepareData::PARAM = _T("%CComp%,%CCompShortName%,%EComp%,%CInfo%,%ICPNo%,%CompanyKind%,%CompanyTrade%,%RegPlace%,%CPrincipal%,%FactoryBrand%,%FactoryBrand1%,%FactoryBrand2%,%FactoryBrand3%,%FactoryPdt%,%FactoryPdt1%,%FactoryPdt2%,%FactoryPdt3%,%CPhilosophy%,%CKouhao%,%MainMarket%,%BankUserName%,%CComanyRegName%,%BankName%,%BankAccount%,%RegTime%,%RegTime-Y%,%RegTime-M%,%RegTime-D%,%CertStartTime%,%CertStartTime-Y%,%CertStartTime-M%,%CertStartTime-D%,%CertEndTime%,%CertEndTime-Y%,%CertEndTime-M%,%CertEndTime-D%,%MoneyInfo%,%MoneyInfo-N%,%PersonsCount%,%PersonsCount-N%,%TTel%,%HandSetNum%,%FFax%,%MMail%,%MMail1%,%MMail2%,%NNameMail%,%MMailPass%,%ICode%,%ACode%,%QQNo%,%PerMsn%,%WWebs%,%WWebs-H%,%ZZip%,%BZZip%,%CSel%,%CCountry%,%PProv%,%CCity%,%AArea2%,%AAddr-L%,%AAddr-S%,%RName%,%Xing%,%Ming%,%ERName%,%EXing%,%EMing%,%SSex%,%PPID%,%YYear%,%Month%,%DDay%,%CCar%,%DeptName%,%NName%,%PPass%,%SSubj%,%BBody%,%BBody-E%,%BBody-C%,%BBody-CI%,%InfoK%,%InfoK1%,%InfoK2%,%InfoK3%,%Expire0%,%ManuFacturer%,%ProName%,%Type%,%PdtSpecif%,%PdtBrand%,%Amount%,%Price%,%PdtMeasure%,%PdtPack%,%PdtOrigin%,%HGCode%,%Batch%,%PdtTextures%,%GuaranteePeriod%,%PdtInfo%,%PdtExtInfo%,%InfoID%,%FullDateyyyymmdd%,%FullDateyyyy\/mm\/dd%,%FullDateyyyy\-mm\-dd%,%FullDatemm\-dd\-yyyy%,%FullDatedd\-mm\-yyyy%,%FullDatemm\/dd\/yyyy%,%FullDatedd\/mm\/yyyy%,%FullDateyymmdd%,%FullDateyy\/mm\/dd%,%FullDateyy\-mm\-dd%,%FullDatemm\-dd\-yy%,%FullDatedd\-mm\-yy%,%FullDatemm\/dd\/yy%,%FullDatedd\/mm\/yy%,%FullDateyyyy\.mm\.dd%,%FullDatedd\.mm\.yyyy%,%FullDatemm\.dd\.yyyy%,%sumpic%,%sumpic1%,%sumpic2%,%sumpic3%,%sumpic4%,%tradcert%,%sumpic-width:hight%,%sumpic1-width:hight%,%sumpic2-width:hight%,%sumpic3-width:hight%,%sumpic4-width:hight%,%OFtype0%,%tradcert-width:hight%,%CSel0%,%CSel1%,%CSel2%,%CSel3%,%CselFirstName%,%CselSecondName%,%CselThirdName%,%CompanyKind%,%CCountry%,%PProv0%,%CCity1%,%AArea2%");
//
CPrepareData::CPrepareData()
{
	InitCountry();
	InitProvValue();
	InitSex();
	InitCompanyKind();
	InitExpire();
	InitPersonsCount();
	InitMoneyInfo();
	LoadAllParam();
	m_strPagePhotoFlag = _T("");
}

//�޸�
CPrepareData::~CPrepareData()
{

}


//��ʼ��ʡ������
int CPrepareData::InitProvValue()
{
	//Ӣ��ʡ��
	m_EProvValue[0]=_T("Anhui");
	m_EProvValue[1]=_T("Beijing");
	m_EProvValue[2]=_T("Fujian");
	m_EProvValue[3]=_T("Gansu");
	m_EProvValue[4]=_T("Guangdong");

	m_EProvValue[5]=_T("Guangxi");
	m_EProvValue[6]=_T("Guizhou");
	m_EProvValue[7]=_T("Hainan");
	m_EProvValue[8]=_T("Hebei");
	m_EProvValue[9]=_T("Henan");

	m_EProvValue[10]=_T("Heilongjiang");
	m_EProvValue[11]=_T("Hubei");
	m_EProvValue[12]=_T("Hunan");
	m_EProvValue[13]=_T("Jilin");
	m_EProvValue[14]=_T("Jiangsu");

	m_EProvValue[15]=_T("Jiangxi");
	m_EProvValue[16]=_T("Liaoning");
	m_EProvValue[17]=_T("Neimenggu");
	m_EProvValue[18]=_T("Ningxia");
	m_EProvValue[19]=_T("Qinghai");

	m_EProvValue[20]=_T("Shandong");
	m_EProvValue[21]=_T("Shanxi");
	m_EProvValue[22]=_T("Shannxi");
	m_EProvValue[23]=_T("Shanghai");
	m_EProvValue[24]=_T("Sichuan");

	m_EProvValue[25]=_T("Tianjin");
	m_EProvValue[26]=_T("Xinjiang");
	m_EProvValue[27]=_T("Xizang");
	m_EProvValue[28]=_T("Yunnan");

	m_EProvValue[29]=_T("Zhejiang");
	m_EProvValue[30]=_T("Chongqing");
	m_EProvValue[31]=_T("HongKong");
	m_EProvValue[32]=_T("Macao");
	m_EProvValue[33]=_T("TaiWan");

	//����ʡ��
	m_CProvValue[0]=_T("����");
	m_CProvValue[1]=_T("����");
	m_CProvValue[2]=_T("����");
	m_CProvValue[3]=_T("����");
	m_CProvValue[4]=_T("�㶫");

	m_CProvValue[5]=_T("����");
	m_CProvValue[6]=_T("����");
	m_CProvValue[7]=_T("����");
	m_CProvValue[8]=_T("�ӱ�");
	m_CProvValue[9]=_T("����");

	m_CProvValue[10]=_T("������");
	m_CProvValue[11]=_T("����");
	m_CProvValue[12]=_T("����");
	m_CProvValue[13]=_T("����");
	m_CProvValue[14]=_T("����");

	m_CProvValue[15]=_T("����");
	m_CProvValue[16]=_T("����");
	m_CProvValue[17]=_T("���ɹ�");
	m_CProvValue[18]=_T("����");
	m_CProvValue[19]=_T("�ຣ");

	m_CProvValue[20]=_T("ɽ��");
	m_CProvValue[21]=_T("ɽ��");
	m_CProvValue[22]=_T("����");
	m_CProvValue[23]=_T("�Ϻ�");
	m_CProvValue[24]=_T("�Ĵ�");

	m_CProvValue[25]=_T("���");
	m_CProvValue[26]=_T("�½�");
	m_CProvValue[27]=_T("����");
	m_CProvValue[28]=_T("����");
	m_CProvValue[29]=_T("�㽭");

	m_CProvValue[30]=_T("����");
	m_CProvValue[31]=_T("���");
	m_CProvValue[32]=_T("����");
	m_CProvValue[33]=_T("̨��");

	return 0;
}

//��ʼ�����Ҷ���
//Ŀǰ��5���������
int CPrepareData::InitCountry()
{
	//���ļ���
	m_strCCountry[0]=_T("�й�");
	m_strCCountry[1]=_T("�й����");
	m_strCCountry[2]=_T("�й�̨��");
	m_strCCountry[3]=_T("�й�����");
	m_strCCountry[4]=_T("�¼���");
	m_strCCountry[5]=_T("Singapore");

	//Ӣ��
	m_strECountry[0]=_T("China");
	m_strECountry[1]=_T("Hong Kong");
	m_strECountry[2]=_T("Tai Wan China");
	m_strECountry[3]=_T("Macao");
	m_strECountry[4]=_T("Singapore");
	m_strECountry[5]=_T("Singapore");

	return 0;
}

//��ʼ���Ա�
void CPrepareData::InitSex()
{
	//����
	m_strCSex[0]=_T("��");
	m_strCSex[1]=_T("Ů");
	m_strCSex[2]=_T("����");

	//Ӣ��
	m_strESex[0]=_T("Male");
	m_strESex[1]=_T("Female");
	m_strESex[2]=_T(" ");

	return;
}

//��ʼ����˾����
void CPrepareData::InitCompanyKind()
{
	//����
	m_strCCompanyKind[0]=_T("��������");
	m_strCCompanyKind[1]=_T("�������");
	m_strCCompanyKind[2]=_T("��ҵ��λ");
	m_strCCompanyKind[3]=_T("������ҵ");
	m_strCCompanyKind[4]=_T("������ҵ");
	m_strCCompanyKind[5]=_T("������ҵ");
	m_strCCompanyKind[6]=_T("������ҵ");
	m_strCCompanyKind[7]=_T("������ҵ");
	m_strCCompanyKind[8]=_T("˽Ӫ��ҵ");
	m_strCCompanyKind[9]=_T("�ɷ�����ҵ");
	m_strCCompanyKind[10]=_T("����");

	//Ӣ��
	m_strECompanyKind[0]=_T("Administrative organ");
	m_strECompanyKind[1]=_T("Public organization");
	m_strECompanyKind[2]=_T("Public institution");
	m_strECompanyKind[3]=_T("State-owned enterprise");
	m_strECompanyKind[4]=_T("Three kinds of foreign-invested enterprises");
	m_strECompanyKind[5]=_T("Collective-owned enterprise");
	m_strECompanyKind[6]=_T("Collective undertaking");
	m_strECompanyKind[7]=_T("Township enterprise");
	m_strECompanyKind[8]=_T("Private enterprise");
	m_strECompanyKind[9]=_T("Share-issuing enterprise");
	m_strECompanyKind[10]=_T("other");

	return;
}

//��ʼ����Ч��
void CPrepareData::InitExpire()
{
	//����
	m_strCExpire[0]=_T("����(10������)");
	m_strCExpire[1]=_T("����(60������)");
	m_strCExpire[2]=_T("����(60������)");

	//Ӣ��
	m_strEExpire[0]=_T("Short-term(under 10 days)");
	m_strEExpire[1]=_T("Mid-term(under 60 days)");
	m_strEExpire[2]=_T("Long-term(over 60 days)");

	return;
}

//��ʼ����˾����
void CPrepareData::InitPersonsCount()
{
	//����
	m_strCPersonsCount[0]=_T("����10��");
	m_strCPersonsCount[1]=_T("10--50��");
	m_strCPersonsCount[2]=_T("50--100��");
	m_strCPersonsCount[3]=_T("100--200��");
	m_strCPersonsCount[4]=_T("200������");

	//Ӣ��
	m_strEPersonsCount[0]=_T("less than 10");
	m_strEPersonsCount[1]=_T("10--50");
	m_strEPersonsCount[2]=_T("50--100");
	m_strEPersonsCount[3]=_T("100--200");
	m_strEPersonsCount[4]=_T("more than 200");

	return;
}

//��ʼ��ע���ʽ�
void CPrepareData::InitMoneyInfo()
{
	//����
	m_strCMoneyInfo[0]=_T("����50��");
	m_strCMoneyInfo[1]=_T("50---100��");
	m_strCMoneyInfo[2]=_T("100--200��");
	m_strCMoneyInfo[3]=_T("200--500��");
	m_strCMoneyInfo[4]=_T("500-1000��");
	m_strCMoneyInfo[5]=_T("1000������");

	//Ӣ��
	m_strEMoneyInfo[0]=_T("less then 500,000");
	m_strEMoneyInfo[1]=_T("500,000---1,000,000");
	m_strEMoneyInfo[2]=_T("1,000,000---2,000,000");
	m_strEMoneyInfo[3]=_T("2,000,000---5,000,000");
	m_strEMoneyInfo[4]=_T("5,000,000---10,000,000");
	m_strEMoneyInfo[5]=_T("more than 10,000,000");

	return;
}

//��ȡ����Ĭ��ֵ
int CPrepareData::GetParamValue(int nValueId, int nParamType, 
								int nLanType,CStdString& strParamValue)
{
	//����
	if (0==nLanType)
	{
		GetCParamValue(nValueId, nParamType, strParamValue);
	}
	//Ӣ��
	else if (2==nLanType)
	{
		GetEParamValue(nValueId, nParamType, strParamValue);
	}
	return 0;
}

//��ȡ���Ĳ���Ĭ��ֵ
int CPrepareData::GetCParamValue(int nValueId, int nParamType, CStdString& strParamValue)
{
	strParamValue.Empty();
	//pprov
	if (1==nParamType)
	{
		if (nValueId<0 || nValueId>33)
		{
			nValueId=0;
		}

		strParamValue=m_CProvValue[nValueId];

	}
	//ssex
	else if (2==nParamType)
	{
		if (nValueId<0 || nValueId>2)
		{
			nValueId=0;
		}
		strParamValue=m_strCSex[nValueId];
	}
	//companykind
	else if (4==nParamType)
	{
		if (nValueId<0 || nValueId>10)
		{
			nValueId=0;
		}
		strParamValue=m_strCCompanyKind[nValueId];
	}
	//expire
	else if (26==nParamType)
	{
		if (nValueId<0 || nValueId>2)
		{
			nValueId=0;
		}
		strParamValue=m_strCExpire[nValueId];
	}
	//Country
	else if (28==nParamType)
	{
		if (nValueId<0 || nValueId>6)
		{
			nValueId=0;
		}
		strParamValue=m_strCCountry[nValueId];
	}
	//personscount
	else if (29 == nParamType)
	{
		if (nValueId<0 || nValueId>4)
		{
			nValueId = 0;
		}
		strParamValue = m_strCPersonsCount[nValueId];
	}
	//moneyinfo
	else if (30 == nParamType)
	{
		if (nValueId < 0 || nValueId >5)
		{
			nValueId = 0;
		}
		strParamValue = m_strCMoneyInfo[nValueId];
	}

	return 0;
}

//��ȡӢ�Ĳ���Ĭ��ֵ
int CPrepareData::GetEParamValue(int nValueId, int nParamType, CStdString& strParamValue)
{
	strParamValue.Empty();
	//pprov
	if (1==nParamType)
	{
		if (nValueId<0 || nValueId>33)
		{
			nValueId=0;
		}

		strParamValue=m_EProvValue[nValueId];

	}
	//ssex
	else if (2==nParamType)
	{
		if (nValueId<0 || nValueId>2)
		{
			nValueId=0;
		}
		strParamValue=m_strESex[nValueId];
	}
	//companykind
	else if (4==nParamType)
	{
		if (nValueId<0 || nValueId>10)
		{
			nValueId=0;
		}
		strParamValue=m_strECompanyKind[nValueId];
	}	
	//expire
	else if (26==nParamType)
	{
		if (nValueId<0 || nValueId>2)
		{
			nValueId=0;
		}
		strParamValue=m_strEExpire[nValueId];
	}
	//Country
	else if (28==nParamType)
	{
		if (nValueId<0 || nValueId>6)
		{
			nValueId=0;
		}
		strParamValue=m_strECountry[nValueId];
	}
	//PersonsCount
	else if (29==nParamType)
	{
		if (nValueId<0 || nValueId>5)
		{
			nValueId=0;
		}
		strParamValue=m_strEPersonsCount[nValueId];
	}
	//MoneyInfo
	else if (30==nParamType)
	{
		if (nValueId<0 || nValueId>6)
		{
			nValueId=0;
		}
		strParamValue=m_strEMoneyInfo[nValueId];
	}

	return 0;
}

//��������
DWORD CPrepareData::ProcessData(const int& nNetCode, const E_TASK_TYPE& eTaskType, 
								TRapalceDataMsg& tRapalceDataMsg, T_TaskData* tTaskData,DWORD dwIndex, DWORD dwPostType)
{
	int dRet;
	T_NetstationBaseInfo tNSDataTemp;

	//������׼���������ͳ�ʼ��Ϊ�ɹ���־���������������ı���ֵ
	tRapalceDataMsg.eErrType=PPD_SUCCESS;

	if (nNetCode < 0)
	{
		tRapalceDataMsg.eErrType=PPD_NETCODELESSZERO_ERR;
		g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("nNetCode err:%d!"), nNetCode);

		return 1;
	}

	//����վģ���еõ���������
	try
	{
		dRet =m_NetData.OpenWebSiteData(nNetCode);
		dRet=GetValueFromDB(tNSDataTemp);
	}
	catch (...)
	{
		g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("��ȡ��վģ�������쳣�������ļ������ڻ��ʽ����ȷ nNetCode:%d!"), nNetCode);
		tRapalceDataMsg.eErrType=PPD_NOWEBDATA;
        return 1;
	}

	tTaskData->strHost = tNSDataTemp.NShost;
	tTaskData->strOCRParam = tNSDataTemp.NSImageProcessCode;		//��ȡ��֤��
	//������֤��ʶ�����
	if (tTaskData->strOCRParam.GetLength() > 10)
	{
		tTaskData->strOCRParam = DesImageCode(nNetCode,tNSDataTemp.NSImageProcessCode);
	}

	m_NetData.GetNStype(tTaskData->siteType);
	m_NetData.GetNSlimit(tTaskData->limitRefresh);

	tRapalceDataMsg.eUserType = tTaskData->siteType.eAccountStyle;
	tRapalceDataMsg.ePsdType = tTaskData->siteType.ePasswordStyle;

	//������������
	CStdString strTmp = _T("");
	CStdString *pstr= &(tTaskData->tTaskDataExser.strEnginData);

	DecodeStationData(tNSDataTemp.NSdata,strTmp);
	m_eCurrentTaskType=eTaskType;
	
	//��ȡǶ����վ���ݣ�
	if (tNSDataTemp.NsEmbedCode.GetLength() > 6)
	{
		//���ж�Ƕ�����վ�Ƿ�Ϊ���£�
		StringUtils Utl;
		T_NetstationBaseInfo tEmbedData;
		std::vector<CStdString> result;
		CStdString strEmbedTmp = _T("");
		
		Utl.SplitStringEx(tNSDataTemp.NsEmbedCode,_T(","),result,false);
		for (int i = 0; i< result.size(); ++i)
		{
			//�����ǰ��ַ�����ڲ���Ҫ���µ���վ���棬������
			if (tTaskData->strNotUpdateWeb.Find(result[i]) != -1)
			{
				continue;
			}
			DWORD dwNscode = StrToLong(result[i]);
			CStdString *pEmbedstr = &(tTaskData->tTaskDataExser.mapEmbedData[dwNscode]);
			
			try
			{
				m_NetData.CloseWebSiteData();
				dRet =m_NetData.OpenWebSiteData(dwNscode);
				dRet=GetValueFromDB(tEmbedData);

				if (tEmbedData.NSImageProcessCode.GetLength() > 10)
				{
					tTaskData->tTaskDataExser.strExOCRParam = DesImageCode(dwNscode, tEmbedData.NSImageProcessCode);
				}
			}
			catch (...)
			{
				g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("��ȡǶ����վģ�������쳣�������ļ������ڻ��ʽ����ȷ nNetCode:%d!"), dwNscode);
				tRapalceDataMsg.eErrType=PPD_NOWEBDATA;
				return 1;
			}

			DecodeStationData(tEmbedData.NSdata,strEmbedTmp);
			//�滻Ƕ����վ��ͨ�����
			ReplaceParam(strEmbedTmp, *pEmbedstr, dwNscode, tRapalceDataMsg);

		}
		//���´�ԭ��վ����
		m_NetData.CloseWebSiteData();
		m_NetData.OpenWebSiteData(nNetCode);
	}

	//��ȡ���ձ�ǵ�ֵ��������ϣ�
	if(m_strPagePhotoFlag == _T(""))
	{
		T_LocalGlobalData *pGlobalData = NULL;
		pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
			E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
		if (NULL != pGlobalData)
		{
			m_strPagePhotoFlag = pGlobalData->TaskDetailInfo.szPagePhotoFlag;
		}
	}

	if (tRapalceDataMsg.strInfoID != _T(""))      //�滻��ϢID;
	{
		strTmp.Replace(_T("%InfoID%"),tRapalceDataMsg.strInfoID);
		g_sysLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("�滻��ϢID, InfoID:%s!"), tRapalceDataMsg.strInfoID.c_str());
	}
	//�滻�ں˴����ͨ���Ϊ#
	ReplaceSumengineParam(strTmp);

	ReplaceParam(strTmp, *pstr, nNetCode, tRapalceDataMsg);
	//����û���滻��%ͨ�����
	if (tTaskData->tTaskDataExser.strEnginData.Find(_T("%")) != -1)
	{
		ReplacePercent(tTaskData->tTaskDataExser.strEnginData,tRapalceDataMsg.vNoRepleaceParam);
	}

	return 0;	
}

//�����ݿ�ȡ�ֶ�ֵ
DWORD CPrepareData::GetValueFromDB(T_NetstationBaseInfo& tNetStationData)
{
	return m_NetData.GetNetData(tNetStationData);;
}

//�滻ͨ���
DWORD CPrepareData::ReplaceParam(const CStdString& strIn, CStdString& strOut, const int& nKey, 
	TRapalceDataMsg& tRapalceDataMsg)
{
	CStdString strOutTemp;
	CStdString strReplaceTemp;
	CStdString strReplaceTemp1;
	CStdString strSub;
	CStdString strFilter;
	CStdString equate;


	int nPosFind;
	int nPosFind1;
	//���ձ�ǵĳ���
	int iPagePhotoFalgLen = m_strPagePhotoFlag.GetLength();
	//LRangeͨ��� ������ʹ��
	CStdString strRangeReplace;
	int EnCodeCount = 0;//��װ����
	BOOL bNeedCode = TRUE;

	int iCharMax;
	int iCharMin;

	nPosFind=-1;
	nPosFind1 =-1;
	nPosFind = strIn.Find(_T("%"), 0);

	if (nPosFind == -1)
	{
		strOut = strIn;
		return 1;
	}

	strOutTemp=strIn.Left(nPosFind);
	CStdString strPassTemp;
	strPassTemp.Empty();
	int nPass = 0;

	while (TRUE)
	{
	   
		nPosFind1=strIn.Find(_T("%"), nPosFind+1);
		if (-1 == nPosFind1)
		{
			strOutTemp += strIn.Right(strIn.GetLength()-nPosFind);	
			strOut=strOutTemp;

			return 0;
		}

		strReplaceTemp.Empty();
		strReplaceTemp = strIn.Mid(nPosFind, nPosFind1-nPosFind);

		while (3==strReplaceTemp.GetLength())
		{
			strOutTemp+=strReplaceTemp;
			nPosFind=nPosFind1;
			nPosFind1=strIn.Find(_T("%"), nPosFind+1);
			if (-1 == nPosFind1)
			{
				strOutTemp += strIn.Right(strIn.GetLength()-nPosFind);	
				strOut=strOutTemp;

				return 0;
			}

			strReplaceTemp.Empty();
			strReplaceTemp = strIn.Mid(nPosFind, nPosFind1-nPosFind);
		}


		if (strReplaceTemp.GetLength() >= 5)
		{
			strSub = strReplaceTemp.Left(5);

			//��iidk�����д���
			if (!strSub.CompareNoCase(_T("%iidk")))
			{
				strReplaceTemp = strIn.Mid(nPosFind, nPosFind1-nPosFind+1);
				strOutTemp += strReplaceTemp;

				nPosFind=strIn.Find(_T("%"), nPosFind1+1);
				if (nPosFind==-1)
				{
					strOutTemp+=strIn.Right(strIn.GetLength() -1-nPosFind1);
					strOut=strOutTemp;
					return 0;
				}
				strOutTemp+=strIn.Mid(nPosFind1+1, nPosFind-nPosFind1-1);

				continue;
			}
		}
		
        iCharMax = 0;
		iCharMin = 0;
		strFilter.Empty(); 
		//�����Ƿ���ͨ���%LRange=4:20%
		if (strReplaceTemp.GetLength() >= 8)
		{
			strSub = strReplaceTemp.Left(8);

			//��LRange�������Ѿ���һ��������
			if (!strSub.CompareNoCase(_T("%LRange=")))
			{
				strReplaceTemp = strIn.Mid(nPosFind, nPosFind1-nPosFind+1);

				nPosFind=strIn.Find(_T("%"), nPosFind1+1);
				if (nPosFind==-1)
				{
					strOutTemp+=strIn.Right(strIn.GetLength() -1-nPosFind1);
					strOut=strOutTemp;
					return 0;
				}
				strOutTemp+=strIn.Mid(nPosFind1+1, nPosFind-nPosFind1-1);

				continue;
			}
			else if (!strSub.CompareNoCase(_T("%Filter=")))
			{
				strReplaceTemp = strIn.Mid(nPosFind, nPosFind1 - nPosFind + 1);
				nPosFind = strIn.Find(_T("%"), nPosFind1 + 1);
				if (nPosFind == -1)
				{
					strOutTemp += strIn.Right(strIn.GetLength() - 1 - nPosFind1);
					strOut = strOutTemp;
					return 0;
				}

				CStdString strTmpPencent;
				strTmpPencent = strIn.Mid(nPosFind + 1, 1);
				if (strTmpPencent == _T("%"))
				{
					nPosFind++;
				}
				strOutTemp += strIn.Mid(nPosFind1 + 1, nPosFind - nPosFind1 - 1);

				continue;

			}

		}

		//�����Ƿ���ͨ���%LRange=4:20%
		strRangeReplace.Empty();
		CStdString strTmp;
		if(nPosFind1<strIn.GetLength()-1)
		{
			strTmp = strIn.Mid(nPosFind1+1,1);
			if(strTmp == _T("%"))
			{
				int tmpPos = strIn.Find(_T("%"), nPosFind1+2);
				if(tmpPos != -1)
				{
					strRangeReplace = strIn.Mid(nPosFind1+2, tmpPos -nPosFind1-2);
					int len = strRangeReplace.GetLength();
					if(len >7)
					{
						CStdString tmp = strRangeReplace.Left(7);

						if(!tmp.CompareNoCase(_T("LRange=")))
						{
							int p,v1,v2;
							int len;
							p=-1;
							v1=0;
							v2 =0;
							p = strRangeReplace.Find(_T(":"),0);
							len = strRangeReplace.GetLength();
							if(p!=-1&&p>7&&p<len-1)
							{
								v1 = _ttoi(strRangeReplace.Mid(7,p-7));
								v2 = _ttoi(strRangeReplace.Right(len-p-1));
								if(v2>=v1)
								{
									iCharMax = v2;
									iCharMin  = v1;
								}
								else
								{
									iCharMax  = v1;
									iCharMin   = v2;
								}
								
								if(!strReplaceTemp.CompareNoCase(_T("%nname")))
								{
									if(tRapalceDataMsg.dNameSize<v1||tRapalceDataMsg.dNameSize>v2)
										tRapalceDataMsg.dNameSize = v2;
								}
								else if(!strReplaceTemp.CompareNoCase(_T("%ppass")))
								{
									if(tRapalceDataMsg.dPsdSize<v1||tRapalceDataMsg.dPsdSize>v2)
										tRapalceDataMsg.dPsdSize = v2;
								}
							}

							//�鿴�����Ƿ����Filter,���ھʹ���
							strTmp = strIn.Mid(tmpPos + 1, 1);
							if (strTmp == _T("%"))
							{
								int tmpFilterPos = strIn.Find(_T("%"), tmpPos + 2);
								if (tmpFilterPos != -1)
								{
									strRangeReplace = strIn.Mid(tmpPos + 2, tmpFilterPos - tmpPos - 2);
									len = strRangeReplace.GetLength();
									if (len > 7)
									{
										tmp = strRangeReplace.Left(7);
										if (!tmp.CompareNoCase(_T("Filter=")))
										{
											strTmp = strIn.Mid(tmpFilterPos + 1, 1);
											if (strTmp == _T("%"))
											{
												//���Ҫ����%��
												strRangeReplace = strIn.Mid(tmpPos + 2, tmpFilterPos - tmpPos - 1);
											}
											int iLent;
											iLent = strRangeReplace.GetLength();
											if (iLent > 7)
											{
												strFilter = strRangeReplace.Mid(7, iLent - 7);
											}
										}

									}
								}
							}
						}
						else if (!tmp.CompareNoCase(_T("Filter=")))
						{							
							strTmp = strIn.Mid(tmpPos + 1, 1);
							if (strTmp == _T("%"))
							{
								//���Ҫ����%��
								strRangeReplace = strIn.Mid(nPosFind1 + 2, tmpPos - nPosFind1 - 1);
							}
							int iLent;							
							iLent = strRangeReplace.GetLength();
							if (iLent > 7)
							{
								strFilter = strRangeReplace.Mid(7, iLent - 7);
							}
						}
						else
						{
							iCharMax = 0;
							iCharMin = 0;
						}
					}
				}
			}
		}

		EnCodeCount  = 0;
		bNeedCode = FALSE;
		//&aaaar EnCodeCount��ʾ�м���ٸ�a,���ж��ٴη�װ%ͨ���%ǰ��Ӧ�ö�������
		strTmp = strOutTemp.Mid(strOutTemp.GetLength()-1,1);
		if(strTmp==_T("r"))
		{
			int PosCodeTmp1 = strOutTemp.ReverseFind(_T("&"));

			if(PosCodeTmp1 != -1)
			{
				bNeedCode = TRUE;
				for(int i = PosCodeTmp1+1; i<strOutTemp.GetLength()-1;i++)
				{
					strTmp = strOutTemp.Mid(i, 1);
					if(strTmp !=_T("a"))
					{
						//ƥ��ʧ�ܣ���û��
						bNeedCode = FALSE;
						EnCodeCount = 0;
						break;
					}
					EnCodeCount++;
				}
			}
		}

		strReplaceTemp1 = strReplaceTemp.Right(strReplaceTemp.GetLength() -1);
		if ( (strReplaceTemp1.GetLength() > 2)&&
			IsChar(strReplaceTemp1.GetBuffer(0)[0])&&
			IsChar(strReplaceTemp1.GetBuffer(0)[1])&&
			(-1==strReplaceTemp1.Find(_T("&")))&&
			(-1==strReplaceTemp1.Find(_T("=")))&&
			Equate(strReplaceTemp1, equate, tRapalceDataMsg) )
		{
			CStdString strEquateTemp;
			CStdString strTmpValue;

			if (!strReplaceTemp1.Compare(_T("ppass")))
			{
				if(nPass == 0)
				{
					tRapalceDataMsg.strWebPass = equate;
					strPassTemp = equate;
				}
				if(nPass > 0)
				{
					equate = strPassTemp;
				}
				nPass++;
			}


			//���滻ǰ���г��ȵ������Է������ں˴���
			if (!strFilter.IsEmpty() && equate.GetLength()>0)
			{
				g_sysLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("��⵽Ҫ���˵��ַ�:%s!"), strFilter.c_str());
				//������˵�ֵ
				for (int iLength = 0; iLength < strFilter.GetLength(); iLength++)
				{					
					CStdString szStr = strFilter.Mid(iLength,1);					
					equate.Replace(szStr, _T(""));
				}
			}
			//����������־�Ĵ�ӡ
			if (iCharMin != iCharMax)
			{
				g_ReplaceParamLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,
					_T("ͨ�����%s,equate��ɾǰ:%s"), strReplaceTemp1.c_str(), equate.c_str());
			}
			int len = equate.GetLength();
			//------��body����  begin -------
			strReplaceTemp1.ToLower();  //Сд
			//body���ȴ���
			if (strReplaceTemp1.Find(_T("bbody")) != -1)
			{
				len += iPagePhotoFalgLen;
			}
			if(len<iCharMin && len > 0)
			{
			  	//����
				CStdString tmp = _T("");
				while(len<iCharMin)
				{
					tmp += equate;
					len = tmp.GetLength();
				}
				equate = tmp;
			}
			if(len>iCharMax&& iCharMax!=0)
			{
				if (strReplaceTemp1.Find(_T("bbody")) != -1)
				{
					equate = equate.Left(iCharMax-iPagePhotoFalgLen);
					//�ضϺ��׼������
					int iEndPos = equate.ReverseFind(_T("��"));
					int iEndPos2 = equate.ReverseFind(_T("\n"));
					if (iEndPos >= iEndPos2)
					{
						equate = equate.Left(iEndPos);
					}
					else
					{
						equate = equate.Left(iEndPos2);
					}
				}
				else
				{
					equate=equate.Left(iCharMax);
				}
				
			}

			//����ץȡ���ձ��
			if (strReplaceTemp1.Find(_T("bbody")) != -1)
			{
				//�̻��й���֧�����߹ؼ��ʱ�ǣ������滻��б��
				if (tRapalceDataMsg.strNetCode == _T("40042283"))
				{
					CStdString strTmpWeb = m_strPagePhotoFlag;
					strTmpWeb.Replace(_T("|"), _T("/"));
					equate = equate + _T("\r\n") + strTmpWeb;
					g_sysLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("�滻ץȡ���ձ��, ���ձ��:%s!"), strTmpWeb.c_str());

				}
				else
				{
					equate = equate + _T("\r\n") + m_strPagePhotoFlag;
				}				
			}
			//�����ͼƬ�Ļ��и�ʽ��ʽ
			if (strReplaceTemp1.Find(_T("bbody-p")) != -1)
			{
				equate.Replace(_T("\n"),_T("<br>"));
			}
			//------body���� end-----------
			if (!strReplaceTemp1.Compare(_T("nname")))                //������վҪ�������󱣴��ʺ����룬���ڴ����ϲ㣻
			{
				tRapalceDataMsg.strWebName=equate;
				tRapalceDataMsg.strDefaultName = equate;
			}

			if (!strReplaceTemp1.Compare(_T("ppass")))
			{
				tRapalceDataMsg.strWebPass=equate;
				tRapalceDataMsg.strDefaultPass = equate;
			}

			if (0 == strReplaceTemp1.CompareNoCase(_T("nname")))
			{
				if (tRapalceDataMsg.strNetCode == _T("40000003"))
				{
					tRapalceDataMsg.strWebName = equate + _T("@126.com");
				}
				else if ((tRapalceDataMsg.strNetCode == _T("40000594")))
				{
					tRapalceDataMsg.strWebName = equate + _T("@sina.com");
				}
			}
			//����������־�Ĵ�ӡ
			if (iCharMax != iCharMin)
			{
				g_ReplaceParamLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,
					_T("ͨ�����%s,equate��ɾ��:%s"), strReplaceTemp1.c_str(), equate.c_str());
			}
			
			CStdString strtemp;
			//CStdString strLine;
			//CStdString strComma;
			CStdString strAnd;

				
			for(int i = 0;i<=EnCodeCount;i++)
			{
				strtemp += _T('a');
			}

			//strLine.Format(_T("&%sc"),strtemp.GetBuffer());
			//strComma.Format(_T("&%sr"),strtemp.GetBuffer());
			strAnd.Format(_T("&%sa"), strtemp.GetBuffer());
                          
			g_ReplaceParamLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,
					_T("ͨ�����%s, equateת������ǰ:%s"),strReplaceTemp1.c_str(), equate.c_str());
			//���ٴ���־��
			int iRep1 = 0;
			int iRep2 = 0;

			iRep1 = equate.Replace(_T("&"), strAnd);//�������滻���
			//equate.Replace(_T("\r\n"), strLine);
			iRep2 = equate.Replace(_T(","), _T("&b"));
			if ((iRep1 + iRep2) > 0 )
			{
				g_ReplaceParamLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,
					_T("ͨ�����%s, equateת�������:%s"),strReplaceTemp1.c_str(), equate.c_str());
			}

			strOutTemp += equate;

			nPosFind=strIn.Find(_T("%"), nPosFind1+1);
			if (nPosFind==-1)
			{
				strOutTemp+=strIn.Right(strIn.GetLength() -1-nPosFind1);
				strOut=strOutTemp;
				return 0;
			}
			strOutTemp+=strIn.Mid(nPosFind1+1, nPosFind-nPosFind1-1);
			nPosFind1=nPosFind;

			g_ReplaceParamLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,
									_T("Net:%d,TaskType:%d,%s Replace param:%s"), 
									nKey, m_eCurrentTaskType,
									strReplaceTemp1.GetBuffer(strReplaceTemp1.GetLength()),
									equate.GetBuffer(equate.GetLength()));

		}
		else
		{
			if (strReplaceTemp1.GetLength() == 2)
			{
				g_unknownParamLog.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,
									_T("Net:%d,TaskType:%d,Unknown param:%s"), 
									nKey, m_eCurrentTaskType,
									strReplaceTemp1.GetBuffer(strReplaceTemp1.GetLength()));
			}
			//�ж�ȡ�����ַ���β�Ƿ�Ϊ=�ַ�
			else if (strReplaceTemp1.GetLength() > 2)
			{
				int len = strReplaceTemp1.GetLength();
				TCHAR ch = strReplaceTemp1.GetAt(len-1);
				if (_T('=') == ch ||
					_T("=0")==strReplaceTemp1.Right(2))
				{
					strOutTemp += strReplaceTemp;
					nPosFind=nPosFind1;
					continue;

					g_sysLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, 
						_T("nNetCode:%d,TaskType:%d,param:%s!"), 
						nKey,
						m_eCurrentTaskType,
						strReplaceTemp1.GetBuffer(strReplaceTemp1.GetLength()));

				}
			}
			else
			{
				g_unknownParamLog.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,
										_T("Net:%d,TaskType:%d,Unknown param:%s"),
										nKey, m_eCurrentTaskType,
										strReplaceTemp1.GetBuffer(strReplaceTemp1.GetLength()));

			}

			strOutTemp += strReplaceTemp;
// 			if (strReplaceTemp1.Find(_T("&")) != -1)
// 			{
// 				strReplaceTemp = strReplaceTemp.Right(strReplaceTemp.GetLength() -1);
// 				strOutTemp += strReplaceTemp;
// 			}
			nPosFind=nPosFind1;
		}
		
		nPosFind = nPosFind1;

		Sleep(50);
	}

	return 0;
}

//�滻����
BOOL CPrepareData::Equate(CStdString& src, CStdString& equate, TRapalceDataMsg& tRapalceDataMsg)
{
	int len = src.GetLength();
	TCHAR ch = src.GetAt(len-1);
	int nCh=ch-0x30;
	CStdString val;
	CStdString temp = src.Left(len-1);
	CStdString strNoRep = src;

	std::vector<CStdString> sa;
	CStdString strData;
	int vtype=-1;

	CStdString strValueTemp1;
	BOOL bFindFlag1=FALSE;

	equate.Empty();
	src.ToLower();
	temp.ToLower();


	//���ж��ǲ���ʱ���滻ͨ���
	if(src.Find(_T("fulldate"),0) != -1)
	{
		GetCurrentTimeByFormats(src, strData);
		if(!strData.IsEmpty())
		{
			equate = strData;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	//�鿴��ƷͼƬ�Ƿ���Ҫ�ı�ͼƬ��С
	if(src.Find(_T("sumpic"),0) != -1)
	{
		if (src.Find(_T("-")) > 0)
		{
			CStdString strTmpsrc;
			int iIndex = -1;	
			CStdString tmp = src.Mid(6,1);
			if (tmp == _T("-"))
			{
				iIndex = 0;
				strTmpsrc = src.Left(6);//ȥ������ -width:heigth
			}
			else
			{
				iIndex = StrToInt(tmp);
				strTmpsrc =src.Left(7);//ȥ������ -width:heigth
			}
			if (GetResizeImage(src, equate,0,iIndex))
				return TRUE;

			equate.Empty();
			src = strTmpsrc;  
		}
		//return FALSE;
	}

	//�鿴Ӫҵִ���Ƿ���Ҫ�ı�ͼƬ��С
	if(src.Find(_T("tradcert-"),0) != -1)
	{
		if (GetResizeImage(src, equate, 1))
			return TRUE;

		equate.Empty();
		src = src.Left(8);  //ȥ������ -width:heigth
		//return FALSE;
	}

	//�鿴Ӫҵִ���Ƿ���Ҫ�ı�ͼƬ��С
	if(src.Find(_T("ccomppic-"),0) != -1)
	{
		if (GetResizeImage(src, equate, 2))
			return TRUE;

		equate.Empty();
		src = src.Left(8);  //ȥ������ -width:heigth
		//return FALSE;
	}

	//�鿴Ӫ��֯��������֤�Ƿ���Ҫ�ı�ͼƬ��С
	if (src.Find(_T("OrgCodeCert-"),0) != -1)
	{
		if (GetResizeImage(src, equate, 3))
			return TRUE;
		
		equate.Empty();
		src = src.Left(11); //ȥ������ -width:heigth
	}

	//�鿴˰�յǼ�֤�Ƿ���Ҫ �ı�ͼƬ��С
	if (src.Find(_T("TaxRegCert-"),0) != -1)
	{
		if (GetResizeImage(src, equate, 4))
			return TRUE;

		equate.Empty();
		src = src.Left(10);  //ȥ������ -width:heigth
	}



	//����TTelS3
	//����TTelE3
	//����FFaxS3
	//����FFaxE4
	//����MMail1
	//����MMail2
	if (src.Find(_T("ttel"),0) != -1 ||
		src.Find(_T("ffax"),0) != -1 ||
		src.Find(_T("mmail1"),0) != -1 ||
		src.Find(_T("mmail2"),0) != -1)
	{
		strValueTemp1.Empty();
		
		for (size_t i=0; i<tRapalceDataMsg.tParamItemTable.size(); i++)
		{
			if (tRapalceDataMsg.tParamItemTable[i].Key.ToLower()==src)
			{
				strValueTemp1=tRapalceDataMsg.tParamItemTable[i].Value;
				i=tRapalceDataMsg.tParamItemTable.size();
			}
		}

		if (_T("ttel")==src || _T("ffax") == src)
		{
			if (strValueTemp1.IsEmpty())
			{
					strValueTemp1=_T("88888888");
			}
			equate=strValueTemp1;
		}
		else if ( _T("mmail") == temp)
		{
			int nPosDiv=-1;
			for (size_t i=0; i<tRapalceDataMsg.tParamItemTable.size(); i++)
			{
				if (tRapalceDataMsg.tParamItemTable[i].Key.ToLower()==temp)
				{
					strValueTemp1=tRapalceDataMsg.tParamItemTable[i].Value;
					i=tRapalceDataMsg.tParamItemTable.size();
				}
			}
			if (!strValueTemp1.IsEmpty()) nPosDiv=strValueTemp1.Find(_T("@"), 0);

			if (strValueTemp1.IsEmpty() || -1==nPosDiv)
			{
				if (1==nCh)
				{
					equate=_T("rerertr");
				}
				else if (2==nCh)
				{
					equate=_T("yahoo.com.cn");
				}
			}
			else
			{
				if (1==nCh)
				{
					equate=strValueTemp1.Left(nPosDiv);
				}
				else if (2==nCh)
				{
					equate=strValueTemp1.Right(strValueTemp1.GetLength()-nPosDiv-1);
				}
			}
		}

		return TRUE;
	}

	//����regplace
	if(src.Find(_T("regplace"),0) != -1)
	{
		for (size_t i=0; i<tRapalceDataMsg.tParamItemTable.size(); i++)
		{
			if (tRapalceDataMsg.tParamItemTable[i].Key.ToLower() == src)
			{
				bFindFlag1=TRUE;
				strValueTemp1=tRapalceDataMsg.tParamItemTable[i].Value;
				i=tRapalceDataMsg.tParamItemTable.size();
			}
		}

		if (bFindFlag1)
		{
			equate=strValueTemp1;
		}
		else
		{
			equate=_T("����");
		}

		return TRUE;
	}

	//����aaddr��ϸ��ַ
	if (src.Find(_T("aaddr-"),0) != -1)
	{
		strValueTemp1.Empty();

		for (size_t i=0; i<tRapalceDataMsg.tParamItemTable.size(); i++)
		{
			if (tRapalceDataMsg.tParamItemTable[i].Key.ToLower()==_T("aaddr"))
			{
				strValueTemp1=tRapalceDataMsg.tParamItemTable[i].Value;

				i=tRapalceDataMsg.tParamItemTable.size();
			}
		}

		if (strValueTemp1.IsEmpty())
		{
			equate=_T("");
		}
		else
		{
			int iPos = -1;
			CStdString tmp;
			equate = strValueTemp1;
			tmp = strCity + _T("��");
			if ((iPos =(equate.ReverseFind(tmp)))!= -1)
			{
				equate = equate.Mid(iPos+tmp.length());
			}
			else
			{
				if ((iPos =(equate.ReverseFind(strCity)))!= -1)
				{
					equate = equate.Mid(iPos + strCity.length() + 1);
				}
				else
				{
					if ((iPos =(equate.ReverseFind(_T("ʡ"))))!= -1)
					{
						equate = equate.Mid(iPos + 1);
					}
					else
					{
						if ((iPos =(equate.ReverseFind(strProv)))!= -1)
						{
							equate = equate.Mid(iPos + strProv.length()+ 1);
						}
					}
				}
			}
			
			tmp = src.Right(1);//��ȡ���ұ����ֽ����ж�
			if (0 == (tmp.CompareNoCase(_T("l"))))
			{
				equate = strValueTemp1;
			}

		}

		return TRUE;
	}
	//�ж����һλ�ǲ�������
	if (_T('0')<=ch && ch<=_T('9'))
	{
		if (temp ==_T("csel"))
		{
			val.Empty();
			PCHAR  pChar = NULL;

			if (0 != tRapalceDataMsg.nNetItemCode)
			{
				if ((tRapalceDataMsg.eTaskType == ETTypeRegaccount) || (tRapalceDataMsg.eTaskType == ETTypeReRegaccount))
				{
					pChar = m_NetData.GetNSIparamater_Register(tRapalceDataMsg.nNetItemCode);
				}


				if (pChar == NULL)
				{
					pChar = m_NetData.GetNSIparamater_Post(tRapalceDataMsg.nNetItemCode);
				}
			}
			if (pChar == NULL)
			{
				if ((tRapalceDataMsg.eTaskType == ETTypeRegaccount) || (tRapalceDataMsg.eTaskType == ETTypeReRegaccount))
				{
					pChar = m_NetData.GetNSIparamater_Register(m_NetData.GetItemCode_Reg());
				}

				if (pChar == NULL)
				{
					pChar = m_NetData.GetNSIparamater_Post(m_NetData.GetItemCode_Post());
				}
				
			}
			m_NetData.ToMByteToWChar(pChar, val);

			if (val.IsEmpty())
			{
				//��ֵ
				equate=_T("");
				g_sysLog.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("ȡ��ҵ��Ϣʧ��,��ĿID:%s!"),
									tRapalceDataMsg.strNetCode.c_str());
			}
			else
			{
				ch -= 0x30;
				AddDefaultSubStr(val, _T("<@@>"), sa);
				if (sa.size()-1<ch)
				{
					equate=_T("");
				}
				else
				{
					equate = sa[ch];
				}
			}
			return TRUE;
		}
		else if(src.Find(_T("factorybrand")) != -1
			|| src.Find(_T("factorypdt")) != -1
			|| src.Find(_T("infok")) != -1
			|| src.Find(_T("sumpic")) != -1)
		{
			for (size_t i=0; i<tRapalceDataMsg.tParamItemTable.size(); i++)
			{
				if (tRapalceDataMsg.tParamItemTable[i].Key.ToLower()==src)
				{
					equate=tRapalceDataMsg.tParamItemTable[i].Value;
					i=tRapalceDataMsg.tParamItemTable.size();
				}
			}

			if ((src.Find(_T("infok")) != -1 
				|| src.Find(_T("factorypdt")) != -1) 
				&& (equate.IsEmpty()))
			{
				T_LocalGlobalData *pGlobalData = NULL;
				pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
					E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
				int iLen = _ttoi(src.Right(1));
				if (pGlobalData->vCompany.size() <= 1) 
				{
					g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("���ڹ�˾���Ϊ�գ����Թؼ����滻Ϊ��"));
					equate = _T("");
					return TRUE;
				}
				if(iLen < pGlobalData->vCompany.size())
				{
					equate = pGlobalData->vCompany[iLen];
				}
				
			}

			return TRUE;
		}
		else
		{
			vtype = KeyToVal(temp);
			if (vtype==0)
			{
				return FALSE;
			}
			for (size_t i=0; i<tRapalceDataMsg.tParamItemTable.size(); i++)
			{
				if (tRapalceDataMsg.tParamItemTable[i].Key.ToLower()==temp)
				{
					bFindFlag1=TRUE;
					strValueTemp1=tRapalceDataMsg.tParamItemTable[i].Value;
					i=tRapalceDataMsg.tParamItemTable.size();
				}
			}
			if (bFindFlag1 && !strValueTemp1.IsEmpty())
			{
				m_NetData.GetNSVrealvalue(vtype, strValueTemp1, val);
			}
		}
		
		if (val.IsEmpty())
		{	
			GetParamValue(_ttoi(strValueTemp1), vtype, tRapalceDataMsg.dwNetLanType, val);
			if (!val.IsEmpty())
			{
				equate = val;
			}
			else
			{
				equate = _T("");
				if ((temp == _T("pprov")) && (_T('0') == ch))
				{
					equate = _T("�㶫");
				}
				//û������һ�����ȫĬ�����ڣ�Ҳ����random
				if((temp==_T("ccity")))
				{
					equate = _T("������");

				    g_sysLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("����ֵΪ�գ�Ĭ�ϸ�ֵ������"));

				}
				if ((temp == _T("aarea")))      //zhl 140526
				{
					equate = _T("������");
				}
			}

			return TRUE;
		}
		else
		{
			ch -= 0x30;
			AddDefaultSubStr(val, _T("<@@>"), sa);
			if (sa.size()-1<ch)
			{
				equate=_T(" ");
			}
			else
			{
				equate = sa[ch];
			}
			
		}
		return TRUE;
	}
	else
	{
		BOOL bRepled = FALSE;

		if (!src.Compare(_T("pprov")) ||
			!src.Compare(_T("ccity")) ||
			!src.Compare(_T("aarea")) ||
			!src.Compare(_T("ssex")) ||
			!src.Compare(_T("companykind")) ||
			!src.Compare(_T("expire")) ||
			!src.Compare(_T("ccountry")) ||
			!src.Compare(_T("ecountry")) ||
			!src.Compare(_T("personscount")) ||
			!src.Compare(_T("moneyinfo")) )
		{
			//�����һЩ���εı������д���
			if(DealVarible(src, equate, tRapalceDataMsg.tParamItemTable, tRapalceDataMsg.dwNetLanType))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		temp = src;        //ͨ�����br  ��ʾ��Ҫ�ѿո�ͻ��з�ת��html���
		int iPos = -1;
		if ((iPos = temp.Find(_T("-br"))) != -1)
		{
			temp = temp.Left(iPos);
		}
		
		//�Ƿ�Ҫ��������ת��������
		if (src.Find(_T("MoneyInfo-"),0) != -1)
		{
			CStdString strNumber;

			for (size_t i=0; i<tRapalceDataMsg.tParamItemTable.size(); i++)
			{
				if (tRapalceDataMsg.tParamItemTable[i].Key.ToLower()==temp)
				{
					bFindFlag1=TRUE;
					strValueTemp1=tRapalceDataMsg.tParamItemTable[i].Value;
					i=tRapalceDataMsg.tParamItemTable.size();
				}
			}

			GetParamValue(_ttoi(strValueTemp1),30,tRapalceDataMsg.dwNetLanType,equate);
			
			//�Ƿ�Ҫ����ת��������
			if (src.Find(_T("MoneyInfo-N"),0) != -1)
			{
				equate.Replace(_T("��"), _T("0000"));
			}
			
			int iPos = 0;
			iPos = equate.ReverseFind(_T("-"));
			if (iPos != 0)
			{
			  equate = equate.Right(equate.length()-iPos-1);
			}
			for (int i=0; i<equate.length(); i++)
			{
				if ( 0 != isdigit(equate[i]))
				{
					strNumber += equate[i];
				}
			}

			equate = strNumber;

			return TRUE;
		}

		//������MMailPass��NName��NNameMail��ͨ���
		for (size_t i=0; i<tRapalceDataMsg.tParamItemTable.size(); i++)
		{
			if (tRapalceDataMsg.tParamItemTable[i].Key.ToLower()==temp)
			{
				equate=tRapalceDataMsg.tParamItemTable[i].Value;
				bRepled=TRUE;
				i=tRapalceDataMsg.tParamItemTable.size();
			}
		}

		//��Ҫ�滻�ո�ͻ��з�
		if (iPos != -1)
		{
			equate.Replace(_T(" "), _T("&nbsp;"));
			equate.Replace(_T("\r\n"), _T("<br/>"));
		}

		//�������ֻ���ڴӱ������ݿ����滻�û���,����,����,��˾����
		if ((m_eCurrentTaskType==ETTypeRegaccount) ||( m_eCurrentTaskType == ETypeRegEmail) || (m_eCurrentTaskType == ETTypeReRegaccount))
		{
			//NName, PPass, MMail, CComp
			if (!src.Compare(_T("nname")))
			{
				if(tRapalceDataMsg.strDefaultName.IsEmpty()) 
				{
					equate.Empty();
					StringUtils baseStr;
					baseStr.CreateUserName2(equate, tRapalceDataMsg.eUserType);

					CheckRegCount(equate, tRapalceDataMsg.dNameSize,  tRapalceDataMsg.eUserType);
				}
				else
				{
					equate = tRapalceDataMsg.strDefaultName;
				}
				bRepled=TRUE;
			}
			else if (!src.Compare(_T("nnamemail")))
			{
				if (!equate.IsEmpty())
				{
					tRapalceDataMsg.strWebName=equate;
					tRapalceDataMsg.strDefaultName = equate;
				}
			}
			
			if (!src.Compare(_T("ppass")))
			{
				if(tRapalceDataMsg.strDefaultPass.IsEmpty())
				{
					equate.Empty();
					StringUtils baseStr;
					baseStr.RandStr(tRapalceDataMsg.dPsdSize, tRapalceDataMsg.ePsdType, equate);

				}
				else
				{
					equate = tRapalceDataMsg.strDefaultPass;
				}
				
				bRepled=TRUE;
			}
		}

		if (!bRepled)
		{
			//�����ﻹҪ��һЩ�ݴ���: �ҷ��������src�п��ܲ���������ͨ���
			if (IsAimStr(src))
			{
				tRapalceDataMsg.vNoRepleaceParam.push_back(strNoRep);
				g_unknownParamLog.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,
										_T("δ�滻ͨ���:%s"), src.GetBuffer(src.GetLength()));
				src.ReleaseBuffer(src.GetLength());
			}
		}

		if (bRepled)
		{
			return TRUE;
		}
	}

	return FALSE;
}

//����ͨ���ȡ�������С����
void CPrepareData::GetMaxMin(const CStdString &strIn, int &iMax, int &iMin )
{
	int iPos1,iPos2;         //�ֱ𱣴�����ַ�λ��

	iMax = iMin = 0;
	iPos1 = strIn.Find('-');
	iPos2 = strIn.Find(':');

	if (iPos1 == -1
		|| (iPos1 != -1 && iPos2 != -1 && iPos2 < iPos1))  //������ͨ���
		return;

	CString strTemp;
	if (iPos2 == -1)  //ֻ���������
	{
		strTemp = strIn.Mid(iPos1+1);
		if (!strTemp.IsEmpty())
		{
			iMax = _ttoi(strTemp);
		}
	}
	else  //�������С����
	{
		strTemp = strIn.Mid(iPos1+1, iPos2-iPos1-1);
		if (!strTemp.IsEmpty())
		{
			iMin = _ttoi(strTemp);
		}

		strTemp = strIn.Mid(iPos2+1, strIn.GetLength()- iPos2 - 1);
		if (!strTemp.IsEmpty())
		{
			iMax = _ttoi(strTemp);
		}
	}

	if (iMax < iMin)   //������ͨ���
		iMin = iMax = 0;
}

BOOL CPrepareData::DealVarible(CStdString &strIn, CStdString &strOut, 
							   std::vector<TParamItem>& tSearchdata, DWORD nNetLanType)
{
	CStdString sel;
	int ntype=0;
	int nPosFind=-1;
	CStdString strValueTemp1;
	BOOL bFindFlag=FALSE;
	CStdString strOutTemp;
	CStdString strOutTemp1;
	int nRetValue=0;

	for (size_t i=0; i<tSearchdata.size(); i++)
	{
		if (tSearchdata[i].Key.ToLower()==strIn)
		{
			bFindFlag=TRUE;
			strValueTemp1=tSearchdata[i].Value;
			i=tSearchdata.size();
		}
	}

	ntype = KeyToVal(strIn);

	if (ntype==0)
	{

		g_sysLog.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, 
			_T("KeyToVal(%s) Failed!"), strIn.GetBuffer(strIn.GetLength()));

		return FALSE;
	}

	
	if (bFindFlag && !strValueTemp1.IsEmpty())
	{
		m_NetData.GetNSVrealvalue(ntype, strValueTemp1, strOutTemp);
	}
	else
	{

		g_sysLog.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, 
							_T("tSearchdata key not bFindFlag !"));

	}

	if (strOutTemp.GetLength()>0)
	{
		nPosFind=strOutTemp.Find(_T("@@"), 0);

		if (strIn == _T("ccity"))
		{
			if (nPosFind>0)
			{
				strOut=strOutTemp.Right(strOutTemp.GetLength()- nPosFind - 3);
			}
			else
			{
				strOut=strOutTemp;
			}
		}
		else
		{
			if (nPosFind>0)
			{
				strOut=strOutTemp.Left(nPosFind);
			}
			else
			{
				strOut=strOutTemp;
			}
		}
		
		return TRUE;
	}
	else
	{
		GetParamValue(_ttoi(strValueTemp1), ntype, nNetLanType, strOut);
		if(strIn == _T("moneyinfo"))
		{
			int iPos = 0;
			CStdString strNumber;
			iPos = strOut.ReverseFind(_T("-"));
			if(iPos != 0)
			{
				strOut = strOut.Right(strOut.length() - iPos -1);
			}
			for (int i=0; i<strOut.length(); i++)
			{
				if ( 0 != isdigit(strOut[i]))
				{
					strNumber += strOut[i];
				}
			}

			strOut = strNumber;
		}

		return TRUE;
	}
	g_sysLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("KeyToVal Sql:%s!"), 
						sel.GetBuffer(sel.GetLength()));

	
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// ���������������õ�ĳ��ʱ���ʽ�ĵ�ǰʱ���ַ�����
// ���룺strTimeForMat��ͬ��ʱ���ʽ
// �������ʱ���ʽ�ĵ�ǰʱ��
// ����ֵ��
// ����˵����ʱ���ʽĿǰ�����¼���
/*
%FullDateyyyymmdd%    19990628
%FullDateyyyy\/mm\/dd%   1999/06/28 /2
%FullDateyyyy\-mm\-dd%   1999-06-28
%FullDatemm\-dd\-yyyy%   06-28-1999
%FullDatedd\-mm\-yyyy%   28-06-1999
%FullDatemm\/dd\/yyyy%   06/28/1999
%FullDatedd\/mm\/yyyy%   28/06/1999
%FullDateyymmdd>-n%      990628
%FullDateyy\/mm\/dd%     99/06/28
%FullDateyy\-mm\-dd%     99-06-28
%FullDatemm\-dd\-yy>-n%  06-28-99
%FullDatedd\-mm\-yy%     28-06-99
%FullDatemm\/dd\/yy%     06/28/99
%FullDatedd\/mm\/yy%     28/06/99
%FullDateyyyy\.mm\.dd%   1999.06.28
%FullDatedd\.mm\.yyyy%   28.06.1999
%FullDatemm\.dd\.yyyy%   06.28.1999
*/
//
///////////////////////////////////////////////////////////////////////////////
DWORD CPrepareData::GetCurrentTimeByFormats(const CStdString strTimeFormat, CStdString &strTimeOut)
{
	time_t timep; 
	struct tm *pTimeTemp;
	time(&timep);
	pTimeTemp=gmtime(&timep);

	CStdString strValue = _T("");
	//ƽ���һ������
	int yearDayNum = 365;


	CStdString strTimeTemp=strTimeFormat;

	if(strTimeTemp.Find(_T("fulldate")) == -1)
	{
		return 1;
	}
	int nPos=-1;
	int n4Year;
	int n2Year;
	int nMonth;
	int nDay;

	n4Year = pTimeTemp->tm_year+1900;
	n2Year = n4Year%100;
	nMonth = 1+pTimeTemp->tm_mon;
	nDay = pTimeTemp->tm_mday;

	//��һ��������
	if ((0==n4Year%4&&0!=n4Year%100)||(0==n4Year%400))
	{
		yearDayNum = 366;
	}

	nPos=strTimeTemp.Find(_T(">"), 0);
	//��Ҫ������ +- �����
	if (-1 != nPos && nPos<strTimeTemp.GetLength())
	{
		int monthDay = 0;
		for (int i=1;i<nMonth;++i)
		{
			monthDay += GetDateNum(i,n4Year);
		}
		int totalDay = n4Year*yearDayNum+monthDay+nDay;//�����������
		int changeDay = _ttoi(strTimeTemp.Right(strTimeTemp.GetLength()-nPos-2));//Ҫ�ı������
		TCHAR ch = strTimeTemp.GetAt(nPos+1);
		if (_T('+') ==ch)
		{
			totalDay += changeDay;//��������
		}
		else if (_T('-') ==ch)
		{
			totalDay -= changeDay;//��������
		}
		n4Year = totalDay/yearDayNum;
		n2Year = n4Year%100;
		int dayCount = totalDay%yearDayNum;
		int monthStart = 1;
		int monthDayNum = 0;

		monthDayNum = GetDateNum(monthStart,n4Year);
		for (int k=0;k<12;++k)
		{
			if (dayCount<=monthDayNum)
			{
				nDay = dayCount;
				nMonth = monthStart;
				break;
			}
			else
			{
				dayCount -= monthDayNum;
				++monthStart;
				monthDayNum = GetDateNum(monthStart,n4Year);
			}
		}

		strTimeTemp = strTimeTemp.Left(nPos);
	}

	strTimeTemp.Replace(_T("fulldate"),_T(""));
	strValue.Format(_T("%04d"),n4Year);
	strTimeTemp.Replace(_T("yyyy"),strValue);
	strValue.Format(_T("%02d"),n2Year);
	strTimeTemp.Replace(_T("yy"),strValue);
	strValue.Format(_T("%02d"),nMonth);
	strTimeTemp.Replace(_T("mm"),strValue);
	//������m
	strTimeTemp.Replace(_T("m"),strValue);
	strValue.Format(_T("%02d"),nDay);
	strTimeTemp.Replace(_T("dd"),strValue);
	//������d
	strTimeTemp.Replace(_T("d"),strValue);

	strTimeTemp.Replace(_T("\\"),_T(""));
	strTimeOut = strTimeTemp;

	return 0;
}
int CPrepareData::GetDateNum(int nMouth, int nYear)
{
	int nDateNum=0;

	switch (nMouth)
	{
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		nDateNum = 31;
		break;
	case 4: case 6: case 9: case 11:
		nDateNum = 30;
		break;
	case 2:
		if ((nYear%4==0 && nYear%100!=0) ||(nYear%400==0))
		{
			nDateNum = 29;
		}
		else
			nDateNum = 28;
		break;
	default: break;
	}
	return nDateNum;
}


//���ע���õ����ʻ���Ϣ�����б�Ҫ������Ӧ�޸�
DWORD CPrepareData::CheckRegCount(CStdString &strInAndOut, DWORD dLen, 
								   E_Account_Style eFirstFlag)
{
	static BOOL s_bShowMsg = FALSE;
	CStdString strBack = strInAndOut;
	
	if (strInAndOut.IsEmpty() || strInAndOut.size()<dLen)
	{
		StringUtils baseStr;
		if (eFirstFlag == asONLY_CHARACTER)
		{
			//ȫ�ַ���
			baseStr.RandStr(dLen, psALL_LOWER_CHART, strInAndOut);
		}
		else if (eFirstFlag == asFIRST_CHINESE)
		{
			//�������ĵ�
			baseStr.RandStr(dLen, psALL_CHINESE, strInAndOut);
		}
		else
		{
			//Ĭ��
			baseStr.RandStr(dLen, psLOWER_CHART_AND_DIGITAL, strInAndOut);
		}		
	}

	//����һλ
	CheckFirstChar(strInAndOut, eFirstFlag);

	return 0;
}



//����ַ�����һλ�ǲ�����Ҫ�����ͣ����ǣ��Զ��޸�
DWORD CPrepareData::CheckFirstChar(CStdString &strInAndOut, E_Account_Style eFlag)
{
	StringUtils baseStr;
	TCHAR chCmp[5] = {0};
	CStdString strRand;

	if (strInAndOut.GetLength()==0)
	{

		g_sysLog.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, _T("strInAndOut==0!"));

		baseStr.RandStr(6, psALL_LOWER_CHART, strInAndOut);
		return 0;
	}

	chCmp[0] = strInAndOut.GetAt(0);
	
	switch(eFlag)
	{
	case asFIRST_CHAR:
	case asNOT_INCLUDE_CHINESE:
		if(!baseStr.IsAlpha(chCmp))
		{
			baseStr.RandStr(2, psALL_LOWER_CHART, strRand);
			strInAndOut.SetAt(0, strRand.GetAt(0));
		}
		break;
	case asFIRST_DIGITAL:
		if(!baseStr.IsNumeric(chCmp))
		{
			baseStr.RandStr(2, psALL_DIGITAL, strRand);
			strInAndOut.SetAt(0, strRand.GetAt(0));
		}
		break;
	case asFIRST_CHINESE:
		strInAndOut.SetAt(0, _T('��'));
		break;
	case asONLY_CHARACTER:
		if (!baseStr.IsAlpha(strInAndOut.c_str()))
		{
			baseStr.CreateUserName2(strInAndOut, asONLY_CHARACTER);
		}
	default:
		break;
	}

	return 0;
}

void CPrepareData::AddDefaultSubStr(const CStdString& strIn, const CStdString& strDelimiter, 
									std::vector<CStdString>& strOut)
{
	
    StringUtils baseStr;
	if (strIn.Find(strDelimiter)==-1)
	{
		strOut.push_back(strIn);
		return;
	}
    baseStr.SplitString(strIn, strDelimiter, strOut);
}


BOOL CPrepareData::IsAimStr(CStdString str)
{
	TCHAR chCurr;
    if (!str.Compare(_T("CrLf")))
    {
        return FALSE;
    }
    
    int nLen = str.GetLength();
    
    for (int i = 0; i < nLen; i++)
    {
        chCurr = str.GetAt(i);
        
        if (chCurr >= _T('a') && chCurr <= _T('z') || 
            chCurr >= _T('A') && chCurr <= _T('Z') || 
            chCurr >= _T('0') && chCurr <= _T('9') )
        {
        }
        else
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

BOOL CPrepareData::IsChar(TCHAR ch)
{
    if ((_T('a')<=ch&&ch<=_T('z'))||((_T('A')<=ch&&ch<=_T('Z')))) 
	{
        return TRUE;
    }

    return FALSE;
}

int CPrepareData::KeyToVal(CStdString& temp)
{
    if (temp == _T("pprov") || temp == _T("eprov"))
	{
        return 1;
    }
	else if (temp == _T("ssex"))
	{
        return 2;
    }
	else if (temp == _T("companykind"))
	{
        return 4;
    }
	else if (temp == _T("ccity") || temp == _T("ecity"))
	{
        return 14;
    }
	else if (temp == _T("expire"))
	{
        return 26;
    }
	else if (temp == _T("ccountry") || temp == _T("ecountry"))
	{
        return 28;
    }
	else if (temp == _T("personscount"))
	{
        return 29;
    }
	else if (temp == _T("moneyinfo"))
	{
        return 30;
    }
	else if (temp == _T("aarea"))					//zhl 20140521
	{
		return 31;
	}

    return 0;
}

//����:�����Body��ֵ�г���Ҫ��ĺ���
int CPrepareData::TrimLenFun(CStdString strIn, CStdString& strOut, int nTrimLen,
			   E_PPD_TRIM_TYPE eType)
{
	LimitedStr(strIn, strOut, PPD_BBodyLen_TYPE, _T(""), nTrimLen);
	return 0;
}

void CPrepareData::DecodeStationData( const CStdString &strCipher , CStdString &strPlain )
{
	USES_CONVERSION;
	string strTemp = W2A(strCipher);
	string m_strKey = "SUMINFO";

	string tmp = m_Base64.DecryptText(strTemp , m_strKey);
	
	int len = 0;
	len = tmp.length();
	int iUnicodeLen = MultiByteToWideChar(CP_ACP,0,tmp.c_str(),-1,NULL,0);

	wchar_t *pUnicodeTmp;
	pUnicodeTmp = new	wchar_t[iUnicodeLen + 1];
	memset(pUnicodeTmp,0,(iUnicodeLen +1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP,0,tmp.c_str(),-1,pUnicodeTmp,iUnicodeLen);

	strPlain = pUnicodeTmp;

	delete []pUnicodeTmp;
	
}


/*
		@breif ����ͨ���ȡ�ÿ�͸�
		@param strTongPei  ͨ���
		@param [out] iWidth  ��
		@param [out] iHeight ��
	*/
void CPrepareData::GetWidthAndHeight(const CStdString &strTongPei, int &iWidth, int &iHeight)
{
	iWidth = 0;
	iHeight = 0;

	int iPos1 = strTongPei.Find(_T("-"));
	int iPos2 = strTongPei.Find(_T(":"));

	if (iPos1 != -1 && iPos2 != -1 && iPos2 > iPos1)
	{
		iWidth = _ttoi(strTongPei.Mid(iPos1+1, iPos2-iPos1-1).c_str());
		iHeight = _ttoi(strTongPei.Mid(iPos2+1, strTongPei.GetLength()-iPos2-1).c_str());
	}
}

/*
		@breif �ı�ͼƬ��С
		@param strTongPei  ͨ���
		@param [out] strImage  ��ͼƬ·��
		@param dwFlag :0,��Ʒ��1��ִ�գ�2����˾;3,��֯��������֤�� 4��˰��Ǽ�֤
		@param iIndex :����ڼ��Ų�ƷͼƬ��
		@return TRUEΪ�ɹ�
	*/
BOOL CPrepareData::GetResizeImage(const CStdString &strTongPei, CStdString &strImage, DWORD dwFlag,int iIndex)
{
	static int Index = 0;
	TCHAR szPath[MAX_PATH] = {0};

 	::GetModuleFileName(NULL, szPath, MAX_PATH);
 	PathAppend(szPath, _T("..\\..\\"));
 	PathAppend(szPath, _T("image\\user\\"));
	T_LocalGlobalData *pGlobalData = NULL;
	pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
	CStdString  strSrcFile;
	if (dwFlag == 0 && iIndex >= 0)
	{
		if (pGlobalData->vProductImageName.size() <= iIndex)
		{
			return FALSE;
		}
		strSrcFile = (CStdString)pGlobalData->vProductImageName[iIndex];
		//strImage = (CStdString)pGlobalData->vProductImageName[iIndex];
		strImage.Format(_T("%simage%d.jpg"), szPath,Index);
	}
	else if (dwFlag == 1)
	{
		strSrcFile = (CStdString)pGlobalData->szTradeImageName;
		strImage.Format(_T("%stradcert%d.jpg"), szPath,Index);
		//strImage =  (CStdString)pGlobalData->szTradeImageName;
	}
	else if (dwFlag == 2)
	{
		strSrcFile = (CStdString)pGlobalData->szCompImageName;
		strImage.Format(_T("%scomplogo%d.jpg"), szPath,Index);
	//	strImage = (CStdString)pGlobalData->szCompImageName;
	}
	else if (dwFlag == 3)
	{
		strSrcFile = (CStdString)pGlobalData->szOrgImageName;
		strImage.Format(_T("%sorgcodecert%d.jpg"), szPath, Index);
	}
	else if (dwFlag == 4)
	{
		strSrcFile = (CStdString)pGlobalData->szTaxImageName;
		strImage.Format(_T("%staxregcert%d.jpg"), szPath, Index);
	}

	CImage  imgOld;
	CImage  imgNew;

	if (SUCCEEDED(imgOld.Load(strSrcFile.c_str())))
	{
		int iWidth;
		int iHeight;

		GetWidthAndHeight(strTongPei, iWidth, iHeight);
		if (iWidth > 0 && iHeight > 0)
		{
			imgNew.Create(iWidth, iHeight, 32, 0);
			RECT rc;
			rc.top = 0;
			rc.left = 0;
			rc.right = iWidth;
			rc.bottom = iHeight;

			SetStretchBltMode(imgNew.GetDC(),STRETCH_HALFTONE);
			imgNew.ReleaseDC();
			imgOld.StretchBlt(imgNew.GetDC(), rc);
			imgNew.ReleaseDC();

			if (SUCCEEDED(imgNew.Save(strImage.c_str(), Gdiplus::ImageFormatJPEG)))
			{
				++Index;
				return TRUE;
			}
		}
	}

	return FALSE;
}

//�滻�ں˴����ͨ���
void CPrepareData::ReplaceSumengineParam( CStdString& strData )
{
	strData.Replace(_T("%VLFirst%"),_T("#VLFirst#"));
	strData.Replace(_T("%VLLast%"),_T("#VLLast#"));
	strData.Replace(_T("%VLRandom%"),_T("#VLRandom#"));
	strData.Replace(_T("%VLAll%"),_T("#VLAll#"));
	strData.Replace(_T("%imgcap%"),_T("#imgcap#"));
	strData.Replace(_T("%random%"),_T("#random#"));
	strData.Replace(_T("%GetPopupInfo%"),_T("#GetPopupInfo#"));
	strData.Replace(_T("%LoopIndex%"),_T("#LoopIndex#"));
	strData.Replace(_T("%LoopIndexMoreOne%"),_T("#LoopIndexMoreOne#"));
	strData.Replace(_T("%LoopIndexLessOne%"),_T("#LoopIndexLessOne#"));
	strData.Replace(_T("%VLIndex%"),_T("#VLIndex#"));
	strData.Replace(_T("%OcrParam%"),_T("#OcrParam#"));
}

//�滻���û���滻��ͨ���
void CPrepareData::ReplacePercent(CStdString& strOut, std::vector<CStdString> &vNoRepalace)
{
	int iCnt = 0;
	std::vector<CStdString>::iterator iter;
	for (iter = m_vstrParam.begin(); iter != m_vstrParam.end(); ++iter)
	{
		for (int i = 0; i< vNoRepalace.size(); ++i)
		{
			CStdString tmp = vNoRepalace[i];
			//�����Ϊ��Сд�������û���滻�ɹ��������滻�ɿգ�
			if (((*iter).ToLower()).Find(tmp.ToLower()) != -1)
			{
				(*iter) = _T("%") + vNoRepalace[i] + _T("%");
				break;
			}
		}
		iCnt = strOut.Replace(*iter,_T(""));
		if (iCnt > 0)
		{
			g_sysLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("�滻δ�����ͨ���:%s"),(*iter).c_str());
		}
	}
}

//��������ͨ�����
void CPrepareData::LoadAllParam()
{
	StringUtils strUtil;
	m_vstrParam.clear();
	strUtil.SplitString(PARAM,_T(","),m_vstrParam,true);

}

/*
@brief ͼƬת��
@param strImagePath; Ҫת����ͼƬ��ַ��
@param dwType;    ת������ (1.jpg,2.png,3.bmp,4.gif)
*/
BOOL CPrepareData::ImageConvert( CStdString &strSrcPath, DWORD dwType )
{
	CStdString strTmp = _T("");
	CStdString strFileName = _T("");
	CStdString strExtension = _T("");
	CImage img;
	HRESULT hr;

	strTmp = strSrcPath;
	int iPotPos  = strSrcPath.Find(_T("."));
	strFileName = strSrcPath.Left(iPotPos);
	img.Load(strSrcPath);
	//����ת����ʽ
	switch (dwType)
	{
	case 1:
		strExtension = _T(".jpg");
		break;
	case 2:
		strExtension = _T(".png");
		break;
	case 3:
		strExtension = _T(".bmp");
		break;
	case 4:
		strExtension = _T(".gif");
		break;
	default:
		break;

	}

	strSrcPath = strFileName + strExtension;

	hr = img.Save(strSrcPath);
	if (FAILED(hr))
	{
		strSrcPath = strTmp; //��ԭ����ͼƬ
		g_sysLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("ת��ͼƬ����(%d)ʧ��:%s"),dwType,strSrcPath.c_str());
		return FALSE;
	}

	return TRUE;

}

/*
@brief DES������֤�����
@param iNetID  ��վID
@param strEncryptData ��������
@return ���ܺ������
*/
CStdString CPrepareData::DesImageCode(const int &iNetID, CStdString& strEncryptData)
{
	long lKey = iNetID + 20120807;
	CString strDest =strEncryptData;
	CString strSour = _T("");
	CString strKey = _T("");
	strKey.Format(_T("%d"), lKey);
	des.Decrypt(strSour, strDest, strKey);

	return strSour;
}
