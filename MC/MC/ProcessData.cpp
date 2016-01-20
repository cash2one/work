//////////////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2008, ��Ѷ����Ϣ���޹�˾
// �汾��
// �ļ�˵����
// �������ڣ�
// ���ߣ�
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
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
// �����������������캯����
// ���룺
// �����
// ����ֵ��
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
CProcessData::CProcessData()
{
    m_pServerSocket = NULL;	
	bAlreadySend = FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// ����������������������
// ���룺
// �����
// ����ֵ��
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
CProcessData::~CProcessData()
{
	
}

//////////////////////////////////////////////////////////////////////////////////////
//
// ������������������socket������Ϣ�Ķ���ָ�봫�Ͳ�����
// ���룺
// �����
// ����ֵ��
// ����˵����
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
// ����������������Socket��������ݽ��з�������
// ���룺socket�Ļ�������ͷָ��,�������ݳ���
// �����
// ����ֵ���ɹ���������0�����򷵻�1
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
DWORD CProcessData::CovertBufData(char *Orgbuf,DWORD dwTotalLen)
{

	CovertBufDataWeb(Orgbuf,dwTotalLen);

	return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
// ����������������Socket��������ݽ��з�������
// ���룺socket�Ļ�������ͷָ��,�������ݳ���
// �����
// ����ֵ���ɹ���������0�����򷵻�1
// ����˵����
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

//	strData = _T("KWAuditAnalyze(;0)�������(;1)ˮ��ͷ(;1)�������(;1)");
	g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("����Ϊ��%s"), strData.Buffer());
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT);

	//�ж��Ƿ�����
	if (bAlreadySend)
	{	
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("MC.exe����ʱ��⵽�ڶ����յ����ݣ��������쳣��"));
		//::MessageBox(NULL, _T("MC.exe����ʱ��⵽�ڶ����յ����ݣ��������쳣��"), _T("MC����"), MB_OK | MB_ICONERROR);
		return 1;
	}
	
	bAlreadySend = TRUE;

	if (dwIndex > 0)
	{
		strDataType = strData.Mid(0,dwIndex);
	}

	if (0 == strDataType.CompareNoCase(_T("MainTask")))  //������
	{
		CreateResultPage(strDataType);
		DoFromWebTaskByMainTask(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("Infopost")))  //��������
	{
		//DoFromWebTaskByMainTask(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("login")))     //���ݸ�ʽ��login(;0)��վ���(;0)�û���(;0)����
	{
		DoFromWebTaskByLogin(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("InfoRefr")))  //ˢ������
	{
		DoFromWebTaskByRefreshAndDelete(ETypeRefresh, strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("InfoDele")))   //ɾ������
	{
		DoFromWebTaskByRefreshAndDelete(ETypeDelete, strData);
	}
	else if(0 == strDataType.CompareNoCase(_T("Registry")))  //ע��
	{
		DoFromWebTaskByRegistry(strData);
	}
	else if(0 == strDataType.CompareNoCase(_T("QuickPhoto")))  //ץȡ�������������
	{
		strData = strData.Right(strData.length()-dwIndex-4);
		GetSearchPhoto(strData);
	}
	else if(0 == strDataType.CompareNoCase(_T("Dominpost")))  //�����ƹ�
	{
		strData = strData.Right(strData.length()-dwIndex-4);
		DoFromWebTaskByDomainPost(strData);
	}
	else if(0 == strDataType.CompareNoCase(_T("webopost")))  //΢������
	{
		strData = strData.Right(strData.length()-dwIndex-4);
		DoFromWebTaskByWeiboPost(strData);
	}
	else if (0 == strDataType.CompareNoCase(_T("UserInfoMigrate")))  //һ�����
	{
		CreateResultPage(strDataType);
		DoFromWebTaskByUserInfoMigrate(strData);
	}
// 	else if (0 == strDataType.CompareNoCase(_T("InfoMody")))     //�޸�����
// 	{
// 		DoFromWebTaskByInfoModify(strData);
// 	}
	else if (0 == strDataType.CompareNoCase(_T("GetProductList")))	//��ȡ��Ʒ�б�
	{
		CreateResultPage(strDataType);
		DoFromWebTaskByUserInfoMigrate(strData);
	}

	else if (0 == strDataType.CompareNoCase(_T("ShopTraffic")))  //ˢ����������
	{
		DoFromWebTaskByRefreshShopTraffic(strData);
	}

	else if (0 == strDataType.CompareNoCase(_T("KWAuditAnalyze"))) //�����ؼ�������
	{
		strData = strData.Right(strData.length() - dwIndex - 4);
		SearchKeyData(strData);
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
// �������������������洢���յ��ļ���
// ���룺   strData ��������
// �����
// ����ֵ������0
// ����˵����  ��������·����д�����ñ�
//
///////////////////////////////////////////////////////////////////////////////

void CProcessData::CreateResultPage(const CStdString &strData)
{
	CStdString strDate,strTime,strPath;
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep); /*ȡ�õ���ʱ��*/

	strDate.Format(_T("%d%02d%02d"), p->tm_year+1900, p->tm_mon + 1, p->tm_mday);
	strTime.Format(_T("%02d%02d"),p->tm_hour,p->tm_min);

	strPath.Format(_T("%s\\image\\result\\%s\\%s")					//����·����ʱ�佨���ļ���  zhl  140526
		,GetInstallPath(),strDate.c_str()
		,strTime.c_str());

	DWORD dwFlag = SHCreateDirectoryEx(NULL, strPath, NULL);
	if (ERROR_SUCCESS  == dwFlag ||
		ERROR_FILE_EXISTS == dwFlag ||
		ERROR_ALREADY_EXISTS == dwFlag)
	{
		g_Config->WriteString(_T("RESULTPATH"),_T("savepath"),strPath);    //д�����ļ����ں˶�ȡ  zhl  140526
		return ;
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
			_T("���������ļ���ʧ��..."));
		g_Config->WriteString(_T("RESULTPATH"),_T("savepath"),_T(""));   
	}
}




