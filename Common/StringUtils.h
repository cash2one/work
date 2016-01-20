#ifndef __STRINGUTILS_H_
#define __STRINGUTILS_H_

#include "StdString.h"
#include "StdStrUtils.h"
#include "common_enum.h"
#include "SiteInfo.h"

class StringUtils :public CStdStrUtils
{
public:
	DWORD SplitStringToInt(const CStdString& input, const CStdString& delimiter, std::vector<int>& results, bool includeEmpties = true);

	DWORD SplitStringToIntArray(const CStdString& input, const CStdString& delimiter, int * & intresult , bool includeEmpties = true);

	//��,GUI���͹������ض���ʽ��VBStringת��Ϊһά���� ���ò��Ѿ������������ռ�,ֱ�����ͺ���,
	DWORD SplitVBStringTo1DIntArray(const CStdString& input, const CStdString& delimiter, int * & intresult , bool includeEmpties = true);

	//��,GUI���͹������ض���ʽ��VBStringת��Ϊ��ά���� ���ò��Ѿ������������ռ�,ֱ�����ͺ���,
	DWORD SplitVBStringTo2DIntArray(const CStdString& input, const CStdString& delimiter, const CStdString& delimiter2, int & intresult , bool includeEmpties = true);

	//��ȡVB���������ַ�����һά����Ĵ�С�ͷָ���
	DWORD Get1DSplitCharAndLengthVBString(const CStdString& input, CStdString& delimiter, int & intCount );
	//��ȡVB���������ַ����Ķ�ά����Ĵ�С�ͷָ���
	DWORD Get2DSplitCharAndLengthVBString(const CStdString& input, CStdString& delimiter, int & intCount , CStdString& delimiter2, int & intCount2 );

	int mrand();
	CStdString GetYunMu(int Num);
	int GetRand(int nStart,int nEnd);
	DWORD GetRandEName(CStdString &strOut);
	DWORD GetRandCName(CStdString &strOut);
	DWORD CreateUserName(CStdString& strOut);
	DWORD CreateUserName2(CStdString& strOut, E_Account_Style eFirstChType);

	//add 20080923
	//��������ַ���
	DWORD RandStr(int nLen, E_Password_Style eType, CStdString& strOut);
	//�ж��ַ����Ƿ�����������
	BOOL IsNumeric(const TCHAR *pszInput);
	//�ж��ַ����Ƿ���Ӣ���ַ�����
	BOOL IsAlpha(const TCHAR *pszInput);
//end add 20080923


private:
	DWORD MatchFirstCharStr(CStdString &strInAndOut, E_Account_Style eFlag,const CStdString &strZN,const CStdString &strEN);
};

#endif
