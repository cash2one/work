//////////////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2008, 商讯网信息有限公司
// 版本：
// 文件说明：
// 生成日期：
// 作者：
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//


#include "stdafx.h"
#include "ProcessData.h"
#include "ServerSocket.h"
#include "Stringutils.h"
#include "unzipwebtaskdata.h"
#include "FileReadAndSave.h"
#include "ZipEncode.h"
#include "HttpUtils.h"
#include "enumdef.h"
#include "CommFunc.h"
#include <fstream>
#include <ShlObj.h>
#include <atlimage.h>
#include "SearchWord.h"
#include "Reg.h"

//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：构造函数，
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
CProcessData::CProcessData()
{
    m_pServerSocket = NULL;	
	bAlreadySend = FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：析构函数
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
CProcessData::~CProcessData()
{
	
}

//////////////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：将向socket发送信息的对象指针传送并保存
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CProcessData::SetPtrOfServer(CServerSocket *pServerSocket)
{
    if (NULL != pServerSocket)
    {
        m_pServerSocket = pServerSocket;
    }
    return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：将Socket传入的数据进行分流处理
// 输入：socket的缓存数据头指针,缓存数据长度
// 输出：
// 返回值：成功分流返回0，否则返回1
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CProcessData::CovertBufData(char *Orgbuf,DWORD dwTotalLen)
{

	CovertBufDataWeb(Orgbuf,dwTotalLen);

	return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：将Socket传入的数据进行分流处理
// 输入：socket的缓存数据头指针,缓存数据长度
// 输出：
// 返回值：成功分流返回0，否则返回1
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CProcessData::CovertBufDataWeb(char *Orgbuf,DWORD dwTotalLen)
{
	CStdString strData;
	CStdString strDataType;

	DWORD dwIndex = 0;
	DWORD dwSize = dwTotalLen - 4;

	/*size_t rsize = mbstowcs(0,Orgbuf+4,0);*/
	int iSize = MultiByteToWideChar(CP_ACP, 0, Orgbuf + 4, -1, NULL, 0);
	TCHAR *dest = NULL;
	
	_NEWA(dest,wchar_t,iSize+1);
	/*mbstowcs(dest,Orgbuf+4,rsize+1);*/
	MultiByteToWideChar(CP_ACP, 0, Orgbuf + 4, -1, dest, iSize+1);
	strData = CStdString(dest);
	_DELETEA(dest);

	/*strData = _T("SoftPost(;0)C3eU9AF7sGaUK05HyYF3hXC7wrUQTO9U61Nl1Cs9bUwZsMvWMICgCRjCVKLyTBkgkJeBiwGmwgDOAolYAE0H0UZA2gRoFwSaMRgDZWDiu/x2+QXWw3iGQLXGQGwIl4eIQEjcZpoymCPp2ZCalRsVvCgaZmoKQyJDMQOExCYGkQEABX8zdyYBAMA=");*/
	g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("数据为：%s"), strData.Buffer());
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT);

	//判断是否重启
	if (bAlreadySend)
	{	
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("MC.exe运行时检测到第二次收到数据，将产生异常！"));
		//::MessageBox(NULL, _T("MC.exe运行时检测到第二次收到数据，将产生异常！"), _T("MC运行"), MB_OK | MB_ICONERROR);
		return 1;
	}
	
	bAlreadySend = TRUE;

	if (dwIndex > 0)
	{
		strDataType = strData.Mid(0,dwIndex);
	}

	if (0 == strDataType.CompareNoCase(_T("MainTask")))  //主任务
	{
		CreateResultPage(strDataType);
		DoFromWebTaskByMainTask(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("Infopost")))  //发布任务
	{
		//DoFromWebTaskByMainTask(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("login")))     //数据格式：login(;0)网站编号(;0)用户名(;0)密码
	{
		DoFromWebTaskByLogin(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("InfoRefr")))  //刷新数据
	{
		DoFromWebTaskByRefreshAndDelete(ETypeRefresh, strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("InfoDele")))   //删除数据
	{
		DoFromWebTaskByRefreshAndDelete(ETypeDelete, strData);
	}
	else if(0 == strDataType.CompareNoCase(_T("Registry")))  //注册
	{
		DoFromWebTaskByRegistry(strData);
	}
	else if(0 == strDataType.CompareNoCase(_T("QuickPhoto")))  //抓取各搜索引擎快照
	{
		strData = strData.Right(strData.length()-dwIndex-4);
		GetSearchPhoto(strData);
	}
	else if(0 == strDataType.CompareNoCase(_T("Dominpost")))  //域名推广
	{
		strData = strData.Right(strData.length()-dwIndex-4);
		DoFromWebTaskByDomainPost(strData);
	}
	else if(0 == strDataType.CompareNoCase(_T("webopost")))  //微博发布
	{
		strData = strData.Right(strData.length()-dwIndex-4);
		DoFromWebTaskByWeiboPost(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("UserInfoMigrate")))  //一键搬家
	{
		CreateResultPage(strDataType);
		DoFromWebTaskByUserInfoMigrate(strData);
	}
// 	else if (0 == strDataType.CompareNoCase(_T("InfoMody")))     //修改资料
// 	{
// 		DoFromWebTaskByInfoModify(strData);
// 	}
	else if (0 == strDataType.CompareNoCase(_T("GetProductList")))	//获取产品列表
	{
		CreateResultPage(strDataType);
		DoFromWebTaskByUserInfoMigrate(strData);
	}

	else if (0 == strDataType.CompareNoCase(_T("ShopTraffic")))  //刷新商铺流量
	{
		DoFromWebTaskByRefreshShopTraffic(strData);
	}

	else if (0 == strDataType.CompareNoCase(_T("KWAuditAnalyze"))) //解析关键词数据
	{
		strData = strData.Right(strData.length() - dwIndex - 4);
		SearchKeyData(strData);
	}

	else if (0 == strDataType.CompareNoCase(_T("TBKeyWord"))) //提取淘宝数据
	{
		strData = strData.Right(strData.length()-dwIndex-4);
		GetTBWordData(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("SoftPost"))) //软文发布
	{
		strData = strData.Right(strData.length() - dwIndex - 4);
		DoFromWebTaskBySoftPost(strData);
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：创建存储快照的文件夹
// 输入：   strData 任务类型
// 输出：
// 返回值：返回0
// 其它说明：  创建快照路径并写入配置表
//
///////////////////////////////////////////////////////////////////////////////

void CProcessData::CreateResultPage(const CStdString &strData)
{
	CStdString strDate,strTime;
	CString strPath;
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep); /*取得当地时间*/

	strDate.Format(_T("%d%02d%02d"), p->tm_year+1900, p->tm_mon + 1, p->tm_mday);
	strTime.Format(_T("%02d%02d"),p->tm_hour,p->tm_min);

	strPath.Format(_T("%s\\image\\result\\%s\\%s")					//保存路径按时间建立文件夹  zhl  140526
		,GetInstallPath(),strDate.c_str()
		,strTime.c_str());

	DWORD dwFlag = SHCreateDirectoryEx(NULL, strPath, NULL);
	if (ERROR_SUCCESS  == dwFlag ||
		ERROR_FILE_EXISTS == dwFlag ||
		ERROR_ALREADY_EXISTS == dwFlag)
	{
		g_Config->WriteString(_T("MC"),_T("RESULTPATH"), _T("savepath"), strPath);    //写配置文件供内核读取  zhl  140526
		return ;
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
			_T("创建快照文件夹失败..."));
		g_Config->WriteString(_T("MC"),_T("RESULTPATH"),_T("savepath"),_T(""));   
	}
}




///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：web 页面压来了任务数据
// 输入：   strData 任务数据
// 输出：
// 返回值：返回0
// 其它说明：  web页面发起了任务,需要主控执行任务
//
///////////////////////////////////////////////////////////////////////////////
DWORD CProcessData::DoFromWebTaskByMainTask(const CStdString &strData)
{
	CStdString strTaskDataZipped;
	CStdString strMainTaskID = "0";

	WEB_TASK_DATA_INFOPOST stWebTaskData;
	DWORD dwIndex = 0;
	DWORD dwServerMintus = 0;
	DWORD dwType = ETTypePost;	

	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);

	if (!pLocalGlobalData)
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 0;
	}

	//先保存web主任务ID到全局数据区
	//先解析出任务ID,再
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT, PREFIX_TASKDATA_LENGTH);
	if (dwIndex > PREFIX_TASKDATA_LENGTH)
	{
		strMainTaskID = strData.Mid(PREFIX_TASKDATA_LENGTH,dwIndex-PREFIX_TASKDATA_LENGTH);
	}
	//保存任务id
	pLocalGlobalData->dwWebMainTaskID = _ttol(strMainTaskID.c_str());	

	CUnZipWebTaskData ZipTaskData;
	strTaskDataZipped = strData.Mid(dwIndex + SPLIT_CHARS_LENGTH);

	//解压缩,分解出 任务详细信息,各个子任务任务的网站ID和网站栏目ID, 产品信息,系统设置信息等
	ZipTaskData.TransInfoPostToTaskData(strTaskDataZipped, stWebTaskData);

	//保存产品信息
	ZipTaskData.SavePostInfoToLocal(stWebTaskData.strPostInfo);

	//保存系统信息到全局数据区
	ZipTaskData.SaveSystemInfoToLocal(stWebTaskData.strSystemInfo);

	//保存发布网站信息
	ZipTaskData.SaveWebInfoToLocal(stWebTaskData.strWebInfo);

	//保存配置信息
	ZipTaskData.SaveConfigInfoToLocal(stWebTaskData.strConfigInfo);

	//保存帐号密码
	ZipTaskData.SaveAccountPasswordToLocal(stWebTaskData.strAccountPassword);

	//保存服务器时间
