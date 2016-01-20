#include "StdAfx.h"
#include "KernelProcessThread.h"

CKernelProcessThread  *gThreadMessage = NULL;

CKernelProcessThread::CKernelProcessThread(void)
:IThreadUnit(E_4KERNELWORK_THREAD,0xFFFF)	//ʮ���ӳ�ʱ
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
	case E_4KERNEL_TASK_IN: // ��������
		{
			g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,_T("�յ�����%d"), pMsg->dwData);
			return DoWorkToKernel(pMsg->dwData);

		}
		break;
	default:
		break;
	}
	return 0;
}


/*
	@breif  ��ʼ����
	@param  ��������
	*/
DWORD CKernelProcessThread::DoWorkToKernel( DWORD dwIndex )
{
	T_TaskData* pTaskData = NULL;

	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwIndex,
		E_PREPAREDATA_CLASS, eUseCommReadWithMulti);

	if (pTaskData != NULL)
	{
		BOOL bSingle = pTaskData->siteType.bIsObject;
		//����ǵ���IE�汾����ʹ����߰汾IE;
		if (taskData.iIEVersion < pTaskData->limitRefresh.eIeEdtion)
		{
			pTaskData->limitRefresh.eIeEdtion = (E_IeLimit)taskData.iIEVersion;
		}
		taskData.PushTaskIndex(dwIndex, bSingle,pTaskData->limitRefresh.eIeEdtion);

		if (bIsSetIE)
		{
			//һ��ʼ����IE�汾
			bIsSetIE = false;
			taskData.SetIEMode(pTaskData->limitRefresh.eIeEdtion);
		}

		if (bSingle && taskData.iOneKernel == 0)
		{
			//���߳�
			if (bIsObjectFirst)
			{
				bIsObjectFirst = false;
				taskData.StartWork(bSingle);
			}
		}
		else
		{
			//���߳�
			if (bIsFirst)
			{
				bIsFirst = false;
				taskData.StartWork();
			}
		
		}
	}



	return 0;
}