///////////////////////////////////////////////////////////////////////////////
//
// ��������������web ҳ��ѹ������������
// ���룺   strData ��������
// �����
// ����ֵ������0
// ����˵����  webҳ�淢��������,��Ҫ����ִ������
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
			_T("��ȡ����ȫ������ ����ȷ!"));
		return 0;
	}

	//�ȱ���web������ID��ȫ��������
	//�Ƚ���������ID,��
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT, PREFIX_TASKDATA_LENGTH);
	if (dwIndex > PREFIX_TASKDATA_LENGTH)
	{
		strMainTaskID = strData.Mid(PREFIX_TASKDATA_LENGTH,dwIndex-PREFIX_TASKDATA_LENGTH);
	}
	//��������id
	pLocalGlobalData->dwWebMainTaskID = _ttol(strMainTaskID.c_str());	

	CUnZipWebTaskData ZipTaskData;
	strTaskDataZipped = strData.Mid(dwIndex + SPLIT_CHARS_LENGTH);

	//��ѹ��,�ֽ�� ������ϸ��Ϣ,�����������������վID����վ��ĿID, ��Ʒ��Ϣ,ϵͳ������Ϣ��
	ZipTaskData.TransInfoPostToTaskData(strTaskDataZipped, stWebTaskData);

	//�����Ʒ��Ϣ
	ZipTaskData.SavePostInfoToLocal(stWebTaskData.strPostInfo);

	//����ϵͳ��Ϣ��ȫ��������
	ZipTaskData.SaveSystemInfoToLocal(stWebTaskData.strSystemInfo);

	//���淢����վ��Ϣ
	ZipTaskData.SaveWebInfoToLocal(stWebTaskData.strWebInfo);

	//����������Ϣ
	ZipTaskData.SaveConfigInfoToLocal(stWebTaskData.strConfigInfo);

	//�����ʺ�����
	ZipTaskData.SaveAccountPasswordToLocal(stWebTaskData.strAccountPassword);

	//���������ʱ��
//	dwServerMintus = _ttol(stWebTaskData.strServerMintus.c_str());
//	WebServerMintustoMcontrol(dwServerMintus);

	int	IsTest;
	IsTest = 0;
	g_Config->ReadInteger(_T("Control"),_T("Test"),IsTest);           //zhl 140411  �������ӵ�176�����ƹ�

	if (1 != IsTest)
	{
		//Ϊ�Ĵ��ں�׼����Ʒ��ͼƬ
		CStdString strPicPath, strPicName;
		int iPos;
		DWORD dwFlag;
		DWORD dwCount;
		StringUtils strUtil;
		std::vector<CStdString> vPdtPhotoItem;
		//��ƷͼƬ(�����ƷͼƬ���أ�
		dwCount = strUtil.SplitString(pLocalGlobalData->TaskProductInfo.szPhototUrl,_T("(;7)"),vPdtPhotoItem,true);
		for (int i = 1; i<dwCount + 1; i++)
		{
			strPicPath = vPdtPhotoItem[i];
			iPos = strPicPath.ReverseFind(_T("/"));
			strPicName = strPicPath.Mid(iPos + 1);
			dwFlag = GetImagePath(strPicName,stWebTaskData.strLocalProductImage);
			if (dwFlag == 0)
			{
				//���ز�ƷͼƬ
				TCHAR szPhotoUrl[300] = {0};
				_tcsncpy(szPhotoUrl, vPdtPhotoItem[i].c_str(), MAX_FILE_PATH_LENGTH-1);
				CHttpUtils::GetFile(szPhotoUrl, stWebTaskData.strLocalProductImage.c_str());
			}
			ImageConvert(stWebTaskData.strLocalProductImage,1);
			pLocalGlobalData->vProductImageName.push_back(stWebTaskData.strLocalProductImage);
		}
		//Ӫҵִ��
		strPicPath.Empty();
		strPicName.Empty();
		strPicPath =(CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL;
		iPos = strPicPath.ReverseFind(_T("/"));
		strPicName = strPicPath.Mid(iPos + 1);
		dwFlag = GetImagePath(strPicName,stWebTaskData.strLocalTradeImage);
		if (dwFlag == 0)
		{
			//����Ӫҵִ��
			CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL, stWebTaskData.strLocalTradeImage.c_str());
		}
		if (stWebTaskData.strLocalTradeImage.GetLength() > 1)
		{
			ImageConvert(stWebTaskData.strLocalTradeImage,1);
		}
		//��˾ͼƬ
		strPicPath =(CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL;
		iPos = strPicPath.ReverseFind(_T("/"));
		strPicName = strPicPath.Mid(iPos + 1);
		dwFlag = GetImagePath(strPicName,stWebTaskData.strLocalCompImage);
		if (dwFlag == 0)
		{
			//���ع�˾ͼƬ
			CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL, stWebTaskData.strLocalCompImage.c_str());
		}
		if (stWebTaskData.strLocalCompImage.GetLength() > 1)
		{
			ImageConvert(stWebTaskData.strLocalCompImage,1);
		}
		//��֯��������ͼƬ
		strPicPath = (CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szOrgCodeURL;
		iPos = strPicPath.ReverseFind(_T("/"));
		strPicName = strPicPath.Mid(iPos + 1);
		dwFlag = GetImagePath(strPicName, stWebTaskData.strLocalOrgImage);
		if (dwFlag == 0)
		{
			//������֯��������ͼƬ
			CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szOrgCodeURL, stWebTaskData.strLocalOrgImage.c_str());
		}
		if (stWebTaskData.strLocalOrgImage.GetLength() > 1)
		{
			ImageConvert(stWebTaskData.strLocalOrgImage, 1);
		}

		//˰��Ǽ�֤ͼƬ
		strPicPath = (CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szTaxRegURL;
		iPos = strPicPath.ReverseFind(_T("/"));
		strPicName = strPicPath.Mid(iPos + 1);
		dwFlag = GetImagePath(strPicName, stWebTaskData.strLocalTaxImage);
		if (dwFlag == 0)
		{
			//������֯��������ͼƬ
			CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szTaxRegURL, stWebTaskData.strLocalTaxImage.c_str());
		}
		if (stWebTaskData.strLocalTaxImage.GetLength() > 1)
		{
			ImageConvert(stWebTaskData.strLocalTaxImage, 1);
		}
	
		//�����ƷͼƬ Ӫҵִ�� ��˾��Ƭ ��֯��������֤ ˰��Ǽ�֤�������ڴ�
	//	_tcsncpy(pLocalGlobalData->szProductImageName, stWebTaskData.strLocalProductImage.c_str(), MAX_FILE_PATH_LENGTH-1);
		_tcsncpy(pLocalGlobalData->szTradeImageName, stWebTaskData.strLocalTradeImage.c_str(), MAX_FILE_PATH_LENGTH-1);
		_tcsncpy(pLocalGlobalData->szCompImageName, stWebTaskData.strLocalCompImage.c_str(), MAX_FILE_PATH_LENGTH-1);
		_tcsncpy(pLocalGlobalData->szOrgImageName, stWebTaskData.strLocalOrgImage.c_str(), MAX_FILE_PATH_LENGTH - 1);
		_tcsncpy(pLocalGlobalData->szTaxImageName, stWebTaskData.strLocalTaxImage.c_str(), MAX_FILE_PATH_LENGTH - 1);
	}