//	dwServerMintus = _ttol(stWebTaskData.strServerMintus.c_str());
//	WebServerMintustoMcontrol(dwServerMintus);

	int	IsTest;
	IsTest = 0;
	g_Config->ReadInt(_T("MC"), _T("Control"), _T("Test"), IsTest);           //zhl 140411  无需连接到176即可推广

	if (1 != IsTest)
	{
		//为四代内核准备产品的图片
		CStdString strPicPath, strPicName;
		int iPos;
		DWORD dwFlag;
		DWORD dwCount;
		BOOL bNoDownPic = FALSE;
		StringUtils strUtil;
		std::vector<CStdString> vPdtPhotoItem;
		//产品图片(多个产品图片下载）
		dwCount = strUtil.SplitString(pLocalGlobalData->TaskProductInfo.szPhototUrl,_T("(;7)"),vPdtPhotoItem,true);
		for (int i = 1; i<dwCount + 1; i++)
		{
			strPicPath = vPdtPhotoItem[i];
			iPos = strPicPath.ReverseFind(_T("/"));
			strPicName = strPicPath.Mid(iPos + 1);
			dwFlag = GetImagePath(strPicName,stWebTaskData.strLocalProductImage);
			if (dwFlag == 0 && !bNoDownPic)
			{
				//下载产品图片
				TCHAR szPhotoUrl[300] = {0};
				_tcsncpy(szPhotoUrl, vPdtPhotoItem[i].c_str(), MAX_FILE_PATH_LENGTH-1);
				if (!CHttpUtils::GetFile(szPhotoUrl, stWebTaskData.strLocalProductImage.c_str()))
				{
					g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
						_T("下载产品图片超时!"));
					bNoDownPic = TRUE;
				}
					
			}
			ImageConvert(stWebTaskData.strLocalProductImage,1);
			pLocalGlobalData->vProductImageName.push_back(stWebTaskData.strLocalProductImage);
		}
		//营业执照
		strPicPath.Empty();
		strPicName.Empty();
		strPicPath =(CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL;
		iPos = strPicPath.ReverseFind(_T("/"));
		strPicName = strPicPath.Mid(iPos + 1);
		dwFlag = GetImagePath(strPicName,stWebTaskData.strLocalTradeImage);
		if (dwFlag == 0)
		{
			//下载营业执照
			if(!CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL, stWebTaskData.strLocalTradeImage.c_str()))
				g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("下载营业执照图片超时!"));
		}
		if (stWebTaskData.strLocalTradeImage.GetLength() > 1)
		{
			ImageConvert(stWebTaskData.strLocalTradeImage,1);
		}
		//公司图片
		strPicPath =(CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL;
		iPos = strPicPath.ReverseFind(_T("/"));
		strPicName = strPicPath.Mid(iPos + 1);
		dwFlag = GetImagePath(strPicName,stWebTaskData.strLocalCompImage);
		if (dwFlag == 0)
		{
			//下载公司图片
			if(!CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL, stWebTaskData.strLocalCompImage.c_str()))
				g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("下载公司图片超时!"));
		}
		if (stWebTaskData.strLocalCompImage.GetLength() > 1)
		{
			ImageConvert(stWebTaskData.strLocalCompImage,1);
		}
		//组织机构代码图片
		strPicPath = (CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szOrgCodeURL;
		iPos = strPicPath.ReverseFind(_T("/"));
		strPicName = strPicPath.Mid(iPos + 1);
		dwFlag = GetImagePath(strPicName, stWebTaskData.strLocalOrgImage);
		if (dwFlag == 0)
		{
			//下载组织机构代码图片
			if (!CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szOrgCodeURL, stWebTaskData.strLocalOrgImage.c_str()))
				g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("下载组织机构代码图片超时!"));
		}
		if (stWebTaskData.strLocalOrgImage.GetLength() > 1)
		{
			ImageConvert(stWebTaskData.strLocalOrgImage, 1);
		}

		//税务登记证图片
		strPicPath = (CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szTaxRegURL;
		iPos = strPicPath.ReverseFind(_T("/"));
		strPicName = strPicPath.Mid(iPos + 1);
		dwFlag = GetImagePath(strPicName, stWebTaskData.strLocalTaxImage);
		if (dwFlag == 0)
		{
			//下载组织机构代码图片
			if(!CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szTaxRegURL, stWebTaskData.strLocalTaxImage.c_str()))
				g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("下载税务登记图片超时!"));
		}
		if (stWebTaskData.strLocalTaxImage.GetLength() > 1)
		{
			ImageConvert(stWebTaskData.strLocalTaxImage, 1);
		}
	
		//保存产品图片 营业执照 公司照片 组织机构代码证 税务登记证到本地内存
	//	_tcsncpy(pLocalGlobalData->szProductImageName, stWebTaskData.strLocalProductImage.c_str(), MAX_FILE_PATH_LENGTH-1);
		_tcsncpy(pLocalGlobalData->szTradeImageName, stWebTaskData.strLocalTradeImage.c_str(), MAX_FILE_PATH_LENGTH-1);
		_tcsncpy(pLocalGlobalData->szCompImageName, stWebTaskData.strLocalCompImage.c_str(), MAX_FILE_PATH_LENGTH-1);
		_tcsncpy(pLocalGlobalData->szOrgImageName, stWebTaskData.strLocalOrgImage.c_str(), MAX_FILE_PATH_LENGTH - 1);
		_tcsncpy(pLocalGlobalData->szTaxImageName, stWebTaskData.strLocalTaxImage.c_str(), MAX_FILE_PATH_LENGTH - 1);
	}

