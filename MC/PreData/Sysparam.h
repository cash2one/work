#pragma once

enum DATETIME
{
	D_YEAR = 0,
	D_MONTH = 1,
	D_DAY = 2
};
class CSysparam
{
public:
	CSysparam(void);
	~CSysparam(void);
	
	//功能:获取系统信息
	int GetSysparam(std::vector<TParamItem>& tParamItemTable, 
						T_TaskData* pTaskData);

	//功能:系统信息测试函数
	int GetSysparamForTest(std::vector<TParamItem>& tParamItemTable);


	//功能:处理对替换值有特殊要求(除长度外,长度的在另一个地方处理)函数
	int TrimFun(const CStdString &strIn, CStdString& strOut,E_PPD_TRIM_TYPE eType);

	/*
	@brief  根据字符串中提取首段数字
	@param  strIn 要提取的字符串
	@return 返回数字字符串
	*/
	CStdString GetNumberFromString(const CStdString &strIn);

	/*
	@brief  根据日期字符串中提取相应的年月日 格式：2011-01-02
	@param  strIn 要提取的字符串
	@param  dType 类型
	@return 返回字符串
	*/
	CStdString GetDateFromString(const CStdString &strIn, DATETIME dType);

private:
	std::vector<CStdString> m_strDupName;
	std::map<int,CStdString> m_EProvValue;

};
