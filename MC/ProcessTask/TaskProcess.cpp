#include "StdAfx.h"
#include "TaskProcess.h"
#include "StringUtils.h"
#include "FeatureFit.h"
#include "InternetHttp.h"

using namespace FeatureFit;
typedef void (WINAPI *LPFN_PGNSI)(LPSYSTEM_INFO);
#pragma comment(lib, "Version.lib")

CTaskProcess::CTaskProcess(void)
{
	bIsWait = FALSE;
/*
	//��ȡ��վcode;
	CStdString strNsCode;
	std::vector<CStdString> vTmpcode;
	StringUtils Util;
	g_Config->ReadString(_T("OBJECTWEB"),_T("NsCode"),strNsCode);
	if (strNsCode.GetLength() > 0)
	{
		Util.SplitStringEx(strNsCode,_T(","),vTmpcode,false);
		for (int i = 0; i < vTmpcode.size(); ++i)
		{
			vNsCodeOfObject.push_back(_ttol(vTmpcode[i]));
		}
	}
	else
	{
		vNsCodeOfObject.push_back(40000519);
		vNsCodeOfObject.push_back(40006512);
		vNsCodeOfObject.push_back(40001567);
		vNsCodeOfObject.push_back(49994692);
		vNsCodeOfObject.push_back(40012911);
		vNsCodeOfObject.push_back(40042556);
		vNsCodeOfObject.push_back(40020637);
		vNsCodeOfObject.push_back(40000511);
	}*/

}


CTaskProcess::~CTaskProcess(void)
{
}

/*
@breif  �����ںˣ�
*/
void CTaskProcess::ConnectKernel(CCommServer &Comserver,int &iPort, pfnDataHandler fn)
{
	int i = 0;
	while (i < 10)
	{
		if (Comserver.Init(iPort,fn) == 0)
		{
			break;
		}

		iPort = GetTickCount()%(65535 - 10000) + 10000; //����˿�
		i++;
	}
}

/*
@breif ����4���ں�
@param Comserver socket���ӣ�
@param iPort    �˿ں�
@param hndEngine �ں˽��̾��
@param lRunEngine	�����е��ںˣ�
@Return True�����ӳɹ�
*/
BOOL CTaskProcess::RunEngine( CCommServer &Comserver, int iPort,HANDLE &hndEngine,long &lRunEngine )
{
	InterlockedIncrement(&lRunEngine);
	//��ֹ��������ںˣ�
	if (lRunEngine > 1)
	{
		InterlockedDecrement(&lRunEngine);
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("����4���������������˳�"));
		return FALSE;
	}

	if(hndEngine != NULL)
	{
		Comserver.CloseClient();
		Sleep(2000);
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("�ں�δ�˳�������TerminateProcess!"));
		TerminateProcess(hndEngine,0);
		CloseHandle(hndEngine);
	}

	//�����ں˽�����Ϣ -----begin------
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	//����ʾ���æµ״̬
	si.dwFlags = STARTF_FORCEOFFFEEDBACK;
	ZeroMemory( &pi, sizeof(pi) );
	//------end-------

	TCHAR szPath[MAX_PATH] = {0};
	CString strEnginePath = _T("");
	CString strParam = _T("");
	CString strPtmp = _T("");  //��ʱ����
	GetModuleFileName(NULL,szPath,MAX_PATH);
	PathAppend(szPath,_T("..\\"));

	int iMode = 0;
	g_Config->ReadInteger(_T("Kernel4"),_T("mode"),iMode);

	T_LocalGlobalData *pLocalGlobalData =
		(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS,eUseCommReadWithMulti);

	if (pLocalGlobalData != NULL
		&& pLocalGlobalData->iMode == rmBrowser)
		iMode = pLocalGlobalData->iMode;

	if (pLocalGlobalData != NULL 
		&& pLocalGlobalData->bIsYunTask)
	{
		strPtmp = _T("-y");
	}

	strEnginePath.Format(_T("%sEngine.exe"),szPath);
	//�����ҪԶ�̴��룬����Ϊ�������Ӳ�����ʽ
	if (pLocalGlobalData->dwOCRType == 1)
	{
		strParam.Format(_T(" -p%d -m%d -f %s"), iPort, iMode,strPtmp);
	}
	else
	{
		strParam.Format(_T(" -p%d -m%d %s"), iPort, iMode,strPtmp);
	}

	//�����ں�
	if (!CreateProcess(strEnginePath.GetBuffer(0),strParam.GetBuffer(0),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi ) )
	{
		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("����4����ʧ��,�ļ�·��Ϊ��%s"),strEnginePath.GetString());
		InterlockedDecrement(&lRunEngine);
		return FALSE;
	}

	hndEngine = pi.hProcess;
	//g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("�����˽��̳ɹ�"));

	strEnginePath.ReleaseBuffer();
	strParam.ReleaseBuffer();
	int iCount = 20;
	while((!Comserver.IsClientConnected())&& (iCount>0))
	{
		iCount--;
		Sleep(500);
	}

	InterlockedDecrement(&lRunEngine);

	return TRUE;

}

