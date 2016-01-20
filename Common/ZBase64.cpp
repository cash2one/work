#include "stdAfx.h"
#include "ZBase64.h"
CZBase64::CZBase64()
{
	//Base64编码表
	this->m_Base64_Table="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	strSeeds =  "vbcdefghinopqrstujklmwxCD<>yzAB&*(}';OPQ)[]{RST:,=+~`^|%_MNUEFGHIKL -.#\\!@$VWXYZ12456890a37J?/" ;
	m_vecBufString.clear() ;
}
CZBase64::~CZBase64()
{
	/*if (m_szBufString)
	{
	delete []m_szBufString ;
	m_szBufString = NULL ;
	}

	if (m_Base64_Table)
	{
	delete[]m_Base64_Table ;
	m_Base64_Table = NULL ;
	}*/
}
string CZBase64::EncodeBase64(const string strSource)
{
	/*
	* 以下是操作二进制数时用到的
	* 11111100  0xFC
	* 11000000  0x3
	* 11110000  0xF0
	* 00001111  0xF
	* 11000000  0xC0
	* 00111111  0x3F
	*/
	string strEncode;
	char cTemp[4];
	//行长,MIME规定Base64编码行长为76字节
	int LineLength=0;
	
	int nSize = strSource.size() ;
	for(int i=0;i<nSize;i+=3)
	{
		memset(cTemp,0,4);
		cTemp[0]=strSource[i];
		if (i+1<nSize)
		{
			cTemp[1]=strSource[i+1];
		}
		if (i+2<nSize)
		{
			cTemp[2]=strSource[i+2];
		}
		
		int len=strlen(cTemp);
		if(len==3)
		{
			strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0xFC)>>2];
			strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0x3)<<4 | ((int)cTemp[1] & 0xF0)>>4];
			strEncode+=this->m_Base64_Table[((int)cTemp[1] & 0xF)<<2 | ((int)cTemp[2] & 0xC0)>>6];
			strEncode+=this->m_Base64_Table[(int)cTemp[2] & 0x3F];
			if(LineLength+=4>=76) strEncode+="\r\n";
		}
		else if(len==2)
		{
			strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0xFC)>>2];
			strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0x3 )<<4 | ((int)cTemp[1] & 0xF0 )>>4];
			strEncode+=this->m_Base64_Table[((int)cTemp[1] & 0x0F)<<2];
			if(LineLength+=4>=76) strEncode+="\r\n";
			strEncode+="=";
		}
		else if(len==1)
		{
			strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0xFC)>>2];
			strEncode+=this->m_Base64_Table[((int)cTemp[0] & 0x3 )<<4];
			if(LineLength+=4>=76) strEncode+="\r\n";
			strEncode+="==";
		}
		memset(cTemp,0,4);
	}
	return strEncode;
}
string CZBase64::DecodeBase64(const string strSource)
{
	//返回值
	string strDecode;
	char cTemp[5];

	int nSize = strSource.size() ;
	if ( nSize%4 )
	{
		return "Decode Error!!" ;
	}

	for(int i=0 ; i<nSize ; i+=4)
	{
		memset(cTemp,0,5);
		cTemp[0]=strSource[i];
		cTemp[1]=strSource[i+1];
		cTemp[2]=strSource[i+2];
		cTemp[3]=strSource[i+3];

		int asc[4];
		for(int j=0 ; j<4 ; j++)
		{
			for(int k=0;k<(int)strlen(this->m_Base64_Table);k++)
			{
				if(cTemp[j]==this->m_Base64_Table[k]) asc[j]=k;
			}
		}
		if('='==cTemp[2] && '='==cTemp[3])
		{
			strDecode+=(char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
		}
		else if('='==cTemp[3])
		{
			strDecode+=(char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
			strDecode+=(char)(int)(asc[1] << 4 | asc[2] << 2 >> 4);
		}
		else
		{
			strDecode+=(char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
			strDecode+=(char)(int)(asc[1] << 4 | asc[2] << 2 >> 4);
			strDecode+=(char)(int)(asc[2] << 6 | asc[3] << 2 >> 2);
		}  
	}

	return strDecode;
}

string CZBase64::DeEncrypt(string &strInput)
{
	int i = 0 ;
	int j = 0 ;
	int k = 0 ;
	int nPos = 0 ;
	string strTmp ;
	string strPart ;
	string *astrSeeds ;

	m_nSeedLen = strSeeds.length() ;
	astrSeeds = new string[m_nSeedLen] ;

	InitializeSeeds(astrSeeds) ;
	m_strLength = 0 ;
	m_vecBufString.clear() ;
	for (int i=0 ; i<strInput.length() ; ++i)
	{
		strTmp = strInput.substr(i , 1) ;
		nPos = strSeeds.find(strTmp , 0 ) ;
		if (nPos == -1)
		{
			return "Decode Error!!" ;
		}
		strPart = m_strKey.substr(j , 1) ;

		for (int k=0 ; k<strSeeds.length() ; ++k)
		{
			if (strPart == astrSeeds[k].substr(nPos , 1))
			{
				AddStr(astrSeeds[k].substr(0 , 1)) ;
				break;
			}
		}

		j ++ ;
		if (j > m_strKey.length()-1)
		{
			j = 0 ;
		}
	}

	if (NULL != astrSeeds)						//zhl  防内存泄漏
	{
		delete[] astrSeeds;
		astrSeeds=NULL;
	}
	string strResult;
	std::vector<char>::iterator iter ;
	for (iter=m_vecBufString.begin() ; iter!=m_vecBufString.end() ; ++iter)
	{
		strResult.append(1,*iter) ;
	}
	return strResult ;
}

void CZBase64::InitializeSeeds(string *astrSeeds)
{
	astrSeeds[0] = strSeeds ;

	for (int i=0 ; i<m_nSeedLen-1 ; ++i)
	{
		astrSeeds[i+1] = astrSeeds[i].substr(1 , astrSeeds[i].length()-1) + astrSeeds[i].substr(0 , 1);	
	}
}

string CZBase64::EncryptText(string &strInput , string &strKey)
{
	m_strKey = strKey ;
	return DeEncrypt(EncodeBase64(strInput)) ;
}

string CZBase64::DecryptText(string &strInput , string &strKey)
{
	m_strKey = strKey ;
	return DecodeBase64(DeEncrypt(strInput)) ;
}

/*
IN:
nSourceLen: 明文长度
pchData:明文
pchKey:加密密钥

OUT:
pchDest:返回密文

return: 密文长度
*/
int CZBase64::Base64Encrypt(char *pchDest, char *pchData, char *pchKey)
{
	m_strKey = pchKey ;

	string strSource = pchData ;
	string strDest = DeEncrypt( EncodeBase64(strSource) ) ;
	strcpy(pchDest, strDest.c_str()) ;

	return strlen(pchDest) ;
}

/*
IN:
nSourceLen: 密文长度
pchData: 密文
pchKey: 解密密钥

OUT:
pchDest: 明文

return: 明文长度
*/
int CZBase64::Base64Decrypt(char *pchData, char *pchDest, char *pchKey)
{
	m_strKey = pchKey ;

	string strSource = pchData ;
	string strDest = DecodeBase64(DeEncrypt(strSource)) ;
	strcpy(pchDest, strDest.c_str() ) ;

	return strlen(pchDest) ;
}

void CZBase64::AddStr(string szText)
{
	int nTextLength = 0 ;      //累加字符串长度
	int nNewStringLen = 0 ;  //累加后字符串长度
	int nNewBufLength = 0 ;  //新缓冲区大小
	nTextLength = szText.length() ;
	if (nTextLength == 0)
	{
		return ;
	}

	nNewStringLen = m_strLength + nTextLength ;
	for (int i=0 ; i<szText.length() ; ++i)
	{
		//	m_szBufString[m_strLength+i] = szText[i] ;
		m_vecBufString.push_back(szText[i]) ;
	}
	m_strLength = nNewStringLen ;
	
	//else
	//{
	//	nNewBufLength = m_nBufLen ;
	//	while (nNewStringLen>nNewBufLength)
	//	{
	//		nNewBufLength = nNewStringLen * 4 ;
	//	}
	//	char *strTemp ;
	//	strTemp = new char[nNewBufLength-m_nBufLen] ;
	//	memset(strTemp , 0 , nNewBufLength-m_nBufLen) ;
	//	strcat(m_szBufString , strTemp) ;
	////	m_szBufString = m_szBufString.append(strTemp) ;
	//	m_nBufLen = nNewBufLength ;
	//	for (int i=0 ; i<szText.length() ; ++i)
	//	{
	//		m_szBufString[m_strLength+1+i] = szText[i] ;
	//	}
	//	m_strLength = nNewStringLen ;

	//	delete []strTemp ;
	//}
}