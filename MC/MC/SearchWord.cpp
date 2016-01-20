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

//�ӿڣ�����ؼ���
void CSearchWord::ProcessKeyWord(const CStdString &strWord)
{
	m_vStrKeyWord.clear();
	CString strKeyHex;
	CString Url;
	CString strWebData;
	CString strKewWord;

	//��ȡ�����ļ�
	ReadConfig();

	if (m_dwPrivilege <= 0 || m_dwStandard <= 0)
	{
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ȡ���������ļ���Ϊ�յ���˳���"));
		return;
	}

	if (m_dwDifftime == 0)
	{
		m_dwDifftime == 1;
	}

	//������ó��վ�ļ�
	GetDefaultWeb();

	g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("�յ������ؼ�������:%s"),strWord.c_str());

	if (!ParseSearchKeyWordData(strWord))
	{
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("���������ؼ�������ʧ�ܣ��˳�"));
		return;
	}

	for (int i = 0; i < m_vStrKeyWord.size(); ++i)
	{
		strKeyHex = _T("");
		Url = _T("");
		strWebData = _T("");
		strKewWord = m_vStrKeyWord[i];

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
		_T("���ҹؼ����ѽ��������͸��ϲ�:%s"), strBack.c_str());
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

BOOL CSearchWord::ParseSearchKeyWordData(const CStdString &strData)
{
	std::vector<CStdString> result;
	StringUtils StringUtil;

	StringUtil.SplitString(strData, _T("(;1)"), result, false);

	if (result.size() <= 0)
	{
		return FALSE;
	}

	m_vStrKeyWord = result;

	return TRUE;
}

BOOL CSearchWord::ReadConfig()
{
	g_Config->ReadInteger(_T("SEEKWORD"), _T("Privilege"), m_dwPrivilege);
	g_Config->ReadInteger(_T("SEEKWORD"), _T("Standard"), m_dwStandard);
	g_Config->ReadInteger(_T("SEEKWORD"), _T("Difftime"), m_dwDifftime);

	return TRUE;
}

//���ֽ�ת���ֽ�
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

// URL����
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

//���ֽ�ת���ֽ�
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

//��ȡ��ҳ������
BOOL CSearchWord::GetWebData(const CString &strData, const CString &strKey)
{
	int iPos,iPos2;
	CStdString strBack;
	CString strTmp = strData;
	CString strValue;
	int iValue;

	iPos = strTmp.Find(_T("�ٶ�Ϊ���ҵ���ؽ��Լ"),0);
	if (iPos < 0)
	{
		if (strTmp.Find(_T("��֤��") > 0))
		{
			g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ǰ����ҳ�������֤�룡"));
			return FALSE;
		}
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("���Ҳ�����Ȩֵ��%s"), strData.GetString());
		return FALSE;
	}

	iPos2 = strTmp.Find(_T("��"), iPos);
	if (iPos2 < 0)
	{
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("������Ȩֵ����iPos:%d,iPos2:%d"), iPos, iPos2);
		return FALSE;
	}

	strValue = strData.Mid(iPos+11, iPos2 - iPos-11);
	strValue.Replace(_T(","), _T(""));
	iValue = _ttoi(strValue);

	if (iValue <= m_dwPrivilege)
	{
		//С����Ȩֵ���������ţ�		
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("��ǰֵ��%d С����Ȩֵ"),iValue);

		strBack.Format(_T("BackResult(;0)KWAuditAnalyze(;0)%s(;0)0"),strKey.GetString());

	}
	else if (iValue >= m_dwStandard)
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("��ǰֵ��%d ���ڱ�׼ֵ"), iValue);
		strBack.Format(_T("BackResult(;0)KWAuditAnalyze(;0)%s(;0)1"), strKey.GetString());
		//���ڱ�׼ֵ���������ţ�
	}
	else if (iValue>m_dwPrivilege && iValue<m_dwStandard)
	{
		//������Ȩֵ��С�ڱ�׼ֵ�����
		if (!CheckWeb(strData))
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("�ӵ�ǰ��ҳδ�ҵ�����ó��վ��Ϊ����"));
			strBack.Format(_T("BackResult(;0)KWAuditAnalyze(;0)%s(;0)1"), strKey.GetString());
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("�ӵ�ǰ��ҳ���ҵ��İ�������ó��վ��Ϊ����"));
			strBack.Format(_T("BackResult(;0)KWAuditAnalyze(;0)%s(;0)0"), strKey.GetString());
		}
	}

	BYTE * pByData = (BYTE *)strBack.c_str();
	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, \
		_T("����GUI������: %s"), strBack.c_str());
	g_server->SendData(strBack.size() * 2, E_GET_EXCUTE_TASK_RESULT, (char*)pByData);

}

//��ȡ������ó��վ
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


//����Ƿ������ǵ���ó��վ��
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
				//��ȡ��ַ  14��ʾ<SPAN class=g>����ַ�������
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
							_T("�ҵ���ǰ��ַ����óƽ̨�У���ַ: %s"), strUrl.GetString());
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

	//���˴�class = g ���ң������л���c-showurl���ң�
	if (!bReturn && strFindChar == _T("<span class=\"g\""))
	{
		strFindChar = _T("<span class=\"c-showurl\"");
		goto FINDSTR;
	}

	return bReturn;

}

/*
@brief  ����urlȡ��
@param  [in/out]strUrl  ��ַ
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
			if (i == 1) //�����м���cn���
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
@brief  ���ַ�ת���ɶ��ֽ�
@param  pChar  ���ַ�
@return ���ض��ֽ�
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