//	pLocalGlobalData->dwTaskType = ETTypePost;
	_tcsncpy(pLocalGlobalData->szUserID, stWebTaskData.strUserID.c_str(),MAX_ID_STRING_LENGTH);
	
	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);
	return S_OK;
}

//一键搬家
DWORD CProcessData::DoFromWebTaskByUserInfoMigrate( const CStdString &strData )
{
	T_LocalGlobalData *pLocalGlobalData = NULL;
	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pMapStationAndKey = NULL;
	std::map<DWORD,T_TaskNetItem*> *pMapStation = NULL;
	std::map<DWORD,T_SaveToServerAccount*> *pMapAccountPass = NULL;

	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,
		LOCALGLOBALDATAINDEX,E_PREPAREDATA_CLASS,eUseCommReadWithMulti);
	pMapStationAndKey = (std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,
		MAG_STATION_INDEX,E_Save_CLASS,eUseBlockWithOne);
	pMapAccountPass = (std::map<DWORD,T_SaveToServerAccount*> *)g_GlobalDataBlock->ReadRealData(E_MAP_SaveAccountPass,
		MAG_ACCOUNT_INDEX,E_Save_CLASS,eUseBlockWithOne);

	TCHAR cTmp[20] ={0};
	StringUtils StringUtil;
	std::vector<CStdString> result;

	StringUtil.SplitString(strData,_T("(;0)"),result,true);

	if (result.size() >= 6 )
	{
		if (pLocalGlobalData != NULL
			&& pMapStationAndKey != NULL
			&& pMapAccountPass != NULL)
		{
			//保存网站信息
			pMapStation = new std::map<DWORD,T_TaskNetItem*>;
			T_TaskNetItem *pNetItem = new T_TaskNetItem;
			_tcscpy(cTmp , result[1]);
			pNetItem->dwWebId = _ttol(cTmp);       //网站id
			(*pMapStation)[pNetItem->dwWebId] = pNetItem;
			(*pMapStationAndKey)[ETTypeUserInfoMigrate] = pMapStation;

			//保存账号密码信息
			T_SaveToServerAccount *AccountData = new T_SaveToServerAccount;
			AccountData->dwNetstationID = pNetItem->dwWebId;
			_tcsncpy(AccountData->szAccount,result[2], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			_tcsncpy(AccountData->szPassword,result[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			(*pMapAccountPass)[AccountData->dwNetstationID ] = AccountData;

			//保存初始迁移数据
			_tcsncpy(pLocalGlobalData->szMigrationCompanyField,result[4],MAX_PRODUCT_DETAIL_LENGTH-1);
			_tcsncpy(pLocalGlobalData->szMigrationProductField,result[5],MAX_PRODUCT_DETAIL_LENGTH-1);
			_tcsncpy(pLocalGlobalData->szMigrationProductNames,result[6],MAX_NET_COUNT_LASTUPDATED-1);
			if (result.size() > 7)
			{
				memset(cTmp,0,20);
				_tcscpy(cTmp,result[7]);
				pLocalGlobalData->dwMigrationProductCount = _ttol(cTmp);  //迁移产品数量
			}
			pLocalGlobalData->dwTaskType = ETTypeUserInfoMigrate;
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStationAndKey->size(); //数量；
			g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

			return 0;
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
				_T("读取本地数据失败..."));
		}
	}
	else if (result.size() >= 3)
	{
			//保存网站信息
			pMapStation = new std::map<DWORD,T_TaskNetItem*>;
			T_TaskNetItem *pNetItem = new T_TaskNetItem;
			_tcscpy(cTmp , result[1]);
			pNetItem->dwWebId = _ttol(cTmp);       //网站id
			(*pMapStation)[pNetItem->dwWebId] = pNetItem;
			(*pMapStationAndKey)[ETTypeGetPdtList] = pMapStation;

			//保存账号密码信息
			T_SaveToServerAccount *AccountData = new T_SaveToServerAccount;
			AccountData->dwNetstationID = pNetItem->dwWebId;
			_tcsncpy(AccountData->szAccount,result[2], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			_tcsncpy(AccountData->szPassword,result[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			(*pMapAccountPass)[AccountData->dwNetstationID ] = AccountData;

			pLocalGlobalData->dwTaskType = ETTypeGetPdtList;
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStationAndKey->size(); //数量；
			g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

			return 0;
	}	
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
			_T("一键搬家数据格式错误..."));
	}

	return 1;
}


//自动登录
DWORD CProcessData::DoFromWebTaskByLogin(const CStdString &strData)
{
	T_LocalGlobalData *pLocalGlobalData = NULL;
	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pMapStationAndKey = NULL;
	std::map<DWORD,T_SaveToServerAccount*> *pMapAccountPass = NULL;
	std::map<DWORD,T_TaskNetItem*> *pMapStation = NULL;

	pMapAccountPass = 
		(std::map<DWORD,T_SaveToServerAccount*>  *)g_GlobalDataBlock->ReadRealData(
		E_MAP_SaveAccountPass,MAG_ACCOUNT_INDEX,E_Save_CLASS,eUseBlockWithOne);
	pMapStationAndKey = (std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,
		MAG_STATION_INDEX,E_Save_CLASS,eUseBlockWithOne);
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	TCHAR cTemp[20] = {0};
	StringUtils StringUtil;
	std::vector<CStdString> result;
	CUnZipWebTaskData ZipTaskData;

	StringUtil.SplitString(strData,_T("(;0)"),result,true);

	if (result.size() > 4)
	{
		if (pLocalGlobalData != NULL
			&& pMapStationAndKey != NULL
			&& pMapAccountPass != NULL)
		{
			//保存网站信息
			pMapStation = new std::map<DWORD,T_TaskNetItem*>;
			T_TaskNetItem *pNetItem = new T_TaskNetItem;
			_tcscpy(cTemp , result[1]);
			pNetItem->dwWebId = _ttol(cTemp);       //网站id
			(*pMapStation)[pNetItem->dwWebId] = pNetItem;
			 (*pMapStationAndKey)[ETypeAutoLogin] = pMapStation;

			//保存账号密码信息
			T_SaveToServerAccount *AccountData = new T_SaveToServerAccount;
			AccountData->dwNetstationID = pNetItem->dwWebId;
			_tcsncpy(AccountData->szAccount,result[2], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			_tcsncpy(AccountData->szPassword,result[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			//由于一键登录邮箱和嵌套邮箱的通配符不一致
			if (result[2].Find(_T("@")) != -1)
			{
				_tcsncpy(AccountData->szConfirmMail, result[2], MAX_EMAIL_NAME_LENGTH-1);
				_tcsncpy(AccountData->szMailPassword, result[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			}
			(*pMapAccountPass)[AccountData->dwNetstationID ] = AccountData;

			ZipTaskData.SaveConfigInfoToLocal(result[4]);
			pLocalGlobalData->dwTaskType = ETypeAutoLogin;
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStationAndKey->size();
			pLocalGlobalData->iMode = (int)rmBrowser;  //设置浏览模式
			
			g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

			return 0;
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
				_T("读取本地数据失败..."));
		}
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
			_T("一键登录登陆数据格式错误..."));
	}

	return 1;
}


/*
	@brief   取得本地图片路径
	@param  [out] strPicName  URL图片名称
	@param  [out] strLocalPic 本地图片
	@return  
	*/
DWORD CProcessData::GetImagePath(CStdString strPicName, CStdString& strLocalPic)
{
// 	const TCHAR chEnginePrdctName[] = _T("image.jpg");
// 	const TCHAR chTradeName[] = _T("tradcert.jpg");
// 	const TCHAR chCompImageName[] = _T("complogo.jpg");
	if (strPicName.IsEmpty()) return 1;
	//转换成小写
	strPicName.MakeLower();

	std::fstream _file;
	TCHAR strFileName[MAX_PATH] = {0};

	::GetModuleFileName(NULL, strFileName, MAX_PATH);
	PathAppend(strFileName, _T("..\\..\\"));
	PathAppend(strFileName, _T("image\\user\\"));

	strLocalPic.Format(_T("%s%s"),strFileName, strPicName);//将之前推广图片删除
	_file.open(strLocalPic.c_str(),ios::in);    //判断要下载的图片存不存在
	if(!_file)
	{
	  //文件不存在
		return 0;
	}
	//文件存在
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：web任务ID 到主控中全局数据区中去
// 输入：   
// 输出：
// 返回值：
// 其它说明：  
//
///////////////////////////////////////////////////////////////////////////////
DWORD CProcessData::WebServerMintustoMcontrol(const DWORD dwWebServerMintus)
{
	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);

	if (!pLocalGlobalData)
	{
		g_GlobalDataBlock->Reset(E_LocalGlobalData, E_GUIDATAIN_CLASS);	
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 0;
	}


	pLocalGlobalData->dwServerMintus = dwWebServerMintus;

	DWORD dwServerMintus = 	pLocalGlobalData->dwServerMintus;
	DWORD dwServerTickcount = (dwServerMintus % 20 *1440) * 60000;  //天的分钟数 ,再换算为毫秒数
	DWORD dwStartTickcount = ::GetTickCount();

	pLocalGlobalData->dwServerTickcount = dwServerTickcount;	
	pLocalGlobalData->dwStartTickcount = dwStartTickcount;	

	//换算分钟为服务器tickcount

	//取本地当前tickcount

	//dwServerTickcount = 0;           //服务器的tickcount
	//dwStartTickcount = 0;             // 和服务器的tickcount 相对应的本机tickcount
	//dwServerMintus = 0;                //和服务器的tickcount 相对应的服务器分钟数,用来判断用户是否升级已过期
	return 1;
}

//注册任务数据解析
DWORD CProcessData::DoFromWebTaskByRegistry( const CStdString &strData )
{
	CStdString strTaskDataZipped;
	CStdString strMainTaskID = "0";

	WEB_TASK_DATA_REGISTRY stWebRegistryData;
	DWORD dwIndex = 0;
	DWORD dwServerMintus = 0;
	DWORD dwTaskType = ETTypeRegaccount;

	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);

	if (!pLocalGlobalData)
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 0;
	}

	//先保存web主任务ID到全局数据区
	//先解析出任务ID,再
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT,PREFIX_TASKDATA_LENGTH);

	if (dwIndex > PREFIX_TASKDATA_LENGTH)
	{
		strMainTaskID = strData.Mid(PREFIX_TASKDATA_LENGTH,dwIndex-PREFIX_TASKDATA_LENGTH);
	}

	
	pLocalGlobalData->dwWebMainTaskID = _ttol(strMainTaskID.c_str());
	CUnZipWebTaskData ZipTaskData;
	
	strTaskDataZipped = strData.Mid(dwIndex + SPLIT_CHARS_LENGTH);

	//解压缩,分解出 任务详细信息,各个子任务任务的网站ID和网站栏目ID, 产品信息,系统设置信息等
	ZipTaskData.TransRegistryToTaskData(strTaskDataZipped, stWebRegistryData);

	//保存系统信息到全局数据区
	ZipTaskData.SaveSystemInfoToLocal(stWebRegistryData.strSystemInfo);

	//保存发布网站信息
	ZipTaskData.SaveWebInfoToLocal(stWebRegistryData.strWebInfo);

	//保存配置信息
	ZipTaskData.SaveConfigInfoToLocal(stWebRegistryData.strConfigInfo);

	dwServerMintus = _ttol(stWebRegistryData.strServerMintus.c_str());
	WebServerMintustoMcontrol(dwServerMintus);

	//为四代内核准备产品的图片
//	GetImagePath(stWebRegistryData.strLocalProctImage, stWebRegistryData.strTradeImage, stWebRegistryData.strLocalCompImage);

	//下载产品图片与营业执照
	CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL, stWebRegistryData.strTradeImage.c_str());
	CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL, stWebRegistryData.strLocalCompImage.c_str());

	//保存产品图片和  营业执照  公司照片到本地内存
	_tcsncpy(pLocalGlobalData->szTradeImageName, stWebRegistryData.strTradeImage.c_str(), MAX_FILE_PATH_LENGTH-1);
	_tcsncpy(pLocalGlobalData->szCompImageName, stWebRegistryData.strLocalCompImage.c_str(), MAX_FILE_PATH_LENGTH-1);


	pLocalGlobalData->dwTaskType = dwTaskType;



	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);
	
	return S_OK;
}