//	pLocalGlobalData->dwTaskType = ETTypePost;
	pLocalGlobalData->dwPostType = stWebTaskData.dwPostType;
	
	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);
	return S_OK;
}

//һ�����
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
			//������վ��Ϣ
			pMapStation = new std::map<DWORD,T_TaskNetItem*>;
			T_TaskNetItem *pNetItem = new T_TaskNetItem;
			_tcscpy(cTmp , result[1]);
			pNetItem->dwWebId = _ttol(cTmp);       //��վid
			(*pMapStation)[pNetItem->dwWebId] = pNetItem;
			(*pMapStationAndKey)[ETTypeUserInfoMigrate] = pMapStation;

			//�����˺�������Ϣ
			T_SaveToServerAccount *AccountData = new T_SaveToServerAccount;
			AccountData->dwNetstationID = pNetItem->dwWebId;
			_tcsncpy(AccountData->szAccount,result[2], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			_tcsncpy(AccountData->szPassword,result[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			(*pMapAccountPass)[AccountData->dwNetstationID ] = AccountData;

			//�����ʼǨ������
			_tcsncpy(pLocalGlobalData->szMigrationCompanyField,result[4],MAX_PRODUCT_DETAIL_LENGTH-1);
			_tcsncpy(pLocalGlobalData->szMigrationProductField,result[5],MAX_PRODUCT_DETAIL_LENGTH-1);
			_tcsncpy(pLocalGlobalData->szMigrationProductNames,result[6],MAX_NET_COUNT_LASTUPDATED-1);
			if (result.size() > 7)
			{
				memset(cTmp,0,20);
				_tcscpy(cTmp,result[7]);
				pLocalGlobalData->dwMigrationProductCount = _ttol(cTmp);  //Ǩ�Ʋ�Ʒ����
			}
			pLocalGlobalData->dwTaskType = ETTypeUserInfoMigrate;
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStationAndKey->size(); //������
			g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

			return 0;
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
				_T("��ȡ��������ʧ��..."));
		}
	}
	else if (result.size() >= 3)
	{
			//������վ��Ϣ
			pMapStation = new std::map<DWORD,T_TaskNetItem*>;
			T_TaskNetItem *pNetItem = new T_TaskNetItem;
			_tcscpy(cTmp , result[1]);
			pNetItem->dwWebId = _ttol(cTmp);       //��վid
			(*pMapStation)[pNetItem->dwWebId] = pNetItem;
			(*pMapStationAndKey)[ETTypeGetPdtList] = pMapStation;

			//�����˺�������Ϣ
			T_SaveToServerAccount *AccountData = new T_SaveToServerAccount;
			AccountData->dwNetstationID = pNetItem->dwWebId;
			_tcsncpy(AccountData->szAccount,result[2], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			_tcsncpy(AccountData->szPassword,result[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			(*pMapAccountPass)[AccountData->dwNetstationID ] = AccountData;

			pLocalGlobalData->dwTaskType = ETTypeGetPdtList;
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStationAndKey->size(); //������
			g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

			return 0;
	}	
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
			_T("һ��������ݸ�ʽ����..."));
	}

	return 1;
}


