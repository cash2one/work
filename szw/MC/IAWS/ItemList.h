#pragma once
#include "StdString.h"
const TCHAR * const  REGISTER_NETSTAION_LIST_TEMP  = _T("SiteList");
class CItemList
{
	enum 
	{
	NETSTATION_ALL_LIST_TYPE  = 99,
	};
public:
    CItemList(void);
    ~CItemList(void);

	static DWORD GetLocalFileNameFTP(const DWORD dwItemListCode,const DWORD dwListType,CStdString & strFileName);

};
