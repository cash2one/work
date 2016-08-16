#include "StdAfx.h"
#include "UUColdRecognize.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "UUWiseHelper.lib")

CUUColdRecognize g_uuRecognize;

//获取文件的CRC检验值
int CUUColdRecognize::Get_CRC(CString &csData, DWORD dwSize)
{
	ULONG  crc(0xffffffff);
	int len;
	unsigned char* buffer;
	len = dwSize;
	buffer = (unsigned char*)(LPCTSTR)csData;
	while(len--)
		crc = (crc >> 8) ^ m_pCrc32Table[(crc & 0xFF) ^ *buffer++];
	return crc^0xffffffff;
}

//动态生成CRC32的码表
void CUUColdRecognize::MakeTable()
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
		m_pCrc32Table[i] = crc;
	}
}

CString CUUColdRecognize::GetFileCrc(const CString& strFileName) 
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

BOOL CUUColdRecognize::GetBytesMD5(const BYTE* lpBuffer, DWORD lpNumberOfBytes, CString& strMD5)
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


BOOL CUUColdRecognize::GetFileMd5(CString strDllPath, CString& strFileMd5)  
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
string CUUColdRecognize::MBCS2Utf8(const string& szMBCS)
{
	if (szMBCS.length() == 0)
		return "";
	// 方法：先转换成CP_ACP再转换成CP_UTF8
	int nLength = MultiByteToWideChar(CP_ACP, 0, szMBCS.c_str(), -1, NULL, NULL);   // 获取缓冲区长度，再分配内存
	WCHAR *tch = new WCHAR[nLength];

	nLength = MultiByteToWideChar(CP_ACP, 0, szMBCS.c_str(), -1, tch, nLength);     // 将MBCS转换成Unicode

	int nUTF8len = WideCharToMultiByte(CP_UTF8, 0, tch, nLength, 0, 0, 0, 0);   // 获取UTF-8编码长度
	char *utf8_string = new char[nUTF8len];
	WideCharToMultiByte(CP_UTF8, 0, tch, nLength, utf8_string, nUTF8len, 0, 0); //转换成UTF-8编码	

	delete[] tch;
	CStringA strResult = utf8_string;
	delete[] utf8_string;
	return strResult;
}


BOOL CUUColdRecognize::GetStringMD5(string str, CString& strMD5)
{
	return GetBytesMD5((BYTE*)str.c_str(), str.length(), strMD5);
}

// DLL文件校验
bool CUUColdRecognize::CheckDll(int softid, const CString& strSoftKey, const CString& strDllPath, const CString& strCheckKey)
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
	strSoftID.Format(_T("%d"), softid);
	//下面是软件id对应的dll校验key。在开发者后台-我的软件里面可以查的到。

	CString strYuanShiInfo = strSoftID + strCheckKey + strGuid + strDllMD5.MakeUpper() + strDllCrc.MakeUpper();
	CString strLocalInfo;

	CStringA strA(strYuanShiInfo.GetBuffer(0));
	strYuanShiInfo.ReleaseBuffer();
	std::string strStringA = strA.GetBuffer(0);
	strA.ReleaseBuffer();

	GetStringMD5(strStringA, strLocalInfo);
	//GetStringMD5(strYuanShiInfo, strLocalInfo);

	TCHAR serverInfo[50];
	uu_CheckApiSignW(softid, strSoftKey, strGuid, strDllMD5, strDllCrc, serverInfo);
	CString strServerInfo = serverInfo;
	return strServerInfo.Compare(strLocalInfo) ? false : true;
}

//strResult参数为服务器返回的字符串，softid为软件id，codeID为验证码id，checkKey为软件的校验key
CString CUUColdRecognize::CheckResult(const CString& strResult, int softid, int codeID, const CString& checkKey)
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
		strCodeID.Format(_T("%d"), codeID);
		CString strSoftID;
		strSoftID.Format(_T("%d"), softid);
		CString strLocalInfo = strSoftID + checkKey + strCodeID + strCodeResult.MakeUpper();
		//在校验之间先将MBCS字符串转化为UTF-8编码
		CStringA strLocalInfoA(strLocalInfo.GetBuffer(0));
		strLocalInfo.ReleaseBuffer();
		std::string strInfo = strLocalInfoA.GetBuffer(0);
		strLocalInfoA.ReleaseBuffer();

		string strLocalInfoUtf8 = MBCS2Utf8(strInfo);
		//		CString strLocalInfoUtf8 = MBCS2Utf8(strLocalInfo);
		//	std::wstring strW = _T("");
		//	StringToWString(strLocalInfoUtf8, strW);
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
			return _T("结果校验不正确");
		}
	}
}

