#include "stdafx.h"
#include "SearchWord.h"
#include "StringUtils.h"
#include <stdio.h>
#include <windows.h>
#include <wininet.h>

#define MAXSIZE 1024
#pragma comment(lib, "Wininet.lib")


CSearchWord::CSearchWord()
{
	m_dwPrivilege = 0;
	m_dwStandard = 0;
	m_dwDifftime = 0;
}


CSearchWord::~CSearchWord()
{

}

//接口，处理淘宝字段
void CSearchWord::ProcessTBWord(const CStdString &strWord)
{
	std::vector<CStdString> vStrTBWord; //关键词数组
	vStrTBWord.clear();
	CString strKeyHex;
	CString Url;
	CString strWebData;
	CString strKewWord;

	g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("收到搜索淘宝词数据:%s"), strWord.c_str());

	if (!ParseSearchKeyWordData(strWord, vStrTBWord))
	{
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("解析搜索淘宝词数据失败，退出"));
		return;
	}

	for (int i = 0; i < vStrTBWord.size(); ++i)
	{
		strKeyHex = _T("");
		Url = _T("");
		strWebData = _T("");
		strKewWord = vStrTBWord[i];

		strKeyHex = UrlEncode(strKewWord);

		Url.Format(_T("https://s.taobao.com/search?q=%s"), strKeyHex.GetString());

		strWebData = urlopen(Url);
		
		ReplacePara(strWebData);

		GetTBData(strWebData,strKewWord);

		Sleep(2000);
	}

	CStdString strBack;

	strBack.Format(_T("BackResult(;0)AllTaskComplete(;0)(;0)"));

	BYTE *pByData = (BYTE *)strBack.c_str();
	g_server->SendData(strBack.size() * 2, E_GET_EXCUTE_TASK_RESULT, (char*)pByData);

	g_log.Trace(LOGL_HIG, LOGT_PROMPT, __TFILE__, __LINE__,
		_T("查找关键词已结束，发送给上层:%s"), strBack.c_str());

}

//接口，处理关键词
void CSearchWord::ProcessKeyWord(const CStdString &strWord)
{
	std::vector<CStdString> vStrKeyWord; //关键词数组
	vStrKeyWord.clear();
	CString strKeyHex;
	CString Url;
	CString strWebData;
	CString strKewWord;

	//读取配置文件
	ReadConfig();

	if (m_dwPrivilege <= 0 || m_dwStandard <= 0)
	{
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("获取到的配置文件有为空的项，退出！"));
		return;
	}

	if (m_dwDifftime == 0)
	{
		m_dwDifftime == 1;
	}

	//载入商贸网站文件
	GetDefaultWeb();

	g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("收到搜索关键词数据:%s"),strWord.c_str());

	if (!ParseSearchKeyWordData(strWord,vStrKeyWord))
	{
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("解析搜索关键词数据失败，退出"));
		return;
	}

	for (int i = 0; i < vStrKeyWord.size(); ++i)
	{
		strKeyHex = _T("");
		Url = _T("");
		strWebData = _T("");
		strKewWord = vStrKeyWord[i];

		strKeyHex = UrlEncode(strKewWord);

		Url.Format(_T("http://www.baidu.com/s?wd=%s"), strKeyHex.GetString());

		strWebData = urlopen(Url);

		GetWebData(strWebData, strKewWord);

		Sleep(m_dwDifftime * 1000);
	}

	CStdString strBack;

	strBack.Format(_T("BackResult(;0)AllTaskComplete(;0)(;0)"));

	BYTE *pByData = (BYTE *)strBack.c_str();
	g_server->SendData(strBack.size() * 2, E_GET_EXCUTE_TASK_RESULT, (char*)pByData);

	g_log.Trace(LOGL_HIG, LOGT_PROMPT, __TFILE__, __LINE__,
		_T("查找关键词已结束，发送给上层:%s"), strBack.c_str());
}


CString CSearchWord::urlopen(CString url)
{
	CString strResult = _T("");
	CStringA strData = _T("");

	if (url.GetLength() < 3)
	{
		return strResult;
	}

	HINTERNET hSession = InternetOpen(_T("SearchWord"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);

		if (hHttp != NULL)
		{
			char Temp[MAXSIZE];
			ULONG Number = 1;
			while (Number > 0)
			{
				InternetReadFile(hHttp, Temp, MAXSIZE - 1, &Number);
				Temp[Number] = '\0';
				strData += Temp;
			}
			InternetCloseHandle(hHttp);
			hHttp = NULL;
		}
		InternetCloseHandle(hSession);

		hSession = NULL;

		strResult = MulitToStringT(strData.GetBuffer());

		return strResult;
	}
}

BOOL CSearchWord::ParseSearchKeyWordData(const CStdString &strData, std::vector<CStdString> &vStrWords)
{
	/*std::vector<CStdString> result;*/
	StringUtils StringUtil;

	StringUtil.SplitString(strData, _T("(;1)"), vStrWords, false);

	if (vStrWords.size() <= 0 && strData.GetLength() > 1)
	{
		vStrWords.push_back(strData);		
	}

	return TRUE;
}

