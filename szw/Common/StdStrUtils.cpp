#include "stdafx.h"
#include "StdStrUtils.h"

//根据指定分将字符串拆分到vector，不存在分隔符时vector为空
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

//根据指定分将字符串拆分到vector，不存在分隔符时vector返回输入字符串
void CStdStrUtils::SplitStringEx(const CStdString& input, const CStdString& delimiter, std::vector<CStdString>& results, bool includeEmpties)
{
	int iSize = SplitString(input, delimiter, results, includeEmpties);
	if (iSize == 0)
	{
		//输入为空并且输出要求不包括空时直接返回
		if (input.IsEmpty() && !includeEmpties)
			return ;
		else
			results.push_back(input);
	}
}

//unicode内码转换的函数,将gb的字样转换为big5的(简体式样)
DWORD CStdStrUtils::UnicodeGBToUnicodeBIG5(const CStdString& szSource, CStdString& szDest)
{
	int dwSourceCount;
	TCHAR * SourceChar = NULL;
	//    TCHAR * RetChar = NULL;

	dwSourceCount = szSource.length() ;
	////传入数组的大小
	_NEWA(SourceChar,TCHAR,dwSourceCount + 1);
	_tcscpy(SourceChar , szSource); 
	SourceChar[dwSourceCount] = 0;

	if (NULL == SourceChar)
	{
		return 0;
	}

	//    //返回数组的大小
	//    _NEWA(RetChar,TCHAR,dwSourceCount + 1);
	//    RetChar[dwSourceCount] = 0;
	//
	//    if (NULL == RetChar)
	//    {
	//        _DELETEA(SourceChar);
	//        return 0;
	//    }

	////转换Gb码简体到Gb码繁体，使用API函数LCMapString 
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

//unicode内码转换的函数,将big5的字样转换为gb的(繁体式样)
DWORD CStdStrUtils::UnicodeBIG5ToUnicodeGB(const CStdString& szSource, CStdString& szDest)
{
	int dwSourceCount;
	TCHAR * SourceChar = NULL;
	//    TCHAR * RetChar = NULL;

	dwSourceCount = szSource.length() ;
	////传入数组的大小
	_NEWA(SourceChar,TCHAR,dwSourceCount + 1);
	_tcscpy(SourceChar , szSource); 
	SourceChar[dwSourceCount] = 0;

	if (NULL == SourceChar)
	{
		return 0;
	}

	//    //返回数组的大小
	//    _NEWA(RetChar,TCHAR,dwSourceCount + 1);
	//    RetChar[dwSourceCount] = 0;
	//
	//    if (NULL == RetChar)
	//    {
	//        _DELETEA(SourceChar);
	//        return 0;
	//    }

	////转换Gb码繁体简体到Gb码，使用API函数LCMapString 
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
// 函数功能描述：   把多字节字符转换为宽字符
// 输入：           lpcwszStr:宽字符,    dwSize:输出buffer长度
// 输出：           lpszStr:输出的buffer,
// 返回值：
// 其它说明：
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