/*
@breif  �Ƿ�ΪObject�������վ
@param	dwWebid ��վIDBOOL CTaskProcess::IsObjectWeb( const DWORD &dwWebid )
{
	for (int i = 0; i < vNsCodeOfObject.size(); ++i)
	{
		if (dwWebid == vNsCodeOfObject[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}*/

/*
	@breif  OBJECT��վ�赥�߳�ִ��
	@param	dwWebid ��վID
	@param  lngRunCnt object������վ����
	@param  lngRuningCnt �����е�����
	@param  iEventCnt	�¼���Ӧ��������������
*/
BOOL CTaskProcess::RunWebBySingle(DWORD dwWebid, long &lngRunCnt, int iEventCnt, long lngRuningCnt, BOOL bIsObject)
{
	while (bIsWait)			
	{
		//�����ǰ��Object��վ���У���һֱ�ȴ���
		Sleep(100);
	}

	//�жϵ�ǰ��վ�Ƿ���Object��վ��
	if (bIsObject)
	{
		if (lngRunCnt > 0)
		{
			//����պ�ǰ���Ѿ�����վ׼�����У�����
			return FALSE;
		}

		++lngRunCnt;		//��ʼ����

		if ((lngRuningCnt > 0) && (iEventCnt != 0))
		{
			//���������Object��վ�������У�������������վ�����У����أ�Object��վֻ�ܵ��߳����У�
			-- lngRunCnt;
			return FALSE;
		}

		bIsWait = TRUE;

		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("���߳��ƹ�OBJECT��վ,��վidΪ%d"), dwWebid);
	}
	

	return TRUE;
}

/*
@brief ��ȡ�������ݿ�����
@param stationData ��ǰ��վ���ݣ�
@param funData ���ܿ����ݣ�
@param curFunction �������ͣ�
@param pTask  �������ݣ�
@param pData  ��������ݣ�
*/
CTaskData* CTaskProcess::GetTaskData(CStationData &stationData, CFunctionData &funcData, E_StationFunction &curFunction, T_TaskData *pTask, long iStep, CStdString &strOCR, DWORD dwMailId)
{
	CStationData StationEmbedData; //Ƕ����վ���ݣ�
	bool bEmbedData;
	T_EmbedItemData tEmbedItem;

	//���ݹ��ܻ�ȡ��ǰ�����Ƿ���Ƕ����վ
	CStdString strStrWebid;
	strStrWebid.Format(_T("%d"), dwMailId);
	bEmbedData = funcData.GetEmbedInfo(curFunction,tEmbedItem,strStrWebid);
	
	//�ж��Ƿ�һ����¼
	if (pTask->eTaskType != ETypeAutoLogin &&
		!pTask->bGetLogin)
	{
		funcData.SetActiveIndex(iStep);
		//Ƕ�빦��
		if (funcData.GetCurTaskType() == ttDataStationFunction)
		{
			//���ж��Ƿ���Ƕ������
			if (bEmbedData)
			{				
				StationEmbedData.SetSource(pTask->tTaskDataExser.mapEmbedData[dwMailId]);
				strOCR = pTask->tTaskDataExser.strExOCRParam;
				return StationEmbedData.GetTaskData((E_TaskType)tEmbedItem.nData);
			}
		}
		else
		{
			strOCR = pTask->strOCRParam;
			return stationData.GetTaskData(funcData.GetCurTaskType());
		}
	}
	else
	{
		strOCR = pTask->strOCRParam;
		return stationData.GetTaskData(ttDataLogin); //ֱ�ӻ�ȡһ����¼�����ݣ�
	}

	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("pTada ΪNULL, ȡ���幦������ʧ�� ����0����վidΪ%d,ִ�й��ܣ�%d"), pTask->dwWebID,funcData.GetCurTaskType());

	return NULL;

}

/*
@brief ��֯Ҫ���͸��ں˵�����
@param dwWebID ��վID;
@param stationData ��ǰ��վ���ݣ�
@param pTData	��������ݣ�
@return �ַ�������
*/
CStdString CTaskProcess::GetSendData(DWORD dwWebID, CStationData &stationData, CTaskData *pTData, CStdString &strOCRParam)
{
	//���͵���������
	CStdString strTaskData= _T("");

	T_DATA_IN dataToEngine;
	dataToEngine.strCmd = _T("EXEC");
	dataToEngine.strNsName = stationData.GetNsName();
	dataToEngine.strInDataset = pTData->GetSource();
	dataToEngine.bNoImage = stationData.GetIsNoImages();
	dataToEngine.dwMaxViewCount = stationData.GetViewCount();
	dataToEngine.strOCRParam = strOCRParam;

	CStdString strDataIn = dataToEngine.GetSource();
	CString strTaskID;
	strTaskID.Format(_T("%d"),dwWebID);
	
	strTaskData = (CStdString)strTaskID + _T(",") + strDataIn;

	return strTaskData;

}

