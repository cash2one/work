//////////////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2008, 商讯网信息有限公司
// 版本：
// 文件说明： 用来处理和公司服务器交互的类,该类只会实例化一次
// 生成日期：2008-11-11
// 作者： yli
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2.
//

#include "stdafx.h"
#include "WebService.h"
#include "StdString.h"
#include "ZipEncode.h"
#include "FileReadAndSave.h"
#include "ItemList.h"

//和服务器交互的具体处理的类
#include "shlobj.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：构造函数,给类变量赋初始值
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////

CWebService::CWebService(void)
:IThreadUnit(E_WEBSERVICE_THREAD,0x055F)
{


}

//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：析构函数，
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
CWebService::~CWebService(void)
{

}

//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：处理发送给线程的各个消息
// 输入：Msg 消息
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::CreateThread()
{
    m_dwNetLastupdateIndex = 900;
    m_dwAllStationLastUpdateGetting = 0;//正在获取全部网站的最后更新时间
	m_dwWebServiceAllStationLastUpdateSended = 0; //还没有发送数据已经准备好的消息
	dwIsAllNetstationLastUpdateGetted = 0;

	memset(m_szCurrPath, 0, sizeof(m_szCurrPath));
	GetModuleFileName(NULL, m_szCurrPath, MAX_PATH);
	PathAppend(m_szCurrPath, _T("..\\..\\"));

	std::cout << "Create WebService Thread   "  << ::GetTickCount() <<std::endl ;

    return 0;

}


