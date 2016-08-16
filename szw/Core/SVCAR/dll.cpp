#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include "cdef.h"
//#include "windows.h"
#include "HttpClient.h"
#include <Wincrypt.h>
#include <zmouse.h>
#include "Shlwapi.h"

#pragma comment(lib, "shlwapi.lib")

//获取文件的CRC检验值
unsigned long Crc32Table[256];
int Get_CRC(CString &csData, DWORD dwSize)//获取crc32值
{
	ULONG  crc(0xffffffff);
	int len;
	unsigned char* buffer;
	len = dwSize;
	buffer = (unsigned char*)(LPCTSTR)csData;
	while(len--)
		crc = (crc >> 8) ^ Crc32Table[(crc & 0xFF) ^ *buffer++];
	return crc^0xffffffff;
}
void MakeTable()//动态生成CRC32的码表
{
	int i,j;
	unsigned long crc;
	for (i = 0; i < 256; i++)
	{
		crc = i;
		for (j = 0; j < 8; j++)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ 0xEDB88320;
			else
				crc >>= 1;
		}
		Crc32Table[i] = crc;
	}
}
CString GetFileCrc(const CString& strFileName) 
{
	HANDLE hFile = {NULL};
	DWORD dwSize, bytes_read;
    
	MakeTable();//生成码表

	hFile = CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	dwSize = GetFileSize(hFile, NULL);

	CString csData(' ', dwSize);
	ReadFile(hFile, csData.GetBuffer(dwSize), dwSize, &bytes_read, NULL);
	CloseHandle(hFile);
	csData.ReleaseBuffer();

	int nCRC = Get_CRC(csData, dwSize);
	char ch[20] = {0};
	itoa(nCRC, ch, 16); 
	USES_CONVERSION;
	CString strFileCrcValue = A2W(ch);
	//不够8位，前面补0
	int len = strFileCrcValue.GetLength();
	if(len < 8)
	{
		int miss = 8 - len;
		for(int i = 0; i < miss; ++i)
		{
			strFileCrcValue = CString(L"0") + strFileCrcValue;
		}
	}

	return strFileCrcValue.MakeUpper();
}

