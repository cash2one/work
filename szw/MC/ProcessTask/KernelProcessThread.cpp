#include "StdAfx.h"
#include "KernelProcessThread.h"

CKernelProcessThread  *gThreadMessage = NULL;

CKernelProcessThread::CKernelProcessThread(void)
:IThreadUnit(E_4KERNELWORK_THREAD,0xFFFF)	//十分钟超时
{
	bIsObjectFirst = true;
	bIsFirst = true;
	bIsSetIE = true;
	gThreadMessage = this;
}


CKernelProcessThread::~CKernelProcessThread(void)
{
}

DWORD CKernelProcessThread::CreateThread()
{

	return 0;
}
DWORD CKernelProcessThread::RestThread()
{
	return 0;
}
DWORD CKernelProcessThread::DestoryThread()
{
	return 0;
}

DWORD CKernelProcessThread::DispatchMessage(T_Message* pMsg)
{
	ASSERT(pMsg != NULL);

	switch (pMsg->dwMsgParam)
	{
	case E_4KERNEL_TASK_IN: // 任务数据
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("收到任务：%d"), pMsg->dwData);
			return DoWorkToKernel(pMsg->dwData);

		}
		break;
	default:
		break;
	}
	return 0;
}


/*
	@breif  开始任务
	@param  任务索引
	*/
DWORD CKernelProcessThread::DoWorkToKernel( DWORD dwIndex )
{
	T_TaskData* pTaskData = NULL;

	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pTaskData != NULL)
	{
		BOOL bSingle = pTaskData->siteType.bIsObject;

		//如果设置了OtherThread,则所有网站均以多线程执行；
		if (taskData.iOneKernel == 1)
			bSingle = FALSE;
		
		//如果是低于IE版本，则使用最高版本IE;
		if (taskData.iIEVersion < pTaskData->limitRefresh.eIeEdtion)
		{
			g_log.Trace(LOGL_LOW, LOGT_WARNING, __TFILE__, __LINE__, _T("网站ID:%d,需求IE：%d"), pTaskData->dwWebID, pTaskData->limitRefresh.eIeEdtion);
			pTaskData->limitRefresh.eIeEdtion = (E_IeLimit)taskData.iIEVersion;
		}
		taskData.PushTaskIndex(dwIndex, bSingle,pTaskData->limitRefresh.eIeEdtion);

		if (bIsSetIE)
		{
			//一开始设置IE版本
			bIsSetIE = false;
			taskData.SetIEMode(pTaskData->limitRefresh.eIeEdtion);
			taskData.m_eCurIeVer = pTaskData->limitRefresh.eIeEdtion;
		}

		if (bSingle)
		{
			//单线程
			if (bIsObjectFirst)
			{
				bIsObjectFirst = false;
				taskData.StartWork(bSingle);
			}
		}
		else
		{
			//多线程
			if (bIsFirst)
			{
				bIsFirst = false;
				taskData.StartWork();
			}
		
		}
	}



	return 0;
}