//刷新与删除数据解析
DWORD CProcessData::DoFromWebTaskByRefreshAndDelete(DWORD dwType, const CStdString &strData )
{
	CStdString strTaskDataZipped;
	CStdString strMainTaskID = "0";

	WEB_TASK_DATA_REFRESHANDDELETE stWebRefreshOrDeleteData;
	DWORD dwIndex = 0;
	DWORD dwServerMintus = 0;
	DWORD dwTaskType = dwType;

	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);

	if (!pLocalGlobalData)
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 1;
	}

	//先保存web主任务ID到全局数据区
	//先解析出任务ID,再
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT,PREFIX_TASKDATA_LENGTH);

	if (dwIndex > PREFIX_TASKDATA_LENGTH)
	{
		strMainTaskID = strData.Mid(PREFIX_TASKDATA_LENGTH,dwIndex-PREFIX_TASKDATA_LENGTH);
	}

	
	pLocalGlobalData->dwWebMainTaskID = _ttol(strMainTaskID.c_str());
	CUnZipWebTaskData ZipTaskData;
	
	strTaskDataZipped = strData.Mid(dwIndex + SPLIT_CHARS_LENGTH);

	//解压缩,分解出 任务详细信息,各个子任务任务的网站ID和网站栏目ID等
	ZipTaskData.TransRefreshAndDeleteToTaskData(strTaskDataZipped, stWebRefreshOrDeleteData);

	//保存发布网站信息
	ZipTaskData.SaveWebInfoToLocal(stWebRefreshOrDeleteData.strWebInfo);

	//保存配置信息
	ZipTaskData.SaveConfigInfoToLocal(stWebRefreshOrDeleteData.strConfigInfo);

	//保存帐号密码
	ZipTaskData.SaveAccountPasswordToLocal(stWebRefreshOrDeleteData.strAccountPassword);

	dwServerMintus = _ttol(stWebRefreshOrDeleteData.strServerMintus.c_str());
	WebServerMintustoMcontrol(dwServerMintus);

	pLocalGlobalData->dwTaskType = dwTaskType;

	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);
	
	return 0;
}

