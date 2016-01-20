// SaveThread.cpp: implementation of the CSaveThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SaveThread.h"
#include "enumdef.h"
#include <stdlib.h>

#include "Reg.h"
#include <imagehlp.h>   
#include <shlobj.h>
#include <iostream>
#include <list>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CLogTrace g_MsgLog(_T("Msg_SaveModule.log"), NULL);

CSaveThread::CSaveThread()
:IThreadUnit(E_SAVE_THREAD,0xFFFF)
{
    m_SendCount = 0;
    m_dwRevCount = 0;

    m_pMapSaveAccountPass = NULL;
	m_hTimerTaskMutex = NULL;
}


CSaveThread::~CSaveThread()
{
   
}

///////////////////////////////////////////////////////////////////////////////
//
// ��������������Save�߳���Ϣ�ַ�
// ���룺
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
DWORD CSaveThread::DispatchMessage(T_Message *Msg)
{
    DWORD dwRet = 0;

    //����ڼ�¼���յ�����Ϣ(����ʱ����Ϣ��)
    if (Msg->dwMsgParam != MSG_TIMER)
    {
  //      g_MsgLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
            _T("RecvMsg, MsgParam: %d"), Msg->dwMsgParam);
    }

    switch (Msg->dwMsgParam)
    {

    case E_SAVE_TASKFINISHED://��������ɵĴ���
        {
            //TaskMgr��������Ϣ����
			SaveTaskResult(Msg->dwData);
	//		g_MsgLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
					_T("E_SAVE_TASKFINISHED"));
        }
        break;
	case E_SAVE_FIRSTDATA:
		//�����һ����������ݣ�
		SaveResultToGui(Msg->dwData);
		break;
    case E_PPD_NETCODELESSZERO_MSG://��վ���С��0
    case E_NEWERR://�����ڴ����
    case E_PPD_SERVERRETURNERR_MSG://����server��������ʧ��
    case E_PPD_NOWEBDATA_MSG://û����վ����
        {
            dwRet = PreDataFail(Msg->dwData);
            PostMessage(E_TASKMGR_THREAD,eTask_TaskFailed,Msg->dwData);
            g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,\
                _T("otherFailed failed, errcode: %d, id:%d"), Msg->dwMsgParam, Msg->dwData);
        }
        break;
    case MSG_TIMER: //ʱ����Ϣ
        {

        }
        break;

    default:
        g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,\
            _T("unknow msg, msgid: %d"), Msg->dwMsgParam);
        break;
    }
    return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
// ����������������Exserver���صĽ������,��������������
// ���룺
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
DWORD CSaveThread::SaveTaskResult(DWORD dwIndex)
{
    T_TaskData *pTaskData=(T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
        E_Save_CLASS, eUseCommReadWithMulti);

    if (NULL == pTaskData)//��ȡ��������Ϊ��,ֱ�ӷ�ʧ��
    {
        g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,\
            _T("1234_readrealdata failed"));
        PostMessage(E_TASKMGR_THREAD,eTask_TaskFailed,dwIndex);
        return 0;
    }

	DWORD dwRet = SaveResultToGui(dwIndex);
	if (200 == dwRet)
		PostMessage(E_TASKMGR_THREAD,eTask_TaskSucceed,dwIndex);
	else
		PostMessage(E_TASKMGR_THREAD,eTask_TaskFailed,dwIndex);

    return 1;
}

//�������ݵ�GUI
//����: ���͵�����
//���: 0:succ   other: failed
DWORD CSaveThread::SendDataToGUI(CStdString strInDtat)
{
    char *pSend = NULL;
    DWORD dwLen = strInDtat.size() * 2;
    _NEWA(pSend,char,dwLen + 2);

    if (NULL == pSend)
        return 1;
	
	pSend[dwLen + 1] = 0;
	pSend[dwLen] = 0;

    memcpy(pSend,strInDtat.c_str(),dwLen);
    int iSendOk = -1;
    while (TRUE)
    {
        iSendOk = g_ServerSocket->SendData(dwLen,E_GET_EXCUTE_TASK_RESULT,(char*)pSend);
        if (-1 != iSendOk)
        {
            m_SendCount ++;

			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
				_T("��ţ�%d ����GUI������: %s"), m_SendCount, strInDtat.c_str());

            printf("�����͵�GUI %d\n",m_SendCount);
            break;
        }
		else
		{
			Sleep(500);
		}
    }
	
    _DELETEA(pSend);


    return 0;
}