/*
@brief �滻��������
@param cParam Ҫ�õ������ݣ�
@param strTaskData	�������ݣ�
@param dwFalg; 0:��Ʒ�滻���ƣ�1.ˢ�����̵�ַ
*/
void CTaskProcess::ReplaceSendData( CommonParam &cParam, CStdString &strTaskData)
{
	if (cParam.dwTaskType == ttDataOnceKeyProduct)
	{
		if (cParam.dwLoopPdtCnt > 0)
		{
			strTaskData = cParam.strSendDataBak;
			//�滻Ҫ��ȡ�Ĳ�Ʒ����
			if (cParam.vPdtLists.size() >= cParam.dwCurPdtCnt )
			{
				strTaskData.Replace(_T("<<ProName>>"),cParam.vPdtLists[cParam.dwCurPdtCnt++]);
			}
		}

	}

}

/*
@brief ���߳�����ʱ�����������
@param iSingle  �Ƿ�����˫�ںˣ�
@param  lngRunCnt object������վ����
@param  lngRuningCnt �����е�����
*/
void CTaskProcess::ClearRunObjectCnt( int iSingle, long &lngRunCnt, long &lngRuningCnt )
{
	if (iSingle == 1)
	{
		bIsWait = FALSE;
		lngRuningCnt --;  //�����е������1
		if(lngRunCnt > 0)
		{
			lngRunCnt--;
		}
	}
}

/*
@brief ��������
@param eTaskType;
*/
void CTaskProcess::ExitMC( E_TASK_TYPE eTaskType )
{
	//һ�����룬���ݷ������ֱ���˳���
	if (eTaskType == ETypeAutoLogin)
	{
		g_log.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__,_T("һ�����뷢������֮�������˳�"));
		ExitProcess(0);	//�˳�����
	}
}

/*
@brief ��ȡ��վʧ�ܽ������
@param pTask	�������ݣ�
@param pTData   ��������ݣ�
@param tDataOut �������ݣ�
@param vResult  ʧ�ܼ��ϣ�
*/
int CTaskProcess::GetFailedResult( T_TaskData *pTask,CTaskData *pTData,T_DATA_OUT &tDataOut,std::vector<CStdString> &vResult )
{
	int iReturn = 0;
	T_FILTER_STRUCTURE *ptResult;

	if (vResult.size() > 0)
	{
		int i;
		vResult[0] += tDataOut.strErrDesc;  //����������Ҳ���ϣ������������
		for (i=1; i<rftTotalCount; i++)  //����ʧ��ԭ��
		{
			ptResult = pTData->GetFilterStructure((E_RESULT_FILTER_TYPE)i);

			if (ptResult != NULL)
			{
				int iResult = FitFilter2(vResult[0], *ptResult, false, pTask->tTaskResultInfo.strResultInfo);
				if (iResult == 0)
				{
					break;
				}
			}
			ptResult = NULL;
		}

		switch(i)
		{
		case rftRePost:
		case rftReReg:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_DUPLICATE;
			break;
		case rftMsgFull:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_POSTFULL;
			break;
		case rftFail:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
			break;
		case rftDataAddExtra:
			{
				pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDEXTRAFAIL;
				if (pTask->eTaskType == ETTypePost
					&& pTask->bIsSendExtra == FALSE)
				{
					pTask->bIsSendExtra = TRUE;
					iReturn = 2;
				}
			}

			break;
			//14.6.25
		case rftAddPhoto:
			{
				pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDPICFAIL;
				if (pTask->eTaskType == ETTypePost
					&& pTask->bIsSendPic == FALSE)
				{
					pTask->bIsSendPic = TRUE;
					iReturn = 3;
				}
			}
			break;
		case rftReEmail:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REEMAIL;
			iReturn = 6;
			break;
		case rftReCompany:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_RECOMPANY;
			iReturn = 4;
			break;

			//�˺���Ч
		case rftInvalidAccount:    
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_INVALIDACCOUNT;
			break;
			//�˺�δ���
		case rftAccountNotAduit:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ACCOUNT_NOT_AUDIT;
			break;
			//ע��ȱ������
		case rftRegLackInfo:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REG_LACK_INFO;
			break;
			//����ȱ������
		case rftAddLackInfo:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADD_LACK_INFO;
			break;
			//����ȱ������
		case rftPostLackInfo:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_POST_LACK_INFO;
			break;
			//�ֻ����ظ�
		case rftRePhoneNumber:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REPHONENUMBER;
			break;
		default:
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL;
			break;
		}
	}

	if (tDataOut.strCodeOcrInfo.length() >= 4)
	{
		CStdString tmp = tDataOut.strCodeOcrInfo.Trim().Left(3);
		g_log.Trace(LOGL_MID, LOGT_DEBUG, __TFILE__, __LINE__, _T("��֤�뷵��Ϊ��%s"), tDataOut.strCodeOcrInfo.c_str());
		//��֤��������
		if (tmp.Find(_T("1")) == -1)
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_AUTH_CODE_ERROR;
	}

	if ((pTask->eTaskType == ETTypeRegaccount) || (pTask->eTaskType == ETTypeReRegaccount))
	{
		if (pTData->GetType() == ttDataLogin)  //ע��ʱ��ע�Ჽ���Ѿ�ִ�гɹ�
		{
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_INVALIDACCOUNT;
		}
	}
	else if (pTask->eTaskType == ETTypePost)
	{
/*
		if (pTask->tTaskResultInfo.eResultFlag != e_TASK_RESULT_ADD_LACK_INFO)
		{
			if (pTData->GetType() == ttDataAddExtra)  //����ʱ��Ҫ�������ʧ��
			{
				pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDEXTRAFAIL;
			}
		}
*/

		if (pTData->GetType() == ttDataUpdatePhoto) //����ʱ��Ҫ�������ʧ��
		{
			pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_ADDPICFAIL;
		}
	}

	if (tDataOut.strErrDesc.length() > 0)
		pTask->tTaskResultInfo.strResultInfo = tDataOut.strErrDesc;

	if (vResult.size() >= 2)
		pTask->tTaskResultInfo.strHtml = vResult[1];

	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("����ִ��ʧ��,��վid��%d,����������%s, �����룺%d"),pTask->dwWebID,tDataOut.strErrDesc.c_str(), pTask->tTaskResultInfo.eResultFlag);

	return iReturn;

}

