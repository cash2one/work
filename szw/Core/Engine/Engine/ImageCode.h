#pragma once
class CImageCode
{
public:
	CImageCode(void);
	~CImageCode(void);
	
	//下载图片
	bool GetImage(CStdString strURL, CStdString strLocFile);
	
	//得到图片文件的类型。
	DWORD GetImageType(LPCTSTR pszFileName);

private:
	//下载
	BOOL InternetGetFile(TCHAR *szUrl, TCHAR *szFileName);         
};

