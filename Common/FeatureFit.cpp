#include "StdAfx.h"
#include "FeatureFit.h"
#include "MyRegEx.h"
#include "StdStrUtils.h"
#include <vector>

bool FeatureFit::SplitSubStr(std::vector<SPLIT> &vecTmpSplit, CStdString str)//假如没有<#>....<#>就认为查找失败
{
	CStdString strTarget;//...234...,...32432...替换后组成的语句，也是返回语句
	CStdString strTmp;//<#>234<#>32432<#>这个语句
	SPLIT spTmp;

	//int  nFirst = str.find_first_of(_T("<#>"));
	//int nLast = str.find_last_of(_T("<#>"));
	
	int nFirst = str.Find(_T("<#>"));
	int nLast  = str.ReverseFind(_T("<#>"));

	if(nFirst >= 0 && nLast > nFirst)
	{
		strTmp = str.Mid(nFirst, nLast - nFirst + 3);
	}
	else
	{
		//vecTmpSplit.push_back(str);
		return false;
	}
	
	while(strTmp != _T("<#>"))
	{
		strTmp = strTmp.Mid(3);//跳过<#>
		int temp   = strTmp.find_first_of(_T("<"));
		CStdString strReplace = strTmp.Left(temp);
		strTmp = strTmp.Mid(temp);

		strTarget = str;
		strTarget.replace(nFirst, nLast - nFirst + 3, strReplace);
	
		spTmp.strReplace = strReplace;
		spTmp.strTarget  = strTarget;
		vecTmpSplit.push_back(spTmp);
	}

	return true;
}
/********************************************************************************
	'*  Function:
	'*      FitDefine
	'*  Description:
	'*      ' 判断输入字符串是否与数组的任何一个元素相似，不区分大小写
	'*  Input:
	'*      1.  szCompare               '待判断的字符串
	'*      2.  vstrRefer               '参照数组
	'*      3.  bSingleFit              '比较判断值（在判断"非""或""与"关系时，判断条件不同
	'*      4.  bDefault                '判断过滤数组为空数组时，返回的结果（没有过滤条件，是否认为有效，由外部判断）
	'*  Output:
	'*      '
	'********************************************************************************/
//判断结果
bool FeatureFit::FitDefine(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault)
{
	if (vstrRefer.size() <= 0)
	{
		return bDefault;
	}

	CStdString strTmp = strCompare.ToLower();  //统一转为小写
	CStdString strReferVal = _T("");
	CMyRegEx reg;    //正则表达式类
	
	for (int i = 0; i < vstrRefer.size(); i++)
	{
 		strReferVal = vstrRefer.at(i);

		if (strTmp.GetLength() > 0)
		{
			if (reg.Like(strTmp, strReferVal.ToLower()) == bSingleFit)
			{
				return bSingleFit;
			}
		}
		else
		{
			if ((strTmp.IsEmpty() && strReferVal.IsEmpty()) == bSingleFit)
			{
				return bSingleFit;
			}
		}
	}

	return bDefault;
}

bool FeatureFit::FitDefine3(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault, bool &bCanDelete, CStdString &strReplace)
{
	if (vstrRefer.size() <= 0)
	{
		return bDefault;
	}

	CStdString strTmp = strCompare.ToLower();  //统一转为小写
	CStdString strReferVal = _T("");
	CMyRegEx reg;    //正则表达式类
	std::vector<SPLIT> vecTmpSplit;//以<#>区分字符串的并行字符串//...234...,...32432...替换后组成的语句

	for (int i = 0; i < vstrRefer.size(); i++)
	{
		strReferVal = vstrRefer.at(i);

		bCanDelete = SplitSubStr(vecTmpSplit, strReferVal);
		if(bCanDelete)
		{
			for(int j = 0; j < vecTmpSplit.size(); j++)
			{
				strReferVal = vecTmpSplit[j].strTarget;

				if (strTmp.GetLength() > 0)
				{
					if (reg.Like(strTmp, strReferVal.ToLower()) == bSingleFit)
					{
						strReplace = vecTmpSplit[j].strReplace;
						return bSingleFit;
					}
					else
					{
						continue;
					}
				}
				else
				{
					if ((strTmp.IsEmpty() && strReferVal.IsEmpty()) == bSingleFit)
					{
						return bSingleFit;
					}
				}
			}
		}
		else
		{
			if (strTmp.GetLength() > 0)
			{
				if (reg.Like(strTmp, strReferVal.ToLower()) == bSingleFit)
				{
					return bSingleFit;
				}
			}
			else
			{
				if ((strTmp.IsEmpty() && strReferVal.IsEmpty()) == bSingleFit)
				{
					return bSingleFit;
				}
			}
		}
	}

	return bDefault;
}