//�Զ���¼
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
			//������վ��Ϣ
			pMapStation = new std::map<DWORD,T_TaskNetItem*>;
			T_TaskNetItem *pNetItem = new T_TaskNetItem;
			_tcscpy(cTemp , result[1]);
			pNetItem->dwWebId = _ttol(cTemp);       //��վid
			(*pMapStation)[pNetItem->dwWebId] = pNetItem;
			 (*pMapStationAndKey)[ETypeAutoLogin] = pMapStation;

			//�����˺�������Ϣ
			T_SaveToServerAccount *AccountData = new T_SaveToServerAccount;
			AccountData->dwNetstationID = pNetItem->dwWebId;
			_tcsncpy(AccountData->szAccount,result[2], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			_tcsncpy(AccountData->szPassword,result[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			//����һ����¼�����Ƕ�������ͨ�����һ��
			if (result[2].Find(_T("@")) != -1)
			{
				_tcsncpy(AccountData->szConfirmMail, result[2], MAX_EMAIL_NAME_LENGTH-1);
				_tcsncpy(AccountData->szMailPassword, result[3], MAX_ACCOUNT_PASSWORD_LENGTH-1);
			}
			(*pMapAccountPass)[AccountData->dwNetstationID ] = AccountData;

			ZipTaskData.SaveConfigInfoToLocal(result[4]);
			pLocalGlobalData->dwTaskType = ETypeAutoLogin;
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStationAndKey->size();
			pLocalGlobalData->iMode = (int)rmBrowser;  //�������ģʽ
			
			g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

			return 0;
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
				_T("��ȡ��������ʧ��..."));
		}
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__,\
			_T("һ����¼��½���ݸ�ʽ����..."));
	}

	return 1;
}


/*
	@brief   ȡ�ñ���ͼƬ·��
	@param  [out] strPicName  URLͼƬ����
	@param  [out] strLocalPic ����ͼƬ
	@return  
	*/
DWORD CProcessData::GetImagePath(CStdString strPicName, CStdString& strLocalPic)
{
// 	const TCHAR chEnginePrdctName[] = _T("image.jpg");
// 	const TCHAR chTradeName[] = _T("tradcert.jpg");
// 	const TCHAR chCompImageName[] = _T("complogo.jpg");
	if (strPicName.IsEmpty()) return 1;
	//ת����Сд
	strPicName.MakeLower();

	std::fstream _file;
	TCHAR strFileName[MAX_PATH] = {0};

	::GetModuleFileName(NULL, strFileName, MAX_PATH);
	PathAppend(strFileName, _T("..\\..\\"));
	PathAppend(strFileName, _T("image\\user\\"));

	strLocalPic.Format(_T("%s%s"),strFileName, strPicName);//��֮ǰ�ƹ�ͼƬɾ��
	_file.open(strLocalPic.c_str(),ios::in);    //�ж�Ҫ���ص�ͼƬ�治����
	if(!_file)
	{
	  //�ļ�������
		return 0;
	}
	//�ļ�����
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
// ��������������web����ID ��������ȫ����������ȥ
// ���룺   
// �����
// ����ֵ��
// ����˵����  
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
			_T("��ȡ����ȫ������ ����ȷ!"));
		return 0;
	}


	pLocalGlobalData->dwServerMintus = dwWebServerMintus;

	DWORD dwServerMintus = 	pLocalGlobalData->dwServerMintus;
	DWORD dwServerTickcount = (dwServerMintus % 20 *1440) * 60000;  //��ķ����� ,�ٻ���Ϊ������
	DWORD dwStartTickcount = ::GetTickCount();

	pLocalGlobalData->dwServerTickcount = dwServerTickcount;	
	pLocalGlobalData->dwStartTickcount = dwStartTickcount;	

	//�������Ϊ������tickcount

	//ȡ���ص�ǰtickcount

	//dwServerTickcount = 0;           //��������tickcount
	//dwStartTickcount = 0;             // �ͷ�������tickcount ���Ӧ�ı���tickcount
	//dwServerMintus = 0;                //�ͷ�������tickcount ���Ӧ�ķ�����������,�����ж��û��Ƿ������ѹ���
	return 1;
}

