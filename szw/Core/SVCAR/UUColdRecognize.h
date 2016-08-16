#pragma once

#include "stdafx.h"
#include <iostream>
#include "HttpClient.h"
#include <Wincrypt.h>
#include <zmouse.h>
#include "Shlwapi.h"
#include <ShlObj.h>
#include <map>
using namespace std;

//定义识别错误的结果及对应的文件路径结构体
typedef struct _terrorreport
{
	CString m_strResult;
	CString m_strImgPath;
	_terrorreport()
	{
		m_strResult = _T("");
		m_strImgPath = _T("");
	}

}_TERRORREPORT;

/********************************
*优优云识别类
********************************/

class CUUColdRecognize : public IRecognize
{
public:
	CUUColdRecognize();
	~CUUColdRecognize();

public:
	// 识别
	long Recognize(LPCTSTR strImgPath, LPTSTR strResult);

	// 提交错误
	long ReportError(long nResultCode);

	// 初始化
	bool InitRecognize();

private:
	// 获取CRC32值
	int Get_CRC(CString &csData, DWORD dwSize);

	// 动态生成CRC32的码表
	void MakeTable();

	// 获取文件CRC
	CString GetFileCrc(const CString& strFileName);

	// 获取MD5值
	BOOL GetBytesMD5(const BYTE* lpBuffer, DWORD lpNumberOfBytes, CString& strMD5);

	// 获取文件MD5值
	BOOL GetStringMD5(string str, CString& strMD5); 

	// MBCS字符集转换成UTF-8
	string MBCS2Utf8(const string& szMBCS);

	// 获取字符串MD5值
	BOOL GetStringMD5(CString str, CString& strMD5);

	// 获取文件MD5
	BOOL GetFileMd5(CString strDllPath, CString& strFileMd5);

	// 检测Dll文件
	bool CheckDll(int softid, const CString& strSoftKey, const CString& strDllPath, const CString& strCheckKey);

	// 检测返回结果
	// strResult参数为服务器返回的字符串，softid为软件id，codeID为验证码id，checkKey为软件的校验key
	CString CheckResult(const CString& strResult, int softid, int codeID, const CString& checkKey);

	//复制打码错误图片到errorcode文件夹
	void CopyErrorFile(long lCode,_TERRORREPORT& tErrorReport);

private:
	TCHAR* m_strUserName;
	TCHAR* m_strPassword;
	TCHAR* m_strSoftKey;
	CString m_strChekKey;
	int   m_iSoftID;
	LONG  m_nCodeType; 
	//是否记录识别结果及保留识别错误图片	
	bool  m_bIsSaveReCResult;	
	//将结果记录下来用于打印及存储错误
	map<LONG, _TERRORREPORT> m_mapErrorPort;
	unsigned long m_pCrc32Table[256];
	bool m_bInit;
};
