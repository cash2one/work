#pragma once

class CHttpUtils
{
public:
	//CHttpUpdate();
	//~CHttpUpdate();

	BOOL GetFile(TCHAR *pURL, TCHAR *pLocalName, int nRetry = 3);
	CString GetSerRespInfo(TCHAR *szUrl);        //��ȡָ��ҳ�淵�ص���Ϣ

private:
	BOOL InternetGetFile(TCHAR *pURL, TCHAR *pLocalName);

};