//ע���������ݽ���
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
			_T("��ȡ����ȫ������ ����ȷ!"));
		return 0;
	}

	//�ȱ���web������ID��ȫ��������
	//�Ƚ���������ID,��
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT,PREFIX_TASKDATA_LENGTH);

	if (dwIndex > PREFIX_TASKDATA_LENGTH)
	{
		strMainTaskID = strData.Mid(PREFIX_TASKDATA_LENGTH,dwIndex-PREFIX_TASKDATA_LENGTH);
	}

	
	pLocalGlobalData->dwWebMainTaskID = _ttol(strMainTaskID.c_str());
	CUnZipWebTaskData ZipTaskData;
	
	strTaskDataZipped = strData.Mid(dwIndex + SPLIT_CHARS_LENGTH);

	//��ѹ��,�ֽ�� ������ϸ��Ϣ,�����������������վID����վ��ĿID, ��Ʒ��Ϣ,ϵͳ������Ϣ��
	ZipTaskData.TransRegistryToTaskData(strTaskDataZipped, stWebRegistryData);

	//����ϵͳ��Ϣ��ȫ��������
	ZipTaskData.SaveSystemInfoToLocal(stWebRegistryData.strSystemInfo);

	//���淢����վ��Ϣ
	ZipTaskData.SaveWebInfoToLocal(stWebRegistryData.strWebInfo);

	//����������Ϣ
	ZipTaskData.SaveConfigInfoToLocal(stWebRegistryData.strConfigInfo);

	dwServerMintus = _ttol(stWebRegistryData.strServerMintus.c_str());
	WebServerMintustoMcontrol(dwServerMintus);

	//Ϊ�Ĵ��ں�׼����Ʒ��ͼƬ
//	GetImagePath(stWebRegistryData.strLocalProctImage, stWebRegistryData.strTradeImage, stWebRegistryData.strLocalCompImage);

	//���ز�ƷͼƬ��Ӫҵִ��
	CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL, stWebRegistryData.strTradeImage.c_str());
	CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL, stWebRegistryData.strLocalCompImage.c_str());

	//�����ƷͼƬ��  Ӫҵִ��  ��˾��Ƭ�������ڴ�
	_tcsncpy(pLocalGlobalData->szTradeImageName, stWebRegistryData.strTradeImage.c_str(), MAX_FILE_PATH_LENGTH-1);
	_tcsncpy(pLocalGlobalData->szCompImageName, stWebRegistryData.strLocalCompImage.c_str(), MAX_FILE_PATH_LENGTH-1);


	pLocalGlobalData->dwTaskType = dwTaskType;



	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);
	
	return S_OK;
}

//ˢ����ɾ�����ݽ���
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
			_T("��ȡ����ȫ������ ����ȷ!"));
		return 1;
	}

	//�ȱ���web������ID��ȫ��������
	//�Ƚ���������ID,��
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT,PREFIX_TASKDATA_LENGTH);

	if (dwIndex > PREFIX_TASKDATA_LENGTH)
	{
		strMainTaskID = strData.Mid(PREFIX_TASKDATA_LENGTH,dwIndex-PREFIX_TASKDATA_LENGTH);
	}

	
	pLocalGlobalData->dwWebMainTaskID = _ttol(strMainTaskID.c_str());
	CUnZipWebTaskData ZipTaskData;
	
	strTaskDataZipped = strData.Mid(dwIndex + SPLIT_CHARS_LENGTH);

	//��ѹ��,�ֽ�� ������ϸ��Ϣ,�����������������վID����վ��ĿID��
	ZipTaskData.TransRefreshAndDeleteToTaskData(strTaskDataZipped, stWebRefreshOrDeleteData);

	//���淢����վ��Ϣ
	ZipTaskData.SaveWebInfoToLocal(stWebRefreshOrDeleteData.strWebInfo);

	//����������Ϣ
	ZipTaskData.SaveConfigInfoToLocal(stWebRefreshOrDeleteData.strConfigInfo);

	//�����ʺ�����
	ZipTaskData.SaveAccountPasswordToLocal(stWebRefreshOrDeleteData.strAccountPassword);

	dwServerMintus = _ttol(stWebRefreshOrDeleteData.strServerMintus.c_str());
	WebServerMintustoMcontrol(dwServerMintus);

	pLocalGlobalData->dwTaskType = dwTaskType;

	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);
	
	return 0;
}

