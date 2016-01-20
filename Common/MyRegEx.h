///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：对Boost的正则表达式进行封装，提供正则匹配的接口
//          使用前需正确编译Boost库并配置好工程目录，
//          具体请参考：http://blog.csdn.net/foxbryant/article/details/8857042
//          
// 生成日期：
// 作者：何培田
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EnDeCode.h"

class CMyRegEx
{
private:
	CEnDeCode EDTool ;
public:
	CMyRegEx(void);
	~CMyRegEx(void);

	//对输入串进行正则匹配，匹配成功返回true,否则返回false
	bool Like(CStdString strInput, CStdString strRegEx);

	//对输入串进行正则匹配，并返回匹配到的值
	bool Match(CStdString strInput, CStdString strRegEx, std::vector<CStdString> &vRes);

private:
	//bool MatchCharList(CStdString strInput , CStdString) ;

	//暂时只支持带*号的字符串的匹配
	//提取[charlist]匹配符
	void ExtractCharList(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords) ;
	//提取*匹配符
	void ExtractAsterisk(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords) ;
	//提取？匹配符
	void ExtractQuestionMark(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords) ;
	//提取#匹配符
	void ExtractSharpMark(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords) ;
};
