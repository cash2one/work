#include "StdAfx.h"
#include "InternetHttp.h"
#include "CommFunc.h"

#include <AtlBase.h>
#include <atlconv.h>

CInternetHttp::CInternetHttp(LPCTSTR strAgent)
{
    m_pSession = new CInternetSession(strAgent);
    m_pConnection = NULL;
    m_pFile = NULL;

	CString strEncrypt;

	g_Config->ReadString(_T("MC"),_T("UPDATE"), _T("Uploadpic"), strEncrypt);

	CString strDecrypt;
	desDecrypt.Decrypt(strDecrypt, strEncrypt);

	if (strDecrypt.GetLength() > 0)
	{
		GetDesCode(strDecrypt.GetBuffer(0), strUrlHome.GetBuffer(ULEN), strUrlUpload.GetBuffer(ULEN), strUserName.GetBuffer(ULEN), strPwd.GetBuffer(ULEN));

		strDecrypt.ReleaseBuffer();
		strUrlHome.ReleaseBuffer();
		strUrlUpload.ReleaseBuffer();
		strUserName.ReleaseBuffer();
		strPwd.ReleaseBuffer();
	}

	g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("http登录地址：%s, 上传地址：%s"), strUrlHome.GetString(), strUrlUpload.GetString());

}


CInternetHttp::~CInternetHttp(void)
{
    Clear();
    if(NULL != m_pSession)
    {
        m_pSession->Close();
        delete m_pSession;
        m_pSession = NULL;
    }
}

void CInternetHttp::Clear()
{
    if(NULL != m_pFile)
    {
        m_pFile->Close();
        delete m_pFile;
        m_pFile = NULL;
    }

    if(NULL != m_pConnection)
    {
        m_pConnection->Close();
        delete m_pConnection;
        m_pConnection = NULL;
    }
}


int CInternetHttp::ExecuteRequest(LPCTSTR strMethod, LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponseText, CString strUserName, CString strPwd)
{
    CString strServer;
    CString strObject;
    DWORD dwServiceType;
    INTERNET_PORT nPort;
    strResponseText = _T("");

    AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);

    if(AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
    {
        return -1;
    }

    try
    {
		m_pConnection = m_pSession->GetHttpConnection(strServer,
            dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,
            nPort, strUserName, strPwd);
        m_pFile = m_pConnection->OpenRequest(strMethod, strObject, 
            NULL, 1, NULL, NULL, 
            (dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));

        //DWORD dwFlags;
        //m_pFile->QueryOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
        //dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
        ////set web server option
        //m_pFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);

        m_pFile->AddRequestHeaders(_T("Accept: *,*/*"));
        m_pFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));
        m_pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded; charset=utf-8"));
		//m_pFile->AddRequestHeaders(_T("Content-Type:text/html; charset=utf-8"));  //统一用utf-8防止乱码
        //m_pFile->AddRequestHeaders(_T("Accept-Encoding: gzip, deflate"));

		//Unicode转utf-8，否则上层收到的数据解析不正确
		DWORD dwSize = 0;
		char *pszBuf = NULL;
		BOOL bRes = FALSE;
		if (strPostData != NULL)
		{
			WCharToMByte((LPWSTR)(strPostData), NULL, &dwSize, CP_UTF8);
			char *pszBuf = new char[dwSize];
			WCharToMByte((LPWSTR)(strPostData), pszBuf, &dwSize, CP_UTF8);
			bRes = m_pFile->SendRequest(NULL, 0, pszBuf, dwSize);
			delete []pszBuf;
		}
		else
		{
			bRes = m_pFile->SendRequest();
		}


		if(!bRes)
		{
			Clear();
			return -1;
		}

		char szChars[BUFFER_SIZE + 1] = {0};
		string strRawResponse = "";
		UINT nReaded = 0;
		while ((nReaded = m_pFile->Read((void*)szChars, BUFFER_SIZE)) > 0)
		{
			szChars[nReaded] = '\0';
			strRawResponse += szChars;
			memset(szChars, 0, BUFFER_SIZE + 1);
		}

		//UTF8转宽字节
 		dwSize = 0;
		MByteToWChar((LPSTR)(strRawResponse.c_str()), NULL, &dwSize, CP_UTF8);
		WCHAR *pwszBuf = new WCHAR[dwSize];
		MByteToWChar((LPSTR)(strRawResponse.c_str()), pwszBuf, &dwSize, CP_UTF8);

		strResponseText = pwszBuf;
		delete []pwszBuf;

		//Clear();
    }
    catch (CInternetException* e)
    {
        Clear();
        DWORD dwErrorCode = e->m_dwError;
        e->Delete();

        DWORD dwError = GetLastError();

        if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
        {
            return OUTTIME;
        }
        else
        {
            return FAILURE;
        }
    }

    return SUCCESS;
}

int CInternetHttp::HttpGet(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse, CString strUserName, CString strPwd)
{
    return ExecuteRequest(_T("GET"), strUrl, strPostData, strResponse, strUserName, strPwd);
}

