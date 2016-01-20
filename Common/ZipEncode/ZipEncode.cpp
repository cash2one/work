#include "ZipEncode.h"
#include "XZipCrypt.h"
#include "md5class.h"
#include <stdlib.h>

#include <stdio.h>
#include "windows.h"

//#include "base64.h"


ZipEncode::ZipEncode(void)
{
}

ZipEncode::~ZipEncode(void)
{
}


int Base64Enc(char *buf, char*text,int size) 
{ 
	static char *base64_encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int buflen = 0; 

	while(size>0)
	{
		*buf++ = base64_encoding[ (text[0] >> 2 ) & 0x3f];
		if(size>2)
		{
			*buf++ = base64_encoding[((text[0] & 3) << 4) | ((text[1] & 0xF0) >> 4)];
			*buf++ = base64_encoding[((text[1] & 0xF) << 2) | ((text[2] & 0xC0) >> 6)];
			*buf++ = base64_encoding[text[2] & 0x3F];
		}
		else
		{
			switch(size)
			{
			case 1:
				*buf++ = base64_encoding[(text[0] & 3) << 4 ];
				*buf++ = '=';
				*buf++ = '=';
				break;
			case 2: 
				*buf++ = base64_encoding[((text[0] & 3) << 4) | ((text[1] & 0xF0) >> 4)]; 
				*buf++ = base64_encoding[((text[1] & 0x0F) << 2) | ((text[2] & 0xC0) >> 6)]; 
				*buf++ = '='; 
				break; 
			} 
		} 
		text +=3; 
		size -=3; 
		buflen +=4; 
	} 

	*buf = 0; 
	return buflen; 
} 


char GetBase64Value(char ch)
{
	if ((ch >= 'A') && (ch <= 'Z')) 
		return ch - 'A'; 
	if ((ch >= 'a') && (ch <= 'z')) 
		return ch - 'a' + 26; 
	if ((ch >= '0') && (ch <= '9')) 
		return ch - '0' + 52; 
	switch (ch) 
	{ 
	case '+': 
		return 62; 
	case '/': 
		return 63; 
	case '=': /* base64 padding */ 
		return 0; 
	default: 
		return 0; 
	} 
}

int Base64Dec(char *buf,char*text,int size) 
{
	if(size%4)
		return -1;
	unsigned char chunk[4];
	int parsenum=0;

	while(size>0)
	{
		chunk[0] = GetBase64Value(text[0]); 
		chunk[1] = GetBase64Value(text[1]); 
		chunk[2] = GetBase64Value(text[2]); 
		chunk[3] = GetBase64Value(text[3]); 

		*buf++ = (chunk[0] << 2) | (chunk[1] >> 4); 
		*buf++ = (chunk[1] << 4) | (chunk[2] >> 2); 
		*buf++ = (chunk[2] << 6) | (chunk[3]);

		text+=4;
		size-=4;
		parsenum+=3;
	}

	return parsenum;
} 


int ZipEncode::GetMaxZipBytesCount(int SourceLength)
{
	//
	return SourceLength + ZIPPED_BYTE_ADD_LENGTH;
}


int ZipEncode::GetUnZipBytesCount(unsigned char *Source,int lngLength)
{
	//
	int intReturn = 0;

	if (Source[lngLength-1] >= 192)
	{
		//
		intReturn = intReturn * 256 + Source[lngLength-1] - 192;
		intReturn = intReturn * 256 + Source[lngLength-2];
		intReturn = intReturn * 256 + Source[lngLength-3];
		intReturn = intReturn * 256 + Source[lngLength-4];

	}

	return intReturn;

}


int ZipEncode::ZipByteArray(unsigned char *Source,const int SourceLength,unsigned char *Dest,int * DestLength )
{
	//

	unsigned int intIndex = 0;
	
	int nRet = 0;

	XZIPCrypt crppt;

	nRet = crppt.GetCompress( Source ,SourceLength ,Dest,DestLength );

	crppt.EnCrypt (Dest,*DestLength);

	intIndex = *DestLength;

	Dest[intIndex] = (unsigned char)(SourceLength & 0xff);
	Dest[intIndex+1] = (unsigned char)((SourceLength & 0xff00) >> 8 );
	Dest[intIndex+2] = (unsigned char)((SourceLength & 0xff0000) >> 16) ;
	Dest[intIndex+3] = (unsigned char)((SourceLength & 0xff000000) >> 24 ) + 192;

	*DestLength += 4;
	return nRet;
}


int ZipEncode::UnzipByteArray(unsigned char *Source,const int SourceLength,unsigned char *Dest,int * DestLength )
{
	//
	int intDataLength = 0;

	intDataLength = SourceLength - 4;

	XZIPCrypt crppt;

	crppt.DeCrypt  (Source,intDataLength);


	return crppt.GetUnCompress( Dest,DestLength,Source ,intDataLength  );

}



