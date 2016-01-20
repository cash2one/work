#include "StdAfx.h"
#include ".\WebsiteData.h"


CWebSiteData::CWebSiteData(void)
{

}

CWebSiteData::~CWebSiteData(void)
{
}



void CWebSiteData::Test()
{
	SetPostion(20);
	printf("NsCode=%d\n",ReadDWORD());
	printf("nsDisplay=%d\n",ReadDWORD());
	printf("nsarea=%d\n",ReadDWORD());
	printf("isdemo=%d\n",ReadDWORD());
	printf("nsonlydown=%d\n",ReadDWORD());
	printf("nsonlypost=%d\n",ReadDWORD());
	printf("nsname=%s\n",ReadStr());
	printf("nsrstep=%s\n",ReadStr());
	printf("nsraction=%s\n",ReadStr());
	printf("nsrconst=%s\n",ReadStr());
	printf("nsrkeyword=%s\n",ReadStr());
	printf("nsrreffer=%s\n",ReadStr());
	printf("nsrsucc=%s\n",ReadStr());
	printf("nspstep=%s\n",ReadStr());
	printf("nspaction=%s\n",ReadStr());
}


//�����Ŀ����
PCHAR CWebSiteData::GetNSIparamater(UINT64 nNetItemId, int nStartPos)
{
	UINT64 dwItemId64 = nNetItemId;

	while (dwItemId64 > 0)
	{
		SetPostion(nStartPos);
		DWORD ItemBlockStart = ReadDWORD();	//��Ŀ����ʼλ��
		SetPostion(ItemBlockStart);
		DWORD ItemCount = ReadDWORD();	//����
		if(ItemCount > 0)
		{
			DWORD BlockDataPos = 0;//������λ��
			ItemBlockStart += sizeof(DWORD); 
			INT ItemBeginIndex = 0;
			INT ItemEndIndex = (ItemCount-1); //����λ��
			INT ItemMidIndex = 0;//�м�λ��	

			UINT64 MidValue64 =0;

			while(ItemBeginIndex<=ItemEndIndex) 
			{
				ItemMidIndex = (ItemEndIndex+ItemBeginIndex) /2;

				SetPostion(ItemBlockStart+ItemMidIndex*sizeof(DWORD)*3);//���м�

				DWORD dwTemp = ReadDWORD() ;
				MidValue64 = ReadDWORD();
				MidValue64 = MidValue64<<32 | dwTemp;

				if(MidValue64 == dwItemId64) 
				{
					SetPostion(ReadDWORD());

					//printf("%d %s %s\n",dwNetItemId,ReadStr(),ReadStr());
					return ReadStr();
				}
				if(dwItemId64 > MidValue64)
				{
					ItemBeginIndex = ItemMidIndex + 1 ;
				}
				else
				{
					ItemEndIndex = ItemMidIndex - 1;
				}
			} 
		}

		dwItemId64 /= 100;
	}
	

	return NULL;
}

PCHAR CWebSiteData::GetNSIparamater_Post(UINT64 nNetItemId)
{
	return GetNSIparamater(nNetItemId, 4) ;
}

PCHAR CWebSiteData::GetNSIparamater_Register(UINT64 nNetItemId)
{
	return GetNSIparamater(nNetItemId, 8) ;
}


//���Ĭ��ITemCode
UINT64 CWebSiteData::GetDefaultItemCode_Reg()
{
	SetPostion(8);
	DWORD ItemBlockStart = ReadDWORD();	//��Ŀ����ʼλ��
	SetPostion(ItemBlockStart);
	DWORD ItemCount = ReadDWORD();	//����

	UINT64  ui64ItemCode = 0;
	DWORD   dwTemp = 0;;
	if(ItemCount > 0)
	{
		ItemBlockStart += sizeof(DWORD); 
		DWORD ItemEndIndex = (ItemCount-1); //����λ��
		SetPostion(ItemBlockStart+ItemEndIndex*sizeof(DWORD)*3);//���м�
		
		dwTemp = ReadDWORD();
		ui64ItemCode = (UINT64)ReadDWORD();

		ui64ItemCode = ui64ItemCode<<32 | dwTemp;
		return ui64ItemCode;
	}
	return 0;
}


