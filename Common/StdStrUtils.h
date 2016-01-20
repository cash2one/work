#pragma once

#include <vector>

class CStdStrUtils
{
public:
	//����ָ���ֽ��ַ�����ֵ�vector�������ڷָ���ʱvectorΪ��
	int SplitString(const CStdString& input, const CStdString& delimiter, std::vector<CStdString>& results, bool includeEmpties = true);

	//����ָ���ֽ��ַ�����ֵ�vector�������ڷָ���ʱvector���������ַ���
	void SplitStringEx(const CStdString& input, const CStdString& delimiter, std::vector<CStdString>& results, bool includeEmpties = true);

	//unicode����ת���ĺ���,��gb������ת��Ϊbig5��(����ʽ��)
	DWORD UnicodeGBToUnicodeBIG5(const CStdString& szSource, CStdString& szDest);

	//unicode����ת���ĺ���,��big5������ת��Ϊgb��(����ʽ��)
	DWORD UnicodeBIG5ToUnicodeGB(const CStdString& szSource, CStdString& szDest);

	DWORD MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize);
	DWORD WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize);

	bool IsNumber(CStdString strData);
};