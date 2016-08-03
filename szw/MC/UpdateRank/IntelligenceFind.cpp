#include "stdafx.h"
#include "IntelligenceFind.h"


CIntelligenceFind::CIntelligenceFind()
{

}


CIntelligenceFind::~CIntelligenceFind()
{
}

/*
@brief 查找是否满足参数的条件
@param strParam1 满足条件1；
@param strParam2 满足条件2；
@param iFlag 0:2个是否都存在。1:2个是否都不存在。2：第一个参数存在，第二个参数不存在；
@param iCmpType 0:与比较， 1：或比较
@return 返回是否查找成功
*/
BOOL CIntelligenceFind::FindResult(const CString &strData, const CString &strParam1, const CString &strParam2, int iFlag/*=0*/, int icmpType)
{
	if (strData.GetLength() <= 0 )
	{
		return FALSE;
	}
	//与比较
	if (icmpType == 0)
	{
		switch (iFlag)
		{
		case 0:	//查找两个参数是否都存在
			if (strData.Find(strParam1) != -1 && strData.Find(strParam2) != -1)
			{
				return TRUE;
			}
			break;
		case 1: //查找两个参数是否都不存在
			if (strData.Find(strParam1) == -1 && strData.Find(strParam2) == -1)
			{
				return TRUE;
			}
			break;
		case 2: //查找第一个参数是否都存在，且第二个不存在
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
	else if(icmpType == 1)		//或比较
	{
		switch (iFlag)
		{
		case 0:	//两个有其中一个存在
			if (strData.Find(strParam1) != -1 || strData.Find(strParam2) != -1)
			{
				return TRUE;
			}
			break;
		case 1: //两个有其中一个不存在
			if (strData.Find(strParam1) == -1 || strData.Find(strParam2) == -1)
			{
				return TRUE;
			}
			break;
		case 2: //第一个存在，或者第二个不存在
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
@brief 查找是否满足参数的条件
@param strData  要查找的数据；
@param strParam1 满足条件1；
@param strParam2 参数2；
@param strParam3 参数3；
@param iFlag 0:3个是否都存在。1:3个是否都不存在。2：一个参数存在，二个参数不存在。3：两个参数存在，一个参数不存在；
@param iCmpType 0:与比较， 1：或比较
@return 返回是否查找成功
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
		case 0:	//查找3个参数是否都存在
			if (strData.Find(strParam1) != -1 && strData.Find(strParam2) != -1
				&& strData.Find(strParam3) != -1)
			{
				return TRUE;
			}
			break;
		case 1: //查找3个参数是否都不存在
			if (strData.Find(strParam1) == -1 && strData.Find(strParam2) == -1
				&& strData.Find(strParam3) == -1)
			{
				return TRUE;
			}
			break;
		case 2: //查找第一个参数是否都存在，且其他二个不存在
			if ((strData.Find(strParam1) != -1 && strData.Find(strParam2) == -1
				&& strData.Find(strParam3) == -1))
			{
				return TRUE;
			}
			break;
		case 3: //查找第一个和第二个参数存在，最后一个参数不存在的情况
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
		case 0:	//查找3个参数是否有一个存在
			if (strData.Find(strParam1) != -1 || strData.Find(strParam2) != -1
				|| strData.Find(strParam3) != -1)
			{
				return TRUE;
			}
			break;
		case 1: //查找3个参数是否有一个不存在
			if (strData.Find(strParam1) == -1 || strData.Find(strParam2) == -1
				|| strData.Find(strParam3) == -1)
			{
				return TRUE;
			}
			break;
		case 2: //查找第一个参数是否都存在，且其他二个有一个不存在不存在
			if ((strData.Find(strParam1) != -1) && (strData.Find(strParam2) == -1
				|| strData.Find(strParam3) == -1))
			{
				return TRUE;
			}
			break;
		case 3: //查找第一个和第二个参数存在，且最后一个参数不存在的情况
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
@brief 提取查找到字符串的位置
@param strData  要查找的数据；
@param 要查找的参数
@param 要查找的参数2
@param iInitPos 初始查找位置；
@return 返回查找到的结果；
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
@breif 提取指定位置字符串
@param strData 要从中提取的字符串；
@param strPara1 参数1
...
@param iInitPos 初始位置；
@return 返回截取到的字符串
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
