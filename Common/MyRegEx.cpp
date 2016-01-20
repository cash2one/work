#include "StdAfx.h"
#include "MyRegEx.h"
//#include <boost/regex.hpp>
#include <vector>
#include <stack>


CMyRegEx::CMyRegEx(void)
{
}


CMyRegEx::~CMyRegEx(void)
{
}

bool CMyRegEx::Like(CStdString strInput, CStdString strRegEx)
{
	std::vector<CStdString> vecLikeWord ;
	EDTool.SplitStringWithSeparator(strRegEx , _T("*") , vecLikeWord) ;

	if (vecLikeWord.size() == 1)
	{
		if ((vecLikeWord[0]).Trim() == strInput.Trim())
			return true ;
		else
			return false;
	}

	int nPos = 0 ;
	for (int i=0 ; i<vecLikeWord.size() ; ++i)
	{
		nPos = strInput.Find(vecLikeWord[i] , nPos) ;
		if (nPos == -1)
		{
			return false ;
		}
	}

	return true ;

}

bool CMyRegEx::Match(CStdString strInput, CStdString strRegEx, std::vector<CStdString> &vRes)
{
	vRes.clear() ;
	if ( strRegEx.Trim().IsEmpty() )
	{
		vRes.push_back(strInput) ;
		return true ;
	}

	int nExtraCount = 0 ;
	int nPosLast = 0 ;
	int nLen = strRegEx.GetLength() ;
	std::vector<int> vecExtraIndex ;
	for ( int i=0 ; i<nLen; ++i )
	{
		if ( _T('*') == strRegEx.GetAt(i) )
		{
			nExtraCount ++ ;
		}
		if ( _T('@') == strRegEx.GetAt(i) )
		{
			vecExtraIndex.push_back(nExtraCount) ;
			nExtraCount ++ ;
		}
	}
	//û����Ҫ��ȡ������
	if ( vecExtraIndex.size() == 0 )
	{
		return true ;
	}

	//��@ͨ����滻Ϊ* ����ͳһƥ����ȡ
	CStdString strRegExTmp = strRegEx ;
	strRegExTmp.Replace( _T("@"), _T("*") ) ;
	std::vector<CStdString> vecLikeWord ;
	EDTool.SplitStringWithSeparator(strRegExTmp , _T("*") , vecLikeWord , FALSE) ;

	int nPosStart = 0 ;
	int nPosEnd = 0 ;
	std::vector<CStdString> vRetAll ;
	CStdString strRetData ;
	for (int i=0 ; i<vecLikeWord.size() ; ++i)
	{
		nPosStart = nPosEnd ;
		nPosEnd = strInput.Find(vecLikeWord[i] , nPosEnd) ;
		if (nPosEnd == -1)
		{
			return false ;
		}

		if ( i == 0 && nPosEnd != 0 )
		{
			strRetData = strInput.Left(nPosEnd) ;
			vRetAll.push_back(strRetData) ;
		}

		if (i > 0)
		{
			strRetData = strInput.Mid(nPosStart , nPosEnd-nPosStart) ;
			vRetAll.push_back(strRetData) ;
		}

		nPosEnd += vecLikeWord[i].GetLength() ;
	}

	//����������ȡ��� ����ȡ��������Ҫ��ȡ��ͨ���ʱ
	nLen = strInput.GetLength() ;
	int nDifferCount = nExtraCount - vRetAll.size() ;
	if ( nPosEnd<nLen && nDifferCount>0 )
	{
		CStdString strRetData = strInput.Mid(nPosEnd , nLen-nPosEnd) ;
		for ( int i=0 ; i<nDifferCount; ++i)
		{
			vRetAll.push_back(strRetData) ;
		}
	}

	//���� @ͨ���������
	for ( int i=0 ; i<vecExtraIndex.size() ; ++i)
	{
		vRes.push_back( vRetAll[ vecExtraIndex[i] ] ) ;
	}

	return true ;
}

