#ifndef _YDMRECONGNIZE_H_
#define _YDMRECONGNIZE_H_

#include "stdafx.h"

class YDMRecognize : public IRecognize
{
public:
	YDMRecognize();
	~YDMRecognize();

public:
	long Recognize(LPCTSTR strImgPath, LPTSTR strResult);
	long ReportError(long nResultCode);
	bool InitRecognize();
	
private:
	char* w2a(const wchar_t *pStr);
	wchar_t* a2w(const char* pStr);

private:
	long m_iSoftID;
	TCHAR* m_strUserName;
	TCHAR* m_strPassword;
	TCHAR* m_strSoftKey;
	long  m_nCodeType; 
	bool m_bInit;
};

#endif