int CInternetHttp::HttpPost(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse, CString strUserName, CString strPwd)
{
    return ExecuteRequest(_T("POST"), strUrl, strPostData, strResponse, strUserName, strPwd);
}

void CInternetHttp::UploadFile(const CString& strFileURLInServer, const CString& strFileLocalFullPath, CString &strResponse, CString strUserName, CString strPwd)
{
	INTERNET_PORT   nPort = 80; 
	CFile fTrack; 
	CHttpFile* pHTTP; 
	CString strHTTPBoundary; 
	CString strPreFileData; 
	CString strHead;
	CString strPostFileData; 
	DWORD dwTotalRequestLength; 
	DWORD dwChunkLength; 
	DWORD dwReadLength; 

	void* pBuffer; 

	CFileException e;

	if (FALSE == fTrack.Open(strFileLocalFullPath, CFile::modeRead | CFile::shareDenyWrite, &e))//读出文件  
	{ 
		return; 
	} 

	strHTTPBoundary = _T("---------------------------7b4a6d158c9");//定义边界值 
	strHead = MakeRequestHeaders(strHTTPBoundary);
	strPreFileData = MakePreFileData(strHTTPBoundary, strFileLocalFullPath); 
	strPostFileData = MakePostFileData(strHTTPBoundary, strFileLocalFullPath); 

	CStringA strPreFileDataA = CW2A(strPreFileData, CP_UTF8);
	CStringA strPostFileDataA = CW2A(strPostFileData, CP_UTF8);

	dwTotalRequestLength = strPreFileDataA.GetLength() + strPostFileDataA.GetLength() + fTrack.GetLength() /*+ strHead.GetLength()*/;//计算整个包的总长度 

	dwChunkLength = fTrack.GetLength(); 

	pBuffer = malloc(dwChunkLength); 

	if (NULL == pBuffer || dwChunkLength <= 0) 
	{ 
		return; 
	} 


	DWORD dwType  =   0 ; 
	CString strServer = _T(""); 
	CString strObject = _T(""); 
	BOOL bRes = FALSE;

	BOOL bResult  =   AfxParseURL(strFileURLInServer, dwType, strServer, strObject, nPort); 
	char *pszBuffer = NULL; 
	try 
	{
		//m_pConnection = m_pSession->GetHttpConnection(strServer,nPort); 
		//pHTTP = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/home")); 
		pHTTP = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject); 


		pHTTP->AddRequestHeaders(strHead);//发送包头请求 
		bRes = pHTTP->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE); 

#ifdef _UNICODE 
		pHTTP->Write((LPCSTR)strPreFileDataA, strPreFileDataA.GetLength()); 
#else 
		pHTTP->Write((LPSTR)(LPCSTR)strPreFileData, strPreFileData.GetLength());//写入服务器所需信息 
#endif 

		dwReadLength = -1; 
		while (0 != dwReadLength) 
		{ 
			dwReadLength = fTrack.Read(pBuffer, dwChunkLength);//文件内容 
			if (0 != dwReadLength) 
			{ 
				pHTTP->Write(pBuffer, dwReadLength);//写入服务器本地文件，用二进制进行传送 
			} 
		} 

#ifdef _UNICODE 
		pHTTP->Write((LPCSTR)strPostFileDataA, strPostFileDataA.GetLength()); 
#else 
		pHTTP->Write((LPSTR)(LPCSTR)strPostFileData, strPostFileData.GetLength()); 
#endif 

		pHTTP->EndRequest(HSR_SYNC); 
		
// 
		const int BUFFER_LENGTH = 1024 * 10; 
		pszBuffer = new char[BUFFER_LENGTH]; 
		memset(pszBuffer, 0, BUFFER_LENGTH);


		DWORD dwWrite = 0; 
		DWORD dwTotalWrite = 0; 
		UINT nRead = pHTTP->Read(pszBuffer, BUFFER_LENGTH); 

		while(nRead > 0) 
		{ 
			//WriteFile(hFile, pszBuffer, nRead, &dwWrite, NULL); 
			strResponse += CA2W(pszBuffer, CP_UTF8);
			dwTotalWrite += dwWrite; 
			nRead = pHTTP->Read(pszBuffer, BUFFER_LENGTH); 
		} 
		delete []pszBuffer; 
		pszBuffer = NULL; 	
	}  
	catch (CException* e) 
	{ 
		delete []pszBuffer; 
		pszBuffer = NULL; 
		e->Delete(); 
	} 

	pHTTP->Close(); 
	delete pHTTP; 

	if (m_pFile)
	{
		m_pFile->Close();
		delete  m_pFile;
		m_pFile = NULL;
	}
	
	if (m_pConnection)
	{
		m_pConnection->Close();
		delete m_pConnection;
		m_pConnection = NULL;
	}

	fTrack.Close(); 

	if (NULL != pBuffer)  
	{ 
		free(pBuffer); 
	}
}