int ZipEncode::EnocdeANDZipString(TCHAR *Source,const int SourceLength,TCHAR **Dest,int * DestLength )
{
	//
	unsigned char *psText;
	char *chartmp;
	int dwNum = 0,dwReturnLength = 0;

	dwReturnLength =  *DestLength * 2;

	psText = (unsigned char *)*Dest;

// 	char *szChar;
// 	DWORD dwSize = WideCharToMultiByte(CP_ACP,0,Source,-1,NULL,0,NULL,FALSE);
// 	szChar = new char[dwSize +1];
// 	memset(szChar,0,dwSize+1);
// 	WideCharToMultiByte(CP_ACP,0,Source,-1,szChar,dwSize,NULL,FALSE);

	int dwResult = ZipByteArray((unsigned char *)Source, SourceLength *2,psText ,&dwReturnLength );

	if (dwReturnLength > 0 && dwResult == 0)
	{

		dwNum=GetCodeLength(dwReturnLength);

		chartmp=new char[dwNum+1];

		memset(chartmp,0,dwNum+1);
		//chartmp[dwNum] = {0};
		Base64Enc ( chartmp,(char *)psText ,dwReturnLength);
		int iSize = mbstowcs(NULL, chartmp, dwNum);
		delete []*Dest;
 		*Dest = new TCHAR[iSize+1];
 		memset(*Dest,0,(iSize+1)*sizeof(TCHAR));
		mbstowcs(*Dest, chartmp, dwNum );

		*DestLength = dwNum;

		delete []chartmp;

	}
	else
	{
		return 1;
	}
	return 0;

}

int ZipEncode::DeocdeANDUnzipString(TCHAR *Source,const int SourceLength, TCHAR * Dest,int * DestLength  )
{
	//
	unsigned char *psText = NULL;
	unsigned char *uncompressdata = NULL;

	char *chartmp;
	int dwNum = 0,dwReturnLength = 0, datalength=0,intreturn;

	if (SourceLength <=0 )
	{
		return 1;
	}

	chartmp = new  char[SourceLength + 1];

	if( NULL == chartmp)
	{
		return 2;
	}
	dwNum=GetDataLength(SourceLength);

	psText = new unsigned char[dwNum+1];

	if( NULL == psText)
	{
		return 3;
	}

	psText[dwNum] = 0;

	wcstombs( chartmp, Source, SourceLength );

	chartmp[SourceLength] = 0;

	dwReturnLength = Base64Dec((char *)psText,chartmp,SourceLength);

	delete []chartmp;


	if (psText[dwReturnLength-1] >= 192)
	{
		//
		datalength = GetUnZipBytesCount(psText,dwReturnLength);
		dwReturnLength = dwReturnLength - 0;
	}
	else if (psText[dwReturnLength-2] >= 192)
	{
		//
		datalength = GetUnZipBytesCount(psText,dwReturnLength-1);
		dwReturnLength = dwReturnLength - 1;
	}
	else if (psText[dwReturnLength-3] >= 192)
	{
		//
		datalength = GetUnZipBytesCount(psText,dwReturnLength-2);
		dwReturnLength = dwReturnLength - 2;
	}
	else if (psText[dwReturnLength-4] >= 192)
	{
		//
		datalength = GetUnZipBytesCount(psText,dwReturnLength-3);
		dwReturnLength = dwReturnLength - 3;
	}
	else
	{
		delete []psText;
		return 4;
	}

	uncompressdata = (unsigned char *)Dest; 


	intreturn = UnzipByteArray(psText,dwReturnLength,uncompressdata,&datalength);

	if (datalength <=0)
	{
		* DestLength = 0;
	}
	* DestLength =datalength / 2 ;

	delete []psText;

	return intreturn ;
}




int ZipEncode::GetDeocdeANDUnzipStringLength(TCHAR *Source,const int SourceLength, int * DestLength  )
{
	//
	unsigned char *psText;
	char *chartmp;
	int dwNum = 0,dwReturnLength = 0, datalength=0;

	if (SourceLength <=0 )
	{
		return 1;
	}
	chartmp = new  char[SourceLength + 1];

	if(!chartmp)
	{
		delete []chartmp;
		return 2;

	}
	dwNum=GetDataLength(SourceLength);

	psText = new unsigned char[dwNum+1];

	if( NULL == psText)
	{
		delete []psText;
		return 3;
	}
	psText[dwNum] = 0;
	wcstombs( chartmp, Source, SourceLength );

	chartmp[SourceLength] = 0;

	dwReturnLength = Base64Dec((char *)psText,chartmp,SourceLength);

	delete []chartmp;

	if(dwReturnLength <= 0)
	{
		delete []psText;
		return 4;
	}
	if (psText[dwReturnLength-1] >= 192)
	{
		//
		datalength = GetUnZipBytesCount(psText,dwReturnLength);
		dwReturnLength = dwReturnLength - 0;
	}
	else if (psText[dwReturnLength-2] >= 192)
	{
		//
		datalength = GetUnZipBytesCount(psText,dwReturnLength-1);
		dwReturnLength = dwReturnLength - 1;
	}
	else if (psText[dwReturnLength-3] >= 192)
	{
		datalength = GetUnZipBytesCount(psText,dwReturnLength-2);
		dwReturnLength = dwReturnLength - 2;
	}
	else if (psText[dwReturnLength-4] >= 192)
	{
		//
		datalength = GetUnZipBytesCount(psText,dwReturnLength-3);
		dwReturnLength = dwReturnLength - 3;
	}
	else
	{
		delete []psText;
		return 4;
	}

	* DestLength =datalength / 2 ;

	delete []psText;
	return 0 ;
}