//�����վ����
PCHAR CWebSiteData::GetNSVrealvalue(DWORD dwVType,DWORD dwVCode)
{
	UINT64 u64Key = dwVType ;
	u64Key = u64Key << 32 | dwVCode;

	SetPostion(12);
	DWORD VarBlockStart = ReadDWORD();	//��������ʼλ��
	SetPostion(VarBlockStart);
	DWORD VarCount = ReadDWORD();	//����
	if(VarCount > 0)
	{
		DWORD BlockDataPos = 0;//������λ��
		VarBlockStart += sizeof(DWORD); 
		INT VarBeginIndex = 0;
		INT VarEndIndex = (VarCount-1); //����λ��
		INT VarMidIndex = 0;//�м�λ��	

		DWORD MidOneValue =0;
		DWORD MidTwoValue =0;
		UINT64 u64Value = 0;

		while(VarBeginIndex<=VarEndIndex) 
		{
			VarMidIndex = (VarEndIndex+VarBeginIndex) /2;

			SetPostion(VarBlockStart+VarMidIndex*sizeof(DWORD)*3);//���м�
			MidOneValue = ReadDWORD();		//����
			MidTwoValue = ReadDWORD();

			u64Value = MidOneValue ;
			u64Value = u64Value << 32 | MidTwoValue;

			if(u64Key == u64Value) 
			{
				SetPostion(ReadDWORD());
				return ReadStr();
			}
			
			if(u64Key > u64Value)
			{
				VarBeginIndex = VarMidIndex + 1 ;
			}
			else
			{
				VarEndIndex = VarMidIndex - 1;
			}
		} 
	}

	return NULL;
}

//������վ���ݻ�ȡ��׼ָ
DWORD CWebSiteData::GetStandardByRealvalue( DWORD dwVType,PCHAR szRealValue )
{
	DWORD NetVarId = -1;
	DWORD VarTmpIndex; //��ʱ����λ��
	DWORD VarTmpType; //������ʱ����
	SetPostion(12);
	DWORD VarBlockStart = ReadDWORD();	//��������ʼλ��
	SetPostion(VarBlockStart);
	DWORD VarCount = ReadDWORD(); //����
	if (VarCount > 0)
	{
		DWORD BlockDataPos = 0; //������λ��
		VarBlockStart += sizeof(DWORD); 
		INT VarBeginIndex = 0;
		INT VarEndIndex = (VarCount-1); //����λ��
		INT VarMidIndex = 0;//�м�λ��	

		DWORD MidOneValue =0;

		while(VarBeginIndex<=VarEndIndex) 
		{
			VarMidIndex = (VarEndIndex+VarBeginIndex) /2;

			SetPostion(VarBlockStart+VarMidIndex*sizeof(DWORD)*3);//���м�
			MidOneValue = ReadDWORD();		

			if (MidOneValue == dwVType)
			{
				break;  //���������Ⱦ��˳���
			}

			if (MidOneValue < dwVType)
			{
				VarBeginIndex = VarMidIndex + 1;
			}
			else
			{
				VarEndIndex = VarMidIndex - 1 ;
			}
		}
		
		VarTmpIndex = VarMidIndex;
		VarTmpType = MidOneValue;
		PCHAR szNsValue;
		while(MidOneValue == dwVType)
		{
			SetPostion(VarBlockStart+VarMidIndex*sizeof(DWORD)*3);
			MidOneValue = ReadDWORD();
			NetVarId = ReadDWORD();
			SetPostion(ReadDWORD());
			szNsValue = ReadStr();
			if (0 == strcmp(szNsValue,szRealValue))
			{
				return NetVarId;
			}
			VarMidIndex +=1;
		}
		MidOneValue = VarTmpType;
		VarMidIndex = VarTmpIndex-1;
		while(MidOneValue == dwVType)
		{
			SetPostion(VarBlockStart+VarMidIndex*sizeof(DWORD)*3);
			MidOneValue = ReadDWORD();
			NetVarId = ReadDWORD();
			SetPostion(ReadDWORD());
			szNsValue = ReadStr();
			if (0 == strcmp(szNsValue,szRealValue))
			{
				return NetVarId;
			}
			VarMidIndex -=1;
		}

	}

	return -1;
}