CString CInternetHttp::MakeRequestHeaders(const CString &strBoundary)
{
	CString strFormat; 
	CString strData; 

	strFormat = _T("Content-Type: multipart/form-data; boundary=%s\r\n");//二进制文件传送Content-Type类型为: multipart/form-data 

	strData.Format(strFormat, strBoundary); 
	return strData; 
}

//生成文件头数据
CString CInternetHttp::MakePreFileData(const CString &strBoundary, const CString &strFileName)
{
	/**///////////////////////////////////////////////////////////////////////////////// 
	//Content-Type: 
	//JPG image/pjpeg 
	//PNG image/x-png 
	//BMP image/bmp 
	//TIF image/tiff 
	//GIF image/gif 
	CString strFormat; 
	CString strData; 


	//strFormat += _T("--%s"); 
	//   strFormat += _T("\r\n"); 
	//strFormat += _T("Content-Disposition: form-data; name=\"username\"");//传给网络上的参数，根据网站抓包查看到底是需要哪些 
	//   strFormat += _T("\r\n\r\n"); 
	//strFormat += _T("sd"); 
	//   strFormat += _T("\r\n"); 
	// 

	strFormat += _T("--%s"); 
	strFormat += _T("\r\n"); 
	strFormat += _T("Content-Disposition: form-data; name=\"file_path\"; filename=\"%s\"");//文件地址信息 
	strFormat += _T("\r\n"); 
	strFormat += _T("Content-Type: text/html"); 

	strFormat += _T("\r\n\r\n"); 
	strData.Format(strFormat, strBoundary, PathFindFileName(strFileName));// 

	return strData; 
}

//生成文件结尾数据
CString CInternetHttp::MakePostFileData(const CString &strBoundary, const CString strFilePath)

{
	CString strFormat; 
	CString strData; 

	strFormat = _T("\r\n");	 
	strFormat += _T("--%s\r\n"); 

	strFormat += _T("Content-Disposition: form-data; name=\"filePath\"\r\n\r\n"); 
	strFormat += _T("NewBusinessExpress/");
	strFormat += _T("\r\n");

	strFormat += _T("--%s\r\n");  
	strFormat += _T("Content-Disposition: form-data; name=\"userid\"\r\n\r\n"); 
	int iTmpId = GetTickCount()%(99999-100) + 100; //随机
	CString tmp = _T("");
	tmp.Format(_T("%d"),iTmpId);
	strFormat += tmp;
	strFormat += _T("\r\n"); 
	
	strFormat += _T("--%s\r\n");  
	strFormat += _T("Content-Disposition: form-data; name=\"filename\"\r\n\r\n"); 
	strFormat += _T("");
	strFormat += _T("\r\n"); 


	strFormat += _T("--%s\r\n");  
	strFormat += _T("Content-Disposition: form-data; name=\"customFileName\"\r\n\r\n"); 
//	strFormat += PathFindFileName(strFilePath.GetString());

	strFormat = strFormat.Left(strFormat.GetLength()-5);
	strFormat += _T("\r\n"); 


	strFormat += _T("--%s--");


	strData.Format(strFormat, strBoundary, strBoundary, strBoundary, strBoundary, strBoundary); 

	return strData; 
}

void CInternetHttp::GetDesCode( TCHAR *code,TCHAR *pHome, TCHAR *pFileUpload,TCHAR *account,TCHAR *psd )
{
	int i = 0;
	TCHAR *p[4];
	TCHAR *buf = code;

	while((p[i]=_tcstok(buf, _T("|"))) != NULL) 
	{
		if(i == 0)
		{
			swprintf(pHome, _T("%s"),p[i]);
		}
		if(i == 1)
		{
			swprintf(pFileUpload,_T("%s"),p[i]);
		}
		if(i == 2)
		{
			swprintf(account,_T("%s"),p[i]);
		}
		if(i == 3)
		{
			swprintf(psd,_T("%s"),p[i]);
		}
		i++;
		if(i == 4)
			break;
		buf=NULL; 
	}
}


BOOL CInternetHttp::LoginAndUploadFile( const CString& strFileLocalFullPath, CString &strServerPath)
{
	CString strResponse;
	HttpGet(strUrlHome, NULL, strResponse, strUserName, strPwd);


	if (strResponse.GetLength() > 0)
	{
		strResponse.Empty();
		UploadFile(strUrlUpload, strFileLocalFullPath, strResponse);

		if (strResponse.Find(_T("uploadfiles/")) != -1)
		{
			g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("http上传图片成功,返回响应文本为：%s"), strResponse.GetString());
			int iPos = strResponse.Find(_T("|"));    //截掉后面的部分
			if (iPos > 0)
			{
				strResponse = strResponse.Left(iPos);
			}
			strResponse = strResponse.Right(strResponse.GetLength()-1); //截掉前面的斜杠
			strServerPath = strUrlHome + strResponse;
			return TRUE;
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("http上传图片失败,返回响应文本为：%s"), strResponse.GetString());
		}
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("http上传时打开页面失败"));
	}

	return FALSE;
}