// Returns maximum size of decoded data based on size of Base64 code.
int ZipEncode::GetDataLength(int codeLength)
{
	return codeLength - codeLength / 4;
}

// Returns maximum length of Base64 code based on size of uncoded data.
int ZipEncode::GetCodeLength(int dataLength)
{
	int len = dataLength + dataLength / 3 + (int)( dataLength % 3 != 0 );

	// output code size must be multiple of 4 bytes
	if( len % 4 )
		len += 4 - len % 4;

	return len;
}


int ZipEncode::GetHashCode(unsigned int dwClientID, unsigned int dwTimeStamp,int DestLen, char * Dest)
{
	XZIPCrypt crppt;
	int dwIndex;

	char mdData[49]={0};
	unsigned char pass[33]={0};
	pass[32]=0;
	const char * Ret;

	sprintf(&mdData[0],"%08x",dwClientID);
	sprintf(&mdData[8],"%08x",dwTimeStamp);

	mdData[48] = 0;
	crppt.GetPassword ((unsigned char *)pass);
	for (dwIndex =0 ; dwIndex < 32 ; dwIndex++)
	{
		mdData[16+dwIndex] = pass[dwIndex];  
	}

	CMD5 mmdd ;

	mmdd.setPlainText(mdData);

	Ret =mmdd.getMD5Digest() ;


	switch (DestLen)
	{
	case 16:
		for (dwIndex =0 ; dwIndex < 16 ; dwIndex++)
		{
			Dest[dwIndex] = Ret[8+dwIndex];  
		}
		break;

	case 32:
		for (dwIndex =0 ; dwIndex < 32 ; dwIndex++)
		{
			Dest[dwIndex] = Ret[dwIndex];  
		}
		break;

	default:
		break;
	}


	return 0;

}

int ZipEncode::GetHashCodeExcuteSaveAccount(const unsigned int dwNetstationID,const  unsigned int dwOriginalClientID,const int dwState,
											const unsigned int dwClientID,const  unsigned int dwTimeStamp,const int DestLen, char * Dest)
{
	XZIPCrypt crppt;
	int dwIndex;


	char mdData[73]={0};
	unsigned char pass[33]={0};

	pass[32]=0;
	const char * Ret;


	sprintf(&mdData[0],"%08x",dwNetstationID);

	sprintf(&mdData[8],"%08x",dwOriginalClientID);
	sprintf(&mdData[16],"%08x",dwState);

	sprintf(&mdData[24],"%08x",dwClientID);
	sprintf(&mdData[32],"%08x",dwTimeStamp);


	mdData[72] = 0;
	crppt.GetPassword ((unsigned char *)pass);
	for (dwIndex =0 ; dwIndex < 32 ; dwIndex++)
	{
		mdData[40+dwIndex] = pass[dwIndex];  
	}

	CMD5 mmdd ;

	mmdd.setPlainText(mdData);

	Ret =mmdd.getMD5Digest() ;


	switch (DestLen)
	{
	case 16:
		for (dwIndex =0 ; dwIndex < 16 ; dwIndex++)
		{
			Dest[dwIndex] = Ret[8+dwIndex];  
		}
		break;

	case 32:
		for (dwIndex =0 ; dwIndex < 32 ; dwIndex++)
		{
			Dest[dwIndex] = Ret[dwIndex];  
		}
		break;

	default:
		break;
	}


	return 0;

}



int ZipEncode::GetHashCodeBinary(unsigned char* cData,int dwLen,int DestLen, char * Dest)
{
	XZIPCrypt crppt;
	int dwIndex;
	CMD5 mmdd ;

	const char * Ret;

	mmdd.setBinaryData (cData,dwLen);
	Ret =mmdd.getMD5Digest() ;

	switch (DestLen)
	{
	case 16:
		for (dwIndex =0 ; dwIndex < 16 ; dwIndex++)
		{
			Dest[dwIndex] = Ret[8+dwIndex];  
		}
		break;

	case 32:
		for (dwIndex =0 ; dwIndex < 32 ; dwIndex++)
		{
			Dest[dwIndex] = Ret[dwIndex];  
		}
		break;

	default:
		break;
	}

	return 0;

}


int ZipEncode::Base64Encode(unsigned char* Dest,const int SourceLen,char * cData)
{
	int dwReturnLength;
	dwReturnLength = Base64Enc ((char *) Dest, cData, SourceLen);

	return dwReturnLength;
}


int ZipEncode::Base64Decode(char* cData,const int SourceLen,unsigned char * Dest)
{
	int dwReturnLength;
	dwReturnLength = Base64Dec((char *)Dest,cData,SourceLen);
	return dwReturnLength;
}