//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：处理发送给线程的各个消息
// 输入：Msg 消息
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::DispatchMessage(T_Message *Msg)
{

    DWORD MsgParam = 0;
    MsgParam = Msg->dwMsgParam;

    switch(MsgParam)
    {
    case MSG_TIMER:	//时钟消息,需要处理
		{
        }
        break;

	case E_WEBS_INITDOWNLOADFILE:      //初始时下载一些必需文件
		{
			GetWebServiceData(Msg->dwData);
		}
		break;
		
	case E_WEBS_GET_NETSTATION_BASEINFO:      //获取单个网站的基本信息 1
		{
			GetNetDataFromFTP(E_WEBS_GET_NETSTATION_BASEINFO,Msg->dwData);
		}
		break;

    case E_WEBS_LASTUPDATE_GETTED:   ////网站最后更新时间已经获取了 2
        {
            GetAllStationLastUpdateALLGettedPPD(Msg->dwData);
        }
        break;
 
	case E_FTP_GET_FILE_FROM_SERVER_SAVED:      //FTP保存完成单个网站的基本信息 2
		{
			GetServerFileFromFtp(Msg->dwData);
		}
		break;
    default:	 //
        {

        }
        break;
    }
    return BIZMC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：PPD线程已经获取了,需要删除全局数据区的内存
// 输入：dwDataIndexID 数据组织类编号
// 输出：
// 返回值：1 成功 ; 0 失败
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::GetAllStationLastUpdateALLGettedPPD(const DWORD dwDataIndexID)           
{
	std::map<DWORD, T_ServerNetStationInfo*> *pAllStationInfo = NULL;

	pAllStationInfo =
		(std::map<DWORD, T_ServerNetStationInfo*>*)g_GlobalDataBlock->DeleteDataNode(
		dwDataIndexID,E_ServerNetStationInfoGroup,E_WEBSERVICE_CLASS,eUseBlockWithOne);
	

	if(pAllStationInfo)
	{
		std::map<DWORD, T_ServerNetStationInfo*>::iterator iter = 
			pAllStationInfo->begin();

		for(;iter != pAllStationInfo->end(); ++iter)
		{
			T_ServerNetStationInfo *pStation = iter->second;
			if (pStation != NULL)
			{
				delete pStation;
				pStation = NULL;
			}
		}

		pAllStationInfo->clear();

		delete pAllStationInfo;
		pAllStationInfo = NULL;
	}

	return BIZMC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::GetServerFileFromFtp(const DWORD dwDataIndexID)
{
	T_FtpFileName *pFtpFileName = NULL;

	pFtpFileName =
		(T_FtpFileName *)g_GlobalDataBlock->ReadRealData(
		E_FtpFileName,dwDataIndexID,E_WEBSERVICE_CLASS,eUseCommReadWithMulti);

	if(NULL == pFtpFileName)
	{
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,\
			_T("FTP 返回的文件 数据组织类为空,数据编号%d ") ,dwDataIndexID);
		return BIZMC_OK;
	}


	CStdString strFile;
	CStdString strSourceFile;
	CStdString strTemp;

	{
		CItemList::GetLocalFileNameFTP(pFtpFileName->dwFileID,pFtpFileName->dwFileType,strTemp);
		strSourceFile.Format(_T("%s"),pFtpFileName->szLocalFileName);
		strFile.Format(_T("%s%s"),m_szCurrPath,strTemp);


		CFileReadAndSave::UnZipFileBinToFile(strFile,strSourceFile);

		//处理网站信息文件的过程
		GetNetstationAllInfoReturn(pFtpFileName->dwFileID,pFtpFileName->dwFileType,strFile);	

		//处理网站最后更新时间的
		GetAllStationLastUpdateALLReturn(pFtpFileName->dwFileID,pFtpFileName->dwFileType,strFile);


	}

	pFtpFileName =
		(T_FtpFileName *)g_GlobalDataBlock->DeleteDataNode(
		dwDataIndexID,E_FtpFileName,E_WEBSERVICE_CLASS,eUseBlockWithOne);

	// 删除压缩文件, 网站最后更新时间不删除. 需要进行加密处理
	if ((1 == pFtpFileName->dwFileID) &&  ( 6 == pFtpFileName->dwFileType))
	{
		//把文件大小写入注册表
	}
	else
	{
		::DeleteFile(strSourceFile.c_str());
	}

	if(pFtpFileName)
	{
		delete pFtpFileName;
		pFtpFileName = NULL;
	}
	
	return BIZMC_OK;
}

DWORD CWebService::GetNetstationAllInfoReturn(const DWORD dwItemListCode,const DWORD dwListType,const CStdString &strFile) //获取网站信息数据文件的返回
{
	if (!(NETSTATION_ALL_LIST_TYPE == dwListType))
	{
		//处理网站信息文件的事情
		return 1;
	}
	
	if (CFileReadAndSave::GetItemFileLength(strFile) <= 3)
	{
		PostMessage(E_DATAPRE_THREAD,E_PPD_SERVERRETURNERR_MSG,dwItemListCode); //获取信息失败,提示准备线程
		return 2;
	}
	//向PPD线程发送已经成功的消息

	PostMessage(E_DATAPRE_THREAD,
		E_PPD_RETURNNETSTATIONBASEINFO_MSG,dwItemListCode); //保存数据库完成,需要发送消息给准备线程

	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
		_T("FTP向发送网站成功数据DataID:%d    TimeTick:%d   网站编号:%d"),dwItemListCode,GetTickCount(),dwItemListCode);

	
	return BIZMC_OK;
}



