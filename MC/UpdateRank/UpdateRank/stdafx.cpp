// stdafx.cpp : 只包括标准包含文件的源文件
// GetUpdateRank.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

CString	 g_strDebugUrl = _T("");
CStringA  g_strWeb;
std::map<CString, int> g_mapSearch;
std::vector<pKeyWordDataInfo> g_vAllKeyWords;
std::vector<CString> g_vDiffCompany;
IServerSocket *g_Socket = NULL;
CLogTrace g_pageLog(_T("UpdateRank.log"), NULL);
CLogTrace g_pLog(_T("VerifyCodeInfo.log"), NULL);


volatile BOOL bSearchFlag[SEARCHFLAGCOUNT];

CRITICAL_SECTION critSection;
CRITICAL_SECTION critSendMsg;
CRITICAL_SECTION critSearchFlag;
CRITICAL_SECTION critCount;

int g_iBaidu  = 0;
int g_iSogou  = 0;
int g_iBing   = 0;
int g_i360    = 0;
int g_iYouDao = 0;

int g_iDelayForKeyWord = -1;
int g_iDalayForSite    = -1;
int g_iThreadCount     = -1;

BOOL g_bIsDebug = FALSE;

TCHAR tSpecialChar[HtmlCharNum][CHAR_LEN] = { _T(" "), _T("+"), _T("/"), _T("?"), _T("%"), _T("#"), _T("&"), _T("="), _T("."), _T("\\"),_T("/"), _T(":"), _T("?"), _T("*"), _T("\""), _T("<"), _T(">"), _T("|") };
// 初始化map
void InitMap()
{
	g_mapSearch[_T("百度")]     = SEARCH_BAIDU;
	g_mapSearch[_T("360搜索")]  = SEARCH_360;
	g_mapSearch[_T("搜狗")]     = SEARCH_SOGOU;
	g_mapSearch[_T("必应")]     = SEARCH_BING;
	g_mapSearch[_T("有道")]     = SEARCH_YOUDAO;
	g_mapSearch[_T("手机百度")] = SEARCH_PHONEBAIDU;
	g_mapSearch[_T("手机搜狗")] = SEARCH_PHONESOGOU;
}

// 在keyword目录下创建公司名目录保存快照
void CreateCompanyDir(CString strDirName)
{
	CString strComName = strDirName;
	ReplaceHtmlChar(strComName);
	TCHAR szFile[MAX_PATH] = { 0 };
	CString strFile;
	::GetModuleFileName(NULL, szFile, MAX_PATH);
	PathAppend(szFile, _T("..\\..\\"));

	strFile = szFile;
	if (g_bIsDebug)
	{
		strFile.Append(_T("image\\"));
		strFile.Append(_T("DEBUG"));

		if (!::CreateDirectory(strFile.GetBuffer(), NULL))
		{
			g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("创建DEBUG目录失败, 路径:%s, ERROR:%d"), strFile, GetLastError());
		}
		strFile.Append(_T("\\"));
	}
	else
	{
		strFile.Append(_T("image\\keyword\\"));
	}
	strFile.Append(strComName);

	if (!::CreateDirectory(strFile.GetBuffer(), NULL))
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("创建目录失败, 路径:%s, ERROR:%d"), strFile, GetLastError());
	}

}

// 获取CPU核心数
int GetCPUCoreNums()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}


// 保存不同用户公司名
void SaveCompanyName(CString strComName)
{
	size_t iSize = g_vDiffCompany.size();
	if (iSize == 0)
	{
		g_vDiffCompany.push_back(strComName);
		return;
	}
	for (int i = 0; i < iSize; i++)
	{
		Sleep(5);
		if (g_vDiffCompany[i] == strComName)
		{
			return;
		}
	}
	g_vDiffCompany.push_back(strComName);
}

// 解析所有关键词数据
void ParseAllKeyWordData(const CStdString &strWord)
{
	// 初始化
	InitMap();

	// 加载默认网站
	GetDefaultWeb();

	// 存储所有关键词数据信息
	std::vector<CStdString> vecKeyWordInfo;
	StringUtils StringUtil;

	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("开始解释数据"));

	// 关键词1数据(;8) 关键词2数据(;8)…….(;8)关键词N数据
	// 解析出所有关键词信息存储在vecKeyWordInfo中
	StringUtil.SplitString(strWord, _T("(;8)"), vecKeyWordInfo, true);
	if (vecKeyWordInfo.size() == 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("没有数据"));
	}

	// 把所有关键词信息解析到全局变量g_vAllKeyWords中
	for (int iCount = 0; iCount < vecKeyWordInfo.size(); iCount++)
	{
		Sleep(5);
		if (!vecKeyWordInfo[iCount].IsEmpty())
		{
			ParseKeyWordData(vecKeyWordInfo[iCount]);
		}
	}

	// 创建不同公司名目录
	for (int i = 0; i < g_vDiffCompany.size(); i++)
	{
		Sleep(5);
		CreateCompanyDir(g_vDiffCompany[i]);
	}

	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("不同公司名个数:%d"), g_vDiffCompany.size());

	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("解析数据完成,收到%d条关键词数据"), vecKeyWordInfo.size());

	vecKeyWordInfo.clear();
}