//判断结果并返回最后比对项（失败时即为比对失败项）
bool FeatureFit::FitDefine2(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault, CStdString &strLastCmpItem)
{
	if (vstrRefer.size() <= 0) return bDefault;

	CStdString strTmp = strCompare.ToLower();  //统一转为小写
	CStdString strReferVal = _T("");
	CMyRegEx reg;    //正则表达式类

	for (int i = 0; i < vstrRefer.size(); i++)
	{
		strReferVal = vstrRefer.at(i);
		strLastCmpItem = strReferVal;

		//FitDefine2用于在网页源码中查找字符串，Like中不需与输入串进行精确相等匹配，故在此加上*
		if (-1 == strReferVal.Find(_T("*")))
		{
			strReferVal.append(_T("*"));
		}

		if (strTmp.GetLength() > 1)//如果strTmp为空，GetLength的长度为1，GetLength包括字符\n
		{
			if (reg.Like(strTmp, strReferVal.ToLower()) == bSingleFit)
				return bSingleFit;
		}
		else
		{
			if ((strTmp.IsEmpty() && strReferVal.IsEmpty()) == bSingleFit)
			{
				return bSingleFit;
			}
		}
	}

	return bDefault;
}


/*********************************************************************************
'*  Function:
'*      GetFSData
'*  Description:
'*      ' 将输入字符串分解成"与或非"的逻辑结构
'*  Input:
'*      1.  strInput                '待判断的字符串
'*      2.  strMainDelimiter        '主隔字符串
'*      3.  strSubDelimiter         '子分隔字符串
'*      4.  tResult                 '返回的"与或非"的逻辑结构
'*  Output:
'*      '
'********************************************************************************/
//将输入字符串分解成"与或非"的逻辑结构
void FeatureFit::GetFSData(CStdString strInput, CStdString strMainDelimiter, CStdString strSubDelimitor, T_FILTER_STRUCTURE &tResult)
{
	CStdStrUtils strUtil;
	std::vector<CStdString> vTmpRes;

	strUtil.SplitStringEx(strInput, strMainDelimiter, vTmpRes, true);
	
	for (int i = 0; i < vTmpRes.size(); i++)
	{
		if (i == 0)
		{
			strUtil.SplitStringEx(vTmpRes.at(i), strSubDelimitor, tResult.straAnd, false);
		}
		else if (i == 1)
		{
			strUtil.SplitStringEx(vTmpRes.at(i), strSubDelimitor, tResult.straOr, false);
		}
		else if (i == 2)
		{
			strUtil.SplitStringEx(vTmpRes.at(i), strSubDelimitor, tResult.straNot, false);
		}
	}
}

/********************************************************************************
	'*  Function:
	'*      FitFilter
	'*  Description:
	'*      ' 按照"与或非"的逻辑，判断输入字符串是否符合"与或非"过滤数组的定义，不区分大小写
	'*  Input:
	'*      1.  strCompare              '待判断的字符串
	'*      2.  tRefer                  '"与或非"逻辑结构
	'*      3.  bDefault                '判断过滤数组为空数组时，返回的结果（没有过滤条件，是否认为有效，由外部判断）
	'*  Output:
	'*      '
	'********************************************************************************/
bool FeatureFit::FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, vector<CStdString> &sFindCanDelete)
{
	CStdString strReplace;
	bool ret = true;
	bool bCanDelete = false;//如果是正常的步骤，没有<#>...<#>分开的步骤，就不用写入sFindCanDelete

	if (tRefer.straAnd.size() == 0 && tRefer.straNot.size() == 0 && tRefer.straOr.size() == 0)
		return bDefult;

	//先判断不能存在的（优先级最高）
	if (FitDefine3(strCompare, tRefer.straNot, true, false, bCanDelete, strReplace))
		return false;

	//再判断必须存在的（优先级高）
	if (!FitDefine3(strCompare, tRefer.straAnd, false, true, bCanDelete, strReplace))
		return false;

	//最后判断可选的（优先级低），如果没有可选的，认为属于默认值
	if (tRefer.straOr.size() > 0)
		ret = FitDefine3(strCompare, tRefer.straOr, true, false, bCanDelete, strReplace);
	//else
		//return FitDefine(strCompare, tRefer.straOr, true, true);
		//return true;
	if(ret && bCanDelete)
	{
		if(strReplace != _T(""))
		{
			sFindCanDelete.push_back(strReplace);
		}
	}

	return ret;
}

