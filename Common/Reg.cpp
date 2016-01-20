///////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2008, ��Ѷ����Ϣ���޹�˾
//
// �汾��1.0
// �ļ�˵����ע�����ʵ���ļ�
// �������ڣ�2008-8-12
// ���ߣ�������
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reg.h"

static DWORD MAX_KEY_LENGTH = 255;		//���Key����


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
// ����������������ȡĳ������ֵ
// ���룺hKey������
//		 pszKeyPath-�Ӽ�·��
//		 pszKeyName-�Ӽ�����
// �����
// ����ֵ���Ӽ�ֵ
// ����˵�����ú������������Ҫ�ͷŷ���ֵ���ڴ�
//
///////////////////////////////////////////////////////////////////////////////
BYTE *CReg::ReadValueOfKey(HKEY hKey, const TCHAR *const pszKeyPath, const TCHAR *const pszKeyName) const
{
//     g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
//         _T("��ע����ж�ȡ��ֵ"));

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
// ��������������д��ĳ������ֵ
// ���룺hKey������
//		 pszKeyPath-�Ӽ�·��
//		 pszKeyName���Ӽ�������
//		 pszKeyValue���Ӽ����ַ���ֵ
//		 dwKeyValue���Ӽ�������ֵ
//		 dwType��Ҫд���ֵ������
//		 bDefault���ü�ֵ�Ƿ���ҪĬ��ֵ
// �����vecNameOfKey-�Ӽ������б��ú������������Ҫ�ͷŸ��б��Ա��ռ���ڴ�
// ����ֵ��
// ����˵����
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
//         _T("��ע�����д���ֵ"));

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
// ��������������ȡ��ĳ�����������������
// ���룺hKey������
//		 szSubKey-�Ӽ�·��
// �����vecNameOfKey-�Ӽ������б�vecValueOfKey���Ӽ�ֵ���б�
//			�ú������������Ҫ�ͷ�����б��Ա��ռ���ڴ�
// ����ֵ��
// ����˵����
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
// ��������������ȡ��ĳ����������������������
// ���룺hKey������
//		 szSubKey-�Ӽ�·��
// �����vecNameOfKey-�Ӽ������б��ú������������Ҫ�ͷŸ��б��Ա��ռ���ڴ�
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
void CReg::GetAllSubNameOfKey(HKEY hKey, 
							  const TCHAR *const pszSubKey, 
							  std::vector<TCHAR*>&vecNameOfKey) const
{
//     g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
//         _T("��ע����У�ȡ��ĳ����������������������"));

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
// ��������������ɾ��ĳ�������ü��������������
// ���룺hKey������
//		 pszKeyPath���Ӽ�·��
//		 pszKeyName������
// �����
// ����ֵ��ɾ���Ƿ�ɹ�
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
BOOL CReg::DeleteKey(HKEY hKey, const TCHAR *const pszKeyPath, const TCHAR *const pszKeyName) const
{
//     g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
//         _T("��ע����У�ɾ��ĳ����"));

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
	//         _T("��ע����У�ɾ��ĳ����"));

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
// ���������������õ�����ϵͳ������
// ���룺
// �����
// ����ֵ��
// ����˵����
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