DWORD CWebService::SendToFtpGetServerFile(const int dwItemListCode,const int dwListType ,const DWORD dwAuto)
{
	T_FtpFileName *pFtpFileName = NULL;
	_NEW(pFtpFileName,T_FtpFileName);

	if (NULL != pFtpFileName)
	{
		//放入到数据组这类中,让数据准备线程整理数据
		DWORD dwDataIndex = g_GlobalDataBlock->GetGlobalID();

		g_GlobalDataBlock->WriteRealData(dwDataIndex,
			E_FtpFileName,pFtpFileName,
			E_WEBSERVICE_CLASS,eWithoutUse);

		pFtpFileName->dwFileID = dwItemListCode;
		pFtpFileName->dwFileType = dwListType;
		pFtpFileName->dwReserve = dwAuto;

		if (NETSTATION_ALL_LIST_TYPE == dwListType)
		{
			wsprintf(pFtpFileName->szLocalFileName ,_T("%sData\\%d.bin"),m_szCurrPath,dwItemListCode);
			wsprintf(pFtpFileName->szServerFileName,_T("99\\%d.bin"),dwItemListCode);
		}
		else
		{
			wsprintf(pFtpFileName->szLocalFileName ,_T("%s%d\\%d.bin"),m_szCurrPath,dwListType,dwItemListCode);
			wsprintf(pFtpFileName->szServerFileName,_T("6\\%d.bin"),dwItemListCode);
		}

		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
			_T("SendToFtpGetServerFile, globalindex=%d,  pFtpFileName->dwFileID =%d,pFtpFileName->dwFileType =%d,pFtpFileName->dwReserve =%d,pFtpFileName->szLocalFileName  =%s,pFtpFileName->szServerFileName= %s"), \
			dwDataIndex,pFtpFileName->dwFileID, pFtpFileName->dwFileType , pFtpFileName->dwReserve ,pFtpFileName->szLocalFileName ,pFtpFileName->szServerFileName);	

		PostMessage(E_UPDATE_THREAD,E_FTP_GET_FILE_FROM_SERVER,dwDataIndex);
	}

	return BIZMC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
void CWebService::GetAllStationLastUpdateALL()
{
	if (1 == m_dwAllStationLastUpdateGetting)
	{
		return;
	}
	
	SendToFtpGetServerFile(1,6 ,0);
	m_dwAllStationLastUpdateGetting = 1; 
}


//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::GetAllStationLastUpdateALLReturn(const DWORD dwItemListCode,const DWORD dwListType,const CStdString &strFile)           
{
	if (!(1 == dwItemListCode &&   6 == dwListType))	
	{
		return 1;
	}

	CStdString szResult;
	CStdString szKeyCode;
	m_dwAllStationLastUpdateGetting = 0;
	
	szKeyCode.Format(_T("%s6\\%s.txt"), m_szCurrPath, NETSTAION_LASTUPDATE_LIST);

	CFileReadAndSave::ReadStringFromFileGB(szKeyCode,szResult);

	GetAllStationLastUpdateALLOK(szResult);

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：处理获取本地或服务器的网站最后更新时间成功的需要做的操作
// 输入：szResult 网站最后更新时间字符串
// 输出：
// 返回值：1 成功 ; 0 失败
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::GetAllStationLastUpdateALLOK(const CStdString &szResult)           
{
	if (szResult.length() ==0)
	{
		CStdString strBack;
		g_log.Trace(LOGL_TOP, LOGT_ERROR,__TFILE__, __LINE__,_T("1.bin文件下载失败，主控退出"));

		strBack.Format(_T("BackResult(;0)1.bin下载失败(;0)(;0)"));
		BYTE * pByData = (BYTE *)strBack.c_str();

		g_ServerSocket->SendData(strBack.size() * 2,E_GET_EXCUTE_TASK_RESULT,(char*)pByData);
		g_ServerSocket->UnInit();

		ExitProcess(0);	//退出主控
		return 1;
	}

	std::map<DWORD, T_ServerNetStationInfo*> *pMapLastUpdateTime = NULL;
	pMapLastUpdateTime = new std::map<DWORD, T_ServerNetStationInfo*>;

	ASSERT(pMapLastUpdateTime != NULL);
	m_dwNetLastupdateIndex = g_GlobalDataBlock->GetGlobalID();
	g_GlobalDataBlock->WriteRealData(m_dwNetLastupdateIndex,
		E_ServerNetStationInfoGroup, pMapLastUpdateTime,
		E_WEBSERVICE_CLASS,eWithoutUse);


    SaveLastUpdateToMap(m_dwNetLastupdateIndex, szResult);

	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
		_T("GetAllStationLastUpdateALLOK,size: %d ") ,pMapLastUpdateTime->size());

	if (0 == pMapLastUpdateTime->size())
	{
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
			_T("GetAllStationLastUpdateALLOK, %s") ,szResult.c_str());
	}

	dwIsAllNetstationLastUpdateGetted = 1;

	if (1 == dwIsAllNetstationLastUpdateGetted 
		&& 0 == m_dwWebServiceAllStationLastUpdateSended)
		//最新的网站列表获取了 &&  最新的网站最后时间也获取了 &&独立三代任务数据的最后时间也获取了 && 还没有向PPD发送数据已经全部准备好的信息
	{
		PostMessage(E_DATAPRE_THREAD,E_PPD_LASTUPDATE_GETTED_MSG,
			m_dwNetLastupdateIndex);  //网站最后更新时间已经获取了,通知准备线程更新

		m_dwWebServiceAllStationLastUpdateSended = 1;
	}

    return BIZMC_OK;
}