CUUColdRecognize::CUUColdRecognize()
{	
	m_mapErrorPort.clear();
	m_bInit = false;
	m_strUserName = new TCHAR[50];
	m_strPassword = new TCHAR[50];
	m_strSoftKey  = new TCHAR[256];

	ASSERT( m_strUserName != NULL && m_strPassword != NULL && m_strSoftKey != NULL);

	CString szEecDaTa = _T("");
	CString szDecData;
	TCHAR szFilePath[MAX_PATH] = {0};

	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	PathAppend(szFilePath, _T("..\\..\\"));
	PathAppend(szFilePath, _T("data2\\mc.dat"));

	IXMLRW xmlcfg;
	xmlcfg.init(szFilePath);

	xmlcfg.ReadInt(_T("MC"), _T("OCRDATA"), _T("id"), m_iSoftID);

	int iCodetype = 0;
	xmlcfg.ReadInt(_T("MC"), _T("OCRDATA"), _T("type"), iCodetype);
	m_nCodeType = iCodetype;

	int iDebug = 0;
	xmlcfg.ReadInt(_T("MC"), _T("OCRDATA"), _T("debug"), iDebug);
	m_bIsSaveReCResult = iDebug;

	xmlcfg.ReadString(_T("MC"), _T("OCRDATA"), _T("data"), szEecDaTa);

	//m_iSoftID = (int)GetPrivateProfileInt(_T("OCRDATA"), _T("id"), 0, szFilePath);
	//m_nCodeType = (int)GetPrivateProfileInt(_T("OCRDATA"), _T("type"), 0, szFilePath);
	//m_bIsSaveReCResult = (int)GetPrivateProfileInt(_T("OCRDATA"), _T("debug"), 0, szFilePath);

	//GetPrivateProfileString(_T("OCRDATA"), _T("data"), _T(""), szEecDaTa, sizeof(szEecDaTa), szFilePath);

	if(_tcslen(szEecDaTa) != 0)
	{
		CString strEncData = szEecDaTa;
		CDES des;
		des.Decrypt(szDecData, strEncData);
	}

	if(szDecData.GetLength() != 0)
	{
		TCHAR* strDecData = szDecData.GetBuffer(0);
		TCHAR *token;

		token = _tcstok(strDecData, _T(";"));

		if(token != NULL)
		{
			_tcscpy(m_strUserName, token);
		}

		token = _tcstok(NULL, _T(";"));

		if(token != NULL)
		{
			_tcscpy(m_strPassword, token);
		}

		token = _tcstok(NULL, _T(";"));

		if(token != NULL)
		{
			_tcscpy(m_strSoftKey, token);
		}

		token = _tcstok(NULL, _T(";"));

		if(token != NULL)
		{
			TCHAR szCheckKey[MAX_PATH] = {0};
			_tcscpy(szCheckKey, token);
			m_strChekKey = szCheckKey;
		}
	}
}


CUUColdRecognize::~CUUColdRecognize()
{
	if(m_strUserName != NULL)
	{
		delete [] m_strUserName;
		m_strUserName = NULL;
	}
	if(m_strPassword != NULL)
	{
		delete [] m_strPassword;
		m_strPassword = NULL;
	}
	if(m_strSoftKey != NULL)
	{
		delete [] m_strSoftKey;
		m_strSoftKey = NULL;
	}
}

