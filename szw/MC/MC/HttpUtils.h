#pragma once

class CHttpUtils
{
public:
	//CHttpUpdate();
	//~CHttpUpdate();

	static BOOL GetFile(const TCHAR *pURL, const TCHAR *pLocalName, int nRetry = 3);

private:
	static BOOL InternetGetFile(const TCHAR *pURL, const TCHAR *pLocalName);

};