// 解析每条具体的关键词信息
void ParseKeyWordData(const CStdString &strKeyWord)
{
	if (strKeyWord.Compare(_T("")) == 0)
	{
		return;
	}

	std::vector<CStdString> VecResult;
	StringUtils StringUtil;

	// 关键词数据:
	// 关键词信息(;9)分割符列表(;9)搜索引擎名称(;9)网站列表(;9)目标网站标记(;9)公司名
	// 分解关键词信息
	StringUtil.SplitString(strKeyWord, _T("(;9)"), VecResult, true);

	int iCountInfo = VecResult.size();
	if (iCountInfo < 6)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("分解单条关键词数据失败"));
		return;
	}


	// 根据分解的字符串填充关键词信息结构
	// 网站标志
	pKeyWordDataInfo pKeyWordInfo = new KeyWordDataInfo();
	pKeyWordInfo->strWebFlag = VecResult[4];
	if (pKeyWordInfo->strWebFlag.IsEmpty())
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("目标网站标记为空"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	// 公司名
	CStdString strCompanys;
	StringUtil.SplitString(VecResult[5], _T("(;3)"), pKeyWordInfo->vAllCompanys, false);
	if (pKeyWordInfo->vAllCompanys.size() <= 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("公司名和简称为空"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	pKeyWordInfo->strComany = pKeyWordInfo->vAllCompanys[0];

	// 保存公司名
	CString strComName = pKeyWordInfo->strComany;
	SaveCompanyName(strComName);

	// 解析网站列表
	// 网站列表信息:
	// 网站1(;1) 网站2(;1) 网站3…(;1) 网站N
	std::vector<CStdString> vecWebList;
	const CStdString &strWebInfo = VecResult[3];
	const CStdString &strWebFlag = VecResult[4];

	if (strWebFlag == _T("2"))  //仅网站列表
	{
		LPSTR strList = WideToChar(strWebInfo.c_str());
		if (strList != NULL)
		{
			pKeyWordInfo->strWebList = strList;
			delete strList;
		}
		//g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("仅网站,网站为:%s"), strWebInfo.c_str());
	}
	else if (strWebFlag == _T("1"))  //网站列表、加旗舰版网站
	{
		LPSTR strList = WideToChar(strWebInfo.c_str());
		if (strList != NULL)
		{
			pKeyWordInfo->strWebList = g_strWeb + strList;
			delete strList;
		}

		//g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("列表加网站,网站为:%s"), strWebInfo.c_str());
	}
	else if (strWebFlag == _T("0"))  //仅默认列表
	{
		pKeyWordInfo->strWebList = g_strWeb;
		//g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("仅列表,网站为:%s"), strWebInfo.c_str());
	}
	else if (strWebFlag == _T("3"))  //从默认列表排除制定网站
	{
		StringUtil.SplitString(strWebInfo, _T("(;1)"), vecWebList, false);
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("排除指定网站个数：%d"), vecWebList.size());
		LPSTR strTmp = NULL;
		for (int i = 0; i < vecWebList.size();i++)
		{
			strTmp = WideToChar(vecWebList[i].c_str());
			g_strWeb.Replace(strTmp,"");
			if (strTmp != NULL)
			{
				delete strTmp;
				strTmp = NULL;
			}
		}
		pKeyWordInfo->strWebList = g_strWeb;
	}

	// 搜索引擎名称
	CString strSearchName = VecResult[2];
	pKeyWordInfo->iFlag = g_mapSearch[strSearchName];
	if (pKeyWordInfo->iFlag == 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("解析搜索引擎名失败"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	// 关键词信息:
	// KEY(;2)关键词名称
	// 解析关键词信息
	std::vector<CStdString> vecKeyWordInfo;
	const CStdString &strWordInfo = VecResult[0];
	StringUtil.SplitString(strWordInfo, _T("(;2)"), vecKeyWordInfo, true);

	if (vecKeyWordInfo.size() < 2)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("解析关键词信息失败"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	pKeyWordInfo->strKey = vecKeyWordInfo[0];
	pKeyWordInfo->strKeyWordName = vecKeyWordInfo[1];
	pKeyWordInfo->strKeyHex = UrlEncode(pKeyWordInfo->strKeyWordName);

	// 解析分割符列表
	// 分割符列表：
	// 分割符1(;3)分割符2(;3)分割符3…(;3)分割符N
	std::vector<CStdString> vecConpNameSlip;
	const CStdString &strConpNameInfo = VecResult[1];
	StringUtil.SplitString(strConpNameInfo, _T("(;3)"), vecConpNameSlip, true);

	if (vecConpNameSlip.size() == 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("解析公司名组合失败"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	for (int j = 0; j < vecConpNameSlip.size(); j++)
	{
		if (vecConpNameSlip[j].size() > 0)
		{
			for (int i = 0; i < pKeyWordInfo->vAllCompanys.size(); ++i)
			{
				CString strComSlip = vecConpNameSlip[j];
				CString strComnyCom = (CString)pKeyWordInfo->vAllCompanys[i] + strComSlip;
				pKeyWordInfo->vCompanys.push_back(strComnyCom);
			}
		}
	}

	if (pKeyWordInfo->vCompanys.size() == 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("解析公司名组合失败"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	if (pKeyWordInfo != NULL)
	{
		g_vAllKeyWords.push_back(pKeyWordInfo);
	}

	return;
}



char* CStringToMutilChar(CString& str, int& chLength, WORD wPage)
{
	char* pszMultiByte;
	int iSize = WideCharToMultiByte(wPage, 0, str, -1, NULL, 0, NULL, NULL);
	pszMultiByte = (char*)malloc((iSize + 1)/**sizeof(char)*/);
	memset(pszMultiByte, 0, iSize + 1);
	WideCharToMultiByte(wPage, 0, str, -1, pszMultiByte, iSize, NULL, NULL);
	chLength = iSize;
	return pszMultiByte;
}

// URL编码
CString  UrlEncode(CString sIn)
{
	int ilength = -1;
	char* pUrl = CStringToMutilChar(sIn, ilength, CP_UTF8);
	CStringA strSrc(pUrl);

	CStringA sOut;
	const int nLen = strSrc.GetLength() + 1;

	register LPBYTE pOutTmp = NULL;
	LPBYTE pOutBuf = NULL;
	register LPBYTE pInTmp = NULL;
	LPBYTE pInBuf = (LPBYTE)strSrc.GetBuffer(nLen);
	BYTE b = 0;

	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen * 3 - 2);//new BYTE [nLen  * 3];

	if (pOutBuf)
	{
		pInTmp = pInBuf;
		pOutTmp = pOutBuf;

		// do encoding
		while (*pInTmp)
		{
			if (isalnum(*pInTmp))
				*pOutTmp++ = *pInTmp;
			else
			if (isspace(*pInTmp))
				*pOutTmp++ = '+';
			else
			{
				*pOutTmp++ = '%';
				*pOutTmp++ = toHex(*pInTmp >> 4);
				*pOutTmp++ = toHex(*pInTmp % 16);
			}
			pInTmp++;
		}
		*pOutTmp = '\0';
		//sOut=pOutBuf;
		//delete [] pOutBuf;
		sOut.ReleaseBuffer();
	}
	strSrc.ReleaseBuffer();
	if (pUrl != NULL)
	{
		delete pUrl;
		pUrl = NULL;
	}
	return CString(sOut);
}


void GetDefaultWeb()
{
	TCHAR szFile[MAX_PATH] = { 0 };
	CString strFilePath;

	if (GetModuleFileName(NULL, szFile, MAX_PATH))
	{
		PathAppend(szFile, _T("..\\..\\"));

		HANDLE hFile = INVALID_HANDLE_VALUE;
		if (g_strWeb.IsEmpty())
		{
			strFilePath.Format(_T("%s6\\SiteList.txt"), szFile);
			hFile = CreateFile(strFilePath.GetString(),               // file to open
				GENERIC_READ,          // open for reading
				FILE_SHARE_READ,       // share for reading
				NULL,                  // default security
				OPEN_EXISTING,         // existing file only
				FILE_ATTRIBUTE_NORMAL, // normal file
				NULL);                 // no attr. template

			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwSize = GetFileSize(hFile, NULL);
				if (INVALID_FILE_SIZE != dwSize
					&& dwSize > 0)
				{
					LPSTR pBuf = g_strWeb.GetBuffer(dwSize + 1);

					DWORD dwFact = 0;
					do
					{
						DWORD dwByte;
						if (!ReadFile(hFile, pBuf + dwFact, dwSize, &dwByte, NULL))
							break;

						dwFact += dwByte;

					} while (dwFact < dwSize);

					g_strWeb.ReleaseBuffer();
				}

				CloseHandle(hFile);
			}
		}
	}
}

/*
@brief  宽字符转换成多字节
@param  pChar  宽字符
@return 返回多字节
*/
char *WideToChar(const wchar_t *pWide, DWORD dwCode)
{
	char *pChar = NULL;
	int  iWlen = 0;

	if (pWide == NULL
		|| (iWlen = wcslen(pWide)) == 0)
	{
		return pChar;
	}

	int iLen = WideCharToMultiByte(dwCode, 0, pWide, iWlen, NULL, NULL, NULL, NULL);
	if (iLen > 0)
	{
		pChar = new char[iLen + 1];
		if (pChar != NULL)
		{
			memset(pChar, 0, iLen + 1);
			WideCharToMultiByte(dwCode, 0, pWide, iWlen, pChar, iLen, NULL, NULL);
		}
	}

	return pChar;
}


//清理百度、搜搜、360等搜索引擎缓存
void DeleteSearchCache()
{
	INTERNET_CACHE_ENTRY_INFO *cache_entry_info;
	HANDLE hcache;
	DWORD size = sizeof(INTERNET_CACHE_ENTRY_INFO);

	FindFirstUrlCacheEntry(NULL, NULL, &size);
	cache_entry_info = (INTERNET_CACHE_ENTRY_INFO*)new char[size];
	hcache = FindFirstUrlCacheEntry(NULL, cache_entry_info, &size);

	if (hcache)
	{
		BOOL flag = TRUE;
		while (flag)
		{
			//cout<<cache_entry_info->lpszSourceUrlName<<" "<<cache_entry_info->lpszSourceUrlName<<endl<<endl;
			CString strUrl = cache_entry_info->lpszSourceUrlName;
			if (strUrl.Find(_T(".baidu.")) != -1
				|| strUrl.Find(_T(".so.")) != -1
				|| strUrl.Find(_T(".soso.")) != -1
				|| strUrl.Find(_T(".sogou.")) != -1
				|| strUrl.Find(_T(".bing.")) != -1
				|| strUrl.Find(_T(".youdao.")) != -1
				|| strUrl.Find(_T(".360.")) != -1)
			{
				DeleteUrlCacheEntry(cache_entry_info->lpszSourceUrlName);
			}

			delete[]cache_entry_info;
			size = 0;

			FindNextUrlCacheEntry(hcache, NULL, &size);
			cache_entry_info = (INTERNET_CACHE_ENTRY_INFO*)new char[size];
			flag = FindNextUrlCacheEntry(hcache, cache_entry_info, &size);
		}
	}
	if (cache_entry_info)
	{
		delete[]cache_entry_info;
	}
}

// Unicode CString URLEncode
BYTE toHex(const BYTE &x)
{
	return x > 9 ? x + 55 : x + 48;
}


//URLDecode
CString Utf8ToStringT(LPSTR str)
{
	CString strResult;
	_ASSERT(str);
	USES_CONVERSION;
	WCHAR *buf;
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	buf = new WCHAR[length + 1];
	ZeroMemory(buf, (length + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, length);

	if (str != NULL)
	{
		delete str;
		str = NULL;
	}
	strResult = (CString(W2T(buf)));
	if (buf != NULL)
	{
		delete[]buf;
		buf = NULL;
	}
	return strResult;
}

CString UrlDecode(LPCTSTR url)
{
	_ASSERT(url);
	USES_CONVERSION;
	LPSTR _url = T2A(const_cast<LPTSTR>(url));

	int i = 0;
	int length = (int)strlen(_url);
	CHAR *buf = new CHAR[length + 1];
	ZeroMemory(buf, length + 1);
	LPSTR p = buf;
	while (i < length)
	{
		if (i <= length - 3 && _url[i] == '%' && IsHexNum(_url[i + 1]) && IsHexNum(_url[i + 2]))
		{
			sscanf(_url + i + 1, "%2x", p++);

			i += 3;
		}
		else
		{
			*(p++) = _url[i++];
		}
	}

	return Utf8ToStringT(buf);
}


// 取任务
pKeyWordDataInfo GetSearKeyWordInfo()
{
	pKeyWordDataInfo pSData = NULL;
	EnterCriticalSection(&critSection);
	if (g_vAllKeyWords.size() > 0)
	{
		pSData = (pKeyWordDataInfo)g_vAllKeyWords.at(g_vAllKeyWords.size() - 1);
		g_vAllKeyWords.pop_back();
	}
	LeaveCriticalSection(&critSection);
	return pSData;
}

//替换网页特殊字符
void ReplaceHtmlChar(CString &strData)
{
	for (int i = 0; i < HtmlCharNum ;i++)
	{
		strData.Replace(tSpecialChar[i], _T(""));
	}
}
