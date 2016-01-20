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

	//把,GUI传送过来的特定格式的VBString转换为一维数组 调用层已经分配好了数组空间,直接填充就好了,
	DWORD SplitVBStringTo1DIntArray(const CStdString& input, const CStdString& delimiter, int * & intresult , bool includeEmpties = true);

	//把,GUI传送过来的特定格式的VBString转换为二维数组 调用层已经分配好了数组空间,直接填充就好了,
	DWORD SplitVBStringTo2DIntArray(const CStdString& input, const CStdString& delimiter, const CStdString& delimiter2, int & intresult , bool includeEmpties = true);

	//获取VB传过来的字符串的一维数组的大小和分隔符
	DWORD Get1DSplitCharAndLengthVBString(const CStdString& input, CStdString& delimiter, int & intCount );
	//获取VB传过来的字符串的二维数组的大小和分隔符
	DWORD Get2DSplitCharAndLengthVBString(const CStdString& input, CStdString& delimiter, int & intCount , CStdString& delimiter2, int & intCount2 );

	int mrand();
	CStdString GetYunMu(int Num);
	int GetRand(int nStart,int nEnd);
	DWORD GetRandEName(CStdString &strOut);
	DWORD GetRandCName(CStdString &strOut);
	DWORD CreateUserName(CStdString& strOut);
	DWORD CreateUserName2(CStdString& strOut, E_Account_Style eFirstChType);

	//add 20080923
	//生成随机字符串
	DWORD RandStr(int nLen, E_Password_Style eType, CStdString& strOut);
	//判断字符串是否都是数字类型
	BOOL IsNumeric(const TCHAR *pszInput);
	//判断字符串是否都是英文字符类型
	BOOL IsAlpha(const TCHAR *pszInput);
//end add 20080923


private:
	DWORD MatchFirstCharStr(CStdString &strInAndOut, E_Account_Style eFlag,const CStdString &strZN,const CStdString &strEN);
};

#endif
