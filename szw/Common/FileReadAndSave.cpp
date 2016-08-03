#include "stdafx.h"

#include "FileReadAndSave.h"
#include "common_enum.h"
#include "common_stru.h"
#include "common_define.h"
#include "ZipEncode/ZipEncode.h"
#include <stdio.h>
#include "StdString.h"
#include "StringUtils.h"



CFileReadAndSave::CFileReadAndSave(void)
{
}

CFileReadAndSave::~CFileReadAndSave(void)
{
}

DWORD CFileReadAndSave::FileReadToByteArray(const CStdString strFileName, BYTE * & pData )
{
	return 0;
}

DWORD CFileReadAndSave::FileReadToByteArray(const CStdString strFileName, BYTE * & pData  ,DWORD dwLength)
{
	
	FILE *fp = _tfopen(strFileName,_T("rb"));
	if(!fp) 
	{ 
		return 1;
	} 
	else
	{
		fread(pData,sizeof(BYTE),dwLength,fp); 
		fclose(fp); 
	}
	
	return 0;
}
 
///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：   返回文件的长度,如果不存在,就返回0
// 输入：   文件名称,可以是绝对路径和相对路径
// 输出：
// 返回值：   返回文件的长度
// 其它说明：  
//
///////////////////////////////////////////////////////////////////////////////
DWORD CFileReadAndSave::GetItemFileLength(CStdString FileName)
{
    
    int dwLen;
	if (FileName.length() == 0) 
	{
		return 0;
	}
    FILE *fp = _tfopen(FileName,_T("rb"));
	if(!fp) 
	{ 
		return 0;
	} 
	else
    {
		fseek(fp,0,SEEK_END);  
		dwLen=ftell(fp);
	    fclose(fp); 
    }
	return dwLen;

}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：把栏目文件的内容写入本地文件
// 输入：   文件的路径(相对路径或绝对路径):strFilePath     需要写入的数据:strData
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CFileReadAndSave::WriteStringToFileGB(const CStdString  strFilePath,const CStdString  strData )
{
	if (0 == strFilePath.length() )
	{
		return 1;
	}

    DWORD dwSize = 0;

    BYTE * SourceChar = NULL;
    _NEWA(SourceChar,BYTE,strData.size() * 2 + 1);
    SourceChar[strData.size() * 2] = 0;

    dwSize = strData.size() * 2;

    WCharToMByte(strData,(char *)SourceChar,&dwSize);

    FILE *fp = _tfopen(strFilePath,_T("wb"));
	if(!fp) 
	{ 
		//return 0;
	} 
	else
    {
	    size_t Size = fwrite(SourceChar,sizeof(BYTE),dwSize,fp);
	    fclose(fp); 
    }
    if(SourceChar)
    {
        delete[] SourceChar;
        SourceChar = NULL;
    }
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：把栏目文件的内容写读为字符串
// 输入：   文件的路径(相对路径或绝对路径):strFilePath     字符串:strData
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CFileReadAndSave::ReadStringFromFileGB(const CStdString  &strFilePath,CStdString  &strData )
{
	

	if (0 == strFilePath.length())
	{
		return 1;
	}

    DWORD dwSize = 0;
	DWORD dwBufferSize = 0;

    BYTE * SourceChar = NULL;
    TCHAR * DestTChar = NULL;

    dwSize = GetItemFileLength (strFilePath);
	if (dwSize <=0)
	{
		return 2;
	}

	dwBufferSize = dwSize + 1;
    _NEWA(SourceChar,BYTE,dwSize + 1);
    _NEWA(DestTChar,TCHAR,dwSize + 1);
    SourceChar[dwSize] = 0;
    DestTChar[dwSize] = 0;

    //WCharToMByte(strData,(char *)SourceChar,&dwSize);

    FILE *fp = _tfopen(strFilePath,_T("rb"));
	if(!fp) 
	{ 
		//return 0;
	} 
	else
    {
     	fread(SourceChar,1,dwSize,fp); 
     	fclose(fp); 
        MByteToWChar((char *)SourceChar, DestTChar, &dwBufferSize);
		strData = CStdString(DestTChar);
    }

    if(DestTChar)
    {
        delete[] DestTChar;
        DestTChar = NULL;
    }
    if(SourceChar)
    {
        delete[] SourceChar;
        SourceChar = NULL;
    }
	return 0;
}





///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：   把宽字符转换为多字节字符
// 输入：           lpcwszStr:宽字符,    dwSize:输出buffer长度
// 输出：           lpszStr:输出的buffer,
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CFileReadAndSave::WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize)
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



///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：   把多字节字符转换为宽字符
// 输入：           lpcwszStr:宽字符,    dwSize:输出buffer长度
// 输出：           lpszStr:输出的buffer,
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CFileReadAndSave::MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize)
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