void CMyRegEx::ExtractCharList(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords)
{
	int nPosStart = 0 ;
	int nPosEnd = 0 ;
	
	CStdString strSymbol ;
	std::vector<CStdString> vecSymbolTmp ;
	while (nPosEnd < strInPut.GetLength())
	{
		nPosStart = strInPut.Find(_T("[") , nPosEnd) ;
		if (nPosStart != -1)
		{
			nPosEnd = strInPut.Find(_T("]") , nPosStart) ;

			if (nPosEnd != -1)
			{
				strSymbol = strInPut.Mid(nPosStart , nPosEnd-nPosStart+1) ;
				vecSymbolTmp.push_back(strSymbol) ;
				nPosEnd = nPosStart + strSymbol.GetLength() ;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	int nSize = vecSymbolTmp.size() ;
	if (nSize == 0)
	{
		return ;
	}

	nPosStart = nPosEnd = 0 ;
	CStdString strWord ;
	for (int i=0 ; i<nSize ; ++i)
	{
		strSymbol = vecSymbolTmp[i] ;
		nPosEnd = strInPut.Find(strSymbol , nPosStart) ;
		if (nPosEnd == -1)
		{
			continue;
		}
		strWord = strInPut.Mid(nPosStart , nPosEnd-nPosStart) ;
		vecMatchWords.push_back(strWord) ;
		vecMatchSymbols.push_back(strSymbol);

		nPosStart = nPosEnd + strSymbol.GetLength() ;
	}
}

void CMyRegEx::ExtractQuestionMark(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords)
{
	int nPosStart = 0 ;
	int nPosEnd = 0 ;
	int nLen = strInPut.GetLength() ;

	CStdString strSymbol , strWord ;
//	std::vector<CStdString> vecSymbolTmp ;
	while (nPosEnd < nLen )
	{
		nPosStart = strInPut.Find(_T("?") , nPosEnd) ;
		strWord = strInPut.Mid(nPosEnd , nPosStart-nPosEnd+1) ;
		vecMatchWords.push_back(strWord) ;

		if (nPosStart == -1)
		{
			break; 
		}
		nPosEnd = nPosStart ;
		while ( strInPut.GetAt(++nPosEnd) == L'?' ) ;

		strSymbol = strInPut.Mid(nPosStart , nPosEnd-nPosStart) ;
		vecMatchSymbols.push_back(strSymbol) ;
	}

	if (nPosEnd < nLen)
	{
		strWord = strInPut.Mid(nPosEnd , nLen-nPosEnd-1) ;
		vecMatchWords.push_back(strWord) ;
	}
}











/********************************************************************************
*  Function:
*      FitFilter
*  Description: �����봮��������ƥ��
*  Input:
*      1.  strInput                '��ƥ����ַ���
*      2.  strRegEx                '������ʽ
*  Output:�ɹ�����true,ʧ�ܷ���false
********************************************************************************/

/*
bool CMyRegEx::Like(CStdString strInput, CStdString strRegEx)
{
	bool bLike = false;

	try
	{
#ifdef _UNICODE	
		boost::wregex reg(strRegEx.GetBuffer());
		bLike = boost::regex_match(strInput.GetBuffer(), reg);
#else
		boost::regex reg(strRegEx.GetBuffer());
		bLike = boost::regex_match(strInput.GetBuffer(), reg);

#endif
	}
	catch (boost::regex_error &err)
	{
		CStdString strMsg = _T("");
		const char *pMsg = err.what();
		DWORD dwLen = strlen(pMsg) + 1;
		WCHAR *pwcsMsg = new WCHAR[dwLen];
		MByteToWChar((LPSTR)pMsg, pwcsMsg, &dwLen);
		strMsg.Format(_T("ƥ�����%s"), pwcsMsg);
		delete pwcsMsg;

		MessageBox(NULL, strMsg, _T("������ʽ"), MB_OK);
	}

	return bLike;
}*/


/********************************************************************************
*  Function:
*      FitFilter
*  Description: �����봮��������ƥ�䣬������ƥ�䵽��ֵ
*  Input:
*      1.  strInput                '��ƥ����ַ���
*      2.  strRegEx                '������ʽ
*      3.  vRes                    '����������ʽƥ�䵽������
*  Output:�ɹ�����true,ʧ�ܷ���false
********************************************************************************/
/*
bool CMyRegEx::Match(CStdString strInput, CStdString strRegEx, std::vector<CStdString> &vRes)
{
	vRes.clear();

	try
	{
#ifdef _UNICODE
		boost::wcmatch what;
		boost::wregex reg(strRegEx.GetBuffer());
#else
		boost::cmatch what;
		boost::regex reg(strRegEx.GetBuffer());
#endif

		if (boost::regex_match(strInput.GetBuffer(), what, reg))
		{
			for (int i = 0; i < what.size(); i++)
			{
				vRes.push_back(what[i].str());
			}

			return true;
		}
	}
	catch (boost::regex_error &err)
	{
		CStdString strMsg = _T("");
		const char *pMsg = err.what();
		DWORD dwLen = strlen(pMsg) + 1;
		WCHAR *pwcsMsg = new WCHAR[dwLen];
		MByteToWChar((LPSTR)pMsg, pwcsMsg, &dwLen);
		strMsg.Format(_T("ƥ�����%s"), pwcsMsg);
		delete pwcsMsg;

		MessageBox(NULL, strMsg, _T("������ʽ"), MB_OK);
	}

	return false;
}*/

//�Ǻ�ƥ�� *�����������ַ�




