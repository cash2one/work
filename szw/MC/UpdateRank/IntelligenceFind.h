#pragma once


class CIntelligenceFind
{
public:
	CIntelligenceFind();
	~CIntelligenceFind();

public:
	/*
	@brief 查找是否满足参数的条件
	@param strData  要查找的数据；
	@param strParam1 满足条件1；
	@param strParam2 满足条件2；
	@param iFlag 0:2个是否都存在。1:2个是否都不存在。2：第一个参数存在，第二个参数不存在；
	@param iCmpType 0:与比较， 1：或比较
	@return 返回是否查找成功
	*/
	BOOL FindResult(const CString &strData, const CString &strParam1,const CString &strParam2,int iFlag=0,int icmpType=0);

	/*
	@brief 查找是否满足参数的条件
	@param strData  要查找的数据；
	@param strParam1 满足条件1；
	@param strParam2 参数2；
	@param strParam3 参数3；
	@param iFlag 0:3个是否都存在。1:3个是否都不存在。2：一个参数存在，二个参数不存在。4：两个参数存在，一个参数不存在；
	@param iCmpType 0:与比较， 1：或比较
	@return 返回是否查找成功
	*/
	BOOL FindResult(const CString &strData, const CString &strParam1, const CString &strParam2, const CString &strParam3, int iFlag = 0, int icmpType = 0);


	/*
	@brief 提取查找到字符串的位置
	@param strData  要查找的数据；
	@param 要查找的参数
	@param 要查找的参数2
	@param iInitPos 初始查找位置；
	@return 返回查找到的结果；
	*/
	int GetFindPos(const CString &strData, const CString &strPara1, const CString& strPara2,int &iInitPos);

	/*
	@breif 提取指定位置字符串
	@param strData 要从中提取的字符串；
	@param strPara1 参数1
	...
	@param iInitPos 初始位置；
	@return 返回截取到的字符串
	*/
	CString GetFindData(const CString &strData, const CString &strPara1,const CString &strPara2,
						const CString &strPara3,int iInitPos=0);
};

