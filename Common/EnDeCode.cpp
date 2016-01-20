///////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2013, ��Ѷ����Ϣ���޹�˾
//
// �汾��1.0
// �ļ�˵������������Է�����
// �������ڣ�2013.04.09
// ���ߣ����Ƿ�
//
// �޸���ʷ��
// 1. ���ڣ�
//    ���ߣ�
//    �޸����ݣ�
// 2. 
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EnDeCode.h"


CStdString CEnDeCode::Encode(const CStdString &strSource)
{
	CStdString strTmp = strSource ;
	strTmp.Replace(_T("&") , _T("&a")) ;
	strTmp.Replace(_T(",") , _T("&r")) ;
//	strSource.Replace(_T("\r") , _T("&c")) ;

	return strTmp ;
}

CStdString CEnDeCode::Decode(const CStdString &strCode)
{
	CStdString strTmp = strCode ;
//	strCode.Replace(_T("&c") , _T("\r")) ;
	strTmp.Replace(_T("&r") , _T(",")) ;
	strTmp.Replace(_T("&a") , _T("&")) ;

	return strTmp ;
}

int CEnDeCode::SplitStringWithSeparator(const CStdString &strSource , const CStdString &strSeparator , std::vector<CStdString> &strVector , BOOL bIsIncludeEmpty)
{
	CStdString strSourceTemp = strSource ;
	CStdString strTemp = _T("") ;
	strVector.clear() ;
	if ( strSource == _T("") )
	{
		return 0 ;
	}

	DWORD dwSeparatorLen = strSeparator.GetLength() ;

	int nPostion = strSourceTemp.Find(strSeparator) ; 
	if (nPostion == -1)
	{
		strVector.push_back(strSourceTemp) ;
		return strVector.size() ;
	}

	while (nPostion != -1)
	{
		strTemp = strSourceTemp.Left(nPostion) ;
		if ( !strTemp.IsEmpty() || bIsIncludeEmpty)
		{
			strVector.push_back(strTemp) ;
		}
		strSourceTemp.Delete(0 , strTemp.GetLength()+dwSeparatorLen) ;
		strTemp.Empty();

		nPostion = strSourceTemp.Find(strSeparator) ;
	}

	if ( !strSourceTemp.IsEmpty() || bIsIncludeEmpty)
	{
		strVector.push_back(strSourceTemp) ;
	}
	

	return strVector.size() ;
}