//�������ݵ�GUI, ��������׼��ʧ�ܵ���������ȥ
//����: ���͵�����
//���: 0:succ   other: failed
DWORD CSaveThread::SendDataToGUI(CStdString strInDtat, T_TaskData& TaskData)
{
	SendDataToGUI(strInDtat);
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
//
// ������������������׼������ʧ�ܵ�����
// ���룺
// �����
// ����ֵ��
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
DWORD CSaveThread::PreDataFail(DWORD dwIndex)
{
	T_TaskData *pTaskData = NULL;

	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_Save_CLASS, eUseCommReadWithMulti);


	if (!pTaskData)
	{
		printf("��������Ϊ�գ�����");
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,\
			_T("��������Ϊ��, �ڶ����: %d"), dwIndex);
		return 0;
	}

	pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_NETZIPPED_DATA_ERR;
	SaveResultToGui(dwIndex);

	return 100;
}

DWORD CSaveThread::CreateThread()
{
	return 0;
}

DWORD CSaveThread::DestoryThread()
{

	return 0;
}
 
DWORD CSaveThread::GetWebSiteName(T_TaskData *pTaskData,CStdString &strWebName, CStdString &strItemName)
{
	T_TaskNetItem *pNetStation =	NULL;
	std::map<DWORD,T_TaskNetItem*> *pMapStation = NULL;
	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pMapStationAndKey = NULL;

	pMapStationAndKey = (std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,MAG_STATION_INDEX,E_Save_CLASS,eUseBlockWithOne);

	if (!pMapStationAndKey)
	{
		g_GlobalDataBlock->Reset(E_MAP_NetStation,E_Save_CLASS);
		return 2;
	}
	pMapStation = (*pMapStationAndKey)[pTaskData->eTaskType];
	pNetStation = (*pMapStation)[pTaskData->dwWebID];
	if (pNetStation)
	{
		strWebName = pNetStation->szWebName;    //��վ����
		strItemName = pNetStation->szWebItemName;
	}
	else
	{
		if (0 == strWebName.size())
			strWebName = _T("unKnowName");
		
		if (0 == strItemName.size())
			strItemName = _T("unKnowName");
	}
	return 0;
}

