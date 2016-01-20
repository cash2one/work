#include "StdAfx.h"
#include ".\sysparam.h"
#include "shlobj.h"

CSysparam::CSysparam(void)
{
	
}

CSysparam::~CSysparam(void)
{
}


//����:������滻ֵ������Ҫ��(��������,���ȵ�����һ���ط�����)����
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
	case PPD_BBodyC_TYPE://����%BBody-C%,������ϵ��ʽ�ķ�����������
		{
			LimitedStr(strIn, strOut, PPD_BBodyC_TYPE, _T(""), 0);
			break;
		}
	case PPD_BBodyCI_TYPE://����%BBody-CI%,���ƹ�˾���ƺ���ϵ��ʽ
		{	
			LimitedStr(strOut, strOut, PPD_BBodyCI_TYPE, 
						_T(""), 0);
			break;
		}
	case PPD_BBodyE_TYPE://����%BBody-E,���ƻس����ķ�����������
		{
			LimitedStr(strIn, strOut, PPD_BBodyE_TYPE, _T("\r\n"), 0);
			break;
		}
	case PPD_WWebsH_TYPE://����%WWebs-H%,����Http��ͷ����ҳ��ַ
		{
			LimitedStr(strIn, strOut, PPD_WWebsH_TYPE, _T("http://"), 0);
			break;
		}
	case PPD_BBodyP_TYPE://����%BBody-P,����ͼƬ������
		{
			T_LocalGlobalData *pGlobalData = NULL;
			pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
				E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
			ASSERT(pGlobalData != NULL);

			std::vector<CStdString> result;
			StringUtils strUtil;
			int iCount = strUtil.SplitString(pGlobalData->TaskProductInfo.szPhototUrl,_T("(;7)"),result,false);
			int PicIndex =  strUtil.GetRand(0,iCount);  //��ȡ���

			CStdString strTmp = _T("<img src =\"") + result[PicIndex] + _T("\">") + _T("\r\n\r\n")+ strIn;

			strOut = strTmp;
		}
	default:
		{
			break;
		}
	}

	return 0;
}

