
#include "XZIPCrypt.h"
#include <io.h>
#include "XCompress.h"
//#include "XUnzip.h"


XZIPCrypt::XZIPCrypt(void)
{
	//初始化加密解密字节数组
	InitPasswordArray();

}

XZIPCrypt::~XZIPCrypt(void)
{
}

int XZIPCrypt::InitPasswordArray()
{
	//char SecurityWord[63];          
	//

 //   SecurityWord = P2PStrconv[SecurityWord, vbFromUnicode]
	//char *SecurityWord;
	//SecurityWord=&SecurityWord;
	//"suminfobizexpress2008date20070326telephone83631066other83627788";
	char SecurityWord[]= "suminfobizexpress2008date20070326telephone83631066other83627788";
    
	Passwordchar[0] = SecurityWord[0];
    Passwordchar[1] = SecurityWord[1];
    Passwordchar[2] = SecurityWord[2];
    Passwordchar[3] = SecurityWord[3];
    Passwordchar[4] = SecurityWord[5];
    Passwordchar[5] = SecurityWord[7];
    Passwordchar[6] = SecurityWord[11];
    Passwordchar[7] = SecurityWord[13];
    Passwordchar[8] = SecurityWord[17];
    Passwordchar[9] = SecurityWord[19];
    Passwordchar[10] = SecurityWord[23];
    Passwordchar[11] = SecurityWord[29];
    Passwordchar[12] = SecurityWord[31];
    Passwordchar[13] = SecurityWord[37];
    Passwordchar[14] = SecurityWord[41];
    Passwordchar[15] = SecurityWord[43];
    Passwordchar[16] = SecurityWord[11];
    Passwordchar[17] = SecurityWord[13];
    Passwordchar[18] = SecurityWord[17];
    Passwordchar[19] = SecurityWord[19];
    Passwordchar[20] = SecurityWord[0];
    Passwordchar[21] = SecurityWord[1];
    Passwordchar[22] = SecurityWord[2];
    Passwordchar[23] = SecurityWord[3];
    Passwordchar[24] = SecurityWord[5];
    Passwordchar[25] = SecurityWord[7];
    Passwordchar[26] = SecurityWord[11];
    Passwordchar[27] = SecurityWord[13];
    Passwordchar[28] = SecurityWord[17];
    Passwordchar[29] = SecurityWord[19];
    Passwordchar[30] = SecurityWord[0];
    Passwordchar[31] = SecurityWord[1];

	return 0;
}

int XZIPCrypt::EnCrypt(unsigned char *PlainData ,int lngLength)
{
	//char PasswordcharFinal[32];

	if (lngLength >=32)
	{

		for (int i = 0; i< 32 ; i++)
		{
			//
			PlainData[i] = PlainData[i] ^  (Passwordchar[i] + i % 7);

		}
	}
	else
	{
		for (int i = 0; i< lngLength ; i++)
		{
			//
			PlainData[i] = PlainData[i] ^  (Passwordchar[i] + i % 7);

		}

	}
	return 0;

}


int XZIPCrypt::DeCrypt(unsigned char *Cryptchar,int lngLength)
{
	if (lngLength >=32)
	{
	
		for (int i = 0; i< 32 ; i++)
		{
			//
			Cryptchar[i] = Cryptchar[i] ^  (Passwordchar[i] + i % 7);

		}
	}
	else
	{

		for (int i = 0; i< lngLength ; i++)
		{
			//
			Cryptchar[i] = Cryptchar[i] ^  (Passwordchar[i] + i % 7);

		}
	}
	return 0;

}

int XZIPCrypt::GetPassword(unsigned char *Password)
{
	for (int i = 0; i< 32 ; i++)
	{
		Password[i] = Passwordchar[i] + i % 7; 
	}
	return 0;

}


int XZIPCrypt::GetCompress(unsigned char *unCompress ,const int lngUnCompressLength,unsigned char *Compress,int *lngCompressLength)
{
	
	int lngResult = 0;
	bool bResult = true;

    if (lngUnCompressLength <= 0) 
	{
		//
		return 1;
	}

    *lngCompressLength = lngUnCompressLength;

	XCompress Comp1;

	lngResult = Comp1.Compress1(unCompress, lngUnCompressLength, Compress, lngCompressLength);

	return lngResult;

}

int XZIPCrypt::GetUnCompress(unsigned char *unCompress , int *lngUnCompressLength,unsigned char *Compress,const int lngCompressLength )
{
	
	int lngResult;

    if (lngCompressLength <= 0) 
	{
		//
		return 1;
	}

	XCompress Comp1;

	lngResult = Comp1.unCompress1(unCompress, lngUnCompressLength, Compress, lngCompressLength);

	return lngResult;

}


