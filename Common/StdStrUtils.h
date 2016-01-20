#pragma once

#include <vector>

class CStdStrUtils
{
public:
	//根据指定分将字符串拆分到vector，不存在分隔符时vector为空
	int SplitString(const CStdString& input, const CStdString& delimiter, std::vector<CStdString>& results, bool includeEmpties = true);

	//根据指定分将字符串拆分到vector，不存在分隔符时vector返回输入字符串
	void SplitStringEx(const CStdString& input, const CStdString& delimiter, std::vector<CStdString>& results, bool includeEmpties = true);

	//unicode内码转换的函数,将gb的字样转换为big5的(简体式样)
	DWORD UnicodeGBToUnicodeBIG5(const CStdString& szSource, CStdString& szDest);

	//unicode内码转换的函数,将big5的字样转换为gb的(繁体式样)
	DWORD UnicodeBIG5ToUnicodeGB(const CStdString& szSource, CStdString& szDest);

	DWORD MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize);
	DWORD WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize);

	bool IsNumber(CStdString strData);
};