//刷新商铺流量
DWORD CProcessData::DoFromWebTaskByRefreshShopTraffic(const CStdString &strData)
{
	T_LocalGlobalData *pLocalGlobalData = NULL;
	std::map<DWORD, std::map<DWORD, T_TaskNetItem*>*> *pMapStationAndKey = NULL;
	std::map<DWORD, T_TaskNetItem*> *pMapStation = NULL;

	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,
		LOCALGLOBALDATAINDEX, E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
	pMapStationAndKey = (std::map<DWORD, std::map<DWORD, T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,
		MAG_STATION_INDEX, E_Save_CLASS, eUseBlockWithOne);

	TCHAR cTmp[20] = { 0 };
	StringUtils StringUtil;
	std::vector<CStdString> result;

	StringUtil.SplitString(strData, _T("(;0)"), result, true);

	if (result.size() >= 4)
	{
		if (pLocalGlobalData != NULL
			&& pMapStationAndKey != NULL)
		{
			//保存网站信息
			pMapStation = new std::map<DWORD, T_TaskNetItem*>;
			T_TaskNetItem *pNetItem = new T_TaskNetItem;
			_tcscpy(cTmp, result[1]);
			pNetItem->dwWebId = _ttol(cTmp);       //网站id
			(*pMapStation)[pNetItem->dwWebId] = pNetItem;
			(*pMapStationAndKey)[ETTypeShopTraffic] = pMapStation;

			//保存初始迁移数据
			_tcsncpy(pLocalGlobalData->szShopTrafficUrl, result[2], MAX_URL_LENGTH - 1);
			pLocalGlobalData->dwPdtClickCnt = _ttoi(result[3]);
			pLocalGlobalData->dwOtherClickCnt = _ttoi(result[4]);

			pLocalGlobalData->dwTaskType = ETTypeShopTraffic;
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStationAndKey->size(); //数量；
			g_MC->GetPtrOfThreadManage()->PostMessage(0, E_TASKMGR_THREAD, eTask_PushGUITask, MAG_STATION_INDEX);

			return 0;
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, \
				_T("读取本地数据失败..."));
		}
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, \
			_T("刷新商铺流量数据格式错误..."));
	}

	return 1;
}