/*
@brief ��ִ�гɹ���վ���ദ��
@param pTask	�������ݣ�
@param pTData   ��������ݣ�
@param tDataOut �������ݣ�
*/
int CTaskProcess::GetSuccResult( T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType)
{
	int iResult = 0; //��ȡ�����

	//-----��ȡ�ں˷�������---------
	GetValueByName(tDataOut.strOutDataset, _T("%PostPageURL%"), pTask->tTaskResultInfo.strHtml);
	GetValueByName(tDataOut.strOutDataset, _T("$CompanyPopularity$"), pTask->tTaskResultInfo.strCompPopular);
	GetValueByName(tDataOut.strOutDataset, _T("$InfoID$"), pTask->tTaskResultInfo.strInfoID);             //��ȡ��ϢID;
	GetValueByName(tDataOut.strOutDataset, _T("$MailCount$"), pTask->tTaskResultInfo.strMailCount);		  //��ȡ�Ҽ�������			
	if (pTask->tTaskResultInfo.strHtml.length() == 0)
	{
		GetValueByName(tDataOut.strOutDataset, _T("$PostPageURL$"), pTask->tTaskResultInfo.strHtml);
	}

	if (pTask->tTaskResultInfo.strCompPopular.GetLength()> 0)
	{
		pTask->tTaskResultInfo.strCompPopular.Replace(_T(","),_T(""));
	}
	// --------��ȡ����end-----

	//----ע��ɹ�����----
	if ((pTask->eTaskType == ETTypeRegaccount) || (pTask->eTaskType == ETTypeReRegaccount))
	{
		ProcessRegData(pTask,tDataOut);
	}

	//----���� --ץȡ��������Ȩ��������վ
	if (pTask->eTaskType == ETTypePost && pTask->dwWebID == 40000010) 
	{
		pTask->tTaskResultInfo.strResultInfo = tDataOut.strOutDataset;
	}

	//----ˢ�³ɹ�����-----
	if (pTask->eTaskType == ETypeRefresh)
	{
		ProcessRefreData(pTask,tDataOut);
	}

	//----��ȡ��Ʒ�б���------
	if (pTask->eTaskType == ETTypeGetPdtList)
	{
		ProcessGetPdtData(pTask,tDataOut,dwTaskType);
	}

	//һ����Ҵ���
	if (pTask->eTaskType == ETTypeUserInfoMigrate)
	{
		ProcessMigrateData(pTask,tDataOut,dwTaskType);
	}

	iResult = 1;

	return iResult;
}

/*
@breif  ����ͨ�����ȡ��ֵ
@param  strData Ҫ���ҵ��ַ���
@param  strName ͨ�����
@param  [out ]strValueֵ
*/
void CTaskProcess::GetValueByName( const CStdString &strData, const CStdString &strName, CStdString &strValue )
{
	strValue = _T("");   //����գ���ֹ��������������Ϻ��ͼƬ��
	if (strData.length() <= 0 || strName.length() <= 0)
		return;
	CStdString strResult = _T("");
	int iStartPos = 0 ;
	int iPos1;
	int iPos2;
	int iNameLen = strName.length();

	while (1)
	{
		iPos1 = strData.Find(strName.c_str(),iStartPos);
		if (iPos1 ==-1)
		{
			break;
		}
		else
		{
			iPos2 = strData.Find(_T("(;,)"), iPos1 + 1);
			if (iPos2 == -1)
			{
				iPos2 =strData.length();
				if (iPos2> iPos1+iNameLen)
				{
					strValue = strData.Mid(iPos1+iNameLen+1, iPos2-(iPos1+iNameLen+1));
					strResult = strValue;
					break;
				}
			}
			else
			{
				if (iPos2> iPos1+iNameLen)
				{
					strValue = strData.Mid(iPos1+iNameLen+1, iPos2-(iPos1+iNameLen+1));
					iStartPos =iPos2;
					if (strResult.length() > 0)
					{
						strResult = strResult + _T(",")+strValue;
					}
					else
					{
						strResult = strValue;
					}
				}
			}
		}
	}

	strValue = strResult;

}