//ˢ����������
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
			//������վ��Ϣ
			pMapStation = new std::map<DWORD, T_TaskNetItem*>;
			T_TaskNetItem *pNetItem = new T_TaskNetItem;
			_tcscpy(cTmp, result[1]);
			pNetItem->dwWebId = _ttol(cTmp);       //��վid
			(*pMapStation)[pNetItem->dwWebId] = pNetItem;
			(*pMapStationAndKey)[ETTypeShopTraffic] = pMapStation;

			//�����ʼǨ������
			_tcsncpy(pLocalGlobalData->szShopTrafficUrl, result[2], MAX_URL_LENGTH - 1);
			pLocalGlobalData->dwPdtClickCnt = _ttoi(result[3]);
			pLocalGlobalData->dwOtherClickCnt = _ttoi(result[4]);

			pLocalGlobalData->dwTaskType = ETTypeShopTraffic;
			pLocalGlobalData->dwCurrentTaskItemCount = pMapStationAndKey->size(); //������
			g_MC->GetPtrOfThreadManage()->PostMessage(0, E_TASKMGR_THREAD, eTask_PushGUITask, MAG_STATION_INDEX);

			return 0;
		}
		else
		{
			g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, \
				_T("��ȡ��������ʧ��..."));
		}
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, \
			_T("ˢ�������������ݸ�ʽ����..."));
	}

	return 1;
}


typedef BOOL  (*GetQuickPhoto)(const CStdString &strWord, IServerSocket *pSocket);
//ץȡ�������������
void CProcessData::GetSearchPhoto( const CStdString &strData )
{
	//�����ǰ����ķ�804������Ҫץȡ����
	CReg reg;	
	CString strInfo;
	TCHAR* pContent = (TCHAR*)reg.ReadValueOfKey(HKEY_CURRENT_USER, _T("Control Panel\\International"), _T("Locale"));
	strInfo = pContent;
	if (strInfo != _T("00000804"))
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("��⵽ϵͳ�����804����ִ��ץȡ����, ϵͳLocale��%s"), pContent);
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
				_T("ȡ����GetQuickPhotoʧ��, �����룺%d"), GetLastError());
		}
	}
	else
	{
		g_log.Trace(LOGL_HIG, LOGT_ERROR, __TFILE__, __LINE__,
			_T("����GetPagePhoto.dllʧ��, �����룺%d"), GetLastError());
	}

}

//�����ƹ�
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
			_T("��ȡ����ȫ������ ����ȷ!"));
		return 1;
	}

	CUnZipWebTaskData ZipTaskData;

	//��ѹ��,�ֽ�� ������ϸ��Ϣ,�����������������վID����վ��ĿID��
	ZipTaskData.TransDomainPostToTaskData(strData, stWebDomainData);

	//���淢����վ��Ϣ
	ZipTaskData.SaveWebInfoToLocal(stWebDomainData.strWebInfo);

	//���������ƹ���Ϣ
	ZipTaskData.SaveDomainInfoToLocal(stWebDomainData.strTaskInfo);

	//����������Ϣ
	ZipTaskData.SaveConfigInfoToLocal(stWebDomainData.strConfigInfo);

	pLocalGlobalData->dwTaskType = dwTaskType;

	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);
	
	return 0;
}

//΢���ƹ�
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
			_T("��ȡ����ȫ������ ����ȷ!"));
		return 1;
	}

	CUnZipWebTaskData ZipTaskData;

	//��ѹ��,�ֽ�� ������ϸ��Ϣ,�����������������վID����վ��ĿID��
	ZipTaskData.TransWeboPostToTaskData(strData, stWebWeoData);

	//���淢����վ��Ϣ
	ZipTaskData.SaveWebInfoToLocal(stWebWeoData.strWebInfo);

	//����΢��Ҫ��������Ϣ
	ZipTaskData.SaveWeboInfoToLocal(stWebWeoData.strTaskInfo);

	//�����˺�������Ϣ
	ZipTaskData.SaveAccountPasswordToLocal(stWebWeoData.strAccountPassword);

	//����������Ϣ
	ZipTaskData.SaveConfigInfoToLocal(stWebWeoData.strConfigInfo);

	pLocalGlobalData->dwTaskType = dwTaskType;

	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

	return 0;
}

