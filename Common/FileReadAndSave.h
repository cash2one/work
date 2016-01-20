#pragma once
#include "StdString.h"
#include "common_stru.h"

class CFileReadAndSave
{
private :
	static const DWORD MAX_UNZIPPED_COUNT = 10000000;
	static const DWORD MIN_ZIPPED_COUNT = 28;

public:
	CFileReadAndSave(void);
	~CFileReadAndSave(void);

	static DWORD FileReadToByteArray(const CStdString strFileName, BYTE * & pData );
	static DWORD FileReadToByteArray(const CStdString strFileName, BYTE * & pData ,DWORD dwLength);

    static DWORD ReadStringFromFileGB(const CStdString  &strFilePath,CStdString  &strData );
    static DWORD WriteStringToFileGB(const CStdString  strFilePath,const CStdString  strData );
    static DWORD WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize);
    static DWORD GetItemFileLength(CStdString FileName); //返回文件的长度,如果不存在,就返回0
    static DWORD MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize);
	static DWORD ZipEncodeStdString(const CStdString &strSource, CStdString & strZipped);
    static DWORD UnZipDecodeStdString(const CStdString &strSource, CStdString & strUnZipped);

	static DWORD UnZipFileBinToFile(const CStdString &strDestFileName, const CStdString  &strSourceFileName);

};
