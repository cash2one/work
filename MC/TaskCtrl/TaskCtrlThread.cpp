// TaskMgrThread.cpp: implementation of the CTaskMgrThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaskCtrlThread.h"
#include <IPHlpApi.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#pragma comment(lib,"Iphlpapi.lib")

CTaskMgrThread::CTaskMgrThread()
{
    /*
    默认数据库关闭
    */
    m_DBState = e_close;

	m_dwGUIDataIndex = 0;
	m_dwFinishCount = 0;
	m_dwTotalTaskCount = 0;
	bIsSendComplete = FALSE;

    printf("创建TaskMgrThread\n");
}

CTaskMgrThread::~CTaskMgrThread()
{
	
}


//从ExSer返回结果，送到SaveThread进行保存和判定
DWORD CTaskMgrThread::RevExSerResult(DWORD dwIndex)
{
	TaskResult(dwIndex);
    return 0;
}

/*从SaveThread返回任务信息
但任务还未确认完成
主要是确定该任务之后是否有任务，比如哪种
*/
DWORD CTaskMgrThread::RevTaskSucceed(DWORD dwIndex)
{
	TaskSucceed(dwIndex);
    return 0;
}

//从SaveThread判定后返回任务失败
DWORD CTaskMgrThread::RevTaskFailed(DWORD dwIndex)
{
	TaskFialed(dwIndex);
    return 0;
}

DWORD CTaskMgrThread::DispatchMessage(T_Message *Msg)
{
    switch(Msg->dwMsgParam)
    {
    case MSG_RESETTHREAD:	//线程重启
        {
            RevThreadReset(Msg->dwData,Msg->dwSourWork);
        }
        break;
    case eTask_PushGUITask: //GUI压下任务
        {
            RevGuiMessage(Msg->dwData);
        }
        break;
    case eTask_PrepareFailed:
        {
            RevErrorPrepareData(Msg->dwData);
        }
        break;
    case eTask_PrepareFinished:
        {
            RevPrepareData(Msg->dwData);
        }
        break;
  
	case E_4KERNEL_TASK_OUT: //四代核完成结果
        {
			RevExSerResult(Msg->dwData);
        }
        break;
    case eTask_TaskFailed://从Save任务失败
        {
            RevTaskFailed(Msg->dwData);
			RemoveTaskData(Msg->dwData);
			m_dwFinishCount++;
        }
        break;
    case eTask_TaskSucceed: //从Save返回回来的任务完成确认信息
        {
            RevTaskSucceed(Msg->dwData); 
			RemoveTaskData(Msg->dwData);
			m_dwFinishCount++;
        }
        break;
    case E_PPD_CONNDBOK_MSG: 
        {
            m_DBState = e_open;
			g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__, _T("数据准备初始化完成\n"));
            //printf("数据准备初始化完成\n");
			PushNormalTaskData(m_dwGUIDataIndex); //把GUI缓存的任务分解出去
        }
        break;
    case E_PPD_GETNETDATAFROMSERVER_MSG:	//没有网站数据，需要更新   改变任务状态
        {
            RevPrepareNoWebData(Msg->dwData);				 
        }break;
    case E_PPD_SERVERRETURNDATA_MSG:	//网站更新成功
        {
            RevPrepareWebUpdateSucceed(Msg->dwData);
        }break;
    case E_PPD_SERVERRETURNERR_MSG:		//网站更新失败
        {
            RevPrepareWebUpdateFailed(Msg->dwData);
			PostMessage(E_SAVE_THREAD,E_PPD_SERVERRETURNERR_MSG,Msg->dwData);
        }break;
    case E_PPD_NETCODELESSZERO_MSG:		//网站已经下架
    case E_NEWERR:
	case E_PPD_NOWEBDATA_MSG:
        {
            RevErrorPrepareData(Msg->dwData);
            PostMessage(E_SAVE_THREAD,Msg->dwMsgParam,Msg->dwData);
        }break;

	case MSG_TIMER:
		{
			g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__,_T("所剩任务总数 = %05d 当前正在做的任务 = %5d 任务完成数 = %5d"),m_TaskDescMap.size(),m_TaskDoingCount,m_dwFinishCount);
			
		}
		break;
	}

	if (m_dwTotalTaskCount > 0 
		&& m_dwFinishCount == m_dwTotalTaskCount
		&& bIsSendComplete == FALSE)
	{
		//所有任务完成
		DelPhotoPageFolder();  //判断是否需要删除快照
		CString strMac = GetMacAddress();
		g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__,_T("Mac地址：%s"),strMac.GetBuffer(strMac.GetLength()));
		CStdString strBack;
		strBack.Format(_T("BackResult(;0)AllTaskComplete(;0)(;0)%s(;0)"),strMac.GetBuffer(strMac.GetLength()));
		strMac.ReleaseBuffer();
		BYTE * pByData = (BYTE *)strBack.c_str();

		g_Socket->SendData(strBack.size() * 2,E_GET_EXCUTE_TASK_RESULT,(char*)pByData);
		g_Socket->UnInit();
		bIsSendComplete = TRUE;

		g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__,_T("所有任务结束，主控退出"));

		ExitProcess(0);	//退出主控
	}

	SchedulerTask();	

    return 0; 
    //返回0代表消息被处理，返回-1代表消息不被处理,不被处理会被再次放入消息队列中
}