DWORD CSaveThread::SaveResultToGui( DWORD dwIndex )
{
	T_TaskData *pTaskData = NULL;
	T_TaskResultInfo *pTaskResultInfo = NULL; 
	T_LocalGlobalData *pLocalGlobalData = NULL;

	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_Save_CLASS, eUseCommReadWithMulti);

	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);


	if (!pTaskData || !pLocalGlobalData)
	{
		printf("��������Ϊ�գ�����");
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,\
			_T("��������Ϊ��, �ڶ����: %d"), dwIndex);
		return 0;
	}

	pTaskResultInfo = &(pTaskData->tTaskResultInfo);
	
	//�������̵ĵ�ַ
	CStdString ShopUrl = _T("");
	if (pTaskResultInfo->eResultFlag != e_TASK_RESULT_SUCC)
	{
		ShopUrl = pLocalGlobalData->szShopTrafficUrl;
	}

	//��������ID
	CStdString strTaskId = _T("");
	if (pTaskData->tTaskNetItem.dwTaskId >= 0)
	{
		strTaskId.Format(_T("(;0)%d"), pTaskData->tTaskNetItem.dwTaskId);
	}

	//д�뷵�ظ�GUI�Ľ��
	CStdString strPostGui,strNetItemName,strWebName, strCodeOcr;
	GetWebSiteName(pTaskData,strWebName, strNetItemName);
	GetCodeOcrInfo(pTaskData, strCodeOcr);
	TCHAR szTemp[32] = {0};
	if (pTaskData->tTaskNetItem.dwNetStationItemID !=0 )	
		_i64tow(pTaskData->tTaskNetItem.dwNetStationItemID, szTemp, 10);

	switch(pTaskData->eTaskType)
	{
	case ETypeRefresh:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%s%s%s"),
				_T("BackResult(;0)Inforefr"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, szTemp,									//��վ��Ŀ���
				SIMPLE_LIST_SPLIT, strNetItemName.c_str(),						//��վ��Ŀ����
				SIMPLE_LIST_SPLIT, _T(""),										//�ɹ����
				SIMPLE_LIST_SPLIT, _T("")										//ʧ�ܱ��
				); 

			strPostGui.Format(_T("%s%s%s%s%d%s%s%s%s%s"), strPostGui, 
				SIMPLE_LIST_SPLIT, _T(""),                 //����·��
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//������
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//ʧ��ԭ�� 
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),									//��֤����Ϣ );
				strTaskId.c_str()
				);														//����ID
		}
		break;
	case ETypeDelete:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%s%s%s"),
				_T("BackResult(;0)InfoDele"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, szTemp,										//��վ��Ŀ���
				SIMPLE_LIST_SPLIT, strNetItemName.c_str(),						//��վ��Ŀ����
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strInfoID.c_str(),										//�ɹ����
				SIMPLE_LIST_SPLIT, _T("")										//ʧ�ܱ��
				); 

			strPostGui.Format(_T("%s%s%s%s%d%s%s%s%s%s"), strPostGui,
				SIMPLE_LIST_SPLIT, _T(""),                 //����·��
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//������
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//ʧ��ԭ�� 
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),										//��֤����Ϣ );
				strTaskId.c_str()
				);														//����ID
		}
		break;
	case ETTypePost:
		{
			DWORD dwResult = pTaskResultInfo->eResultFlag;
			if (pTaskData->bAddExtraIsSucc)
			{
				//�������ϳɹ���+1000
				dwResult += 1000;
			}
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%d%s%s"),
				_T("BackResult(;0)Infopost"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, szTemp,										//��վ��Ŀ���
				SIMPLE_LIST_SPLIT, strNetItemName.c_str(),						//��վ��Ŀ����
				SIMPLE_LIST_SPLIT, pLocalGlobalData->TaskDetailInfo.dwNnumber,  //�������
				SIMPLE_LIST_SPLIT, pLocalGlobalData->TaskDetailInfo.szSubject  //��������
				); 

			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%d%s%s%s"), strPostGui.c_str(),
				SIMPLE_LIST_SPLIT, dwResult,					//������
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strHtml.c_str(),                 //����·��		
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//ʧ��ԭ�� 
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//��֤����Ϣ 
				SIMPLE_LIST_SPLIT, pTaskData->siteType.eDisAfterPost,			//�Ƿ����� 
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strInfoID.c_str(),				//��ϢID;
				strTaskId.c_str()														//����ID
				);
		}
		break;
	case ETypeRegEmail:
	case ETTypeRegaccount:
	case ETTypeReRegaccount:
		{
			CStdString strRegType;
			if (pTaskData->eTaskType == ETypeRegEmail)
			{
				strRegType = _T("BackResult(;0)MailRegistry");
			}
			else if (pTaskData->eTaskType == ETTypeReRegaccount)
			{
				strRegType = _T("BackResult(;0)ReRegistry");
			}
			else
			{
				strRegType = _T("BackResult(;0)Registry");
			}
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%s%s%d%s%s%s%s"),
				strRegType,
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, pTaskData->strHost.c_str(),					//��վ��ַ
				SIMPLE_LIST_SPLIT, pTaskData->szAccount,                        //�˺�
				SIMPLE_LIST_SPLIT, pTaskData->szPassword,						//����
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//������
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//ʧ��ԭ��
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str()								//��֤����Ϣ 				
				); 

			strPostGui.Format(_T("%s%s"), strPostGui.c_str(),
				strTaskId.c_str()														//����ID
				);
		}
		break;

	case ETypeDomainpost:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s%s%s%d%s"),
				_T("BackResult(;0)Dominpost"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//������
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//ʧ��ԭ��
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//��֤����Ϣ 
				SIMPLE_LIST_SPLIT, pTaskData->siteType.eDisAfterPost,			//���Ա�־ 
				strTaskId.c_str()														//����ID
				); 
		}
		break;

	case ETypeWebopost:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s%s%s%d%s"),
				_T("BackResult(;0)webopost"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//������
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//ʧ��ԭ��
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//��֤����Ϣ 
				SIMPLE_LIST_SPLIT, pTaskData->siteType.eDisAfterPost,			//���Ա�־ 
				strTaskId.c_str()														//����ID
				); 
		}
		break;

	case ETypeAutoLogin:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d"),
				_T("BackResult(;0)login"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag				//������
				); 
		}
		break;
	case ETypeCompanyPopular:
		{
 			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%d%s%s%s%s%s"),
 				_T("BackResult(;0)CompanyPopularity"),
 				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
 				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
 				SIMPLE_LIST_SPLIT, pTaskData->strHost.c_str(),					//��վ��ַ
 				SIMPLE_LIST_SPLIT, pTaskResultInfo->strCompPopular.c_str(),					//֪����
 				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//������
 				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//ʧ��ԭ��
 				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//��֤����Ϣ 
				strTaskId.c_str()														//����ID
				); 
		}
		break;
	case ETypeMailCount:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%d%s%s%s%s%s%s%s"),
				_T("BackResult(;0)MailCount"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, pTaskData->strHost.c_str(),					//��վ��ַ
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//������
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//ʧ��ԭ��
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//��֤����Ϣ 
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strMailCount.c_str(),					//�ʼ�����
				strTaskId.c_str()														//����ID
				); 
		}
		break;
	case ETTypeUserInfoMigrate:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s%d%s%d%s"),
				_T("BackResult(;0) UserInfoMigrate"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, pTaskData->tTaskResultInfo.dwType,			//��������
				SIMPLE_LIST_SPLIT, pTaskData->tTaskResultInfo.strMigrateResult,  //Ǩ���ֶν��
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,				//������
				SIMPLE_LIST_SPLIT,pTaskData->tTaskResultInfo.dwSuccFieldCnt,		//�滻�ɹ��Ĺؼ�������
				strTaskId.c_str()														//����ID
				); 
		}
		break;
	case ETTypeModify:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s"),
				_T("BackResult(;0)InfoMody"),
				SIMPLE_LIST_SPLIT,pTaskData->tTaskNetItem.dwWebId,			//��վ���
				SIMPLE_LIST_SPLIT,strWebName.c_str(),						//��վ����
				SIMPLE_LIST_SPLIT,pTaskResultInfo->eResultFlag,				//������
				SIMPLE_LIST_SPLIT,pTaskResultInfo->strResultInfo.c_str(),	//ʧ��ԭ��
				strTaskId.c_str()											//����ID
				);
		}
		break;
	case ETTypeGetPdtList:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%d%s"),
				_T("BackResult(;0)GetProductList"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, pTaskData->tTaskResultInfo.strMigrateResult,  //Ǩ���ֶν��
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,				//������
				strTaskId.c_str()														//����ID
				); 
		}
		break;
	case ETTypeMailActivate:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%d%s%s%s"),
				_T("BackResult(;0)MailActivate"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
				SIMPLE_LIST_SPLIT, pTaskData->strHost.c_str(),					//��վ��ַ
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,				//������
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),		//ʧ��ԭ��
				strTaskId.c_str()														//����ID
				); 
		}
		break;
	case ETTypeShopTraffic:
	{				  
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s"),
				 _T("BackResult(;0)ShopTraffic"),
					SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//��վ���
					SIMPLE_LIST_SPLIT, strWebName.c_str(),							//��վ����
					SIMPLE_LIST_SPLIT, pTaskData->tTaskResultInfo.eResultFlag,					//������
					SIMPLE_LIST_SPLIT, ShopUrl.c_str(),			//ʧ�������б�
					strTaskId.c_str()														//����ID

					);
	}
		break;
	default:

		break;
	}
	

	/*g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, 
		_T("���ؽ��: %s"), strPostGui.c_str());*/

	if (strPostGui.length() <= 0)
	{
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, 
			_T("���ؽ��ʧ�ܡ���Ч��������"));
		return 0;
	}

	SendDataToGUI(strPostGui);

	return 200;
}

