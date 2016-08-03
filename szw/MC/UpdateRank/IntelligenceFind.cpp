#include "stdafx.h"
#include "IntelligenceFind.h"


CIntelligenceFind::CIntelligenceFind()
{

}


CIntelligenceFind::~CIntelligenceFind()
{
}

/*
@brief �����Ƿ��������������
@param strParam1 ��������1��
@param strParam2 ��������2��
@param iFlag 0:2���Ƿ񶼴��ڡ�1:2���Ƿ񶼲����ڡ�2����һ���������ڣ��ڶ������������ڣ�
@param iCmpType 0:��Ƚϣ� 1����Ƚ�
@return �����Ƿ���ҳɹ�
*/
BOOL CIntelligenceFind::FindResult(const CString &strData, const CString &strParam1, const CString &strParam2, int iFlag/*=0*/, int icmpType)
{
	if (strData.GetLength() <= 0 )
	{
		return FALSE;
	}
	//��Ƚ�
	if (icmpType == 0)
	{
		switch (iFlag)
		{
		case 0:	//�������������Ƿ񶼴���
			if (strData.Find(strParam1) != -1 && strData.Find(strParam2) != -1)
			{
				return TRUE;
			}
			break;
		case 1: //�������������Ƿ񶼲�����
			if (strData.Find(strParam1) == -1 && strData.Find(strParam2) == -1)
			{
				return TRUE;
			}
			break;
		case 2: //���ҵ�һ�������Ƿ񶼴��ڣ��ҵڶ���������
			if (strData.Find(strParam1) != -1 && strData.Find(strParam2) == -1)			
			{
				return TRUE;
			}
			break;
		default:
			return FALSE;
			break;
		}
	}
	else if(icmpType == 1)		//��Ƚ�
	{
		switch (iFlag)
		{
		case 0:	//����������һ������
			if (strData.Find(strParam1) != -1 || strData.Find(strParam2) != -1)
			{
				return TRUE;
			}
			break;
		case 1: //����������һ��������
			if (strData.Find(strParam1) == -1 || strData.Find(strParam2) == -1)
			{
				return TRUE;
			}
			break;
		case 2: //��һ�����ڣ����ߵڶ���������
			if (strData.Find(strParam1) != -1 || strData.Find(strParam2) == -1)
			{
				return TRUE;
			}
			break;
		default:
			return FALSE;
			break;
		}
	}

	return FALSE;

}

/*
@brief �����Ƿ��������������
@param strData  Ҫ���ҵ����ݣ�
@param strParam1 ��������1��
@param strParam2 ����2��
@param strParam3 ����3��
@param iFlag 0:3���Ƿ񶼴��ڡ�1:3���Ƿ񶼲����ڡ�2��һ���������ڣ��������������ڡ�3�������������ڣ�һ�����������ڣ�
@param iCmpType 0:��Ƚϣ� 1����Ƚ�
@return �����Ƿ���ҳɹ�
*/
BOOL CIntelligenceFind::FindResult(const CString &strData, const CString &strParam1, const CString &strParam2, const CString &strParam3, int iFlag /*= 0*/, int icmpType)
{
	if (strData.GetLength() <= 0)
	{
		return FALSE;
	}

	if (icmpType == 0)
	{
		switch (iFlag)
		{
		case 0:	//����3�������Ƿ񶼴���
			if (strData.Find(strParam1) != -1 && strData.Find(strParam2) != -1
				&& strData.Find(strParam3) != -1)
			{
				return TRUE;
			}
			break;
		case 1: //����3�������Ƿ񶼲�����
			if (strData.Find(strParam1) == -1 && strData.Find(strParam2) == -1
				&& strData.Find(strParam3) == -1)
			{
				return TRUE;
			}
			break;
		case 2: //���ҵ�һ�������Ƿ񶼴��ڣ�����������������
			if ((strData.Find(strParam1) != -1 && strData.Find(strParam2) == -1
				&& strData.Find(strParam3) == -1))
			{
				return TRUE;
			}
			break;
		case 3: //���ҵ�һ���͵ڶ����������ڣ����һ�����������ڵ����
			if ((strData.Find(strParam1) != -1 && strData.Find(strParam2) != -1
				&& strData.Find(strParam3) == -1))
			{
				return TRUE;
			}
			break;
		default:
			return FALSE;
			break;
		}
	}
	else if (icmpType == 1)
	{
		switch (iFlag)
		{
		case 0:	//����3�������Ƿ���һ������
			if (strData.Find(strParam1) != -1 || strData.Find(strParam2) != -1
				|| strData.Find(strParam3) != -1)
			{
				return TRUE;
			}
			break;
		case 1: //����3�������Ƿ���һ��������
			if (strData.Find(strParam1) == -1 || strData.Find(strParam2) == -1
				|| strData.Find(strParam3) == -1)
			{
				return TRUE;
			}
			break;
		case 2: //���ҵ�һ�������Ƿ񶼴��ڣ�������������һ�������ڲ�����
			if ((strData.Find(strParam1) != -1) && (strData.Find(strParam2) == -1
				|| strData.Find(strParam3) == -1))
			{
				return TRUE;
			}
			break;
		case 3: //���ҵ�һ���͵ڶ����������ڣ������һ�����������ڵ����
			if ((strData.Find(strParam1) != -1 || strData.Find(strParam2) != -1)
				|| (strData.Find(strParam3) == -1))
			{
				return TRUE;
			}
			break;
		default:
			return FALSE;
			break;
		}
	}


	return FALSE;
}

/*
@brief ��ȡ���ҵ��ַ�����λ��
@param strData  Ҫ���ҵ����ݣ�
@param Ҫ���ҵĲ���
@param Ҫ���ҵĲ���2
@param iInitPos ��ʼ����λ�ã�
@return ���ز��ҵ��Ľ����
*/
int CIntelligenceFind::GetFindPos(const CString &strData, const CString &strPara1, const CString& strPara2, int &iInitPos)
{
	int iPos1 = -1;

	if (strData.GetLength()<= 0)
	{
		return iPos1;
	}

	 iInitPos = strData.Find(strPara1,iInitPos);
	 if (iInitPos != -1)
	{
		iPos1 = strData.Find(strPara2,iInitPos);
	}

	return iPos1;

}

/*
@breif ��ȡָ��λ���ַ���
@param strData Ҫ������ȡ���ַ�����
@param strPara1 ����1
...
@param iInitPos ��ʼλ�ã�
@return ���ؽ�ȡ�����ַ���
*/
CString CIntelligenceFind::GetFindData(const CString &strData, const CString &strPara1, const CString &strPara2, const CString &strPara3, int iInitPos)
{
	CString strResult = _T("");

	if (strData.GetLength() <= 0)
	{
		return strResult;
	}

	iInitPos = strData.Find(strPara1);

	if (iInitPos == -1)
		iInitPos = strData.Find(strPara2);
	
	if (iInitPos == -1)
		iInitPos = strData.Find(strPara3);

	if (iInitPos != -1)
	{
		strResult = strData.Left(iInitPos);
	}

	return strResult;
}