/*
@brief ��ע��ɹ����ݴ���
@param pTask	�������ݣ�
@param tDataOut �������ݣ�
*/
void CTaskProcess::ProcessRegData( T_TaskData *pTask,T_DATA_OUT &tDataOut )
{
	CStdString strValue;
	GetValueByName(tDataOut.strOutDataset, _T("%AsNName%"), strValue);

	if (strValue.length() == 0)
	{
		GetValueByName(tDataOut.strOutDataset, _T("$AsNName$"), strValue);
	}

	if (strValue.length() > 0)
	{
		pTask->szAccount = strValue;
	}

	if (pTask->siteType.eAccountState == asEmailConfirm)
	{
		pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REGMAIL_FAIL;
	}
	else if (pTask->siteType.eAccountState == asNeedVerify)
	{
		pTask->tTaskResultInfo.eResultFlag = e_TASK_RESULT_REGAUDIT_FAIL;
	}
}

/*
@brief ��ˢ�³ɹ����ݴ���
@param pTask	�������ݣ�
@param tDataOut �������ݣ�
*/
void CTaskProcess::ProcessRefreData( T_TaskData *pTask,T_DATA_OUT &tDataOut )
{
	CStdString strTotalCout;
	GetValueByName(tDataOut.strOutDataset, _T("%TotalPostCount%"), strTotalCout);

	if (strTotalCout.length() == 0)
	{
		GetValueByName(tDataOut.strOutDataset, _T("$TotalPostCount$"), strTotalCout);
	}

	if (strTotalCout.length() > 0)
	{
		DWORD dwTotal = _wtol(strTotalCout.c_str());
		DWORD dwValue = 1;  //Ĭ��ˢ��һ��
		DWORD dwRefreshTimes = dwTotal;  //Ĭ��ˢȫ��

		if (pTask->limitRefresh.eNewPosRefresh == npStart)  //ÿ��ˢ�����һ��
		{
			dwValue = dwTotal;
		}

		if (!(pTask->limitRefresh.dwSingleRefreshCount == -1
			|| pTask->limitRefresh.dwSingleRefreshCount >= dwTotal))  
		{
			//����ˢ�������Ϳ�ˢ���������ڵ����ѷ�����
			//��ˢ��ȫ��
			dwRefreshTimes = pTask->limitRefresh.dwSingleRefreshCount;
		}

		for (DWORD dwIndex=1; dwIndex<= dwRefreshTimes; dwIndex++)
		{
			pTask->vRefreshIndex.push_back(dwValue);
		}

	}
}

/*
@brief ����ȡ��Ʒ�б�ɹ����ݴ���
@param pTask	�������ݣ�
@param tDataOut �������ݣ�
*/
void CTaskProcess::ProcessGetPdtData( T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType)
{
	//����ǵ�¼�Ͳ�Ҫ��ȥ
	if (dwTaskType == 1)
	{
		return ;
	}

	CStdString strSource = _T("");
	CStdString strResult = _T("");
	strSource = tDataOut.strOutDataset;
	if (!tDataOut.strOutDataset.IsEmpty())
	{
		GetProductList(strSource,strResult);
		pTask->tTaskResultInfo.strMigrateResult = strResult;
	}
	else
	{
		g_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("�ϲ���ȡ��Ʒ�б�����ʧ�ܣ�"));
	}
}

/*
@breif ��ȡ�ں˷��صĲ�Ʒ�б�����
@param strSource Դ����
@param strResult Ҫ���ص�����
*/
void CTaskProcess::GetProductList( CStdString strSource, CStdString &strResult )
{
	std::vector<CStdString> vProductName; 
	StringUtils strUtil;
	CStdString strTmp;
	//����Ʒ����ǰ����ַ��滻�ɿ�
	strSource.Replace(_T("$ProName$="),_T(""));

	vProductName.clear();
	int iCount = strUtil.SplitString(strSource,_T("(;,)"),vProductName,false);

	for (int i = 0; i < iCount ; ++i)
	{
		strTmp = vProductName[i];
		//ȥ���ظ�������
		if (strResult.Find(strTmp) != -1)
		{
			continue;
		}

		if (strResult == _T(""))
		{
			//��һ�β���Ҫ���Ϸָ���
			strResult = strTmp;
		}
		else
		{
			strResult = strResult + _T("(;1)")+ strTmp;
		}
	}
}