//���ݽ���֤��Ϣת���ɷ����ϲ��ʽ
void CSaveThread::GetCodeOcrInfo( T_TaskData *pTaskData, CStdString &strInfo )
{
	std::list<CStdString> &lCode = pTaskData->tTaskResultInfo.listOcrCode;
	size_t nCount = lCode.size();

	StringUtils strUtils;
	std::vector<CStdString> VecResult;

	DWORD dwCodeValue = 0;

	std::list<CStdString>::iterator iter = lCode.begin();
	for (; iter != lCode.end(); ++iter)
	{
		strInfo += *iter;
// 		VecResult.clear();
// 		strUtils.SplitString(*iter, _T("|"), VecResult, true); 
// 
// 		if (VecResult.size() == 5)
// 		{
// 			if (VecResult[0] == _T("1"))
// 			{
// 				if (VecResult[2] == VecResult[3])
// 				{
// 					dwCodeValue = AUTO_CORRECT_AND_NO_IN;
// 				}
// 				else
// 				{
// 					dwCodeValue = AUTO_ERROR_BUT_CORRECT_IN;
// 				}
// 			}
// 			else
// 			{
// 				if (VecResult[2] == VecResult[3])
// 				{
// 					dwCodeValue = AUTO_ERROR_BUT_NO_IN;
// 				}
// 				else
// 				{
// 					dwCodeValue = AUTO_ERROR_AND_ERROR_IN;
// 				}
// 			}
// 			strInfo.Format(_T("%d%s%s0,%s"), dwCodeValue, VecResult[1], VecResult[4].Left(1), strInfo);
// 		}
	}
	
	
	int iLen = strInfo.length();
	
	if (iLen > 0)  //���ȥ������Ķ���
	{
		CStdString strTmp = strInfo.Right(1);
		if (strTmp == _T(","))
		{
			strInfo = strInfo.Left(iLen - 1);
		}
	}
}