//����:��ȡϵͳ��Ϣ
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

	//%CComp%:��˾����.
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CComp");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCompanName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));
	if (pGlobalData->vCompany.size() <= 0)
	{
		pGlobalData->vCompany.push_back(strValue);
	}

	//%CCompshortname%:��˾���.
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
				case 1:
					{
						strKey.Empty();
						strValue.Empty();
						strKey=_T("CCompshortname");
						strValue=strTmp;
						tParamItemTable.push_back(TParamItem(strKey, strValue));
					}
				case 2: case 3: case 4: case 5:
					{
						//��Ӷ����˾����
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

	//%CInfo%:��˾���
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CInfo");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCCINFO;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PersonsCount%:��˾��ģ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PersonsCount");
	strValue=pGlobalData->aSystemInfo.systemInfo.szScale;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PersonsCount-N%:��˾��ģ ������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PersonsCount-N");
	strValue=pGlobalData->aSystemInfo.systemInfo.szScale;
	strValue = GetNumberFromString(strValue);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CompanyKind%:��ҵ����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CompanyKind");
	strValue=pGlobalData->aSystemInfo.systemInfo.szProperty;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MoneyInfo%:ע���ʽ�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MoneyInfo");
	strValue=pGlobalData->aSystemInfo.systemInfo.szFund;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MoneyInfo-N%:ע���ʽ�(��������ת��������0000)
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MoneyInfo-N");
	strValue=pGlobalData->aSystemInfo.systemInfo.szFund;
	strValue = GetNumberFromString(strValue);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MoneyInfo-M%:ע���ʽ�(��ȥ������)
	strKey.Empty();
	strValue.Empty();
	strKey = _T("MoneyInfo-M");
	strValue = pGlobalData->aSystemInfo.systemInfo.szFund;
	strValue = GetNumberFromString(strValue);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CompanyIndustry%:��˾������ҵ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CompanyTrade");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTrade;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Company place%:��˾ע�����ڵ�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegPlace");
	strValue=pGlobalData->aSystemInfo.systemInfo.szAddress;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegTime%:ע��ʱ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegTime");
	strValue=pGlobalData->aSystemInfo.systemInfo.szDate;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegTime%:ע��ʱ�� ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegTime-Y");
	strValue=pGlobalData->aSystemInfo.systemInfo.szDate;
	strValue = GetDateFromString(strValue, D_YEAR);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegTime%:ע��ʱ��   ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegTime-M");
	strValue=pGlobalData->aSystemInfo.systemInfo.szDate;
	strValue = GetDateFromString(strValue, D_MONTH);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegTime%:ע��ʱ�� ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RegTime-D");
	strValue=pGlobalData->aSystemInfo.systemInfo.szDate;
	strValue = GetDateFromString(strValue, D_DAY);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CPrincipal%%:���˴���
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CPrincipal");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCompanOwner;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ECinfo%:Ӣ�Ĺ�˾����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ECinfo");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCCINFO;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%EComp%:Ӣ�Ĺ�˾��
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
				//%FactoryBrand%:��˾Ʒ��
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
	

	//%CPhilosophy%:��Ӫ����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CPhilosophy");
	strValue=pGlobalData->aSystemInfo.systemInfo.szIdea;
	if (strValue.IsEmpty())
		strValue = _T("����Ϊ����������һ");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CKouhao%:��˾�ں�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CKouhao");
	strValue=pGlobalData->aSystemInfo.systemInfo.szSlogan;
	if (strValue.IsEmpty())
		strValue = _T("�˿�����");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MainMarket%:��Ҫ�г�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MainMarket");
	strValue=pGlobalData->aSystemInfo.systemInfo.szMarket;
	if (strValue.IsEmpty())
		strValue = _T("�й�");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CComanyFeature%:��˾�ص�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CComanyFeature");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTrait;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BankUserName%:���п�����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BankUserName");
	strValue=pGlobalData->aSystemInfo.systemInfo.szBrankPersonName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CComanyRegName%:����ע���
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CComanyRegName");
	strValue=pGlobalData->aSystemInfo.systemInfo.szBussisNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BankName%:��������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BankName");
	strValue=pGlobalData->aSystemInfo.systemInfo.szBrankName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BankAccount%:��ҵ�����˺�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BankAccount");
	strValue=pGlobalData->aSystemInfo.systemInfo.szBrankNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%tradcert%:Ӫҵִ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("tradcert");
	strValue=pGlobalData->szTradeImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ccomppic%:��˾��Ƭ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ccomppic");
	strValue=pGlobalData->szCompImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%OrgCodeCert%  ��֯��������֤��
	strKey.Empty();
	strValue.Empty();
	strKey = _T("OrgCodeCert");
	strValue = pGlobalData->szOrgImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%TaxRegCert%  ˰��Ǽ�֤
	strKey.Empty();
	strValue.Empty();
	strKey = _T("TaxRegCert");
	strValue = pGlobalData->szTaxImageName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%FactoryPdt%:��Ӫ��Ʒ
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

	//%ICPNo%:��վICP����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ICPNo");
	strValue=pGlobalData->aSystemInfo.systemInfo.szICPBackup;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertStartTime%:֤����ʼ����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertStartTime");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeStartDate;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertStartTime%:֤����ʼ���� ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertStartTime-Y");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeStartDate;
	strValue = GetDateFromString(strValue, D_YEAR);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertStartTime%:֤����ʼ���� ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertStartTime-M");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeStartDate;
	strValue = GetDateFromString(strValue, D_MONTH);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertStartTime%:֤����ʼ���� ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertStartTime-D");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeStartDate;
	strValue = GetDateFromString(strValue, D_DAY);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertEndTime%:֤���������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertEndTime");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeEndDate;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertEndTime%:֤��������� ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertEndTime-Y");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeEndDate;
	strValue = GetDateFromString(strValue, D_YEAR);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertEndTime%:֤��������� ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertEndTime-M");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeEndDate;
	strValue = GetDateFromString(strValue, D_MONTH);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CertEndTime%:֤��������� ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CertEndTime-D");
	strValue=pGlobalData->aSystemInfo.systemInfo.szTradeEndDate;
	strValue = GetDateFromString(strValue, D_DAY);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ProName%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ProName");
	strValue=pGlobalData->TaskProductInfo.szProductName;
	if(strValue.IsEmpty())
		strValue =_T("��");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Price%:�۸�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Price");
	strValue=pGlobalData->TaskProductInfo.szProductPrice;
	if(strValue.IsEmpty()
		|| strValue.GetAt(0) == '0')
		strValue =_T("1");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtMeasure%:��λ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtMeasure");
	strValue=pGlobalData->TaskProductInfo.szProductUnit;
	if(strValue.IsEmpty())
		strValue = _T("��");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//��Ʒ�ķ�װ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtPack");
	strValue=pGlobalData->TaskProductInfo.szProductSetup;
	if(strValue.IsEmpty())
		strValue =_T("��");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//��ƷƷ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtBrand");
	strValue=pGlobalData->TaskProductInfo.szProductBrand;
	if(strValue.IsEmpty())
		strValue =_T("��");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtOrigin%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtOrigin");
	strValue=pGlobalData->TaskProductInfo.szProductAddress;
	if(strValue.IsEmpty())
		strValue =_T("��");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Type%:�ͺ�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Type");
	strValue=pGlobalData->TaskProductInfo.szProductModel;
	if(strValue.IsEmpty())
		strValue =_T("NULL");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtSpecif%:���ߴ�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtSpecif");
	strValue=pGlobalData->TaskProductInfo.szProductSpec;
	if(strValue.IsEmpty())
		strValue =_T("1*1");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Amount%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Amount");
	strValue=pGlobalData->TaskProductInfo.szProductAmount;
	if(strValue.IsEmpty()
		|| strValue.GetAt(0) == '0')
		strValue = _T("1");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%HGCode%:���ر���
	strKey.Empty();
	strValue.Empty();
	strKey=_T("HGCode");
	strValue=pGlobalData->TaskProductInfo.szProductHGCode;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtTextures%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtTextures");
	strValue=pGlobalData->TaskProductInfo.szMaterial;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ManuFacturer%:��������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ManuFacturer");
	strValue=pGlobalData->TaskProductInfo.szProductFactory;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%GuaranteePeriod%:����/����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("GuaranteePeriod");
	strValue=pGlobalData->TaskProductInfo.szDateToDate;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtInfo%:��Ʒ����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtInfo");
	strValue=pGlobalData->TaskProductInfo.szProductDetail;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PdtExtInfo%:��Ʒ����˵��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PdtExtInfo");
	strValue=pGlobalData->TaskProductInfo.szAddDetail;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%sumpic%:��ƷͼƬ
	result.clear();
	result = pGlobalData->vProductImageName;
	for(int i=0; i< result.size(); i++)
	{
		const CStdString &strTmp = result[i];
		if (strTmp.length() > 0)
		{
			if (i == 0)
			{
				//%sumpic%:��ƷͼƬ
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

	//%CselFirstName%:һ��������Ŀ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CselFirstName");
	strValue=pGlobalData->TaskProductInfo.szCselFirName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CselSecondName%:����������Ŀ
	strKey.Empty();
	strKey=_T("CselSecondName");
	if (_tcslen(pGlobalData->TaskProductInfo.szCselSecName) > 0)
	{
		strValue.Empty();
		strValue = pGlobalData->TaskProductInfo.szCselSecName;
	}
	
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CselThirdName%:����������Ŀ
	strKey.Empty();	
	strKey=_T("CselThirdName");
	//�¿ͻ�û��������ҵ
	if (_tcslen(pGlobalData->TaskProductInfo.szCselThdName) > 0)
	{
		strValue.Empty();
		strValue = pGlobalData->TaskProductInfo.szCselThdName;
	}	
	
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Batch%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Batch");
	strValue=pGlobalData->TaskProductInfo.szProductBatchno;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PProv:ʡ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PProv");
	strValue=pGlobalData->aSystemInfo.userInfo.szProvNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PProv:ʡ�ݣ����ģ�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("szPProv");
	strValue=pGlobalData->aSystemInfo.userInfo.szProvin;
	strProv = strValue;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CCity:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CCity");
	strValue=pGlobalData->aSystemInfo.userInfo.szCitNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CCity:����(���ģ�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("szCCity");
	strValue=pGlobalData->aSystemInfo.userInfo.szCity;
	strCity=strValue;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%AArea:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("AArea");
	strValue=pGlobalData->aSystemInfo.userInfo.szAreaNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));
	
	//%AArea:����(����)
	strKey.Empty();
	strValue.Empty();
	strKey=_T("szAArea");
	strValue=pGlobalData->aSystemInfo.userInfo.szArea;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%SSex%:�Ա�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("SSex");
	strValue=pGlobalData->aSystemInfo.userInfo.szSex;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CCountry%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CCountry");
	strValue=pGlobalData->aSystemInfo.userInfo.szCountry;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Expire:��Ч��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Expire0");
	strValue=pGlobalData->TaskDetailInfo.szExpire;

	if (strValue.IsEmpty())
	{
		strValue=_T("0");
	}
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	//%ACode%:��������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ACode");
	strValue=pGlobalData->aSystemInfo.userInfo.szACode;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ICode%:��������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ICode");
	strValue=pGlobalData->aSystemInfo.userInfo.szICode;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RName%:��ϵ������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("RName");
	strValue=_T("��ϵ��:");
	strValue=pGlobalData->aSystemInfo.userInfo.szXing;
	strValue += pGlobalData->aSystemInfo.userInfo.szMing;
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	//%Xing%:��
	strKey.Empty();
	strKey=_T("Xing");
	strValue= pGlobalData->aSystemInfo.userInfo.szXing;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Ming%:��
	strKey.Empty();
	strKey=_T("Ming");
	strValue= pGlobalData->aSystemInfo.userInfo.szMing;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ZZip%:�ʱ�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ZZip");
	strValue=pGlobalData->aSystemInfo.userInfo.szZip;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%PPID%:���֤
	strKey.Empty();
	strValue.Empty();
	strKey=_T("PPID");
	strValue=pGlobalData->aSystemInfo.userInfo.szPersonNumber;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%YYear%:������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("YYear");
	strValue=pGlobalData->aSystemInfo.userInfo.szYear;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%Month%:��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("Month");
	strValue=pGlobalData->aSystemInfo.userInfo.szMonth;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%DDay%:��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("DDay");
	strValue=pGlobalData->aSystemInfo.userInfo.szDay;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%QQNo%:QQ��
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

	//%TTel%:�绰
	strKey.Empty();
	strValue.Empty();
	strKey=_T("TTel");
	strValue=pGlobalData->aSystemInfo.userInfo.szPhone;
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	//%FFax%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("FFax");
	strValue=pGlobalData->aSystemInfo.userInfo.szFax;;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%EMing%:Ӣ����ϵ����firstname
	strKey.Empty();
	strValue.Empty();
	strKey=_T("EMing");
	strValue=pGlobalData->aSystemInfo.userInfo.szEName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%EXing%:Ӣ����ϵ����lastname
	strKey.Empty();
	strValue.Empty();
	strKey=_T("EXing");
	strValue=pGlobalData->aSystemInfo.userInfo.szEName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ERName%:Ӣ����ϵ��
	strKey.Empty();
	strValue.Empty();
	strKey=_T("ERName");
	strValue= pGlobalData->aSystemInfo.userInfo.szEName;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%AAddr%:�ֵ�
	strKey.Empty();
	strValue.Empty();
	strKey=_T("AAddr");
	strValue=pGlobalData->aSystemInfo.systemInfo.szAddress;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%CCar%:ְλ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("CCar");
	strValue=pGlobalData->aSystemInfo.userInfo.szJob;
	if (strValue.IsEmpty())
		strValue = _T("����");
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//���ڲ���:%DeptName%
	strKey.Empty();
	strValue.Empty();
	strKey=_T("DeptName");
	strValue=pGlobalData->aSystemInfo.userInfo.szDept;
	if (strValue.IsEmpty())
		strValue = _T("�г���");
	tParamItemTable.push_back(TParamItem(strKey, strValue));


	//%SSubj%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("SSubj");
	strValue=pGlobalData->TaskDetailInfo.szSubject;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%InfoK%:������Ϣ�ؼ���
	result.clear();
	strUtil.SplitString(pGlobalData->TaskDetailInfo.szrKeyWords,_T("(;3)"),result,true);
	for(int i=1; i<result.size(); i++)
	{
		const CStdString &strTmp = result[i];
		if (strTmp.length() > 0)
		{
			if (i == 1)
			{
				//%InfoK%:�ؼ���
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

	//%MMail%:�����ʼ�
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

	//%NNameMail%:(ע��ʱ)��������Ϊ�˺���
	strKey.Empty();
	strValue.Empty();
	strKey=_T("NNameMail");
	strValue=pGlobalData->aSystemInfo.userInfo.szMail;
	if (pTaskData->eTaskType == ETypeAutoLogin)
	{
		//һ����¼�����ʺ�Ϊ����ʱ��
		if (pTaskData->szAccount.Find(_T("@")) != -1)
		{
			strValue = pTaskData->szAccount;
		}
	}
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%MMailPass%:��������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("MMailPass");
	strValue=pTaskData->szMailPassword;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%HandSetNum%:�ֻ�����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("HandSetNum");
	strValue=pGlobalData->aSystemInfo.userInfo.szMobile;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%WName%:��ҳ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("WWebs-H");
	strValue=pGlobalData->aSystemInfo.systemInfo.szCompanURL;
	tParamItemTable.push_back(TParamItem(strKey, strValue));
	
	//%WWebs%:��ҳ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("WWebs");
	strValue = _T("http://");
	strValue = strValue + pGlobalData->aSystemInfo.systemInfo.szCompanURL;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody%:����
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody");
	strValue=pGlobalData->TaskDetailInfo.szContext;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-C%:������ϵ��ʽ�ķ�����������

	CStdString strTemp;
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-C");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyC_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-E:���ƻس����ķ�����������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-E");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyE_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-:���Ƴ��ȵķ�����������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyLen_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-CI%:���ƹ�˾���ƺ���ϵ��ʽ�ķ�����������
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-CI");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyCI_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%BBody-P%:�����������ݰ���ͼƬ
	strKey.Empty();
	strValue.Empty();
	strKey=_T("BBody-P");
	strTemp=pGlobalData->TaskDetailInfo.szContext;
	TrimFun(strTemp, strValue, PPD_BBodyP_TYPE);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%RegisterOffice%:֤��Ǽǻ���
	strKey.Empty();
	strValue.Empty();
	strKey = _T("RegisterOffice");
	strValue = pGlobalData->aSystemInfo.systemInfo.szCertReg;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ShopURL%  ���̵�ַ
	strKey.Empty();
	strValue.Empty();
	strKey = _T("ShopURL");
	strValue = pGlobalData->szShopTrafficUrl;
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ClickPdtPages% ��Ʒҳ�����
	strKey.Empty();
	strValue.Empty();
	strKey = _T("ClickPdtPages");
	strValue.Format(_T("%d"),pGlobalData->dwPdtClickCnt);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ClickOtherPages% �������ҳ����
	strKey.Empty();
	strValue.Empty();
	strKey = _T("ClickOtherPages");
	strValue.Format(_T("%d"),pGlobalData->dwOtherClickCnt);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

	//%ProNum% ��Ʒ���
	strKey.Empty();
	strValue.Empty();
	strKey = _T("ProNum");
	strValue.Format(_T("%d"), pGlobalData->TaskDetailInfo.dwNnumber);
	tParamItemTable.push_back(TParamItem(strKey, strValue));

    return 0;
}


//����:ϵͳ��Ϣ���Ժ���
int CSysparam::GetSysparamForTest(std::vector<TParamItem>& tParamItemTable)
{
	CStdString strKeyTemp;
	CStdString strValueTemp;

	//%CompanyKind%:��ҵ����
	strKeyTemp=_T("CompanyKind");
	strValueTemp=_T("8");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CCity:����
	strKeyTemp=_T("CCity");
	strValueTemp=_T("440300");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Prov%:ʡ��(û����,�ȱ���)
	strKeyTemp=_T("Prov");
	strValueTemp=_T("4");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%PProv:ʡ��
	strKeyTemp=_T("PProv");
	strValueTemp=_T("4");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	strKeyTemp=_T("AArea");
	strValueTemp=_T("1");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%OFtype:��Ϣ����
	strKeyTemp=_T("OFtype");
	strValueTemp=_T("0");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%SSex%:�Ա�
	strKeyTemp=_T("SSex");
	strValueTemp=_T("1");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CCountry%:����
	strKeyTemp=_T("CCountry");
	strValueTemp=_T("0");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Expire:��Ч��
	strKeyTemp=_T("Expire");
	strValueTemp=_T("0");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%PersonsCount%:��˾��ģ
	strKeyTemp=_T("PersonsCount");
	strValueTemp=_T("1");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%MoneyInfo%:ע���ʽ�
	strKeyTemp=_T("MoneyInfo");
	strValueTemp=_T("1");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CSel:���ѡ�������Ŀ����(����ת��)
	strKeyTemp=_T("CSel");
	strValueTemp=_T("0755");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	strKeyTemp=_T("CrLf");
	strValueTemp=_T("\r\n");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ACode%:��������
	strKeyTemp=_T("ACode");
	strValueTemp=_T("0755");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BACode%:��������
	strKeyTemp=_T("BACode");
	strValueTemp=_T("0755");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CInfo%:��˾���
	strKeyTemp=_T("CInfo");
	strValueTemp=_T("���Ҽ���������Ŀ��60Mt�۷Ź������豸�����뼼���о����У�\
					ZFS35�͵�Һ������ƷŶ�úҺѹ֧�ܵ�Һ�ص��򷧺Ͱ�ȫ��ȫ���ɹ���������\
					�����ϵ�Һ�ص��򷧺Ͱ�ȫ���𻵱Ƚ�Ƶ�������Ұ�ȫ���ķ�о��ж��ʱ������\
					�������©Һʹ�������޷��ﵽ������������ˣ����ݿ�ҵ�����ţ�\
					��˾��¡ׯú�������˹���������Һ�ص��򷧺Ͱ�ȫ������������������Ҫ��");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%RName%:��ϵ������
	strKeyTemp=_T("RName");
	strValueTemp=_T("����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BRName%:��ϵ������
	strKeyTemp=_T("BRName");
	strValueTemp=_T("����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BERName%:��ϵ������
	strKeyTemp=_T("BERName");
	strValueTemp=_T("����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ZZip%:�ʱ�
	strKeyTemp=_T("ZZip");
	strValueTemp=_T("518100");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BZZip%:�ʱ�
	strKeyTemp=_T("BZZip");
	strValueTemp=_T("51800");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%PPID%:���֤
	strKeyTemp=_T("PPID");
	strValueTemp=_T("360371198007177972");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%YYear%:������
	strKeyTemp=_T("YYear");
	strValueTemp=_T("1980");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Month%:��
	strKeyTemp=_T("Month");
	strValueTemp=_T("07");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%DDay%:��
	strKeyTemp=_T("DDay");
	strValueTemp=_T("17");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%NewTradeBig%:��˾��������ҵ
	strKeyTemp=_T("NewTradeBig");
	strValueTemp=_T("22");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%NewTradeDetail%:��˾����С��ҵ
	strKeyTemp=_T("NewTradeDetail");
	strValueTemp=_T("2235");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CompanyIndustry%:��˾������ҵ
	strKeyTemp=_T("CompanyIndustry");
	strValueTemp=_T("2");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%SEARCHKEY%:������Ϣ�ؼ���
	strKeyTemp=_T("SEARCHKEY");
	strValueTemp=_T("����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%QQNo%:QQ��
	strKeyTemp=_T("QQNo");
	strValueTemp=_T("480120350");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%TTel%:�绰
	strKeyTemp=_T("TTel");
	strValueTemp=_T("45102302");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BTTel%:�绰
	strKeyTemp=_T("BTTel");
	strValueTemp=_T("45102302");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%FFax%:����
	strKeyTemp=_T("FFax");
	strValueTemp=_T("45102302");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProName%:����
	strKeyTemp=_T("ProName");
	strValueTemp=_T("����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Price%:�۸�
	strKeyTemp=_T("Price");
	strValueTemp=_T("0");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProFactory%:������(û����,�ȱ���)
	strKeyTemp=_T("ProFactory");
	strValueTemp=_T("�й�");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%ManuFacturer%:��������
	strKeyTemp=_T("ManuFacturer");
	strValueTemp=_T("�й�");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProArea%:����
	strKeyTemp=_T("ProArea");
	strValueTemp=_T("����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Type%:�ͺ�
	strKeyTemp=_T("Type");
	strValueTemp=_T("1000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));		

	//%ProAmount%:��Ʒ���������(û����,�ȱ���)
	strKeyTemp=_T("ProAmount");
	strValueTemp=_T("1000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%Amount%:����
	strKeyTemp=_T("Amount");
	strValueTemp=_T("1000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProSetup%:��װ(û����,�ȱ���)
	strKeyTemp=_T("ProSetup");
	strValueTemp=_T("����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%EncapSulation%:��װ
	strKeyTemp=_T("EncapSulation");
	strValueTemp=_T("����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ECity%:Ӣ����/������
	strKeyTemp=_T("ECity");
	strValueTemp=_T("shen zhen");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EAddr%:Ӣ�Ľֵ�����
	strKeyTemp=_T("EAddr");
	strValueTemp=_T("shenzhenshiwuhaidagongsi");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BEAddr%:Ӣ�Ľֵ�����
	strKeyTemp=_T("BEAddr");
	strValueTemp=_T("shenzhenshiwuhaidagongsi");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EMing%:Ӣ����ϵ����firstname
	strKeyTemp=_T("EMing");
	strValueTemp=_T("qing");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EXing%:Ӣ����ϵ����lastname
	strKeyTemp=_T("EXing");
	strValueTemp=_T("li");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ERName%:Ӣ����ϵ��
	strKeyTemp=_T("ERName");
	strValueTemp=_T("qing li");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BERName%:Ӣ����ϵ��
	strKeyTemp=_T("BERName");
	strValueTemp=_T("qing li");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%WWebs%:��ҳ
	strKeyTemp=_T("WWebs");
	strValueTemp=_T("http://www.wuhaidagongsi.com");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%WWebs%:��ҳ
	strKeyTemp=_T("WWebs-H");
	strValueTemp=_T("www.wuhaidagongsi.com");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BWWebs%:��ҳ
	strKeyTemp=_T("BWWebs");
	strValueTemp=_T("http://www.wuhaidagongsi.com");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%AAddr%:�ֵ�
	strKeyTemp=_T("AAddr");
	strValueTemp=_T("������·58��205��");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BAAddr%:�ֵ�
	strKeyTemp=_T("BAAddr");
	strValueTemp=_T("������·58��205��");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ECountry%:Ӣ�Ĺ�������
	strKeyTemp=_T("ECountry");
	strValueTemp=_T("China");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%SSubj%:����
	strKeyTemp=_T("SSubj");
	strValueTemp=_T("��ȫ��");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody%:����
	strKeyTemp=_T("BBody");
	strValueTemp=_T("���ǹ�Ӧ��ȫ�򷧹��Ҽ���������Ŀ��60Mt�۷Ź������豸�����뼼���о����У�\
					ZFS35�͵�Һ������ƷŶ�úҺѹ֧�ܵ�Һ�ص��򷧺Ͱ�ȫ��ȫ���ɹ���������\
					�����ϵ�Һ�ص��򷧺Ͱ�ȫ���𻵱Ƚ�Ƶ�������Ұ�ȫ���ķ�о��ж��ʱ����\
					�ܷ������©Һʹ�������޷��ﵽ������������ˣ�\
					���ݿ�ҵ�����ţ���˾��¡ׯú�������˹���������Һ�ص���\
					�Ͱ�ȫ������������������Ҫ��******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%InfoK%:������Ϣ�ؼ���
	strKeyTemp=_T("InfoK");
	strValueTemp=_T("��ȫ��");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%MMail%:�����ʼ�
	strKeyTemp=_T("MMail");
	strValueTemp=_T("yaohong562@163.com");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ICode%:��������
	strKeyTemp=_T("ICode");
	strValueTemp=_T("086");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%BICode%:��������
	strKeyTemp=_T("BICode");
	strValueTemp=_T("086");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CComp%:��˾����.
	strKeyTemp=_T("CComp");
	strValueTemp=_T("shenzhenshiwuhaidajixiegongsi");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProBatchno%:����(û����,�ȱ���)
	strKeyTemp=_T("ProBatchno");
	strValueTemp=_T("1000000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));
	//%Batch%:����
	strKeyTemp=_T("Batch");
	strValueTemp=_T("1000000");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ECinfo%:Ӣ�Ĺ�˾����
	strKeyTemp=_T("ECinfo");
	strValueTemp=_T("China has been working in the field of jet printing packaging code cause, \
					after many years of research and development, has now established non-contact and \
					contact Damaji Stenciling Unit two series of more than 10 varieties, \
					fully able to meet the different customers different requirements.");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EComp%:Ӣ�Ĺ�˾��
	strKeyTemp=_T("EComp");
	strValueTemp=_T("shenzhenshiwuhaidajixiegongsi");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%HGCode%:���ر���
	strKeyTemp=_T("HGCode");
	strValueTemp=_T("");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%HandSetNum%:�ֻ�����
	strKeyTemp=_T("HandSetNum");
	strValueTemp=_T("13421582256");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%CPrincipal%:���˴�����(����)
	strKeyTemp=_T("CPrincipal");
	strValueTemp=_T("·����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%MSTitle%:�ظ���������
	strKeyTemp=_T("MSTitle");
	strValueTemp=_T("REQRQRRWEQREW");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%MSBody%:�ظ���������
	strKeyTemp=_T("MSBody");
	strValueTemp=_T("erwfefsdfwefdsfdsffffffffffffdsfsdfsdfdsfdsfds");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//ImageLogo
	//ImageValue
	//%NName%:�˺�
	//%PPass%:����
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

	//%VKey:����.��ͨ����Ŀ,����Ŀ��ͬ����ͬ//û����
	strKeyTemp=_T("VKey");
	strValueTemp=_T("VKey");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody-C%:������ϵ��ʽ�ķ�����������
	strKeyTemp=_T("BBody-C");
	strValueTemp=_T("���ǹ�Ӧ��ȫ�򷧹��Ҽ���������Ŀ��60Mt�۷Ź������豸�����뼼���о����У�\
					ZFS35�͵�Һ������ƷŶ�úҺѹ֧�ܵ�Һ�ص��򷧺Ͱ�ȫ��ȫ���ɹ���������\
					�����ϵ�Һ�ص��򷧺Ͱ�ȫ���𻵱Ƚ�Ƶ�������Ұ�ȫ���ķ�о��ж��ʱ����\
					�ܷ������©Һʹ�������޷��ﵽ������������ˣ�\
					���ݿ�ҵ�����ţ���˾��¡ׯú�������˹���������Һ�ص���\
					�Ͱ�ȫ������������������Ҫ��******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody-E:���ƻس����ķ�����������
	strKeyTemp=_T("BBody-E");
	strValueTemp=_T("���ǹ�Ӧ��ȫ�򷧹��Ҽ���������Ŀ��60Mt�۷Ź������豸�����뼼���о����У�\
					ZFS35�͵�Һ������ƷŶ�úҺѹ֧�ܵ�Һ�ص��򷧺Ͱ�ȫ��ȫ���ɹ���������\
					�����ϵ�Һ�ص��򷧺Ͱ�ȫ���𻵱Ƚ�Ƶ�������Ұ�ȫ���ķ�о��ж��ʱ����\
					�ܷ������©Һʹ�������޷��ﵽ������������ˣ�\
					���ݿ�ҵ�����ţ���˾��¡ׯú�������˹���������Һ�ص���\
					�Ͱ�ȫ������������������Ҫ��******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody-:���Ƴ��ȵķ�����������
	strKeyTemp=_T("BBody-");
	strValueTemp=_T("���ǹ�Ӧ��ȫ�򷧹��Ҽ���������Ŀ��60Mt�۷Ź������豸�����뼼���о����У�\
					ZFS35�͵�Һ������ƷŶ�úҺѹ֧�ܵ�Һ�ص��򷧺Ͱ�ȫ��ȫ���ɹ���������\
					�����ϵ�Һ�ص��򷧺Ͱ�ȫ���𻵱Ƚ�Ƶ�������Ұ�ȫ���ķ�о��ж��ʱ����\
					�ܷ������©Һʹ�������޷��ﵽ������������ˣ�\
					���ݿ�ҵ�����ţ���˾��¡ׯú�������˹���������Һ�ص���\
					�Ͱ�ȫ������������������Ҫ��******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%BBody-CI%:���ƹ�˾���ƺ���ϵ��ʽ�ķ�����������
	strKeyTemp=_T("BBody-CI");
	strValueTemp=_T("���ǹ�Ӧ��ȫ�򷧹��Ҽ���������Ŀ��60Mt�۷Ź������豸�����뼼���о����У�\
					ZFS35�͵�Һ������ƷŶ�úҺѹ֧�ܵ�Һ�ص��򷧺Ͱ�ȫ��ȫ���ɹ���������\
					�����ϵ�Һ�ص��򷧺Ͱ�ȫ���𻵱Ƚ�Ƶ�������Ұ�ȫ���ķ�о��ж��ʱ����\
					�ܷ������©Һʹ�������޷��ﵽ������������ˣ�\
					���ݿ�ҵ�����ţ���˾��¡ׯú�������˹���������Һ�ص���\
					�Ͱ�ȫ������������������Ҫ��******");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Xing%:������//ע������
	strKeyTemp=_T("Xing");
	strValueTemp=_T("��");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%Ming%:������//ע������
	strKeyTemp=_T("��");
	strValueTemp=_T("0755");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EProv%:Ӣ��ʡ��//����
	strKeyTemp=_T("EProv");
	strValueTemp=_T("guangdong");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%IDEmail%:ID�ʼ���ID��//��Ҫ��
	strKeyTemp=_T("IDEmail");
	strValueTemp=_T("aaa@yahoo.com.cn");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%ProPicture%:��Ʒ��ͼƬͨ���//��Ҫ��
	strKeyTemp=_T("ProPicture");
	strValueTemp=_T("");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%EPrincipal%:���˴�����(Ӣ��)//��
	strKeyTemp=_T("EPrincipal");
	strValueTemp=_T("qing li");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	//%RegisterOffice%:֤��Ǽǻ���
	strKeyTemp = _T("RegisterOffice");
	strValueTemp = _T("�Ǽǹ����");
	tParamItemTable.push_back(TParamItem(strKeyTemp, strValueTemp));

	return 0;
}

/*
	@brief  �����ַ�������ȡ�׶�����
	@param  strIn Ҫ��ȡ���ַ���
	@return ���������ַ���
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
	@brief  ���������ַ�������ȡ��Ӧ�������� ��ʽ��2011-01-02
	@param  strIn Ҫ��ȡ���ַ���
	@param  dType ����
	@return �����ַ���
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