/*
@brief ��һ����ҳɹ����ݴ���
@param pTask	�������ݣ�
@param tDataOut �������ݣ�
@param dwTaskType ��������
@param 
*/
void CTaskProcess::ProcessMigrateData( T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType )
{
	DWORD dwSuccCnt  =0;
	//����ǵ�¼�Ͳ�Ҫ��ȥ
	if (dwTaskType == 1)
	{
		return ;
	}

	CStdString strMigrateField = _T("");
	T_LocalGlobalData *pGlobalData = NULL;

	DWORD ret = m_NetData.OpenWebSiteData(pTask->dwWebID);
	if (ret == 0)
	{
		g_log.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�ļ���ʧ��. webid= %d",pTaskData->dwWebID));
	}
	
	pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if(dwTaskType == ttDataOnceKeyMove)
	{
		strMigrateField = pGlobalData->szMigrationCompanyField;
	}
	else if(dwTaskType == ttDataOnceKeyProduct)
	{
		strMigrateField = pGlobalData->szMigrationProductField;
	}
	else
	{
		m_NetData.CloseWebSiteData();
		return ;
	}


	dwSuccCnt = ReplaceMigrateData(tDataOut,strMigrateField);

	m_NetData.CloseWebSiteData();

	g_log.Trace(LOGL_DBG, LOGT_ERROR, __TFILE__, __LINE__,_T("�滻��ɣ����ͣ�%d,�ַ�����%s,ƥ��������%d �� "), dwTaskType,strMigrateField,dwSuccCnt);
	pTask->tTaskResultInfo.strMigrateResult = strMigrateField;
	pTask->tTaskResultInfo.dwSuccFieldCnt =dwSuccCnt;
}

/*
@brief ����һ�����ͨ����滻
@param strMigrateData;
*/
DWORD CTaskProcess::ReplaceMigrateData( T_DATA_OUT &tDataOut, CStdString &strMigrateData )
{
	CStdString strProv = _T("");
	CStdString strCity = _T("");
	CStdString strTmp = _T("");
	int dwVarId = -1;	//����ID
	int iType = 0;		//��������
	DWORD dwCnt = 0;	//ͳ���滻ʧ�ܵ�����
	std::vector<CStdString> vMigrateParam;
	
	StringUtils strUtil;
	vMigrateParam.clear();
	strUtil.SplitString(strMigrateData,_T(","),vMigrateParam,false);

	for (int i = 0; i < vMigrateParam.size();++i)
	{
		//��ȡ��һ��ͨ���
		CStdString strTmpParam;
		int iPosStart = vMigrateParam[i].Find(_T("$"));
		int iPosEnd = vMigrateParam[i].ReverseFind(_T("$"));
		strTmpParam = vMigrateParam[i].Mid(iPosStart,iPosEnd-iPosStart+1);

		if (strTmpParam.length() <= 3 || 
			strTmpParam == _T("$PProvId$") ||
			strTmpParam == _T("$CCityId$"))
		{
			continue;
		}

		//��ȡ�ں���ȡ��ͨ���������
		GetValueByName(tDataOut.strOutDataset, strTmpParam, strTmp);
		//����ո����ַ
		strTmp = strTmp.Trim();

		if (strTmp.GetLength() <= 0)
		{
			if (strTmpParam == _T("$PProv$"))								
			{
				strMigrateData.Replace(_T("$PProvId$"),_T(""));
			}
			if (strTmpParam == _T("$CCity$"))								
			{
				strMigrateData.Replace(_T("$CCityId$"),_T(""));
			}
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("û�л�ȡ����ͨ�����%s ��ֵ,�滻�ɿգ� "), strTmpParam.c_str());
			strMigrateData.Replace(strTmpParam,_T(""));
			dwCnt++;

			continue;
		}

		if (strTmpParam == _T("$WWebs$"))
		{
			//�ж���ַ�Ƿ���http
			if (strTmp.Find(_T("http://")) == -1)
			{
				strTmp = _T("http://") + strTmp;
			}
		}

		ReplaceNewLine(strTmp); //�滻�ո������ֶΣ�
		iType = KeyToVal(strTmpParam);

		if (iType > 0)
		{
			if (iType == 14)
			{
				strMigrateData.Replace(strTmpParam,_T(""));
				dwCnt++;
				strCity = strProv + _T("<@@>") + strTmp;
				strTmp = strCity;
			}
			if (iType == 31)
			{
				strCity = strCity +  _T("<@@>") + strTmp;
				strTmp = strCity;
			}

			dwVarId = m_NetData.GetStandardValue(iType,strTmp);
			CStdString strVarId = _T("");
			strVarId.Format(_T("%d"),dwVarId);  //ת�����ַ��ͱ��ں����滻
			if ((iType == 1) || iType == 14)   
			{
				//ʡ�ݺͳ���
				if (iType == 1)
				{
					strMigrateData.Replace(strTmpParam,_T(""));
					dwCnt++;
					strProv = strTmp;
					if (dwVarId >= 0)
					{
						strMigrateData.Replace(_T("$PProvId$"),strVarId);
					}
					else
					{
						strMigrateData.Replace(_T("$PProvId$"),_T(""));
						dwCnt++;
					}
				}
				else if (iType == 14)
				{
					if ( dwVarId >= 0 )
					{
						strMigrateData.Replace(_T("$CCityId$"),strVarId);
					}
					else
					{
						strMigrateData.Replace(_T("$CCityId$"),_T(""));
						dwCnt++;
					}
				}
				else if (iType == 31)
				{
					if (dwVarId >= 0)
					{
						strMigrateData.Replace(strTmpParam,strVarId);
					}
					else
					{
						strMigrateData.Replace(strTmpParam,_T(""));
						dwCnt++;
					}
				}
				continue;
			}
			else
			{
				//��������
				if (dwVarId >= 0)
				{
					strMigrateData.Replace(strTmpParam,strVarId);
				}
				else
				{
					dwCnt++;
					strMigrateData.Replace(strTmpParam,_T(""));  //û��ƥ��
				}
			}
		}
		else if (strTmpParam == _T("$sumpic$")
			|| strTmpParam == _T("$CCompPic$")
			|| strTmpParam == _T("$Tradcert$"))
		{
			UpLoadPic(strTmpParam,strTmp,strMigrateData,dwCnt);
		}
		else
		{
			//�Ǳ�������
			strMigrateData.Replace(strTmpParam,strTmp);
		}
	
	}

	if (vMigrateParam.size()>0)
	{
		return vMigrateParam.size() - dwCnt;
	}

	return 0;
}

