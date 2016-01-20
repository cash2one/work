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
// 函数功能描述：Save线程消息分发
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CSaveThread::DispatchMessage(T_Message *Msg)
{
    DWORD dwRet = 0;

    //在入口记录接收到的消息(除定时器消息外)
    if (Msg->dwMsgParam != MSG_TIMER)
    {
  //      g_MsgLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
            _T("RecvMsg, MsgParam: %d"), Msg->dwMsgParam);
    }

    switch (Msg->dwMsgParam)
    {

    case E_SAVE_TASKFINISHED://对任务完成的处理
        {
            //TaskMgr过来的信息保存
			SaveTaskResult(Msg->dwData);
	//		g_MsgLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
					_T("E_SAVE_TASKFINISHED"));
        }
        break;
	case E_SAVE_FIRSTDATA:
		//保存第一个任务块数据；
		SaveResultToGui(Msg->dwData);
		break;
    case E_PPD_NETCODELESSZERO_MSG://网站编号小于0
    case E_NEWERR://分配内存出错
    case E_PPD_SERVERRETURNERR_MSG://数据server更新数据失败
    case E_PPD_NOWEBDATA_MSG://没有网站数据
        {
            dwRet = PreDataFail(Msg->dwData);
            PostMessage(E_TASKMGR_THREAD,eTask_TaskFailed,Msg->dwData);
            g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,\
                _T("otherFailed failed, errcode: %d, id:%d"), Msg->dwMsgParam, Msg->dwData);
        }
        break;
    case MSG_TIMER: //时间消息
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
// 函数功能描述：对Exserver返回的结果处理,包括派生的任务
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CSaveThread::SaveTaskResult(DWORD dwIndex)
{
    T_TaskData *pTaskData=(T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
        E_Save_CLASS, eUseCommReadWithMulti);

    if (NULL == pTaskData)//读取任务数据为空,直接返失败
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

//发送数据到GUI
//输入: 发送的数据
//输出: 0:succ   other: failed
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
				_T("序号：%d 返回GUI的数据: %s"), m_SendCount, strInDtat.c_str());

            printf("任务发送到GUI %d\n",m_SendCount);
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