bool CUUColdRecognize::InitRecognize()
{
	//g_UUinitLock.Lock();
	bool bSuccess = true;
	if (!m_bInit)
	{
		TCHAR exeFullPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, exeFullPath, MAX_PATH);
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		::_tsplitpath_s(exeFullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);

		CString strDllPath(drive);
		strDllPath += dir;
		strDllPath += "UUWiseHelper.dll";

		if (!CheckDll(m_iSoftID, m_strSoftKey, strDllPath, m_strChekKey))
		{
			g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("Dll文件校验失败"));
			bSuccess = false;
		}
		else
		{
			g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("Dll文件校验成功"));
		}

		g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("初始化优优uu_setSoftInfoW"));

		uu_setSoftInfoW(m_iSoftID, m_strSoftKey);

		g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("登录"));

		LONG loginStatus = uu_loginW(m_strUserName, m_strPassword);

		if (loginStatus > 0)
		{
			LONG score = uu_getScoreW(m_strUserName, m_strPassword);

			g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("登录成功，用户ID为：%d,帐户内剩余题分为：%d"), loginStatus, score);
		}
		else
		{
			bSuccess = false;
			g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("登录失败，错误代码为：%d"), loginStatus);
		}
		m_bInit = true;
	}
	//g_UUinitLock.Unlock();
	return bSuccess;
}

long CUUColdRecognize::ReportError(long nResultCode)
{	
	if (m_bIsSaveReCResult)
	{	
		//找到了Code对应的打码信息
		if (m_mapErrorPort.find(nResultCode) != m_mapErrorPort.end())
		{
			_TERRORREPORT tReportError = m_mapErrorPort[nResultCode];
			CopyErrorFile(nResultCode, tReportError);
		}
	}

	LONG nCode = uu_reportError(nResultCode);
	if(nCode != 0)
		g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ReportError返回不正常,返回代码为：%d"), nCode);
	return nCode;
}

long CUUColdRecognize::Recognize(LPCTSTR strImgPath, LPTSTR strResult)
{
	InitRecognize();
	
	LONG codeID = 0;
	TCHAR recoResult[100] = {0};


	g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("开始识别"));

	DWORD dwBegin = GetTickCount();
	codeID = uu_recognizeByCodeTypeAndPathW(strImgPath, m_nCodeType, recoResult);

	DWORD dwUseTime = GetTickCount() - dwBegin;
	if(codeID > 0)
	{
		CString strCodeResult = CheckResult(recoResult, m_iSoftID, codeID, m_strChekKey);
		LPTSTR strTemp = strCodeResult.GetBuffer(0);
		strCodeResult.ReleaseBuffer();

		_tcscpy(strResult, strTemp);

		//如果是要保存结果及路径
		if (m_bIsSaveReCResult)
		{	
			_TERRORREPORT tReport;
			tReport.m_strResult = strResult;
			tReport.m_strImgPath = strImgPath;

			m_mapErrorPort[codeID] = tReport;
		}

		g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("识别完成,文件名：%s, 验证码ID为：%d,识别结果为：%s, 识别时间：%d"), strImgPath, codeID, strCodeResult, dwUseTime);
	}
	else
	{
		_tcscpy(strResult, _T(""));
		g_SvcarLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("识别出现错误, 文件名：%s, 返回的错误代码为：%d,resutErrorCode: %s"), strImgPath, codeID, recoResult);
	}

	return codeID;
}

void CUUColdRecognize::CopyErrorFile(long lCode,_TERRORREPORT& tErrorReport)
{
	TCHAR szFilePath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	PathAppend(szFilePath, _T("..\\..\\"));
	PathAppend(szFilePath, _T("image\\errorcode"));

	::SHCreateDirectoryEx(NULL, szFilePath, NULL);
	CString strFileName = tErrorReport.m_strImgPath.Mid(tErrorReport.m_strImgPath.ReverseFind(_T('\\')) + 1);
	CString strNewFileName = CString(szFilePath) + _T("\\") + strFileName;

	//复制文件
	if (!CopyFile(tErrorReport.m_strImgPath, strNewFileName, FALSE))
	{
		g_ResultLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,_T("复制文件失败!src:%s des:%s ErrorCode:%d"),
			tErrorReport.m_strImgPath, strNewFileName,GetLastError());

		return;
	}

	//记录错误日志
	g_ResultLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("识别报错:CodeID:%d,imgpath:%s,result:%s"),
		lCode, strNewFileName, tErrorReport.m_strResult);
}
