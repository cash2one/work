#pragma once

#include "DES.h"
#include <afxinet.h>
#include <string>

using namespace std;

// 操作成功
#define SUCCESS        0

// 操作失败
#define FAILURE        1

// 操作超时
#define OUTTIME        2

#define  BUFFER_SIZE       1024
#define  NORMAL_CONNECT             INTERNET_FLAG_KEEP_CONNECTION
#define  SECURE_CONNECT             NORMAL_CONNECT | INTERNET_FLAG_SECURE
#define  NORMAL_REQUEST             INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE 
#define  SECURE_REQUEST             NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID

#define  IE_AGENT  _T("Mozilla/5.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)")


#define  ULEN  100

class CInternetHttp
{

public:
	CInternetHttp(LPCTSTR strAgent = IE_AGENT);
	//CInternetHttp(LPCTSTR strAgent = _T("IE_AGENT"));
	virtual ~CInternetHttp(void);

	int HttpGet(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse, CString strUserName = _T(""), CString strPwd = _T(""));
	int HttpPost(LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse, CString strUserName = _T(""), CString strPwd = _T(""));
	void UploadFile(const CString& strFileURLInServer, const CString& strFileLocalFullPath, CString &strResponse, CString strUserName = _T(""), CString strPwd = _T(""));

	BOOL LoginAndUploadFile(const CString& strFileLocalFullPath,CString &strServerPath);

private:
	int ExecuteRequest(LPCTSTR strMethod, LPCTSTR strUrl, LPCTSTR strPostData, CString &strResponse, CString strUserName = _T(""), CString strPwd = _T(""));
	void Clear();

	CString MakeRequestHeaders(const CString &strBoundary);

	//生成文件头数据
	CString MakePreFileData(const CString &strBoundary, const CString &strFileName);

	//生成文件结尾数据
	CString MakePostFileData(const CString &strBoundary, const CString strFilePath);
private:
	CInternetSession *m_pSession;
	CHttpConnection *m_pConnection;
	CHttpFile *m_pFile;

	CDES  desDecrypt;


	CString  strUrlHome;
	CString  strUrlUpload;
	CString  strUserName;
	CString  strPwd;


	void GetDesCode(TCHAR *code, TCHAR *pHome, TCHAR *pFileUpload, TCHAR *account, TCHAR *psd);
};

