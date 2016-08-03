#pragma once

class CHttpUtils
{
public:
	//CHttpUpdate();
	//~CHttpUpdate();

	BOOL GetFile(TCHAR *pURL, TCHAR *pLocalName, int nRetry = 3);
	CString GetSerRespInfo(TCHAR *szUrl);        //获取指定页面返回的信息

private:
	BOOL InternetGetFile(TCHAR *pURL, TCHAR *pLocalName);

};

