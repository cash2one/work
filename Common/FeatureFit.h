///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：进行与或非特征匹配的相关接口
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
#include <vector>

struct SPLIT
{
	CStdString strTarget;
	CStdString strReplace;
};

namespace FeatureFit
{
	

	//判断结果
	bool FitDefine(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault);
	bool FitDefine3(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault, bool &bCanDelete, CStdString &);
	//判断结果并返回最后比对项（失败时即为比对失败项）
	bool FitDefine2(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault, CStdString &strErrItem);

	//将输入字符串分解成"与或非"的逻辑结构
	void GetFSData(CStdString strInput, CStdString strMainDelimiter, CStdString strSubDelimitor, T_FILTER_STRUCTURE &tResult);

	//按照"与或非"的逻辑，判断输入字符串是否符合"与或非"过滤数组的定义，不区分大小写
	bool FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, std::vector<CStdString> &sFindCanDelete);
	bool FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, CStdString &strFindCanDelete);
	bool FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult);
	
	//按照"与或非"的逻辑，判断输入字符串是否符合"与或非"过滤数组的定义并返回失败原因
	int FitFilter2(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, CStdString &strErrInfo);

	bool SplitSubStr(std::vector<SPLIT> &vecTmpSplit, CStdString str);
}