
#include "stdafx.h"
#include "mcctrl.h"
#include "Reg.h"

CMCCtrl::CMCCtrl()
{
}

CMCCtrl::~CMCCtrl()
{
}


//ExSer dll 接口
 
typedef  IThreadUnit *(*CREATEITHREAD)(IGlobalDataBlock* pFindDataByID,IServerSocket *pSocket,IConfigInfo *pConfig);

DWORD  CMCCtrl::Start(BOOL bYun)
{
	m_threadManage.Create(); 

	for(int i=0;i<10;i++)
	{

		TCHAR strField[200] = {0};
		CStdString strFile = _T("");
		wsprintf(strField,_T("THREAD%d"),i);

		int nCount = 0;
		g_Config->ReadInteger(strField,_T("count"),nCount);
		g_Config->ReadString(strField,_T("file"),strFile);

		if(nCount>0)
		{
			HINSTANCE  hd = LoadLibrary(strFile);
			if(hd != NULL)
			{
				CREATEITHREAD CreateIThread = NULL;
				CreateIThread = (CREATEITHREAD)GetProcAddress(hd,"CreateIThread");
				if(CreateIThread!=NULL)
				{
					for(int j=0;j<nCount;j++)
						m_threadManage.InsertThread(CreateIThread(g_GlobalDataBlock,g_server,g_Config));
				}
				else
				{
					g_log.Trace(LOGL_TOP, LOGT_ERROR,__TFILE__,__LINE__,_T("%s 中函数 CreateIThread 未找到, err:%d"),strFile.c_str(), GetLastError());
					wprintf(_T("%s 中函数 CreateIThread 未找到, err:%d\n"),strFile.c_str(), GetLastError());
				}
			}
			else
			{
				DWORD dwerr = GetLastError();
				g_log.Trace(LOGL_TOP, LOGT_ERROR,__TFILE__,__LINE__,_T("%s 装入失败, err:%d"),strFile.c_str(), GetLastError());
				wprintf(_T("%s 装入失败, err:%d\n"),strFile.c_str(), GetLastError());
			}
				
		}

	}

	int	IsTest;
	IsTest = 0;
	g_Config->ReadInteger(_T("Control"),_T("Test"),IsTest);           //zhl 140411  无需连接到176即可推广

	if (IsTest == 1 || bYun)
	{
		T_LocalGlobalData *pLocalGlobalData = NULL;
		pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
			E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

		if (pLocalGlobalData != NULL)
		{
			if (bYun)
				pLocalGlobalData->bIsYunTask = TRUE;		
			if (IsTest == 1)
			{
				pLocalGlobalData->bIsUpdate = FALSE;
				return 1;
			}
		}
		
	}

	CReg reg;
	time_t localTime;
	time_t updateTime;
	
	BYTE *pValue = reg.ReadValueOfKey(HKEY_CURRENT_USER, _T("Software\\szw\\MasterZ"), _T("StationLastupdateTime"));

	if (pValue != NULL)
	{
		time(&localTime);
		updateTime = (time_t)_wtol((const wchar_t*)pValue);

		if ((localTime - updateTime) > 12*60*60)  //离上次更新时间大于12个小时
		{
			//初始下载必需文件
			m_threadManage.PostMessage(0,E_WEBSERVICE_THREAD,E_WEBS_INITDOWNLOADFILE,0);
		}
		else
		{
			T_LocalGlobalData *pLocalGlobalData = NULL;
			pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
				E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

			if (pLocalGlobalData != NULL)
				pLocalGlobalData->bIsUpdate= FALSE;
		}

		delete pValue;
	}
	else
	{
		//初始下载必需文件
		m_threadManage.PostMessage(0,E_WEBSERVICE_THREAD,E_WEBS_INITDOWNLOADFILE,0);
	}

	return 1;
}
DWORD  CMCCtrl::Stop()
{
	m_threadManage.Destory(); //停止所有线程
	return 1;
}