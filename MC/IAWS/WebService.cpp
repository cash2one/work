//////////////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2008, ��Ѷ����Ϣ���޹�˾
// �汾��
// �ļ�˵���� ��������͹�˾��������������,����ֻ��ʵ����һ��
// �������ڣ�2008-11-11
// ���ߣ� yli
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2.
//

#include "stdafx.h"
#include "WebService.h"
#include "StdString.h"
#include "ZipEncode.h"
#include "FileReadAndSave.h"
#include "ItemList.h"

//�ͷ����������ľ��崦�����
#include "shlobj.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
// �����������������캯��,�����������ʼֵ
// ���룺
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////

CWebService::CWebService(void)
:IThreadUnit(E_WEBSERVICE_THREAD,0x055F)
{


}

//////////////////////////////////////////////////////////////////////////////////////
//
// ������������������������
// ���룺
// �����
// ����ֵ��
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
CWebService::~CWebService(void)
{

}

//////////////////////////////////////////////////////////////////////////////////////
//
// �������������������͸��̵߳ĸ�����Ϣ
// ���룺Msg ��Ϣ
// �����
// ����ֵ��
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::CreateThread()
{
    m_dwNetLastupdateIndex = 900;
    m_dwAllStationLastUpdateGetting = 0;//���ڻ�ȡȫ����վ��������ʱ��
	m_dwWebServiceAllStationLastUpdateSended = 0; //��û�з��������Ѿ�׼���õ���Ϣ
	dwIsAllNetstationLastUpdateGetted = 0;

	memset(m_szCurrPath, 0, sizeof(m_szCurrPath));
	GetModuleFileName(NULL, m_szCurrPath, MAX_PATH);
	PathAppend(m_szCurrPath, _T("..\\..\\"));

	std::cout << "Create WebService Thread   "  << ::GetTickCount() <<std::endl ;

    return 0;

}


//////////////////////////////////////////////////////////////////////////////////////
//
// �������������������͸��̵߳ĸ�����Ϣ
// ���룺Msg ��Ϣ
// �����
// ����ֵ��
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::DispatchMessage(T_Message *Msg)
{

    DWORD MsgParam = 0;
    MsgParam = Msg->dwMsgParam;

    switch(MsgParam)
    {
    case MSG_TIMER:	//ʱ����Ϣ,��Ҫ����
		{
        }
        break;

	case E_WEBS_INITDOWNLOADFILE:      //��ʼʱ����һЩ�����ļ�
		{
			GetWebServiceData(Msg->dwData);
		}
		break;
		
	case E_WEBS_GET_NETSTATION_BASEINFO:      //��ȡ������վ�Ļ�����Ϣ 1
		{
			GetNetDataFromFTP(E_WEBS_GET_NETSTATION_BASEINFO,Msg->dwData);
		}
		break;

    case E_WEBS_LASTUPDATE_GETTED:   ////��վ������ʱ���Ѿ���ȡ�� 2
        {
            GetAllStationLastUpdateALLGettedPPD(Msg->dwData);
        }
        break;
 
	case E_FTP_GET_FILE_FROM_SERVER_SAVED:      //FTP������ɵ�����վ�Ļ�����Ϣ 2
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
// ��������������PPD�߳��Ѿ���ȡ��,��Ҫɾ��ȫ�����������ڴ�
// ���룺dwDataIndexID ������֯����
// �����
// ����ֵ��1 �ɹ� ; 0 ʧ��
// ����˵����
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
// ��������������
// ���룺
// �����
// ����ֵ��
// ����˵����
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
			_T("FTP ���ص��ļ� ������֯��Ϊ��,���ݱ��%d ") ,dwDataIndexID);
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

		//������վ��Ϣ�ļ��Ĺ���
		GetNetstationAllInfoReturn(pFtpFileName->dwFileID,pFtpFileName->dwFileType,strFile);	

		//������վ������ʱ���
		GetAllStationLastUpdateALLReturn(pFtpFileName->dwFileID,pFtpFileName->dwFileType,strFile);


	}

	pFtpFileName =
		(T_FtpFileName *)g_GlobalDataBlock->DeleteDataNode(
		dwDataIndexID,E_FtpFileName,E_WEBSERVICE_CLASS,eUseBlockWithOne);

	// ɾ��ѹ���ļ�, ��վ������ʱ�䲻ɾ��. ��Ҫ���м��ܴ���
	if ((1 == pFtpFileName->dwFileID) &&  ( 6 == pFtpFileName->dwFileType))
	{
		//���ļ���Сд��ע���
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

DWORD CWebService::GetNetstationAllInfoReturn(const DWORD dwItemListCode,const DWORD dwListType,const CStdString &strFile) //��ȡ��վ��Ϣ�����ļ��ķ���
{
	if (!(NETSTATION_ALL_LIST_TYPE == dwListType))
	{
		//������վ��Ϣ�ļ�������
		return 1;
	}
	
	if (CFileReadAndSave::GetItemFileLength(strFile) <= 3)
	{
		PostMessage(E_DATAPRE_THREAD,E_PPD_SERVERRETURNERR_MSG,dwItemListCode); //��ȡ��Ϣʧ��,��ʾ׼���߳�
		return 2;
	}
	//��PPD�̷߳����Ѿ��ɹ�����Ϣ

	PostMessage(E_DATAPRE_THREAD,
		E_PPD_RETURNNETSTATIONBASEINFO_MSG,dwItemListCode); //�������ݿ����,��Ҫ������Ϣ��׼���߳�

	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
		_T("FTP������վ�ɹ�����DataID:%d    TimeTick:%d   ��վ���:%d"),dwItemListCode,GetTickCount(),dwItemListCode);

	
	return BIZMC_OK;
}