DWORD CFileReadAndSave::ZipEncodeStdString(const CStdString &strSource, CStdString & strZipped)
{
	//
	int dwSourceCount,dwZippedLen,dwMaxZippedLen;
	ZipEncode ZipFunction;
	int dwReturn = 0;

	dwSourceCount = strSource.length() ;

	TCHAR * SourceChar = NULL;
	_NEWA(SourceChar,TCHAR,dwSourceCount + 1);
	_tcscpy(SourceChar , strSource); 
	SourceChar[dwSourceCount] = 0;
	if (NULL == SourceChar)
	{
		return 2;
	}
	dwMaxZippedLen = ZipFunction.GetMaxZipBytesCount(dwSourceCount);

	//传入数组的大小
	dwZippedLen = dwMaxZippedLen;

	TCHAR * RetChar = NULL;
	_NEWA(RetChar,TCHAR,dwMaxZippedLen + 1);
	if (NULL == RetChar)
	{
		if(SourceChar)
        {
            delete[] SourceChar;
            SourceChar = NULL;
        }
		return 3;
	}
	memset(RetChar, 0, (dwMaxZippedLen + 1) * sizeof(TCHAR));

	dwReturn = ZipFunction.EnocdeANDZipString(SourceChar,dwSourceCount,&RetChar,&dwZippedLen);

	if (0 != dwReturn)
	{
		RetChar[0] = 0;
	}

	RetChar[dwZippedLen] = 0;

	strZipped = CStdString(RetChar);


	if(NULL != SourceChar)
    {
        delete[] SourceChar;
        SourceChar = NULL;
    }
	if(NULL != RetChar)
    {
        delete[] RetChar;
        RetChar = NULL;
    }
	return 0;
}


DWORD CFileReadAndSave::UnZipDecodeStdString(const CStdString &strSource, CStdString & strUnZipped)
{
	//
	int dwSourceCount = 0;
	int dwUnZippedLen = 0;
	int dwMaxUnZippedLen = 0;
	DWORD dwReturn = 0;
	TCHAR *RetChar = NULL;
	ZipEncode ZipFunction;

	dwSourceCount = strSource.length() ;
	TCHAR * SourceChar = NULL;
	_NEWA(SourceChar,TCHAR,dwSourceCount + 1);

	if (NULL == SourceChar)
	{
		return 1;
	}

	SourceChar[dwSourceCount] = 0;
	_tcscpy(SourceChar , strSource); 

	dwReturn = ZipFunction.GetDeocdeANDUnzipStringLength(SourceChar,dwSourceCount,&dwUnZippedLen);
	if (dwUnZippedLen>0 && dwUnZippedLen<MAX_UNZIPPED_COUNT)   //需要更加仔细的考虑这个问题
	{

		TCHAR * RetChar = NULL;
		//dwMaxUnZippedLen = dwUnZippedLen + 1000;
		_NEWA(RetChar,TCHAR,dwUnZippedLen + 1);

		if (NULL == RetChar)
		{
			if(SourceChar)
            {
                delete[] SourceChar;
                SourceChar = NULL;
            }
			return 2;
		}

		RetChar[dwUnZippedLen] = 0;

		dwReturn = ZipFunction.DeocdeANDUnzipString(SourceChar,dwSourceCount,RetChar,&dwUnZippedLen);

		if (0 != dwReturn)
		{   //解压出错了
			RetChar[dwUnZippedLen] = 0;
		}

		if ( dwUnZippedLen > 0)
		{
			RetChar[dwUnZippedLen] = 0;
			strUnZipped = CStdString(RetChar);
		}

		if(NULL != RetChar)
        {
            delete[] RetChar;
            RetChar = NULL;
        }
	}
	if(NULL != SourceChar)
    {
        delete[] SourceChar;
        SourceChar = NULL;
    }
	return 0;
}


DWORD CFileReadAndSave::UnZipFileBinToFile(const CStdString &strDestFileName, const CStdString  &strSourceFileName)
{

	DWORD dwFileLength = 0;
	DWORD dwUnZippedDataLength = 0;
	int dwUnZippedFileLength = 0;
	DWORD dwRet = 0;

	if (0 ==strDestFileName.length() || 0 ==strSourceFileName.length())
	{
		//传入的参数有错误
		return 1;
	}

	dwFileLength = GetItemFileLength (strSourceFileName);

	if (dwFileLength < MIN_ZIPPED_COUNT)
	{
		return 2;
	}

	ZipEncode ZipFunction;
	
	

	BYTE *pData = NULL;


	_NEWA(pData,BYTE,dwFileLength);

	if (NULL == pData)
	{
		return 3;
	}

	//读取文件到内存;

	dwRet = FileReadToByteArray(strSourceFileName,pData,dwFileLength);

	if (0 != dwRet)
	{
		_DELETEA(pData);
		return 4;
	}

	//获取压缩后的文件的长度
	dwUnZippedDataLength = ZipFunction.GetUnZipBytesCount(pData,dwFileLength);

	if (0 == dwUnZippedDataLength || dwUnZippedDataLength>MAX_UNZIPPED_COUNT)
	{
		_DELETEA(pData);
		return 5;
	}

	BYTE *pUnZippedData = NULL;

	_NEWA(pUnZippedData,BYTE,dwUnZippedDataLength + 1000);

	dwUnZippedFileLength = dwUnZippedDataLength;

	dwRet = ZipFunction.UnzipByteArray(pData,dwFileLength,pUnZippedData,&dwUnZippedFileLength);

	if (0 !=  dwRet
		|| dwUnZippedFileLength != dwUnZippedDataLength)
	{
		//解压有错误
		_DELETEA(pData);
		_DELETEA(pUnZippedData);
		return 6;
	}


	//成功解压,保存本地文件
	FILE *fp = _tfopen(strDestFileName,_T("wb"));
	if(!fp) 
	{ 
	} 
	else
	{
		size_t Size = fwrite(pUnZippedData,sizeof(BYTE),dwUnZippedDataLength,fp);
		fclose(fp); 
	}

	_DELETEA(pData);
	_DELETEA(pUnZippedData);
	return 0;
}