DWORD CWebSiteData::GetLastUpdateTime()
{

	SetPostion(0);
	return ReadDWORD();
}

DWORD CWebSiteData::getNScode()
{
	SetPostion(28);
	return ReadDWORD();
}

PCHAR CWebSiteData::getNetStationValue(DWORD type)
{
	PCHAR retValue(NULL),temp(NULL);
	SetPostion(32);
	while(type != 0)
	{
		temp = ReadStr();
		type --;
	}
	retValue = temp;
	return retValue;
}

CStdString CWebSiteData::getNSname()
{
	PCHAR pchNSname = getNetStationValue(etNSname) ;
	CStdString strNSname ;
	ToMByteToWChar(pchNSname, strNSname) ;
	return strNSname ;
}

CStdString CWebSiteData::GetNShost()
{
	PCHAR pchNShost = getNetStationValue(etNShost) ;
	CStdString strNShost ;
	ToMByteToWChar(pchNShost, strNShost) ;
	return strNShost ;
}


void CWebSiteData::GetNStype(T_Site_Type &tSiteType)
{
	PCHAR pchNStype = getNetStationValue(etNStype) ;
	CStdString strNStype ;
	ToMByteToWChar(pchNStype, strNStype) ;
	tSiteType.Set(strNStype) ;
}

CStdString CWebSiteData::GetNSdata() 
{
	PCHAR pchNSdata = getNetStationValue(etNsdata) ;
	CStdString strNSdata ;
	ToMByteToWChar(pchNSdata, strNSdata) ;
	return strNSdata ;
}

void CWebSiteData::GetNSlimit(T_Limit &tLimit)
{
	PCHAR pchNSlimit = getNetStationValue(etNSlimit) ;
	CStdString strNSlimit ;
	ToMByteToWChar(pchNSlimit, strNSlimit) ;
	tLimit.Set(strNSlimit) ;
}

CStdString CWebSiteData::GetNSImageProcessCode()
{
	PCHAR pchNSimageProcessCode = getNetStationValue(etNSImageProcessCode) ;
	CStdString strNSImageProcessCode ;
	ToMByteToWChar(pchNSimageProcessCode, strNSImageProcessCode) ;
	return strNSImageProcessCode ;
}


DWORD CWebSiteData::GetNetBaseInfo(T_NetstationBaseInfo& t_NetstationBaseInfo)
{
	SetPostion(28);
	t_NetstationBaseInfo.NScode.Format(_T("%d"),ReadDWORD());

	ToMByteToWChar(ReadStr() , t_NetstationBaseInfo.NSname);
	ToMByteToWChar(ReadStr(), t_NetstationBaseInfo.NShost);
	ToMByteToWChar(ReadStr() , t_NetstationBaseInfo.NStype);
	ToMByteToWChar(ReadStr(), t_NetstationBaseInfo.NSdata);
	ToMByteToWChar(ReadStr() ,t_NetstationBaseInfo.NSLimit);
	ToMByteToWChar(ReadStr() , t_NetstationBaseInfo.NSImageProcessCode);
	
	return 0;
}

UINT64 CWebSiteData::GetDefaultItemCode_Post()
{
	SetPostion(4);
	DWORD ItemBlockStart = ReadDWORD();	//��Ŀ����ʼλ��
	SetPostion(ItemBlockStart);
	DWORD ItemCount = ReadDWORD();	//����

	UINT64  ui64ItemCode = 0;
	DWORD   dwTemp = 0;;
	if(ItemCount > 0)
	{
		ItemBlockStart += sizeof(DWORD); 
		DWORD ItemEndIndex = (ItemCount-1); //����λ��
		SetPostion(ItemBlockStart+ItemEndIndex*sizeof(DWORD)*3);//���м�

		dwTemp = ReadDWORD();
		ui64ItemCode = (UINT64)ReadDWORD();

		ui64ItemCode = ui64ItemCode<<32 | dwTemp;
		return ui64ItemCode;
	}
	return 0;
}