//发送数据到GUI, 保存数据准备失败的任务结果回去
//输入: 发送的数据
//输出: 0:succ   other: failed
DWORD CSaveThread::SendDataToGUI(CStdString strInDtat, T_TaskData& TaskData)
{
	SendDataToGUI(strInDtat);
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
//
// 函数功能描述：处理准备数据失败的任务
// 输入：
// 输出：
// 返回值：
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD CSaveThread::PreDataFail(DWORD dwIndex)
{
	T_TaskData *pTaskData = NULL;

	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex, 
		E_Save_CLASS, eUseCommReadWithMulti);


	if (!pTaskData)
	{
		printf("任务数据为空！！！");
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,\
			_T("任务数据为空, 内定编号: %d"), dwIndex);
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
		strWebName = pNetStation->szWebName;    //网站名称
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
		printf("任务数据为空！！！");
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__,\
			_T("任务数据为空, 内定编号: %d"), dwIndex);
		return 0;
	}

	pTaskResultInfo = &(pTaskData->tTaskResultInfo);
	
	//返回商铺的地址
	CStdString ShopUrl = _T("");
	if (pTaskResultInfo->eResultFlag != e_TASK_RESULT_SUCC)
	{
		ShopUrl = pLocalGlobalData->szShopTrafficUrl;
	}

	//返回任务ID
	CStdString strTaskId = _T("");
	if (pTaskData->tTaskNetItem.dwTaskId >= 0)
	{
		strTaskId.Format(_T("(;0)%d"), pTaskData->tTaskNetItem.dwTaskId);
	}

	//写入返回给GUI的结果
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
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, szTemp,									//网站栏目编号
				SIMPLE_LIST_SPLIT, strNetItemName.c_str(),						//网站栏目名称
				SIMPLE_LIST_SPLIT, _T(""),										//成功编号
				SIMPLE_LIST_SPLIT, _T("")										//失败编号
				); 

			strPostGui.Format(_T("%s%s%s%s%d%s%s%s%s%s"), strPostGui, 
				SIMPLE_LIST_SPLIT, _T(""),                 //快照路径
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//任务结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//失败原因 
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),									//验证码信息 );
				strTaskId.c_str()
				);														//任务ID
		}
		break;
	case ETypeDelete:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%s%s%s"),
				_T("BackResult(;0)InfoDele"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, szTemp,										//网站栏目编号
				SIMPLE_LIST_SPLIT, strNetItemName.c_str(),						//网站栏目名称
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strInfoID.c_str(),										//成功编号
				SIMPLE_LIST_SPLIT, _T("")										//失败编号
				); 

			strPostGui.Format(_T("%s%s%s%s%d%s%s%s%s%s"), strPostGui,
				SIMPLE_LIST_SPLIT, _T(""),                 //快照路径
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//任务结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//失败原因 
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),										//验证码信息 );
				strTaskId.c_str()
				);														//任务ID
		}
		break;
	case ETTypePost:
		{
			DWORD dwResult = pTaskResultInfo->eResultFlag;
			if (pTaskData->bAddExtraIsSucc)
			{
				//完善资料成功，+1000
				dwResult += 1000;
			}
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%d%s%s"),
				_T("BackResult(;0)Infopost"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, szTemp,										//网站栏目编号
				SIMPLE_LIST_SPLIT, strNetItemName.c_str(),						//网站栏目名称
				SIMPLE_LIST_SPLIT, pLocalGlobalData->TaskDetailInfo.dwNnumber,  //发布编号
				SIMPLE_LIST_SPLIT, pLocalGlobalData->TaskDetailInfo.szSubject  //发布主题
				); 

			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%d%s%s%s"), strPostGui.c_str(),
				SIMPLE_LIST_SPLIT, dwResult,					//任务结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strHtml.c_str(),                 //快照路径		
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//失败原因 
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//验证码信息 
				SIMPLE_LIST_SPLIT, pTaskData->siteType.eDisAfterPost,			//是否立显 
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strInfoID.c_str(),				//信息ID;
				strTaskId.c_str()														//任务ID
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
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, pTaskData->strHost.c_str(),					//网站网址
				SIMPLE_LIST_SPLIT, pTaskData->szAccount,                        //账号
				SIMPLE_LIST_SPLIT, pTaskData->szPassword,						//密码
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//任务结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//失败原因
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str()								//验证码信息 				
				); 

			strPostGui.Format(_T("%s%s"), strPostGui.c_str(),
				strTaskId.c_str()														//任务ID
				);
		}
		break;

	case ETypeDomainpost:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s%s%s%d%s"),
				_T("BackResult(;0)Dominpost"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//任务结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//失败原因
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//验证码信息 
				SIMPLE_LIST_SPLIT, pTaskData->siteType.eDisAfterPost,			//立显标志 
				strTaskId.c_str()														//任务ID
				); 
		}
		break;

	case ETypeWebopost:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s%s%s%d%s"),
				_T("BackResult(;0)webopost"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//任务结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//失败原因
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//验证码信息 
				SIMPLE_LIST_SPLIT, pTaskData->siteType.eDisAfterPost,			//立显标志 
				strTaskId.c_str()														//任务ID
				); 
		}
		break;

	case ETypeAutoLogin:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d"),
				_T("BackResult(;0)login"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag				//任务结果
				); 
		}
		break;
	case ETypeCompanyPopular:
		{
 			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%s%s%d%s%s%s%s%s"),
 				_T("BackResult(;0)CompanyPopularity"),
 				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
 				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
 				SIMPLE_LIST_SPLIT, pTaskData->strHost.c_str(),					//网站网址
 				SIMPLE_LIST_SPLIT, pTaskResultInfo->strCompPopular.c_str(),					//知名度
 				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//任务结果
 				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//失败原因
 				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//验证码信息 
				strTaskId.c_str()														//任务ID
				); 
		}
		break;
	case ETypeMailCount:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%d%s%s%s%s%s%s%s"),
				_T("BackResult(;0)MailCount"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, pTaskData->strHost.c_str(),					//网站网址
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,					//任务结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),										//失败原因
				SIMPLE_LIST_SPLIT, strCodeOcr.c_str(),							//验证码信息 
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strMailCount.c_str(),					//邮件数量
				strTaskId.c_str()														//任务ID
				); 
		}
		break;
	case ETTypeUserInfoMigrate:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s%d%s%d%s"),
				_T("BackResult(;0) UserInfoMigrate"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, pTaskData->tTaskResultInfo.dwType,			//任务类型
				SIMPLE_LIST_SPLIT, pTaskData->tTaskResultInfo.strMigrateResult,  //迁移字段结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,				//任务结果
				SIMPLE_LIST_SPLIT,pTaskData->tTaskResultInfo.dwSuccFieldCnt,		//替换成功的关键字数量
				strTaskId.c_str()														//任务ID
				); 
		}
		break;
	case ETTypeModify:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s"),
				_T("BackResult(;0)InfoMody"),
				SIMPLE_LIST_SPLIT,pTaskData->tTaskNetItem.dwWebId,			//网站编号
				SIMPLE_LIST_SPLIT,strWebName.c_str(),						//网站名称
				SIMPLE_LIST_SPLIT,pTaskResultInfo->eResultFlag,				//任务结果
				SIMPLE_LIST_SPLIT,pTaskResultInfo->strResultInfo.c_str(),	//失败原因
				strTaskId.c_str()											//任务ID
				);
		}
		break;
	case ETTypeGetPdtList:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%d%s"),
				_T("BackResult(;0)GetProductList"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, pTaskData->tTaskResultInfo.strMigrateResult,  //迁移字段结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,				//任务结果
				strTaskId.c_str()														//任务ID
				); 
		}
		break;
	case ETTypeMailActivate:
		{
			strPostGui.Format(_T("%s%s%d%s%s%s%s%s%d%s%s%s"),
				_T("BackResult(;0)MailActivate"),
				SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
				SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
				SIMPLE_LIST_SPLIT, pTaskData->strHost.c_str(),					//网站网址
				SIMPLE_LIST_SPLIT, pTaskResultInfo->eResultFlag,				//任务结果
				SIMPLE_LIST_SPLIT, pTaskResultInfo->strResultInfo.c_str(),		//失败原因
				strTaskId.c_str()														//任务ID
				); 
		}
		break;
	case ETTypeShopTraffic:
	{				  
			strPostGui.Format(_T("%s%s%d%s%s%s%d%s%s%s"),
				 _T("BackResult(;0)ShopTraffic"),
					SIMPLE_LIST_SPLIT, pTaskData->tTaskNetItem.dwWebId,				//网站编号
					SIMPLE_LIST_SPLIT, strWebName.c_str(),							//网站名称
					SIMPLE_LIST_SPLIT, pTaskData->tTaskResultInfo.eResultFlag,					//错误码
					SIMPLE_LIST_SPLIT, ShopUrl.c_str(),			//失败商铺列表
					strTaskId.c_str()														//任务ID

					);
	}
		break;
	default:

		break;
	}
	

	/*g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, 
		_T("返回结果: %s"), strPostGui.c_str());*/

	if (strPostGui.length() <= 0)
	{
		g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, 
			_T("返回结果失败、无效任务类型"));
		return 0;
	}

	SendDataToGUI(strPostGui);

	return 200;
}

//根据将验证信息转换成返回上层格式
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
	
	if (iLen > 0)  //最后去掉后面的逗号
	{
		CStdString strTmp = strInfo.Right(1);
		if (strTmp == _T(","))
		{
			strInfo = strInfo.Left(iLen - 1);
		}
	}
}
