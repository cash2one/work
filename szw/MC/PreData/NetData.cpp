#include "StdAfx.h"
#include ".\netdata.h"
#include "FileReadAndSave.h"


CNetData::CNetData(void)
{

}

CNetData::~CNetData(void)
{

}

//功能:打开本地安装目录下的文件（默认 A0.bin.src）
//输入:file,文件名
//输出:
//返回值:
//其他说明:
DWORD CNetData::OpenWebSiteData(DWORD dwNetId)
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path,MAX_PATH);
	PathAppend(path, _T("..\\..\\"));
	
	swprintf(path,_T("%sData\\%d.dat"),path,dwNetId);
	

	return m_webSiteData.Open(path);
}


//功能:关闭本地安装目录下的文件（默认 A0.bin.src）
//输入:file,文件名
//输出:
//返回值:
//其他说明:

DWORD CNetData::CloseWebSiteData()
{
	m_webSiteData.Close();
	return 0;
}



//功能:根据网站id和任务类型获取网站数据
//输入:dwNetId,网站编号;eTaskType,任务类型
//输出:tNetstationBaseInfo,网站数据
//返回值:-1,未知的任务类型;-2,打开记录集失败;1,没有该网站数据;0,成功
//其他说明:
int CNetData::GetNetData(T_NetstationBaseInfo& tNetstationBaseInfo)
{
	return m_webSiteData.GetNetBaseInfo(tNetstationBaseInfo);;
}


//功能:获取NSVrealvalue参数
//输入:nNSVnscode;nNSVtype;strNSVcode
//输出:strNSVrealvalue
//返回值:-1,打开记录集失败;1,取得参数值
//其他说明:
int CNetData::GetNSVrealvalue(int nNSVtype, CStdString strNSVcode, 
							  CStdString& strNSVrealvalue)
{

	PCHAR cNSVrealValue = m_webSiteData.GetNSVrealvalue(nNSVtype,_ttoi(strNSVcode.c_str()));
	if (cNSVrealValue == NULL)
	{
		return 0;
	}
	
	m_webSiteData.ToMByteToWChar(cNSVrealValue , strNSVrealvalue);

	if (strNSVrealvalue.GetLength()>0)
	{
		return 1;
	}

	return 0;
}


UINT64 CNetData::GetItemCode_Reg()
{
	return m_webSiteData.GetDefaultItemCode_Reg();
}


///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：获取客户机上的最后更新时间
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////

DWORD CNetData::GetNetLastUpdateTime()
{
	DWORD lastUpdataTime =  m_webSiteData.GetLastUpdateTime();
	return lastUpdataTime;
}

void CNetData::GetNStype( T_Site_Type &tSiteType )
{
	m_webSiteData.GetNStype(tSiteType);
}

PCHAR CNetData::GetNSIparamater_Post( UINT64 nNetItemId)
{
	return m_webSiteData.GetNSIparamater_Post(nNetItemId);
}

PCHAR CNetData::GetNSIparamater_Register( UINT64 nNetItemId )
{
	return m_webSiteData.GetNSIparamater_Register(nNetItemId);
}

UINT64 CNetData::GetItemCode_Post()
{
	return  m_webSiteData.GetDefaultItemCode_Post();
}

DWORD CNetData::ToMByteToWChar( IN PCHAR startPos,OUT CStdString &str )
{
	return m_webSiteData.ToMByteToWChar(startPos, str);
}

/*
	@brief  取得网站刷新和发布要求
	@param [out] tLimit 保存站网站刷新和发布数据
	*/
void CNetData::GetNSlimit( T_Limit &tLimit )
{
	m_webSiteData.GetNSlimit(tLimit);
}

/*
@brief 根据网站的真实值获取标准变量值
@param nsType 类型 
@param realValue 网站真实值
@return 标准变量值
*/
DWORD CNetData::GetStandardValue( int nsType,CStdString& realValue )
{
	PCHAR szValue = m_webSiteData.WCharToMbyte(realValue); //转多字节
	if (szValue == NULL)
	{
		return -1;
	}
	DWORD dwResult = m_webSiteData.GetStandardByRealvalue(nsType,szValue);

	delete szValue;

	return dwResult;
}