DWORD CWebService::SendToFtpGetServerFile(const int dwItemListCode,const int dwListType ,const DWORD dwAuto)
{
	T_FtpFileName *pFtpFileName = NULL;
	_NEW(pFtpFileName,T_FtpFileName);

	if (NULL != pFtpFileName)
	{
		//���뵽������������,������׼���߳���������
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
// ��������������
// ���룺
// �����
// ����ֵ��
// ����˵����
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
// ��������������
// ���룺
// �����
// ����ֵ��
// ����˵����
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
// �������������������ȡ���ػ����������վ������ʱ��ɹ�����Ҫ���Ĳ���
// ���룺szResult ��վ������ʱ���ַ���
// �����
// ����ֵ��1 �ɹ� ; 0 ʧ��
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::GetAllStationLastUpdateALLOK(const CStdString &szResult)           
{
	if (szResult.length() ==0)
	{
		CStdString strBack;
		g_log.Trace(LOGL_TOP, LOGT_ERROR,__TFILE__, __LINE__,_T("1.bin�ļ�����ʧ�ܣ������˳�"));

		strBack.Format(_T("BackResult(;0)1.bin����ʧ��(;0)(;0)"));
		BYTE * pByData = (BYTE *)strBack.c_str();

		g_ServerSocket->SendData(strBack.size() * 2,E_GET_EXCUTE_TASK_RESULT,(char*)pByData);
		g_ServerSocket->UnInit();

		ExitProcess(0);	//�˳�����
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
		//���µ���վ�б��ȡ�� &&  ���µ���վ���ʱ��Ҳ��ȡ�� &&���������������ݵ����ʱ��Ҳ��ȡ�� && ��û����PPD���������Ѿ�ȫ��׼���õ���Ϣ
	{
		PostMessage(E_DATAPRE_THREAD,E_PPD_LASTUPDATE_GETTED_MSG,
			m_dwNetLastupdateIndex);  //��վ������ʱ���Ѿ���ȡ��,֪ͨ׼���̸߳���

		m_dwWebServiceAllStationLastUpdateSended = 1;
	}

    return BIZMC_OK;
}



////////////////////////////////////////////////////////////////////////////////////// 
//
// ��������������
// ���룺
// �����
// ����ֵ��
// ����˵����
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

			//�����ݴ���
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
// ��������������
// ���룺
// �����
// ����ֵ��
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
void CWebService::GetWebServiceData(DWORD dwIndex)
{
	//������վ������ʱ���ļ�
	//�ļ�������ʱ��
	//������վ��������ʱ��ҲҪ��ȡ.

	GetAllStationLastUpdateALL();
}


//////////////////////////////////////////////////////////////////////////////////////
//
// ����������������Ӷ��û����֤�Ź��ڵĴ���,  ����û�������,�ͱȽ���վ������ʱ���Ƿ���û��ĵ���ʱ��Ƚ�,
//												���ڹ���ʱ��Ͳ�������վ�Ļ�����Ϣ
// ���룺
// �����
// ����ֵ��
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD CWebService::GetNetDataFromFTP(const DWORD dwMessageType,const DWORD dwWebID)
{
	//�����ڵ��û�,ֱ��ȥ��������
	switch(dwMessageType)
	{
	case E_WEBS_GET_NETSTATION_BASEINFO:      //��ȡ������վ�Ļ�����Ϣ
		{
			SendToFtpGetServerFile(dwWebID,NETSTATION_ALL_LIST_TYPE ,0);  //9�ļ���Ϊ��վ��Ϣ���ļ���
		}
		break;
	default:	 //
		{

		}
		break;
	}
	

	return 0;
}


