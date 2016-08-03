///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2008, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：注册表类声明文件
// 生成日期：2008-8-12
// 作者：陈清松
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REG_H__80BA29DA_7039_4A96_9F85_A4CCD45BDBE6__INCLUDED_)
#define AFX_REG_H__80BA29DA_7039_4A96_9F85_A4CCD45BDBE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReg  
{
public:
	CReg();
	virtual ~CReg();
	
	//读取某个键的值
	BYTE *ReadValueOfKey(HKEY hKey, const TCHAR *const pszKeyPath, 
		const TCHAR *const pszKeyName) const;

	//写入某个键的值
	BOOL WriteValueOfKey(HKEY hKey, 
		const TCHAR *const pszKeyPath, 
		const TCHAR *const pszKeyName,
		const TCHAR *const pszKeyValue,
		DWORD dwKeyValue = 0,
		DWORD dwType = REG_SZ,
		BOOL bDefault = FALSE) const;

	//取得某个键下面的所有子项
	void GetAllSubKey(HKEY hKey, const TCHAR *const pszSubKey, 
		std::vector<TCHAR*>&vecNameOfKey, std::vector<TCHAR*>&vecValueOfKey) const;
	
	//取得某个键下面的所有子项的名字
	void GetAllSubNameOfKey(HKEY hKey, const TCHAR *const pszSubKey, 
		std::vector<TCHAR*>&vecNameOfKey) const;
	
	//删除某个键及该键下面的所有子项
	BOOL DeleteKey(HKEY hKey, const TCHAR *const pszKeyPath, 
		const TCHAR *const pszKeyName) const;

	BOOL DeleteValue(HKEY hKey, const TCHAR *const pszKeyPath, 
		const TCHAR *const pszKeyName) const;
private:
	//得到操作系统的类型
	DWORD GetOSPlatformType() const;
};

#endif // !defined(AFX_REG_H__80BA29DA_7039_4A96_9F85_A4CCD45BDBE6__INCLUDED_)