//GUI 下来任务
DWORD CTaskMgrThread::RevGuiMessage(DWORD dwData)
{
	//注册发布刷新删除都走这里

    DWORD Ret = PushNormalTaskData(dwData);	
    g_tskLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
        _T("压任务...."));

    return Ret;
}

//数据准备发过来需要更新网站数据，未能准备数据的一种
DWORD CTaskMgrThread::RevPrepareNoWebData(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_updatedata);	

	g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__,__LINE__, _T("网站需要更新数据 taskid:%d"),dwIndex);
    return 0;
}
//数据准备发来网站数据更新完成
DWORD CTaskMgrThread::RevPrepareWebUpdateSucceed(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_updatefinished);
	g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__,__LINE__, _T("网站更新数据完成 taskid:%d"),dwIndex);
	
    return 0;
}

//数据准备发来网站数据更新失败
DWORD CTaskMgrThread::RevPrepareWebUpdateFailed(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_updateerr);

	g_tskLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__,__LINE__, _T("网站更新数据失败 taskid:%d"),dwIndex);
    return 0;
}


DWORD CTaskMgrThread::RevErrorPrepareData(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_fialed);
    return 1;

}

//数据准备完成的任务 返回
DWORD CTaskMgrThread::RevPrepareData(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_dataprepared);
    return 0;
}
//线程重启处理 
DWORD CTaskMgrThread::RevThreadReset(DWORD dwIndex,DWORD dwThreadType)
{

    return 0;
}
//清除任务结构
DWORD CTaskMgrThread::RemoveTaskData(DWORD dwIndex)
{
	RemoveTask(dwIndex);
    return 0;
}

//modfiy by yzh
DWORD CTaskMgrThread::PushNormalTaskData(DWORD dwGUIDataIndex)
{
	m_dwGUIDataIndex = dwGUIDataIndex; //用于缓存任务

	T_LocalGlobalData *pLocalGlobalData = NULL;
	pLocalGlobalData=(T_LocalGlobalData*)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pLocalGlobalData != NULL)
	{
		if ((m_DBState == e_open && m_dwGUIDataIndex != 0)
			||(!pLocalGlobalData->bIsUpdate && m_dwGUIDataIndex != 0))
		{

			m_dwTotalTaskCount  += BuildTaskList(m_dwGUIDataIndex);

			m_dwGUIDataIndex = 0;
		}
		else
		{
			g_tskLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__,__LINE__, _T("网站数据还未准备好。"));
		}
	}
	

    return 0;
}

/*
@brief 获取Mac地址
@return Mac地址
*/
CString CTaskMgrThread::GetMacAddress()
{
	CString strMac = _T("");

	ULONG ulSize = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAdapterAddr = new IP_ADAPTER_ADDRESSES();

	int nRel = GetAdaptersAddresses(AF_INET,0,NULL,pAdapterAddr,&ulSize);

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		delete pAdapterAddr;

		//重新申请内存大小
		pAdapterAddr = (PIP_ADAPTER_ADDRESSES)new BYTE[ulSize];
		nRel = GetAdaptersAddresses(AF_INET,0,NULL,pAdapterAddr,&ulSize);
	}

	if (ERROR_SUCCESS == nRel)
	{
		if (pAdapterAddr->PhysicalAddressLength == 6)
		{
			strMac.AppendFormat(_T("%02X-%02X-%02X-%02X-%02X-%02X"),
				(int)pAdapterAddr->PhysicalAddress[0],
				(int)pAdapterAddr->PhysicalAddress[1],
				(int)pAdapterAddr->PhysicalAddress[2],
				(int)pAdapterAddr->PhysicalAddress[3],
				(int)pAdapterAddr->PhysicalAddress[4],
				(int)pAdapterAddr->PhysicalAddress[5]);
		}
	}

	if (pAdapterAddr)
	{
		delete pAdapterAddr;
	}

	return strMac;
}