typedef BOOL  (*GetQuickPhoto)(const CStdString &strWord, IServerSocket *pSocket);
//抓取各搜索引擎快照
void CProcessData::GetSearchPhoto( const CStdString &strData )
{
	//如果当前定义的非804，则不需要抓取排名
	CReg reg;	
	CString strInfo;
	TCHAR* pContent = (TCHAR*)reg.ReadValueOfKey(HKEY_CURRENT_USER, _T("Control Panel\\International"), _T("Locale"));
	strInfo = pContent;
	if (strInfo != _T("00000804"))
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("检测到系统编码非804，不执行抓取排名, 系统Locale：%s"), pContent);
		ExitProcess(0);
		return;
	}

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathAppend(szPath, _T("..\\"));
	PathAppend(szPath, _T("UpdateRank.dll"));

	HMODULE hInst =  LoadLibrary(szPath);

	GetQuickPhoto getPhoto = NULL;
	if (hInst != NULL)
	{
		getPhoto = (GetQuickPhoto)GetProcAddress(hInst, "GetQuickPhoto");
		if (getPhoto != NULL)
		{
			getPhoto(strData, g_server);
		}
		else
		{
			g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
				_T("取函数GetQuickPhoto失败, 错误码：%d"), GetLastError());
		}
	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("加载GetPagePhoto.dll失败, 错误码：%d"), GetLastError());
	}

}