/*
@breif ����ͨ����Ҷ�������
@param temp  ͨ���
@return ��������
*/
int CTaskProcess::KeyToVal( CStdString& temp )
{
	if (temp == _T("$PProv$"))
	{
		return 1;
	}
	else if (temp == _T("$SSex$"))
	{
		return 2;
	}
	else if (temp == _T("$CompanyKind$"))
	{
		return 4;
	}
	else if (temp == _T("$CCity$"))
	{
		return 14;
	}
	else if (temp == _T("$PersonsCount$"))
	{
		return 29;
	}
	else if (temp == _T("$MoneyInfo$"))
	{
		return 30;
	}
	else if (temp == _T("$CompanyTrade$"))					
	{
		return 5;
	}
	else if (temp == _T("$AAreaID$"))
	{
		return 31;
	}

	return 0;
}

/*
@brief ���ַ����л��з�����3���ģ�����ֻ��������
@param  strSource  Դ�ַ�����
@return 
*/
void CTaskProcess::ReplaceNewLine( CStdString &strSource )
{
	int iPosBegin;
	int iPosEnd;
	int iPosTmp;

	strSource = strSource.Trim();

	iPosTmp = strSource.Find(_T("\r\n\r\n\r\n\r\n"));
	//�����������з������滻��
	if (iPosTmp == -1)
	{
		return;
	}

	CStdString strBegin;
	CStdString strEnd;
	CStdString strTmp;

	strBegin = strSource.Left(iPosTmp);

	strTmp = strSource.Right(iPosTmp+4);

	ReplaceNewLine(strTmp);

	strSource = strBegin + _T("\r\n\r\n") + strTmp;
}

/*
@brief ��ȡ����ͼƬ�ϴ���������
@param strParam   ͨ�����
@param strParamValue ͨ�����ֵ;
@param strMigrateData Ҫ�滻���ַ���;
@param dwCnt �滻ʧ��������
*/
DWORD CTaskProcess::UpLoadPic( CStdString &strParam,CStdString &strParamValue,CStdString &strMigrateData,DWORD &dwCnt )
{
	//��ƷͼƬ�����ϴ���������
	CString strPicPath = strParamValue;
	if (PathFileExists(strPicPath))
	{
		CString serverPath = _T(""); //���ط�����·��
		CInternetHttp http;
		//����ȡ������վ�ϴ��������������·������
		if(http.LoginAndUploadFile(strPicPath,serverPath))  
		{
			strMigrateData.Replace(strParam,serverPath);
		}
		else
		{
			g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,_T("ͼƬ�ϴ�ʧ�ܣ�"));
			strMigrateData.Replace(strParam,_T(""));
			dwCnt++;
		}
	}
	else
	{
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,_T("ͼƬ·����%s�����ڣ�û�л�ȡ����ͨ�����%s ��ֵ,�滻�ɿգ� "),strPicPath.GetBuffer(), strParam.c_str());
		strMigrateData.Replace(strParam,_T(""));
		dwCnt++;
	}	

	return 0;
}

/*
@brief ��˾���ظ����滻����
@param strData  ���滻���ַ�����
@param strReplace Ҫ�滻���ַ��� 
@Return TRUE: �滻�ɹ�
*/
BOOL CTaskProcess::ReplaceCompName( CStdString &strData, CStdString &strReplace )
{
	T_LocalGlobalData *pGlobalData = NULL;
	pGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);
	
	for (int i =0; i<pGlobalData->vCompany.size()-1; ++i)
	{
		//�״��滻�ҵ�һ����
		if (strReplace == _T(""))
		{										
			if (strData.Find(pGlobalData->vCompany[i])!= -1)
			{
				strData.Replace(pGlobalData->vCompany[i],pGlobalData->vCompany[i+1]);
				g_log.Trace(LOGL_LOW,LOGT_PROMPT,__TFILE__,__LINE__,_T("��һ���滻��ԭ��˾����%s,�滻��˾��: %s"),pGlobalData->vCompany[i].c_str(),pGlobalData->vCompany[i+1].c_str());
				strReplace = pGlobalData->vCompany[i+1];
				return TRUE;
			}
		}
		else
		{
			//�Ѿ��滻����
			if (strReplace == pGlobalData->vCompany[i])
			{
				if (strData.Find(pGlobalData->vCompany[i])!= -1)
				{
					strData.Replace(pGlobalData->vCompany[i],pGlobalData->vCompany[i+1]);
					g_log.Trace(LOGL_LOW,LOGT_PROMPT,__TFILE__,__LINE__,_T("ԭ��˾����%s,�滻��˾��: %s"),pGlobalData->vCompany[i].c_str(),pGlobalData->vCompany[i+1].c_str());
					strReplace = pGlobalData->vCompany[i+1];

					return TRUE;
				}
			}
		}

	}

	return FALSE;
}

