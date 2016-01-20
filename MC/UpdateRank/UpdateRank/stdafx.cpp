// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// GetUpdateRank.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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
// ��ʼ��map
void InitMap()
{
	g_mapSearch[_T("�ٶ�")]     = SEARCH_BAIDU;
	g_mapSearch[_T("360����")]  = SEARCH_360;
	g_mapSearch[_T("�ѹ�")]     = SEARCH_SOGOU;
	g_mapSearch[_T("��Ӧ")]     = SEARCH_BING;
	g_mapSearch[_T("�е�")]     = SEARCH_YOUDAO;
	g_mapSearch[_T("�ֻ��ٶ�")] = SEARCH_PHONEBAIDU;
	g_mapSearch[_T("�ֻ��ѹ�")] = SEARCH_PHONESOGOU;
}

// ��keywordĿ¼�´�����˾��Ŀ¼�������
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
			g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("����DEBUGĿ¼ʧ��, ·��:%s, ERROR:%d"), strFile, GetLastError());
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
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("����Ŀ¼ʧ��, ·��:%s, ERROR:%d"), strFile, GetLastError());
	}

}

// ��ȡCPU������
int GetCPUCoreNums()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}


// ���治ͬ�û���˾��
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

// �������йؼ�������
void ParseAllKeyWordData(const CStdString &strWord)
{
	// ��ʼ��
	InitMap();

	// ����Ĭ����վ
	GetDefaultWeb();

	// �洢���йؼ���������Ϣ
	std::vector<CStdString> vecKeyWordInfo;
	StringUtils StringUtil;

	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��ʼ��������"));

	// �ؼ���1����(;8) �ؼ���2����(;8)����.(;8)�ؼ���N����
	// ���������йؼ�����Ϣ�洢��vecKeyWordInfo��
	StringUtil.SplitString(strWord, _T("(;8)"), vecKeyWordInfo, true);
	if (vecKeyWordInfo.size() == 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("û������"));
	}

	// �����йؼ�����Ϣ������ȫ�ֱ���g_vAllKeyWords��
	for (int iCount = 0; iCount < vecKeyWordInfo.size(); iCount++)
	{
		Sleep(5);
		if (!vecKeyWordInfo[iCount].IsEmpty())
		{
			ParseKeyWordData(vecKeyWordInfo[iCount]);
		}
	}

	// ������ͬ��˾��Ŀ¼
	for (int i = 0; i < g_vDiffCompany.size(); i++)
	{
		Sleep(5);
		CreateCompanyDir(g_vDiffCompany[i]);
	}

	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��ͬ��˾������:%d"), g_vDiffCompany.size());

	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�����������,�յ�%d���ؼ�������"), vecKeyWordInfo.size());

	vecKeyWordInfo.clear();
}

// ����ÿ������Ĺؼ�����Ϣ
void ParseKeyWordData(const CStdString &strKeyWord)
{
	if (strKeyWord.Compare(_T("")) == 0)
	{
		return;
	}

	std::vector<CStdString> VecResult;
	StringUtils StringUtil;

	// �ؼ�������:
	// �ؼ�����Ϣ(;9)�ָ���б�(;9)������������(;9)��վ�б�(;9)Ŀ����վ���(;9)��˾��
	// �ֽ�ؼ�����Ϣ
	StringUtil.SplitString(strKeyWord, _T("(;9)"), VecResult, true);

	int iCountInfo = VecResult.size();
	if (iCountInfo < 6)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�ֽⵥ���ؼ�������ʧ��"));
		return;
	}


	// ���ݷֽ���ַ������ؼ�����Ϣ�ṹ
	// ��վ��־
	pKeyWordDataInfo pKeyWordInfo = new KeyWordDataInfo();
	pKeyWordInfo->strWebFlag = VecResult[4];
	if (pKeyWordInfo->strWebFlag.IsEmpty())
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("Ŀ����վ���Ϊ��"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	// ��˾��
	CStdString strCompanys;
	StringUtil.SplitString(VecResult[5], _T("(;3)"), pKeyWordInfo->vAllCompanys, false);
	if (pKeyWordInfo->vAllCompanys.size() <= 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��˾���ͼ��Ϊ��"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	pKeyWordInfo->strComany = pKeyWordInfo->vAllCompanys[0];

	// ���湫˾��
	CString strComName = pKeyWordInfo->strComany;
	SaveCompanyName(strComName);

	// ������վ�б�
	// ��վ�б���Ϣ:
	// ��վ1(;1) ��վ2(;1) ��վ3��(;1) ��վN
	std::vector<CStdString> vecWebList;
	const CStdString &strWebInfo = VecResult[3];
	const CStdString &strWebFlag = VecResult[4];

	if (strWebFlag == _T("2"))  //����վ�б�
	{
		LPSTR strList = WideToChar(strWebInfo.c_str());
		if (strList != NULL)
		{
			pKeyWordInfo->strWebList = strList;
			delete strList;
		}
		//g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("����վ,��վΪ:%s"), strWebInfo.c_str());
	}
	else if (strWebFlag == _T("1"))  //��վ�б����콢����վ
	{
		LPSTR strList = WideToChar(strWebInfo.c_str());
		if (strList != NULL)
		{
			pKeyWordInfo->strWebList = g_strWeb + strList;
			delete strList;
		}

		//g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("�б����վ,��վΪ:%s"), strWebInfo.c_str());
	}
	else if (strWebFlag == _T("0"))  //��Ĭ���б�
	{
		pKeyWordInfo->strWebList = g_strWeb;
		//g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("���б�,��վΪ:%s"), strWebInfo.c_str());
	}
	else if (strWebFlag == _T("3"))  //��Ĭ���б��ų��ƶ���վ
	{
		StringUtil.SplitString(strWebInfo, _T("(;1)"), vecWebList, false);
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("�ų�ָ����վ������%d"), vecWebList.size());
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

	// ������������
	CString strSearchName = VecResult[2];
	pKeyWordInfo->iFlag = g_mapSearch[strSearchName];
	if (pKeyWordInfo->iFlag == 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��������������ʧ��"));
		if (pKeyWordInfo != NULL)
		{
			delete pKeyWordInfo;
			pKeyWordInfo = NULL;
		}
		return;
	}

	// �ؼ�����Ϣ:
	// KEY(;2)�ؼ�������
	// �����ؼ�����Ϣ
	std::vector<CStdString> vecKeyWordInfo;
	const CStdString &strWordInfo = VecResult[0];
	StringUtil.SplitString(strWordInfo, _T("(;2)"), vecKeyWordInfo, true);

	if (vecKeyWordInfo.size() < 2)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�����ؼ�����Ϣʧ��"));
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

	// �����ָ���б�
	// �ָ���б�
	// �ָ��1(;3)�ָ��2(;3)�ָ��3��(;3)�ָ��N
	std::vector<CStdString> vecConpNameSlip;
	const CStdString &strConpNameInfo = VecResult[1];
	StringUtil.SplitString(strConpNameInfo, _T("(;3)"), vecConpNameSlip, true);

	if (vecConpNameSlip.size() == 0)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("������˾�����ʧ��"));
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
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("������˾�����ʧ��"));
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

// URL����
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
@brief  ���ַ�ת���ɶ��ֽ�
@param  pChar  ���ַ�
@return ���ض��ֽ�
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


//����ٶȡ����ѡ�360���������滺��
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


// ȡ����
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

//�滻��ҳ�����ַ�
void ReplaceHtmlChar(CString &strData)
{
	for (int i = 0; i < HtmlCharNum ;i++)
	{
		strData.Replace(tSpecialChar[i], _T(""));
	}
}
