#include "RegisterExport.h"


/////////////////////////////////////////////////////////////////////////////////////////////
//// Imputs :	1. Rootkey name as CString i.e HKEY_LOCAL_MACHINE, HKEY_CURRENT_USERS...etc
////			2. Subkey name as CString i.e Software\Microsoft, Software\Fonts
////			3. Filename to store the values.
////			example: ExportRegistry(_T("HKEY_CURRENT_USER"), _T(""), _T("C:\\temp\\test.reg"));	
/////////////////////////////////////////////////////////////////////////////////////////////
void CRegisterExport::ExportRegistry(CString cstrKeyRootName, CString cstrKeyName, CString cstrFileName)
{
	FILE *fp;
	HKEY hKeyRootName;
	CString cstrFullPathStr(_T(""));

	if(cstrKeyRootName == _T("HKEY_CLASSES_ROOT"))
		hKeyRootName = HKEY_CLASSES_ROOT;
	else if(cstrKeyRootName == _T("HKEY_CURRENT_USER"))
		hKeyRootName = HKEY_CURRENT_USER;
	else if(cstrKeyRootName == _T("HKEY_LOCAL_MACHINE"))
		hKeyRootName = HKEY_LOCAL_MACHINE;
	else if(cstrKeyRootName == _T("HKEY_USERS"))
		hKeyRootName = HKEY_USERS;
	else if(cstrKeyRootName == _T("HKEY_PERFORMANCE_DATA"))
		hKeyRootName = HKEY_PERFORMANCE_DATA;
	else if(cstrKeyRootName == _T("HKEY_CURRENT_CONFIG"))
		hKeyRootName = HKEY_CURRENT_CONFIG;
	else if(cstrKeyRootName == _T("HKEY_DYN_DATA"))
		hKeyRootName = HKEY_DYN_DATA;

	fp = _tfopen(cstrFileName, _T("w+"));
	if(fp == NULL)
	{
		//MessageBox(NULL, _T("Error while creating the file"), _T("Registry export"), MB_OK);
		return;
	}

	if(cstrKeyName.IsEmpty())
		cstrFullPathStr = cstrKeyRootName;
	else
		cstrFullPathStr = cstrKeyRootName + _T("\\") + cstrKeyName;

	//// First print the header ..... this may be different for some version of Windows... do manually change if required
	//Here need to add version check
	DWORD dwVersion = GetVersion();
	// Get build numbers for Windows NT or Win32s
	if (dwVersion < 0x80000000)                // Windows NT
	{
		_ftprintf(fp, _T("%s\n"), _T("Windows Registry Editor Version 5.00"));
	}
	else        // Win32s
	{
		_ftprintf(fp, _T("%s\n"), _T("REGEDIT4"));
	}

	EnumerateValues(hKeyRootName, cstrKeyName, fp, cstrFullPathStr);
	EnumerateKey(hKeyRootName, cstrKeyName , fp , cstrFullPathStr);
	fclose(fp);
}