BOOL CSearchWord::ReadConfig()
{
	g_Config->ReadInt(_T("MC"), _T("SEEKWORD"), _T("Privilege"), m_dwPrivilege);
	g_Config->ReadInt(_T("MC"), _T("SEEKWORD"), _T("Standard"), m_dwStandard);
	g_Config->ReadInt(_T("MC"), _T("SEEKWORD"), _T("Difftime"), m_dwDifftime);

	return TRUE;
}

//宽字节转多字节
char* CSearchWord::CStringToMutilChar(CString& str, int& chLength, WORD wPage)
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
CString CSearchWord::UrlEncode(CString sIn)
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

//多字节转宽字节
CString CSearchWord::MulitToStringT(LPSTR str)
{
	CString strResult;
	_ASSERT(str);
	USES_CONVERSION;
	WCHAR *buf;
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	buf = new WCHAR[length + 1];
	ZeroMemory(buf, (length + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, length);

	strResult = (CString(W2T(buf)));
	if (buf != NULL)
	{
		delete[]buf;
		buf = NULL;
	}
	return strResult;
}

//获取网页的数据
BOOL CSearchWord::GetWebData(const CString &strData, const CString &strKey)
{
	int iPos,iPos2;
	CStdString strBack;
	CString strTmp = strData;
	CString strValue;
	int iValue;

	iPos = strTmp.Find(_T("百度为您找到相关结果约"),0);
	if (iPos < 0)
	{
		if (strTmp.Find(_T("验证码") > 0))
		{
			g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("当前搜索页面包含验证码！"));
			return FALSE;
		}
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("查找不到特权值：%s"), strData.GetString());
		return FALSE;
	}

	iPos2 = strTmp.Find(_T("个"), iPos);
	if (iPos2 < 0)
	{
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("查找特权值出错，iPos:%d,iPos2:%d"), iPos, iPos2);
		return FALSE;
	}

	strValue = strData.Mid(iPos+11, iPos2 - iPos-11);
	strValue.Replace(_T(","), _T(""));
	iValue = _ttoi(strValue);

	if (iValue <= m_dwPrivilege)
	{
		//小于特权值，返回冷门；		
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("当前值：%d 小于特权值"),iValue);

		strBack.Format(_T("BackResult(;0)KWAuditAnalyze(;0)%s(;0)0"),strKey.GetString());

	}
	else if (iValue >= m_dwStandard)
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("当前值：%d 大于标准值"), iValue);
		strBack.Format(_T("BackResult(;0)KWAuditAnalyze(;0)%s(;0)1"), strKey.GetString());
		//大于标准值，返回热门；
	}
	else if (iValue>m_dwPrivilege && iValue<m_dwStandard)
	{
		//大于特权值，小于标准值，检测
		if (!CheckWeb(strData))
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("从当前网页未找到的商贸网站，为热门"));
			strBack.Format(_T("BackResult(;0)KWAuditAnalyze(;0)%s(;0)1"), strKey.GetString());
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("从当前网页查找到的包含的商贸网站，为冷门"));
			strBack.Format(_T("BackResult(;0)KWAuditAnalyze(;0)%s(;0)0"), strKey.GetString());
		}
	}

	BYTE * pByData = (BYTE *)strBack.c_str();
	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, \
		_T("返回GUI的数据: %s"), strBack.c_str());
	g_server->SendData(strBack.size() * 2, E_GET_EXCUTE_TASK_RESULT, (char*)pByData);

}

//获取本地商贸网站
void CSearchWord::GetDefaultWeb()
{
	TCHAR szFile[MAX_PATH] = { 0 };
	CString strFilePath;

	if (GetModuleFileName(NULL, szFile, MAX_PATH))
	{
		PathAppend(szFile, _T("..\\..\\"));

		HANDLE hFile = INVALID_HANDLE_VALUE;
		if (g_strAllWeb.IsEmpty())
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
					LPSTR pBuf = g_strAllWeb.GetBuffer(dwSize + 1);

					DWORD dwFact = 0;
					do
					{
						DWORD dwByte;
						if (!ReadFile(hFile, pBuf + dwFact, dwSize, &dwByte, NULL))
							break;

						dwFact += dwByte;

					} while (dwFact < dwSize);

					g_strAllWeb.ReleaseBuffer();
				}

				CloseHandle(hFile);
			}
		}
	}
}