//域名推广
DWORD CProcessData::DoFromWebTaskByDomainPost( const CStdString &strData )
{
	_WEB_TASK_DATA_DOMAINPOST stWebDomainData;
	DWORD dwTaskType = ETypeDomainpost;

	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);

	if (!pLocalGlobalData)
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 1;
	}

	CUnZipWebTaskData ZipTaskData;

	//解压缩,分解出 任务详细信息,各个子任务任务的网站ID和网站栏目ID等
	ZipTaskData.TransDomainPostToTaskData(strData, stWebDomainData);

	//保存发布网站信息
	ZipTaskData.SaveWebInfoToLocal(stWebDomainData.strWebInfo);

	//保存域名推广信息
	ZipTaskData.SaveDomainInfoToLocal(stWebDomainData.strTaskInfo);

	//保存配置信息
	ZipTaskData.SaveConfigInfoToLocal(stWebDomainData.strConfigInfo);

	pLocalGlobalData->dwTaskType = dwTaskType;

	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);
	
	return 0;
}

//微博推广
DWORD CProcessData::DoFromWebTaskByWeiboPost( const CStdString &strData )
{
	_WEB_TASK_DATA_WEBOPOST stWebWeoData;
	DWORD dwTaskType = ETypeWebopost;

	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);

	if (!pLocalGlobalData)
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("获取本地全局数据 不正确!"));
		return 1;
	}

	CUnZipWebTaskData ZipTaskData;

	//解压缩,分解出 任务详细信息,各个子任务任务的网站ID和网站栏目ID等
	ZipTaskData.TransWeboPostToTaskData(strData, stWebWeoData);

	//保存发布网站信息
	ZipTaskData.SaveWebInfoToLocal(stWebWeoData.strWebInfo);

	//保存微博要发布的信息
	ZipTaskData.SaveWeboInfoToLocal(stWebWeoData.strTaskInfo);

	//保存账号密码信息
	ZipTaskData.SaveAccountPasswordToLocal(stWebWeoData.strAccountPassword);

	//保存配置信息
	ZipTaskData.SaveConfigInfoToLocal(stWebWeoData.strConfigInfo);

	pLocalGlobalData->dwTaskType = dwTaskType;

	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

	return 0;
}

/*
@brief  修改资料功能
@param  strData  上层传过来的加密数据
@Return 
*/

DWORD CProcessData::DoFromWebTaskByInfoModify( const CStdString &strData )
{
	CStdString strTaskDataZipped;
	CStdString strMainTaskID = "0";

	WEB_TASK_DATA_INFOMODIFY strInfoModifyData;
	DWORD dwIndex = 0;

	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,
		LOCALGLOBALDATAINDEX,E_GUIDATAIN_CLASS,eUseBlockWithOne);

	if (!pLocalGlobalData)
	{
		g_log.Trace(LOGL_HIG,LOGT_ERROR,__TFILE__,__LINE__,_T("获取本地全局数据不正确！"));
		return 0;
	}

	//先保存任务ID
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT,PREFIX_TASKDATA_LENGTH);

	if (dwIndex > PREFIX_TASKDATA_LENGTH)
	{
		strMainTaskID = strData.Mid(PREFIX_TASKDATA_LENGTH,dwIndex-PREFIX_TASKDATA_LENGTH);
	}
	pLocalGlobalData->dwWebMainTaskID = _ttol(strMainTaskID);

	CUnZipWebTaskData ZipTaskData;

	strTaskDataZipped = strData.Mid(dwIndex + SPLIT_CHARS_LENGTH);

	//解压缩任务
	ZipTaskData.TransInfoModifyToTaskData(strData,strInfoModifyData);

	//保存全部网站信息
	ZipTaskData.SaveWebInfoToLocalOfOneTask(strInfoModifyData.strWebInfo);

	//保存系统设置信息
	ZipTaskData.SaveSystemInfoToLocal(strInfoModifyData.strSystemInfo);

	//保存配置信息
	ZipTaskData.SaveConfigInfoToLocal(strInfoModifyData.strConfigInfo);

	//保存帐号密码信息
	ZipTaskData.SaveAccountPasswordToLocal(strInfoModifyData.strAccountPassword);

	//下载图片
	CStdString strPicPath, strPicName;
	int iPos;
	DWORD dwFlag;
	DWORD dwCount;
	//营业执照
	strPicPath.Empty();
	strPicName.Empty();
	strPicPath =(CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL;
	iPos = strPicPath.ReverseFind(_T("/"));
	strPicName = strPicPath.Mid(iPos + 1);
	dwFlag = GetImagePath(strPicName,strInfoModifyData.strTradeImage);
	if (dwFlag == 0)
	{
		//下载营业执照
		CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL, strInfoModifyData.strTradeImage.c_str());
	}
	//公司图片
	strPicPath =(CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL;
	iPos = strPicPath.ReverseFind(_T("/"));
	strPicName = strPicPath.Mid(iPos + 1);
	dwFlag = GetImagePath(strPicName,strInfoModifyData.strLocalCompImage);
	if (dwFlag == 0)
	{
		//下载公司图片
		CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL, strInfoModifyData.strLocalCompImage.c_str());
	}

	pLocalGlobalData->dwTaskType = ETTypeModify;

	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

	return S_OK;

}