/////////////////////////////////////////////////////////////////////////////////////////
//// Enumerates through the values of the given key and store the values into the file.
//// Invokes "FormatDataWithDataType" function to format the data
//// 
//// Inputs	:	1. Handle to the key
////			2. Key name
////			3. File pointer
////			4. Full path of the key to store in the file
/////////////////////////////////////////////////////////////////////////////////////////
void CRegisterExport::EnumerateValues(HKEY hKey, CString cstrKey, FILE *fp, CString cstrFullPath)
{
	static HKEY hLastKey = hKey;
	LONG lResult;
	DWORD dwIndex = 0;
	HKEY hCurKey = hKey;
	DWORD dwKeyType; 
	DWORD dwKeyDataLength, dwKeyNameLen;
	LPBYTE pbbinKeyData = NULL; 
	TCHAR *tcKeyName = NULL;
	TCHAR tcDataType[1024] = _T("");

	lResult = RegOpenKeyEx(hCurKey, cstrKey, 0, KEY_QUERY_VALUE , &hKey);
	if(lResult != ERROR_SUCCESS)
		return;

	DWORD lNoOfValues = 0;
	DWORD lLongestKeyNameLen = 1;
	DWORD lLongestDataLen = 1;

	lResult = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &lNoOfValues, &lLongestKeyNameLen, &lLongestDataLen,
							NULL, NULL);
	
	if(lResult != ERROR_SUCCESS)
		return;

	_ftprintf(fp, _T("\n[%s]\n"), cstrFullPath);

	hLastKey = hKey;

	lLongestKeyNameLen++;
	lLongestDataLen++;

	tcKeyName =	new TCHAR[lLongestKeyNameLen];
	pbbinKeyData = new BYTE[lLongestDataLen];

	CString cstrFinalData, cstrTemp;

	while(TRUE)
	{
		memset(pbbinKeyData, 0, lLongestDataLen);
		memset(tcKeyName, 0, lLongestKeyNameLen);
		dwKeyType = dwKeyDataLength = dwKeyNameLen = 0;

		dwKeyNameLen = lLongestKeyNameLen;
		dwKeyDataLength = lLongestDataLen;

		lResult = RegEnumValue(hKey, dwIndex, tcKeyName, &dwKeyNameLen, NULL, &dwKeyType, pbbinKeyData, &dwKeyDataLength);
		if(lResult == ERROR_NO_MORE_ITEMS)
			break;

		FormatDataWithDataType(dwKeyType, pbbinKeyData, dwKeyDataLength, cstrFinalData);
	
		//// For (default) key names the tcKeyName is empty and dwKeyNameLen is zero ...in such case we need to 
		//// have assignment like @ = "value"
		CString cstrTest;
		cstrTest = tcKeyName;
		if(cstrTest.IsEmpty())
		{
			cstrTemp.Format(_T("@ = "));
		}
		else
		{
			cstrTemp.Format(_T("\"%s\" = "), tcKeyName);
		}
		cstrTemp += cstrFinalData;

		_ftprintf(fp, _T("%s"), (LPCTSTR)cstrTemp);
		dwIndex++;
	}
	RegCloseKey(hKey);
	delete tcKeyName;
	delete pbbinKeyData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Format the value with the actual data as the same way the REGEDIT does....
//// Inputs: 1. Data type of the key
////		 2. Data to be formatted
////		 3. Length of the data
//// Output: 4. Formatted string
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CRegisterExport::FormatDataWithDataType(DWORD dwKeyType, LPBYTE pbbinKeyData, DWORD dwKeyDataLength, CString &cstrOutput)	
{
	CString cstrTemp, cstrTemp1 ,cstrTemp2;
	int nIndex = 0;
	switch(dwKeyType)
	{
		case REG_SZ:
		{
			cstrTemp.Format(_T("\"%s\"\n"), pbbinKeyData);
			for(int i=0;i<cstrTemp.GetLength();i++)
			{
				cstrTemp1 = cstrTemp.Mid(i,1); 
				if(cstrTemp.Mid(i,1) == "\\")
				{
					cstrTemp1 = cstrTemp.Left(i); 
					cstrTemp2 = cstrTemp.Right(cstrTemp.GetLength()-i-1);
					cstrTemp = cstrTemp1+"\\\\"+cstrTemp2;
					i++;
				}
			}
			cstrOutput = cstrTemp;
			break;
		}

		case REG_DWORD: /// same is for REG_DWORD_LITTLE_ENDIAN
		{
			DWORD dwValue;
			memcpy(&dwValue, pbbinKeyData, sizeof DWORD);
			cstrTemp.Format(_T("dword:%08x\n"), dwValue);
			cstrOutput = cstrTemp;
			break;
		}

		case REG_BINARY:
		case REG_MULTI_SZ:
		case REG_EXPAND_SZ:
		case REG_FULL_RESOURCE_DESCRIPTOR:
		case REG_RESOURCE_LIST:
		case REG_RESOURCE_REQUIREMENTS_LIST:
		{
			if(dwKeyType != REG_BINARY)
				cstrOutput.Format(_T("hex(%d):"), dwKeyType);
			else
				cstrOutput.Format(_T("hex:"));

			for(DWORD dwIndex = 0; dwIndex < dwKeyDataLength; dwIndex++)
			{
				cstrTemp1.Format(_T("%02x"), pbbinKeyData[dwIndex]);
				if(dwIndex != 0 && (dwIndex % 0x15 == 0))
				{
					cstrTemp += _T(",\\\n");
					cstrTemp += cstrTemp1;
				}
				else
				{
					if( cstrTemp.IsEmpty() )
						cstrTemp = cstrTemp1;
					else
						cstrTemp += ","+cstrTemp1;
				}
			}

			cstrTemp += _T("\n");
			cstrOutput += cstrTemp;
			break;
		}

		case REG_NONE:
		case REG_DWORD_BIG_ENDIAN:
		case REG_LINK:
			//// TODO : add code for these types...
			break;

	}
}

