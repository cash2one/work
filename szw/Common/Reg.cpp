///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2008, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：注册表类实现文件
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

#include "stdafx.h"
#include "Reg.h"

static DWORD MAX_KEY_LENGTH = 255;		//最长的Key名字


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReg::CReg()
{

}

CReg::~CReg()
{

}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：读取某个键的值
// 输入：hKey－根键
//		 pszKeyPath-子键路径
//		 pszKeyName-子键名字
// 输出：
// 返回值：子键值
// 其它说明：该函数调用完后，需要释放返回值的内存
//
///////////////////////////////////////////////////////////////////////////////
BYTE *CReg::ReadValueOfKey(HKEY hKey, const TCHAR *const pszKeyPath, const TCHAR *const pszKeyName) const
{
//     g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
//         _T("在注册表中读取键值"));

	HKEY hSubKey;
	DWORD dwType = 0;
	BYTE *pbyValue = NULL;
	DWORD dwLen = 0;
	long lRes = RegOpenKeyEx(hKey, pszKeyPath, 0, KEY_ALL_ACCESS, &hSubKey);
	if (lRes == ERROR_SUCCESS)
	{
		RegQueryValueEx(hSubKey, pszKeyName, 0, &dwType, pbyValue, &dwLen);

		if (dwLen > 0)
		{
			dwLen += 1;
			pbyValue = new BYTE[dwLen];
			RegQueryValueEx(hSubKey, pszKeyName, 0, &dwType, pbyValue, &dwLen);
			pbyValue[dwLen] = 0;
		}
	}
	RegCloseKey(hKey);

	return pbyValue;
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：写入某个键的值
// 输入：hKey－根键
//		 pszKeyPath-子键路径
//		 pszKeyName－子键的名字
//		 pszKeyValue－子键的字符串值
//		 dwKeyValue－子键的整数值
//		 dwType－要写入键值的类型
//		 bDefault－该键值是否需要默认值
// 输出：vecNameOfKey-子键名字列表，该函数调用完后，需要释放该列表成员所占的内存
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
BOOL CReg::WriteValueOfKey(HKEY hKey, 
						   const TCHAR *const pszKeyPath, 
						   const TCHAR *const pszKeyName,
						   const TCHAR *const pszKeyValue,
						   DWORD dwKeyValue,
						   DWORD dwType,
						   BOOL bDefault) const
{
	
//     g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
//         _T("在注册表中写入键值"));

	HKEY hKeySub;
	long lRes = RegCreateKey(hKey, pszKeyPath, &hKeySub);
	if (ERROR_SUCCESS == lRes)
	{
		if (REG_SZ == dwType)
		{
			lRes = RegSetValueEx(hKeySub, pszKeyName, 0, dwType, (BYTE*)pszKeyValue, _tcslen(pszKeyValue) * 2);
		}
		else
		{
			lRes = RegSetValueEx(hKeySub, pszKeyName, 0, dwType, (BYTE*)&dwKeyValue, sizeof(DWORD));
		}
	}

	RegCloseKey(hKeySub);
	return (lRes == ERROR_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：取得某个键下面的所有子项
// 输入：hKey－根键
//		 szSubKey-子键路径
// 输出：vecNameOfKey-子键名字列表，vecValueOfKey－子键值的列表
//			该函数调用完后，需要释放输出列表成员所占的内存
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
void CReg::GetAllSubKey(HKEY hKey,
						const TCHAR *const pszSubKey, 
						std::vector<TCHAR*>&vecNameOfKey,
						std::vector<TCHAR*>&vecValueOfKey) const
{	
	int nIndex = 0;
	HKEY hSubKey;
	TCHAR *pszKeyName = NULL;
	DWORD dwLenOfKeyName = MAX_KEY_LENGTH;
	long lRes = RegOpenKeyEx(hKey, pszSubKey, 0, KEY_ALL_ACCESS, &hSubKey);
	if (lRes == ERROR_SUCCESS)
	{
		while (TRUE)
		{
			pszKeyName = new TCHAR[MAX_KEY_LENGTH];
			long lSubRes = RegEnumValue(hSubKey, nIndex++, pszKeyName, 
				&dwLenOfKeyName, NULL, NULL, NULL, NULL);
			if (lSubRes != ERROR_SUCCESS)
			{
				delete []pszKeyName;
				break;
			}

			vecNameOfKey.push_back(pszKeyName);
			dwLenOfKeyName = MAX_KEY_LENGTH;
		}
	}

	RegCloseKey(hKey);
	
	for (UINT i=0; i<vecNameOfKey.size(); i++)
	{
		TCHAR *pszTmp = vecNameOfKey[i];
		BYTE *pbyValue = ReadValueOfKey(HKEY_CURRENT_USER, pszSubKey, vecNameOfKey[i]);
		vecValueOfKey.push_back((TCHAR*)pbyValue);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：取得某个键下面的所有子项的名字
// 输入：hKey－根键
//		 szSubKey-子键路径
// 输出：vecNameOfKey-子键名字列表，该函数调用完后，需要释放该列表成员所占的内存
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
void CReg::GetAllSubNameOfKey(HKEY hKey, 
							  const TCHAR *const pszSubKey, 
							  std::vector<TCHAR*>&vecNameOfKey) const
{
//     g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
//         _T("在注册表中，取得某个键下面的所有子项的名字"));

	int nIndex = 0;
	HKEY hSubKey;
	TCHAR *pszKeyName = NULL;
	DWORD dwLenOfKeyName = MAX_KEY_LENGTH;
	long lRes = RegOpenKeyEx(hKey, pszSubKey, 0, KEY_ALL_ACCESS, &hSubKey);
	if (lRes == ERROR_SUCCESS)
	{
		while (TRUE)
		{
			pszKeyName = new TCHAR[MAX_KEY_LENGTH];
			long lSubRes = RegEnumValue(hSubKey, nIndex++, pszKeyName, 
				&dwLenOfKeyName, NULL, NULL, NULL, NULL);
			if (lSubRes != ERROR_SUCCESS)
			{
				delete []pszKeyName;
				break;
			}

			vecNameOfKey.push_back(pszKeyName);
			dwLenOfKeyName = MAX_KEY_LENGTH;
		}
	}

	RegCloseKey(hKey);
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：删除某个键及该键下面的所有子项
// 输入：hKey－根键
//		 pszKeyPath－子键路径
//		 pszKeyName－键名
// 输出：
// 返回值：删除是否成功
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
BOOL CReg::DeleteKey(HKEY hKey, const TCHAR *const pszKeyPath, const TCHAR *const pszKeyName) const
{
//     g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
//         _T("在注册表中，删除某个键"));

	BOOL bSuccess = FALSE;
	
	CRegKey regKey;
	long lRes = regKey.Open(hKey, pszKeyPath);
	if (ERROR_SUCCESS == lRes)
	{
		if (GetOSPlatformType() == VER_PLATFORM_WIN32_NT)
		{
			bSuccess = (regKey.RecurseDeleteKey(pszKeyName) == ERROR_SUCCESS);
		}
		else
		{
			bSuccess = (regKey.DeleteSubKey(pszKeyName) == ERROR_SUCCESS);
		}
	}

	regKey.Close();
	
	return bSuccess;
}

BOOL CReg::DeleteValue(HKEY hKey, const TCHAR *const pszKeyPath, const TCHAR *const pszKeyName) const
{
	//     g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
	//         _T("在注册表中，删除某个键"));

	BOOL bSuccess = FALSE;

	CRegKey regKey;
	long lRes = regKey.Open(hKey, pszKeyPath);
	if (ERROR_SUCCESS == lRes)
	{
		if (GetOSPlatformType() == VER_PLATFORM_WIN32_NT)
		{
			bSuccess = (regKey.DeleteValue(pszKeyName) == ERROR_SUCCESS);
		}
		//else
		//{
		//	bSuccess = (regKey.DeleteSubKey(pszKeyName) == ERROR_SUCCESS);
		//}
	}

	regKey.Close();

	return bSuccess;
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：得到操作系统的类型
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CReg::GetOSPlatformType() const
{
    OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx ((OSVERSIONINFO *) &osvi);

	return osvi.dwPlatformId;
}
