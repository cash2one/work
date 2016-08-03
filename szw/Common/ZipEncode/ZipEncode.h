//#include "StdAfx.h"
#pragma once

#include <stdio.h>
#include <malloc.h>
#include <tchar.h>
//#include "..\StdString.h"

#define ZIPPED_BYTE_ADD_LENGTH 100

class ZipEncode
{
public:
	ZipEncode(void);
	
	~ZipEncode(void);

	int EnocdeANDZipString(TCHAR *Source, const int SourceLength, TCHAR **Dest, int * DestLength );
	

	int DeocdeANDUnzipString(TCHAR *Source, const int SourceLength, TCHAR *Dest, int * DestLength );

	//int EnocdeANDZipStringT(unsigned char *Source,const int SourceLength,unsigned char *Dest,int * DestLength );
	//int DeocdeANDUnzipStringT(unsigned char *Source,const int SourceLength,unsigned char *Dest,int * DestLength );



	int ZipByteArray(unsigned char *Source, const int SourceLength, unsigned char *Dest, int * DestLength );
	int UnzipByteArray(unsigned char *Source, const int SourceLength, unsigned char *Dest, int * DestLength );
	


	int GetUnZipBytesCount(unsigned char *Source, int lngLength);
	int GetMaxZipBytesCount(int SourceLength );


	static int GetDataLength(int codeLength);

	// Returns maximum length of Base64 code based on size of uncoded data.
	static int GetCodeLength(int dataLength);

    static int GetHashCode(const unsigned int dwClientID,const  unsigned int  dwTimeStamp,const  int DestLen, char * Dest);

    static int GetHashCodeExcuteSaveAccount(const unsigned int dwNetstationID,const  unsigned int dwOriginalClientID,const int dwState,
            const unsigned int dwClientID,const  unsigned int dwTimeStamp,const int DestLen, char * Dest);

    static int GetHashCodeBinary(unsigned char* cData,const int dwLen,const int DestLen, char * Dest);
    
	int Base64Encode(unsigned char* Dest,const int SourceLen,char * cData);
	int Base64Decode(char* cData,const int SourceLen,unsigned char * Dest);

    int GetDeocdeANDUnzipStringLength(TCHAR *Source,const int SourceLength,int *DestLength);

};
