#pragma once
class CSearchWord
{
public:
	CSearchWord();
	~CSearchWord();

	//接口，处理关键词
	void ProcessKeyWord(const CStdString &strWord);
private:
	//解析搜索关键词数据
	BOOL ParseSearchKeyWordData(const CStdString &strData);
	//打开URL获取源
	CString urlopen(CString url);

	//读取配置信息
	BOOL ReadConfig();

	//宽字节转多字节
	char* CStringToMutilChar(CString& str, int& chLength, WORD wPage);
	//URL编码
	CString  UrlEncode(CString sIn);

	//多字节转宽字节
	CString MulitToStringT(LPSTR str);

	//获取网页的数据
	BOOL GetWebData(const CString &strData,const CString &strKey);

	//检查是否在我们的商贸网站中
	BOOL CheckWeb(const CString &strData);

	//获取本地商贸网站
	void GetDefaultWeb();

	//获取域名
	void GetMainDomain(CString &strUrl);

	/*
	@brief  宽字符转换成多字节
	@param  pChar  宽字符
	@return 返回多字节
	*/
	char* WideToChar(const wchar_t *pWide, DWORD dwCode = CP_ACP);

	BYTE toHex(const BYTE &x);

private:
	std::vector<CStdString> m_vStrKeyWord; //关键词数组

	int m_dwPrivilege;	//特权值
	int m_dwStandard;		//标准值
	int m_dwDifftime;     //两个词的时间差

	CStringA g_strAllWeb;

};