/*
@brief ��Win8����ϵͳ������Ȩ
*/
BOOL CTaskProcess::RaisePrivileges()
{
	HANDLE TokenHandle;
	TOKEN_PRIVILEGES t_privileges = { 0 };

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
	{
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &t_privileges.Privileges[0].Luid))
	{
		return TRUE;
	}

	t_privileges.PrivilegeCount = 1;
	t_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(TokenHandle, FALSE, &t_privileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		CloseHandle(TokenHandle);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*
@brief ����IE����ģʽ
@param eVer IE�汾
*/
void CTaskProcess::SetIECoreVersion(E_IeLimit eVer)
{
	HKEY hMainKey = HKEY_CURRENT_USER;
	if (IsWOW64())
	{
		hMainKey = HKEY_LOCAL_MACHINE;
	}
	TCHAR* path = _T("SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION");
	TCHAR* valueName = _T("Engine.exe");
	long version = 8888;
	HKEY hKey;
	DWORD dwDisposition;
	//��ȡIE�汾��ֵ
	switch (eVer)
	{
	case IE6:
		version = 6000;
		break;
	case IE7:
		version = 7000;
		break;
	case IE8:
		version = 8888;
		break;
	case IE9:
		version = 9999;
		break;
	case IE10:
		version = 10001;
		break;
	case IE11:
		version = 11001;
		break;
	default:
		version = 8888;
		break;
	}
	long ret = RegOpenKeyEx(hMainKey, path, 0, REG_LEGAL_OPTION, &hKey);
	if (ret != ERROR_SUCCESS)
	{

		ret = RegCreateKeyEx(hMainKey, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
		if (ret != ERROR_SUCCESS)
		{
			g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("д��IEģʽʧ�ܣ�IE�汾��%d"), version);
			return;
		}
	}

	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("д��IE�汾��%d"), version);

	ret = RegSetValueEx(hKey, valueName, NULL, REG_DWORD, (BYTE*)&version, sizeof(version));
	
	RegCloseKey(hKey);
	if (ret != ERROR_SUCCESS)
		return;
}

/*
@breif �ж��Ƿ�64λϵͳ
*/
BOOL CTaskProcess::IsWOW64()
{
	BOOL bRetVal = FALSE;
	SYSTEM_INFO si = { 0 };
	LPFN_PGNSI pGNSI = (LPFN_PGNSI)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if (pGNSI == NULL)
	{
		return FALSE;
	}
	pGNSI(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		bRetVal = TRUE;
	}
	return bRetVal;
}


int CTaskProcess::GetIEVersion()
{
	TCHAR szFilename[] = _T("mshtml.dll");

	int	  iVer = -1;
	DWORD dwMajorVersion = 0;
	DWORD dwHandle = 0;

	DWORD dwVerInfoSize = GetFileVersionInfoSize(szFilename, &dwHandle);

	if (dwVerInfoSize)
	{
		LPVOID lpBuffer = LocalAlloc(LPTR, dwVerInfoSize);
		if (lpBuffer)
		{
			if (GetFileVersionInfo(szFilename, dwHandle, dwVerInfoSize, lpBuffer))
			{
				VS_FIXEDFILEINFO * lpFixedFileInfo = NULL;
				UINT nFixedFileInfoSize = 0;
				if (VerQueryValue(lpBuffer, TEXT("\\"), (LPVOID*)&lpFixedFileInfo, &nFixedFileInfoSize) && (nFixedFileInfoSize))
				{					
					dwMajorVersion = HIWORD(lpFixedFileInfo->dwFileVersionMS);
					g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("��ǰIE�汾Ϊ��%d"), dwMajorVersion);
				}
			}
			LocalFree(lpBuffer);
		}
	}

	switch (dwMajorVersion)
	{
	case 6:
		iVer = 0;
		break;
	case 7:
		iVer = 1;
		break;
	case 8:
		iVer = 2;
		break;
	case 9:
		iVer = 3;
		break;
	case 10:
		iVer = 4;
		break;
	case 11:
		iVer = 5;
		break;
	default:
		iVer = 2;
		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("��ȡ��ǰIE�汾ʧ�ܣ�Ĭ��ΪIE8"));
		break;
	}

	return iVer;
}