/*
@brief  �޸����Ϲ���
@param  strData  �ϲ㴫�����ļ�������
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
		g_log.Trace(LOGL_HIG,LOGT_ERROR,__TFILE__,__LINE__,_T("��ȡ����ȫ�����ݲ���ȷ��"));
		return 0;
	}

	//�ȱ�������ID
	dwIndex = strData.Find(SIMPLE_LIST_SPLIT,PREFIX_TASKDATA_LENGTH);

	if (dwIndex > PREFIX_TASKDATA_LENGTH)
	{
		strMainTaskID = strData.Mid(PREFIX_TASKDATA_LENGTH,dwIndex-PREFIX_TASKDATA_LENGTH);
	}
	pLocalGlobalData->dwWebMainTaskID = _ttol(strMainTaskID);

	CUnZipWebTaskData ZipTaskData;

	strTaskDataZipped = strData.Mid(dwIndex + SPLIT_CHARS_LENGTH);

	//��ѹ������
	ZipTaskData.TransInfoModifyToTaskData(strData,strInfoModifyData);

	//����ȫ����վ��Ϣ
	ZipTaskData.SaveWebInfoToLocalOfOneTask(strInfoModifyData.strWebInfo);

	//����ϵͳ������Ϣ
	ZipTaskData.SaveSystemInfoToLocal(strInfoModifyData.strSystemInfo);

	//����������Ϣ
	ZipTaskData.SaveConfigInfoToLocal(strInfoModifyData.strConfigInfo);

	//�����ʺ�������Ϣ
	ZipTaskData.SaveAccountPasswordToLocal(strInfoModifyData.strAccountPassword);

	//����ͼƬ
	CStdString strPicPath, strPicName;
	int iPos;
	DWORD dwFlag;
	DWORD dwCount;
	//Ӫҵִ��
	strPicPath.Empty();
	strPicName.Empty();
	strPicPath =(CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL;
	iPos = strPicPath.ReverseFind(_T("/"));
	strPicName = strPicPath.Mid(iPos + 1);
	dwFlag = GetImagePath(strPicName,strInfoModifyData.strTradeImage);
	if (dwFlag == 0)
	{
		//����Ӫҵִ��
		CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szTradeURL, strInfoModifyData.strTradeImage.c_str());
	}
	//��˾ͼƬ
	strPicPath =(CStdString)pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL;
	iPos = strPicPath.ReverseFind(_T("/"));
	strPicName = strPicPath.Mid(iPos + 1);
	dwFlag = GetImagePath(strPicName,strInfoModifyData.strLocalCompImage);
	if (dwFlag == 0)
	{
		//���ع�˾ͼƬ
		CHttpUtils::GetFile(pLocalGlobalData->aSystemInfo.systemInfo.szCompImageURL, strInfoModifyData.strLocalCompImage.c_str());
	}

	pLocalGlobalData->dwTaskType = ETTypeModify;

	g_MC->GetPtrOfThreadManage()->PostMessage(0,E_TASKMGR_THREAD,eTask_PushGUITask,MAG_STATION_INDEX);

	return S_OK;

}

/*
@brief ͼƬת��
@param strImagePath; Ҫת����ͼƬ��ַ��
@param dwType;    ת������ (1.jpg,2.png,3.bmp,4.gif)
*/
BOOL CProcessData::ImageConvert( CStdString &strSrcPath, DWORD dwType )
{
	CStdString strTmp = _T("");
	CStdString strFileName = _T("");
	CStdString strExtension = _T("");
	CImage img;
	HRESULT hr;

	//jpg��תֱ������
	if ((strSrcPath.Find(_T(".jpg")) != -1 && dwType == 1)
		||(!PathFileExists(strSrcPath)))
	{
		return TRUE;
	}

	strTmp = strSrcPath;
	int iPotPos  = strSrcPath.ReverseFind(_T("."));
	strFileName = strSrcPath.Left(iPotPos);
	img.Load(strSrcPath);
	//����ת����ʽ
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
		strSrcPath = strTmp; //��ԭ����ͼƬ
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("ת��ͼƬ����(%d)ʧ��:%s"),dwType,strSrcPath.c_str());
		return FALSE;
	}

	return TRUE;
}

//���ҹؼ�������
void CProcessData::SearchKeyData(const CStdString &strData)
{
	CSearchWord searchWord;

	searchWord.ProcessKeyWord(strData);
}

