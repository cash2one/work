#include "StdAfx.h"
#include "FeatureFit.h"
#include "MyRegEx.h"
#include "StdStrUtils.h"
#include <vector>

bool FeatureFit::SplitSubStr(std::vector<SPLIT> &vecTmpSplit, CStdString str)//����û��<#>....<#>����Ϊ����ʧ��
{
	CStdString strTarget;//...234...,...32432...�滻����ɵ���䣬Ҳ�Ƿ������
	CStdString strTmp;//<#>234<#>32432<#>������
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
		strTmp = strTmp.Mid(3);//����<#>
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
	'*      ' �ж������ַ����Ƿ���������κ�һ��Ԫ�����ƣ������ִ�Сд
	'*  Input:
	'*      1.  szCompare               '���жϵ��ַ���
	'*      2.  vstrRefer               '��������
	'*      3.  bSingleFit              '�Ƚ��ж�ֵ�����ж�"��""��""��"��ϵʱ���ж�������ͬ
	'*      4.  bDefault                '�жϹ�������Ϊ������ʱ�����صĽ����û�й����������Ƿ���Ϊ��Ч�����ⲿ�жϣ�
	'*  Output:
	'*      '
	'********************************************************************************/
//�жϽ��
bool FeatureFit::FitDefine(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault)
{
	if (vstrRefer.size() <= 0)
	{
		return bDefault;
	}

	CStdString strTmp = strCompare.ToLower();  //ͳһתΪСд
	CStdString strReferVal = _T("");
	CMyRegEx reg;    //������ʽ��
	
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

	CStdString strTmp = strCompare.ToLower();  //ͳһתΪСд
	CStdString strReferVal = _T("");
	CMyRegEx reg;    //������ʽ��
	std::vector<SPLIT> vecTmpSplit;//��<#>�����ַ����Ĳ����ַ���//...234...,...32432...�滻����ɵ����

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

//�жϽ�����������ȶ��ʧ��ʱ��Ϊ�ȶ�ʧ���
bool FeatureFit::FitDefine2(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault, CStdString &strLastCmpItem)
{
	if (vstrRefer.size() <= 0) return bDefault;

	CStdString strTmp = strCompare.ToLower();  //ͳһתΪСд
	CStdString strReferVal = _T("");
	CMyRegEx reg;    //������ʽ��

	for (int i = 0; i < vstrRefer.size(); i++)
	{
		strReferVal = vstrRefer.at(i);
		strLastCmpItem = strReferVal;

		//FitDefine2��������ҳԴ���в����ַ�����Like�в��������봮���о�ȷ���ƥ�䣬���ڴ˼���*
		if (-1 == strReferVal.Find(_T("*")))
		{
			strReferVal.append(_T("*"));
		}

		if (strTmp.GetLength() > 1)//���strTmpΪ�գ�GetLength�ĳ���Ϊ1��GetLength�����ַ�\n
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
'*      ' �������ַ����ֽ��"����"���߼��ṹ
'*  Input:
'*      1.  strInput                '���жϵ��ַ���
'*      2.  strMainDelimiter        '�����ַ���
'*      3.  strSubDelimiter         '�ӷָ��ַ���
'*      4.  tResult                 '���ص�"����"���߼��ṹ
'*  Output:
'*      '
'********************************************************************************/
//�������ַ����ֽ��"����"���߼��ṹ
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
	'*      ' ����"����"���߼����ж������ַ����Ƿ����"����"��������Ķ��壬�����ִ�Сд
	'*  Input:
	'*      1.  strCompare              '���жϵ��ַ���
	'*      2.  tRefer                  '"����"�߼��ṹ
	'*      3.  bDefault                '�жϹ�������Ϊ������ʱ�����صĽ����û�й����������Ƿ���Ϊ��Ч�����ⲿ�жϣ�
	'*  Output:
	'*      '
	'********************************************************************************/
bool FeatureFit::FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, vector<CStdString> &sFindCanDelete)
{
	CStdString strReplace;
	bool ret = true;
	bool bCanDelete = false;//����������Ĳ��裬û��<#>...<#>�ֿ��Ĳ��裬�Ͳ���д��sFindCanDelete

	if (tRefer.straAnd.size() == 0 && tRefer.straNot.size() == 0 && tRefer.straOr.size() == 0)
		return bDefult;

	//���жϲ��ܴ��ڵģ����ȼ���ߣ�
	if (FitDefine3(strCompare, tRefer.straNot, true, false, bCanDelete, strReplace))
		return false;

	//���жϱ�����ڵģ����ȼ��ߣ�
	if (!FitDefine3(strCompare, tRefer.straAnd, false, true, bCanDelete, strReplace))
		return false;

	//����жϿ�ѡ�ģ����ȼ��ͣ������û�п�ѡ�ģ���Ϊ����Ĭ��ֵ
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
	bool bCanDelete = false;//����������Ĳ��裬û��<#>...<#>�ֿ��Ĳ��裬�Ͳ���д��sFindCanDelete

	if (tRefer.straAnd.size() == 0 && tRefer.straNot.size() == 0 && tRefer.straOr.size() == 0)
		return bDefult;

	//���жϲ��ܴ��ڵģ����ȼ���ߣ�
	if (FitDefine3(strCompare, tRefer.straNot, true, false, bCanDelete, strFindCanDelete))
		return false;

	//���жϱ�����ڵģ����ȼ��ߣ�
	if (!FitDefine3(strCompare, tRefer.straAnd, false, true, bCanDelete, strFindCanDelete))
		return false;

	//����жϿ�ѡ�ģ����ȼ��ͣ������û�п�ѡ�ģ���Ϊ����Ĭ��ֵ
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

	//���жϲ��ܴ��ڵģ����ȼ���ߣ�
	if (FitDefine(strCompare, tRefer.straNot, true, false))
		return false;

	//���жϱ�����ڵģ����ȼ��ߣ�
	if (!FitDefine(strCompare, tRefer.straAnd, false, true))
		return false;

	//����жϿ�ѡ�ģ����ȼ��ͣ������û�п�ѡ�ģ���Ϊ����Ĭ��ֵ
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
	'*      ' ����"����"���߼����ж������ַ����Ƿ����"����"��������Ķ��壬�����ִ�Сд
	'*  Input:
	'*      1.  strCompare              '���жϵ��ַ���
	'*      2.  tRefer                  '"����"�߼��ṹ
	'*      3.  bDefault                '�жϹ�������Ϊ������ʱ�����صĽ����û�й����������Ƿ���Ϊ��Ч�����ⲿ�жϣ�
	'*  Output:
	'*      '0:�ɹ� 1�����Notʧ�� 2�����Andʧ�� 3�����Orʧ��
	'********************************************************************************/
int FeatureFit::FitFilter2(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, CStdString &strErrInfo)
{
	if (tRefer.straAnd.size() == 0 && tRefer.straNot.size() == 0 && tRefer.straOr.size() == 0)
		return bDefult ? 0 : 3;  //Ĭ�ϳɹ�ʱ����0�����򷵻�3

	//���жϲ��ܴ��ڵģ����ȼ���ߣ�
	if (FitDefine2(strCompare, tRefer.straNot, true, false, strErrInfo))
	{
		strErrInfo = _T("���ҵ�Not�ؼ���'") + strErrInfo + _T("'") ;
		return 1;     //1�����Notʧ��
	}
		

	//���жϱ�����ڵģ����ȼ��ߣ�
	if (!FitDefine2(strCompare, tRefer.straAnd, false, true, strErrInfo))
	{
		strErrInfo = _T("����'") + strErrInfo + _T("'ʧ��");
		return 2;     //2�����Andʧ��
	}

	//����жϿ�ѡ�ģ����ȼ��ͣ������û�п�ѡ�ģ���Ϊ����Ĭ��ֵ
	if (tRefer.straOr.size() > 0 && !FitDefine2(strCompare, tRefer.straOr, true, false, strErrInfo))
	{
		strErrInfo = _T("����'") + strErrInfo + _T("'ʧ��");
		return 3;	  //3�����Orʧ��
	}

	return 0;         //0:�ɹ�
}