bool FeatureFit::FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, CStdString &strFindCanDelete)
{
	CStdString strReplace;
	bool ret = true;
	bool bCanDelete = false;//如果是正常的步骤，没有<#>...<#>分开的步骤，就不用写入sFindCanDelete

	if (tRefer.straAnd.size() == 0 && tRefer.straNot.size() == 0 && tRefer.straOr.size() == 0)
		return bDefult;

	//先判断不能存在的（优先级最高）
	if (FitDefine3(strCompare, tRefer.straNot, true, false, bCanDelete, strFindCanDelete))
		return false;

	//再判断必须存在的（优先级高）
	if (!FitDefine3(strCompare, tRefer.straAnd, false, true, bCanDelete, strFindCanDelete))
		return false;

	//最后判断可选的（优先级低），如果没有可选的，认为属于默认值
	if (tRefer.straOr.size() > 0)
		ret = FitDefine3(strCompare, tRefer.straOr, true, false, bCanDelete, strFindCanDelete);

	if(ret && bCanDelete)
	{
		if(strReplace != _T(""))
		{
			strFindCanDelete = strReplace;
		}
	}

	return ret;
}


bool FeatureFit::FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult)
{
	if (tRefer.straAnd.size() == 0 && tRefer.straNot.size() == 0 && tRefer.straOr.size() == 0)
		return bDefult;

	//先判断不能存在的（优先级最高）
	if (FitDefine(strCompare, tRefer.straNot, true, false))
		return false;

	//再判断必须存在的（优先级高）
	if (!FitDefine(strCompare, tRefer.straAnd, false, true))
		return false;

	//最后判断可选的（优先级低），如果没有可选的，认为属于默认值
	if (tRefer.straOr.size() > 0)
		return FitDefine(strCompare, tRefer.straOr, true, false);
	else
	//return FitDefine(strCompare, tRefer.straOr, true, true);
	return true;
}
/********************************************************************************
	'*  Function:
	'*      FitFilter2
	'*  Description:
	'*      ' 按照"与或非"的逻辑，判断输入字符串是否符合"与或非"过滤数组的定义，不区分大小写
	'*  Input:
	'*      1.  strCompare              '待判断的字符串
	'*      2.  tRefer                  '"与或非"逻辑结构
	'*      3.  bDefault                '判断过滤数组为空数组时，返回的结果（没有过滤条件，是否认为有效，由外部判断）
	'*  Output:
	'*      '0:成功 1：检查Not失败 2：检查And失败 3：检查Or失败
	'********************************************************************************/
int FeatureFit::FitFilter2(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, CStdString &strErrInfo)
{
	if (tRefer.straAnd.size() == 0 && tRefer.straNot.size() == 0 && tRefer.straOr.size() == 0)
		return bDefult ? 0 : 3;  //默认成功时返回0，否则返回3

	//先判断不能存在的（优先级最高）
	if (FitDefine2(strCompare, tRefer.straNot, true, false, strErrInfo))
	{
		strErrInfo = _T("查找到Not关键字'") + strErrInfo + _T("'") ;
		return 1;     //1：检查Not失败
	}
		

	//再判断必须存在的（优先级高）
	if (!FitDefine2(strCompare, tRefer.straAnd, false, true, strErrInfo))
	{
		strErrInfo = _T("查找'") + strErrInfo + _T("'失败");
		return 2;     //2：检查And失败
	}

	//最后判断可选的（优先级低），如果没有可选的，认为属于默认值
	if (tRefer.straOr.size() > 0 && !FitDefine2(strCompare, tRefer.straOr, true, false, strErrInfo))
	{
		strErrInfo = _T("查找'") + strErrInfo + _T("'失败");
		return 3;	  //3：检查Or失败
	}

	return 0;         //0:成功
}