BOOL GetBytesMD5(const BYTE* lpBuffer, DWORD lpNumberOfBytes, CString& strMD5)
{
	HCRYPTPROV hProv=NULL;
	if(CryptAcquireContext(&hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)       //获得CSP中一个密钥容器的句柄
	{
		return FALSE;
	}
	HCRYPTPROV hHash=NULL;
	//初始化对数据流的hash，创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被    CryptHashData调用。
	if(CryptCreateHash(hProv,CALG_MD5,0,0,&hHash)==FALSE)
	{
		return FALSE;
	}
	if(CryptHashData(hHash,lpBuffer,lpNumberOfBytes,0)==FALSE)      //hash文件  
	{
		return FALSE;
	}
	BYTE *pbHash;
	DWORD dwHashLen=sizeof(DWORD);
	if (!CryptGetHashParam(hHash,HP_HASHVAL,NULL,&dwHashLen,0)) //我也不知道为什么要先这样调用CryptGetHashParam，这块是参照的msdn       
	{
 		return FALSE;
	}
	pbHash=(byte*)malloc(dwHashLen);
	if(CryptGetHashParam(hHash,HP_HASHVAL,pbHash,&dwHashLen,0))//获得md5值 
	{
		for(DWORD i=0;i<dwHashLen;i++)         //输出md5值 
		{
   			TCHAR str[3]={0};
  			//CString strFilePartM=_T("");
   			_stprintf(str,_T("%02x"),pbHash[i]);
			strMD5 += str;
		}
	} 
 
	//善后工作
	if(CryptDestroyHash(hHash)==FALSE)          //销毁hash对象  
	{
		return FALSE;
	}
	if(CryptReleaseContext(hProv,0)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL GetFileMd5(CString strDllPath, CString& strFileMd5)  
{  
	HANDLE hFile=CreateFile(strDllPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)                                        //如果CreateFile调用失败  
	{   
		//提示CreateFile调用失败，并输出错误号。visual studio中可在“工具”>“错误查找”中利用错误号得到错误信息。  
		CloseHandle(hFile);
		return FALSE;
	}

	DWORD dwFileSize=GetFileSize(hFile,0);    //获取文件的大小
	if (dwFileSize==0xFFFFFFFF)               //如果获取文件大小失败  
	{
		return FALSE;
	}
	byte* lpReadFileBuffer=new byte[dwFileSize];
	DWORD lpReadNumberOfBytes;
	if (ReadFile(hFile,lpReadFileBuffer,dwFileSize,&lpReadNumberOfBytes,NULL)==0)        //读取文件  
	{
		return FALSE;
	}

	BOOL bRet = GetBytesMD5(lpReadFileBuffer, lpReadNumberOfBytes, strFileMd5);

	delete[] lpReadFileBuffer;
	CloseHandle(hFile);          //关闭文件句柄
	return bRet;
}  

// MBCS字符集转换成UTF-8
CString MBCS2Utf8(const CString& szMBCS)
{
	if (szMBCS.IsEmpty())
		return "";
	// 方法：先转换成CP_ACP再转换成CP_UTF8
	int nLength = MultiByteToWideChar(CP_ACP, 0, szMBCS, -1, NULL, NULL);   // 获取缓冲区长度，再分配内存
	WCHAR *tch = new WCHAR[nLength];

	nLength = MultiByteToWideChar(CP_ACP, 0, szMBCS, -1, tch, nLength);     // 将MBCS转换成Unicode

	int nUTF8len = WideCharToMultiByte(CP_UTF8, 0, tch, nLength, 0, 0, 0, 0);   // 获取UTF-8编码长度
	char *utf8_string = new char[nUTF8len];
	WideCharToMultiByte(CP_UTF8, 0, tch, nLength, utf8_string, nUTF8len, 0, 0); //转换成UTF-8编码	

	delete[] tch;
	CString strResult = utf8_string;
	delete[] utf8_string;
	return strResult;
}

BOOL GetStringMD5(CString str, CString& strMD5)
{
	return GetBytesMD5((BYTE*)(LPCTSTR)str, str.GetLength(), strMD5);
}

bool CheckDll(int softid, const CString& strSoftKey, const CString& strDllPath, const CString& strCheckKey)
{
	//获取dll文件的md5值
	CString strDllMD5;
	GetFileMd5(strDllPath, strDllMD5);
	//获取dll文件的crc校验值
	CString strDllCrc = GetFileCrc(strDllPath);
	//获取一个guid
	CString strGuid;
	GUID guid;
	CoCreateGuid(&guid);
	strGuid.AppendFormat(_T("%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x"), 
		guid.Data1,
		guid.Data2,
		guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	strGuid = strGuid.MakeUpper();

	CString strSoftID;
	strSoftID.Format("%d", softid);
	//下面是软件id对应的dll校验key。在开发者后台-我的软件里面可以查的到。
	
	CString strYuanShiInfo = strSoftID + strCheckKey + strGuid + strDllMD5.MakeUpper() + strDllCrc.MakeUpper();
	CString strLocalInfo;
	GetStringMD5(strYuanShiInfo, strLocalInfo);

	char serverInfo[50];
	uu_CheckApiSignA(softid, strSoftKey, strGuid, strDllMD5, strDllCrc, serverInfo);
	CString strServerInfo = serverInfo;
	return strServerInfo.Compare(strLocalInfo) ? false : true;
}

//strResult参数为服务器返回的字符串，softid为软件id，codeID为验证码id，checkKey为软件的校验key
CString CheckResult(const CString& strResult, int softid, int codeID, const CString& checkKey)
{
	//服务器返回的是错误代码
	if(_ttol(strResult) < 0)
	{
		return strResult;
	}
	//对验证码结果进行校验，防止dll被替换
	else
	{
		int iPos = strResult.Find('_');
		//解析出服务器返回的校验结果
		CString strServerKey = strResult.Left(iPos);
		CString strCodeResult = strResult.Right(strResult.GetLength() - iPos - 1);
		//本地计算校验结果
		CString strCodeID;
		strCodeID.Format("%d", codeID);
		CString strSoftID;
		strSoftID.Format("%d", softid);
		CString strLocalInfo = strSoftID + checkKey + strCodeID + strCodeResult.MakeUpper();
		//在校验之间先将MBCS字符串转化为UTF-8编码
		CString strLocalInfoUtf8 = MBCS2Utf8(strLocalInfo);
		CString strLocalKey;
		GetStringMD5(strLocalInfoUtf8, strLocalKey);
		strLocalKey = strLocalKey.MakeUpper();
		//相等则校验通过
		if(strLocalKey.CompareNoCase(strServerKey) == 0)
		{
			return strCodeResult;
		}
		//校验不通过
		else
		{
			return "结果校验不正确";
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH在WINDEF.h中定义了，等于260
	memset(exeFullPath,0,MAX_PATH);
	::GetModuleFileName(NULL,exeFullPath,MAX_PATH);
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	::_tsplitpath_s( exeFullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0 );

	CString strDllPath(drive);
	strDllPath += dir;
	strDllPath += "UUWiseHelper.dll";

	int softID=94126;
    char *softKEY="234a4269ca324d6189e915bd85c040b1";
	CString strCheckKey = "C81CCE2C-B67C-4332-A8A4-576E381EEB80";
	if(!CheckDll(softID, softKEY, strDllPath, strCheckKey))
	{
		printf("Dll文件校验失败。");
		system("pause");
		//return 0;
	}
	else
	{
		printf("Dll文件校验成功。");
	}

	char *userName,*userPassword;
    int loginStatus,codeID,score;
	char recoResult[50] = {0};

    userName="foxbryant";
    userPassword="1qaz@WSX";

    printf("初始化优优uu_setSoftInfoA\n");

	uu_setSoftInfoA(softID,softKEY);
    printf("\n调用登录函数uu_loginA\n");
    loginStatus=uu_loginA(userName,userPassword);

    if(loginStatus>0)
	{
        score=uu_getScoreA(userName,userPassword);
        printf("恭喜您，登录成功，您的用户ID为：%d,您帐户内剩余题分为：%d\n",loginStatus,score);
        printf("开始调用识别函数,请耐心等待返回结果……\n");

		char szImgPath[MAX_PATH] = {0};
		for (int i = 1; i < 100; i++)
		{
			sprintf(szImgPath, "d:\\uuyuntestcode\\%d.jpg", i);
			if (PathFileExists(szImgPath))
			{
				DWORD dwBegin = GetTickCount();
				codeID=uu_recognizeByCodeTypeAndPathA(szImgPath, 8002, recoResult);

				DWORD dwUseTime = GetTickCount() - dwBegin;
				if(codeID > 0)
				{
					CString strCodeResult = CheckResult(recoResult, softID, codeID, strCheckKey);
					printf("识别完成,文件名：%s, 验证码ID为：%d,识别结果为：%s, 识别时间：%d\r\n",szImgPath, codeID,strCodeResult, dwUseTime);
				}
				else
				{
					printf("识别出现错误, 文件名：%s, 返回的错误代码为：%d,resutErrorCode:%s\r\n", szImgPath, codeID, recoResult);
				}
			}
		}
    }
	else
	{
        printf("对不起，登录失败，错误代码为：%d\n",loginStatus);
    }

	system("pause");
    return 0;
}

