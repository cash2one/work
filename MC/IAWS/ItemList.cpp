#include "StdAfx.h"
#include ".\itemlist.h"



CItemList::CItemList(void)
{
}

CItemList::~CItemList(void)
{
}



DWORD  CItemList::GetLocalFileNameFTP(const DWORD dwItemListCode,const DWORD dwListType,CStdString & strFileName)
{

	switch (dwListType )
	{

	case ItemListAccountManage:
		{
			if (1 == dwItemListCode)
			{
				//栏目的最后更新时间
				strFileName.Format(_T("%d\\%s.txt"),dwListType,
					NETSTAION_LASTUPDATE_LIST);
			}
			else if(0 == dwItemListCode)
			{
			//栏目文件先转成A9990.txt
				strFileName.Format(_T("%d\\%s.txt"),dwListType,
					REGISTER_NETSTAION_LIST_TEMP);
			}
		}
		break;
	case NETSTATION_ALL_LIST_TYPE:
		{
			strFileName.Format(_T("Data\\%d.dat"), dwItemListCode);
		}
		break;

	default:
		break;
	}

	return BIZMC_OK;

}