//检查是否在我们的商贸网站中
BOOL CSearchWord::CheckWeb(const CString &strData)
{
	CString strHtml = strData;
	int iPos1, iPos2;
	BOOL bReturn = FALSE;
	strHtml.MakeLower();
	CStringA strWebList = g_strAllWeb;
	strWebList.MakeLower();
	CString strFindChar;
	strFindChar = _T("<span class=\"g\"");

FINDSTR:
	iPos1 = iPos2 = 0;
	iPos1 = strHtml.Find(strFindChar, iPos2);

	while (true)
	{
		Sleep(10);
		if (iPos1 != -1)
		{
			iPos1 = strHtml.Find(_T(">"), iPos1);
			iPos2 = strHtml.Find(_T("</span>"), iPos1);
			if (iPos2 != -1)
			{
				//截取网址  14表示<SPAN class=g>这个字符串长度
				CString strUrl = strHtml.Mid(iPos1 + 1, iPos2 - iPos1 - 1);
				int iPos3 = strUrl.Find(_T("/"));

				if (iPos3 == -1)
					iPos3 = strUrl.Find(_T("..."));

				if (iPos3 == -1)
					iPos3 = strUrl.Find(_T("&nbsp;"));

				if (iPos3 != -1)
				{
					strUrl = strUrl.Left(iPos3);
				}

				GetMainDomain(strUrl);

				char *pChar = WideToChar(strUrl.GetString());
				if (pChar != NULL)
				{
					if (strWebList.Find(pChar) != -1)
					{
						g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, \
							_T("找到当前网址在商贸平台中，网址: %s"), strUrl.GetString());
						bReturn = TRUE;
					}
					delete[]pChar;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}

		if (bReturn)
			break;


		iPos1 = strHtml.Find(strFindChar, iPos2);

	}

	//除了从class = g 中找，还得切换从c-showurl中找；
	if (!bReturn && strFindChar == _T("<span class=\"g\""))
	{
		strFindChar = _T("<span class=\"c-showurl\"");
		goto FINDSTR;
	}

	return bReturn;

}

/*
@brief  根据url取得
@param  [in/out]strUrl  网址
*/
void CSearchWord::GetMainDomain(CString &strUrl)
{
	int iPos1 = -1;
	int iPos2 = -1;

	TCHAR  szUrl[3][5] = { _T(".com"), _T(".cn"), _T(".net") };

	for (int i = 0; i<3; i++)
	{
		iPos2 = strUrl.Find(szUrl[i]);

		if (iPos2 != -1)
		{
			if (i == 1) //避免有几个cn情况
			{
				if (strUrl.Find(szUrl[i], iPos2 + 1) != -1)
				{
					iPos2 = strUrl.Find(szUrl[i], iPos2 + 1);
				}
			}

			strUrl = strUrl.Left(iPos2);
			iPos1 = strUrl.ReverseFind(_T('.'));
			break;
		}
	}
	if (iPos1 != -1)
	{
		strUrl = strUrl.Right(strUrl.GetLength() - iPos1);
		strUrl.Append(_T("."));
	}
}

/*
@brief  宽字符转换成多字节
@param  pChar  宽字符
@return 返回多字节
*/
char* CSearchWord::WideToChar(const wchar_t *pWide, DWORD dwCode /*= CP_ACP*/)
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

// Unicode CString URLEncode
BYTE CSearchWord::toHex(const BYTE &x)
{
	return x > 9 ? x + 55 : x + 48;
}

//获取淘宝网页查询到的数据
BOOL CSearchWord::GetTBData(const CString &strData, const CString &strKey)
{
	if (strData.GetLength()<100)
	{
		return FALSE;
	}
		
	int iPos1=-1;
	CString strResult;
	CStdString strBack; //返回数据	

	iPos1 = strData.Find(_T("\"pid\""));

	while (iPos1 != -1)
	{
		CString strTmp;
		CString strKey;
		strTmp = strData.Mid(iPos1,120);
		int iPos2 = strTmp.Find(_T("\"title\":"));
		if (iPos2 != -1)
		{
			int iPos3 = strTmp.Find(_T("\""),iPos2+10);
			if (iPos3 != -1)
			{
				strKey = strTmp.Mid(iPos2+9,iPos3-iPos2-9);
			}
		}

		if (strKey.GetLength()>1)
		{
			strResult += strKey + _T("(;1)");
		}

		iPos1 = strData.Find(_T("\"pid\""),iPos1+2);
	}

	if (strResult.GetLength() > 1)
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("从淘宝网页查找到的包含的关键词：%s"),strResult.GetString());
		strBack.Format(_T("BackResult(;0)TBKeyWord(;0)%s(;0)%s"),strKey.GetString(), strResult.GetString());

		BYTE * pByData = (BYTE *)strBack.c_str();
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, \
			_T("返回GUI的数据: %s"), strBack.c_str());
		g_server->SendData(strBack.size() * 2, E_GET_EXCUTE_TASK_RESULT, (char*)pByData);
	}

	return TRUE;
}

//替换网页一些不用的数据
void CSearchWord::ReplacePara(CString &strData)
{
	strData.Replace(_T("\\u003c"), _T(""));
	strData.Replace(_T("span class"), _T(""));
	strData.Replace(_T("\\u003dH"), _T(""));
	strData.Replace(_T("\\u003e"), _T(""));
	strData.Replace(_T("/span"), _T(""));
}