/*
@brief 图片转换
@param strImagePath; 要转换的图片地址；
@param dwType;    转换类型 (1.jpg,2.png,3.bmp,4.gif)
*/
BOOL CProcessData::ImageConvert( CStdString &strSrcPath, DWORD dwType )
{
	CStdString strTmp = _T("");
	CStdString strFileName = _T("");
	CStdString strExtension = _T("");
	CImage img;
	HRESULT hr;

	//jpg自转直接跳过
	if ((strSrcPath.Find(_T(".jpg")) != -1 && dwType == 1)
		||(!PathFileExists(strSrcPath)))
	{
		return TRUE;
	}

	strTmp = strSrcPath;
	int iPotPos  = strSrcPath.ReverseFind(_T("."));
	strFileName = strSrcPath.Left(iPotPos);
	img.Load(strSrcPath);
	//区分转换格式
	switch (dwType)
	{
	case 1:
		strExtension = _T(".jpg");
		break;
	case 2:
		strExtension = _T(".png");
		break;
	case 3:
		strExtension = _T(".bmp");
		break;
	case 4:
		strExtension = _T(".gif");
		break;
	default:
		break;

	}

	strSrcPath = strFileName + strExtension;

	hr = img.Save(strSrcPath);
	if (FAILED(hr))
	{
		strSrcPath = strTmp; //用原来的图片
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("转换图片类型(%d)失败:%s"),dwType,strSrcPath.c_str());
		return FALSE;
	}

	return TRUE;
}

//查找关键词数据
void CProcessData::SearchKeyData(const CStdString &strData)
{
	CSearchWord searchWord;

	searchWord.ProcessKeyWord(strData);
}

//提取淘宝词
void CProcessData::GetTBWordData(const CStdString &strData)
{
	CSearchWord searchWord;

	searchWord.ProcessTBWord(strData);
}

//软文发布
DWORD CProcessData::DoFromWebTaskBySoftPost(const CStdString &strData)
{	
	T_LocalGlobalData *pLocalGlobalData = NULL;
	std::map<DWORD, std::map<DWORD, T_TaskNetItem*>*> *pMapStationAndKey = NULL;
	std::map<DWORD, T_TaskNetItem*> *pMapStation = NULL;
	std::map<DWORD, T_SaveToServerAccount*> *pMapAccountPass = NULL;

	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,
		LOCALGLOBALDATAINDEX, E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
	pMapStationAndKey = (std::map<DWORD, std::map<DWORD, T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,
		MAG_STATION_INDEX, E_Save_CLASS, eUseBlockWithOne);
	pMapAccountPass = (std::map<DWORD, T_SaveToServerAccount*> *)g_GlobalDataBlock->ReadRealData(E_MAP_SaveAccountPass,
		MAG_ACCOUNT_INDEX, E_Save_CLASS, eUseBlockWithOne);
	
	CStdString strDeData;
	CFileReadAndSave::UnZipDecodeStdString(strData, strDeData);

	TCHAR cTmp[20] = { 0 };
	StringUtils StringUtil;
	std::vector<CStdString> result;
	std::vector<CStdString> resultItem;

	StringUtil.SplitString(strDeData, _T("(;0)"), result, true);

	if (result.size() >= 5)
	{
		_tcsncpy(pLocalGlobalData->aSystemInfo.systemInfo.szCompanName, result[0], MAX_MID_LENGTH - 1);
		_tcsncpy(pLocalGlobalData->TaskDetailInfo.szPagePhotoFlag, result[1], MAX_NAME_LENGTH - 1);
		_tcsncpy(pLocalGlobalData->TaskDetailInfo.szSubject, result[2], MAX_NAME_LENGTH - 1);
		_tcsncpy(pLocalGlobalData->TaskDetailInfo.szContext, result[3], MAX_CONTEXT_LENGTH - 1);
		_tcscpy(cTmp, result[4]);
		pLocalGlobalData->dwOCRType = _ttol(cTmp);

		std::vector<CStdString> resultAccount;
		int dwCnt = StringUtil.SplitString(result[5], _T("(;8)"), resultAccount, true);
		pMapStation = new std::map<DWORD, T_TaskNetItem*>;
		for (int i = 1; i < dwCnt + 1; i++)
		{
			if (pMapStation != NULL)
			{				
				resultItem.clear();
				int dwCnt1 = StringUtil.SplitString(resultAccount[i], _T("(;2)"), resultItem, true);
				if (dwCnt1 >= 2)
				{
					_tcscpy(cTmp, resultItem[1]);
					T_TaskNetItem *pNetItem = new T_TaskNetItem;
					//网站账号信息
					T_SaveToServerAccount *AccountData = new T_SaveToServerAccount;
					AccountData->dwNetstationID = _ttol(cTmp);
					_tcsncpy(AccountData->szAccount, resultItem[3], MAX_ACCOUNT_PASSWORD_LENGTH - 1);
					_tcsncpy(AccountData->szPassword, resultItem[4], MAX_ACCOUNT_PASSWORD_LENGTH - 1);
					(*pMapAccountPass)[AccountData->dwNetstationID] = AccountData;

					//网站ID
					pNetItem->dwWebId = _ttol(cTmp);
					//网站名称
					_tcsncpy(pNetItem->szWebName, resultItem[2].c_str(), MAX_MID_LENGTH - 1);
					
					(*pMapStation)[pNetItem->dwWebId] = pNetItem;					
				}
				else
				{
					g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
						_T("读取网站数据失败"));
				}
			}
		}
		(*pMapStationAndKey)[ETTypeSoftPost] = pMapStation;
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("总任务数：%d！"),dwCnt);
		pLocalGlobalData->dwTaskType = ETTypeSoftPost;		
		g_MC->GetPtrOfThreadManage()->PostMessage(0, E_TASKMGR_THREAD, eTask_PushGUITask, MAG_STATION_INDEX);

	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("数据格式错误！"));
	}

	return S_OK;
}