////////////////////////////////////////////////////////////////////////////////////// 
//
// 函数功能描述：
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::SaveLastUpdateToMap(DWORD dwIndex,const CStdString &strData)
{
	int ItemCount;
	CStdString szDelimiter;
	CStdString szDelimiter1;
	CStdString szTemp;

	TCHAR TempTch[30];
	std::vector<CStdString> results;
	std::vector<CStdString> resultItem;

	szDelimiter=_T("(;0)");
	szDelimiter1=_T("(;1)");
	StringUtils StringUtil ;

	int nCount = StringUtil.SplitString(strData,szDelimiter,results,true);
	int intindex;

	if (nCount >= 2 ) 
	{
		if (nCount > MAX_NET_COUNT_LASTUPDATED)
			nCount = MAX_NET_COUNT_LASTUPDATED;

		std::map<DWORD, T_ServerNetStationInfo*> *pMapLastUpdateTime = NULL;

		pMapLastUpdateTime=(std::map<DWORD, T_ServerNetStationInfo*>*)g_GlobalDataBlock->ReadRealData(E_ServerNetStationInfoGroup, 
			dwIndex, E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

		ASSERT(pMapLastUpdateTime != NULL);

		for(intindex=0;intindex<nCount-1;intindex++)
		{		
			szTemp=results[intindex+2];
			resultItem.clear() ;
			ItemCount = StringUtil.SplitString(szTemp, szDelimiter1,resultItem,true);

			//增加容错处理
			if (ItemCount<1)
			{
				continue;
			}

			T_ServerNetStationInfo *pNetStationTime = new T_ServerNetStationInfo;
			ASSERT(pNetStationTime != NULL);

			_tcscpy(TempTch , resultItem[0]);
			pNetStationTime->dwNsCode = _ttol(TempTch);

			_tcscpy(TempTch , resultItem[1]);
			pNetStationTime->dwLastUpdateTime = _ttol(TempTch);

			(*pMapLastUpdateTime)[pNetStationTime->dwNsCode] = pNetStationTime;
		}   

	}

    return BIZMC_OK;

}


//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
void CWebService::GetWebServiceData(DWORD dwIndex)
{
	//下载网站最后更新时间文件
	//文件最后更新时间
	//所有网站的最后更新时间也要获取.

	GetAllStationLastUpdateALL();
}


//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：添加对用户许可证号过期的处理,  如果用户过期了,就比较网站最后更新时间是否和用户的到期时间比较,
//												大于过期时间就不下载网站的基本信息
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::GetNetDataFromFTP(const DWORD dwMessageType,const DWORD dwWebID)
{
	//不过期的用户,直接去下载任务
	switch(dwMessageType)
	{
	case E_WEBS_GET_NETSTATION_BASEINFO:      //获取单个网站的基本信息
		{
			SendToFtpGetServerFile(dwWebID,NETSTATION_ALL_LIST_TYPE ,0);  //9文件夹为网站信息的文件夹
		}
		break;
	default:	 //
		{

		}
		break;
	}
	

	return 0;
}