BOOL CRegisterExport::IsExist(HKEY hKey,LPCTSTR lpSubKey)
{
	HKEY phkResult;
	if( ERROR_SUCCESS == RegOpenKeyEx(hKey,lpSubKey,0,
		KEY_EXECUTE,&phkResult))
	{
		RegCloseKey(phkResult);
		return TRUE;
	}
	return FALSE;
}

BOOL CRegisterExport::IsSubKey(HKEY hKey,LPCTSTR lpSubKey)
{
	if(IsExist(hKey,lpSubKey))
	{
		HKEY phkResult;
		if( ERROR_SUCCESS == RegOpenKeyEx(hKey,lpSubKey,0,
			KEY_READ,&phkResult))
		{
			TCHAR sz[MAX_PATH];
			if( ERROR_SUCCESS == RegEnumKey(phkResult, 0, sz, MAX_PATH))
			{
				RegCloseKey(phkResult);
				return TRUE;
			}
			RegCloseKey(phkResult);
		}
	}
	return FALSE;
}

void CRegisterExport::EnumerateKey(HKEY hKey,CString strKey, FILE *fp,CString cstrFullPath)
{
	TCHAR strCurString[1024];
	long lResult;
	DWORD dwCurIndex = 0;
	HKEY hCurKey;
	CString strCurKey;
	CString strSubKey = strKey;
	CString strFullKey = cstrFullPath;

	//// first open the root key to get the handle...
	lResult = RegOpenKeyEx(hKey, strKey, 0, KEY_ENUMERATE_SUB_KEYS, &hCurKey);
	if( lResult != ERROR_SUCCESS )
		return;

	do
	{
		lResult = RegEnumKey(hCurKey, dwCurIndex, strCurString , sizeof(strCurString));

		if((lResult == ERROR_NO_MORE_ITEMS) || (lResult == ERROR_INVALID_HANDLE))
		{
			break;
		}
		else
		{
			strCurKey.Format(_T("%s"),strCurString);
			if(strCurKey.IsEmpty())
				strFullKey = cstrFullPath;
			else
			{
				strFullKey = cstrFullPath + _T("\\") + strCurKey;
				strSubKey  = strKey + _T("\\") + strCurString;
			}
			EnumerateValues(hKey, strSubKey , fp, strFullKey);
			if(IsSubKey(hKey , strSubKey))
			{				
				EnumerateKey(hKey , strSubKey , fp, strFullKey);
			}
			dwCurIndex++;
		}

	}while(TRUE);
	RegCloseKey(hCurKey);
}

void CRegisterExport::ShellExportRegister(LPCTSTR lpExportStr,LPCTSTR lpExportFile)
{
	CString strItem(lpExportStr);
	CString strFileName(lpExportFile);
	CString strFilePath;
	CString strParameters;
	int nIndex = 0;

	strParameters = _T("/e \"") + strFileName + _T("\" \"") + strItem + _T("\"");
	ShellExecute(0, _T("open"), _T("regedit.exe"),
		strParameters, NULL	, SW_SHOWNORMAL);
}

void CRegisterExport::ShellImportRegister(LPCTSTR lpImportFile)
{
	CString strItem(lpImportFile);
	CString strParameters;

	strParameters = _T("/s \"") + strItem + _T("\"");
	ShellExecute(NULL, _T("open"), _T("regedit.exe"),
		strParameters,NULL,SW_HIDE);
}