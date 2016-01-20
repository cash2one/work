#include "stdafx.h"
#include "StdStrUtils.h"

//����ָ���ֽ��ַ�����ֵ�vector�������ڷָ���ʱvectorΪ��
int CStdStrUtils::SplitString(const CStdString& input, const CStdString& delimiter, std::vector<CStdString>& results, bool includeEmpties)
{
	int iPos = 0;
	int newPos = -1;
	int sizeS2 = (int)delimiter.size();
	int isize = (int)input.size();

	int offset = 0;
	CStdString s;

	if( 
		( isize == 0 )
		||
		( sizeS2 == 0 )
		)
	{
		return 0;
	}

	std::vector<int> positions;

	newPos = input.find (delimiter, 0);

	if( newPos < 0 )
	{ 
		return 0; 
	}

	int numFound = 0;

	while( newPos >= iPos )
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		if (iPos+sizeS2 < isize)
		{
			newPos = input.find (delimiter, iPos+sizeS2);
		}
		else
		{
			newPos = -1;
		}
	}

	if( numFound == 0 )
	{
		return 0;
	}

	for( int i=0; i <= (int)positions.size(); ++i )
	{
		s.Empty();
		if( i == 0 ) 
		{ 
			s = input.substr( i, positions[i] ); 
		}
		else
		{
			offset = positions[i-1] + sizeS2;

			if( offset < isize )
			{
				if( i == positions.size() )
				{
					s = input.substr(offset);
				}
				else if( i > 0 )
				{
					s = input.substr( positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2 );
				}
			}
		}

		if( includeEmpties || ( s.size() > 0 ) )
		{
			results.push_back(s);
		}
	}
	return numFound;
}

//����ָ���ֽ��ַ�����ֵ�vector�������ڷָ���ʱvector���������ַ���
void CStdStrUtils::SplitStringEx(const CStdString& input, const CStdString& delimiter, std::vector<CStdString>& results, bool includeEmpties)
{
	int iSize = SplitString(input, delimiter, results, includeEmpties);
	if (iSize == 0)
	{
		//����Ϊ�ղ������Ҫ�󲻰�����ʱֱ�ӷ���
		if (input.IsEmpty() && !includeEmpties)
			return ;
		else
			results.push_back(input);
	}
}

//unicode����ת���ĺ���,��gb������ת��Ϊbig5��(����ʽ��)
DWORD CStdStrUtils::UnicodeGBToUnicodeBIG5(const CStdString& szSource, CStdString& szDest)
{
	int dwSourceCount;
	TCHAR * SourceChar = NULL;
	//    TCHAR * RetChar = NULL;

	dwSourceCount = szSource.length() ;
	////��������Ĵ�С
	_NEWA(SourceChar,TCHAR,dwSourceCount + 1);
	_tcscpy(SourceChar , szSource); 
	SourceChar[dwSourceCount] = 0;

	if (NULL == SourceChar)
	{
		return 0;
	}

	//    //��������Ĵ�С
	//    _NEWA(RetChar,TCHAR,dwSourceCount + 1);
	//    RetChar[dwSourceCount] = 0;
	//
	//    if (NULL == RetChar)
	//    {
	//        _DELETEA(SourceChar);
	//        return 0;
	//    }

	////ת��Gb����嵽Gb�뷱�壬ʹ��API����LCMapString 
	LCMapString(0x0804,LCMAP_TRADITIONAL_CHINESE, szSource, -1, SourceChar, dwSourceCount*2); 

	szDest = SourceChar;


	if(SourceChar)
	{
		delete[] SourceChar;
		SourceChar = NULL;
	}
	//    _DELETEA(RetChar);
	return 1;
}

//unicode����ת���ĺ���,��big5������ת��Ϊgb��(����ʽ��)
DWORD CStdStrUtils::UnicodeBIG5ToUnicodeGB(const CStdString& szSource, CStdString& szDest)
{
	int dwSourceCount;
	TCHAR * SourceChar = NULL;
	//    TCHAR * RetChar = NULL;

	dwSourceCount = szSource.length() ;
	////��������Ĵ�С
	_NEWA(SourceChar,TCHAR,dwSourceCount + 1);
	_tcscpy(SourceChar , szSource); 
	SourceChar[dwSourceCount] = 0;

	if (NULL == SourceChar)
	{
		return 0;
	}

	//    //��������Ĵ�С
	//    _NEWA(RetChar,TCHAR,dwSourceCount + 1);
	//    RetChar[dwSourceCount] = 0;
	//
	//    if (NULL == RetChar)
	//    {
	//        _DELETEA(SourceChar);
	//        return 0;
	//    }

	////ת��Gb�뷱����嵽Gb�룬ʹ��API����LCMapString 
	LCMapString(0x0804,LCMAP_TRADITIONAL_CHINESE, szSource, -1, SourceChar, dwSourceCount*2); 


	szDest = SourceChar;


	if(SourceChar)
	{
		delete[] SourceChar;
		SourceChar = NULL;
	}
	//    _DELETEA(RetChar);
	return 1;

}

///////////////////////////////////////////////////////////////////////////////
//
// ��������������   �Ѷ��ֽ��ַ�ת��Ϊ���ַ�
// ���룺           lpcwszStr:���ַ�,    dwSize:���buffer����
// �����           lpszStr:�����buffer,
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
DWORD CStdStrUtils::MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize)
{
	DWORD dwMinSize;

	dwMinSize = MultiByteToWideChar(CP_ACP,NULL,lpcszStr,-1,NULL,0);
	if(*dwSize < dwMinSize)
	{
		return FALSE;
	}


	MultiByteToWideChar(CP_ACP,NULL,lpcszStr,-1,lpwszStr,*dwSize);

	*dwSize = dwMinSize;
	return TRUE;
}

DWORD CStdStrUtils::WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
	if(*dwSize < dwMinSize)
	{
		return FALSE;
	}
	WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,*dwSize,NULL,FALSE);
	*dwSize = dwMinSize;
	return TRUE;
}

bool CStdStrUtils::IsNumber(CStdString strData)
{
	if (strData.IsEmpty())
		return false;

	bool bNumFlag = true;

	for(int i = 0; i< strData.GetLength(); i++)
	{
		if(((int)strData.GetAt(i) >= 48 && (int)strData.GetAt(i) <= 57) || strData.GetAt(i) == _T('.'))
			continue;	
		else
		{
			bNumFlag = false;
			break;
		}
	}

	return bNumFlag;
}
