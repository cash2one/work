// IThreadUnit.cpp: implementation of the IThreadUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IThreadUnit.h"
#include "IThreadManage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IThreadUnit::IThreadUnit(DWORD dwThreadType,DWORD dwTimeOut)
{
	m_dwThreadType = dwThreadType;
	m_dwTimeOut = dwTimeOut;
	m_isRest = FALSE;
	m_Busy = FALSE;
}

IThreadUnit::~IThreadUnit()
{
	
}


DWORD IThreadUnit::Create(IMsgQueue *pIMsgQueue)
{
	m_CurMsg.EnterTime = 0xFFFFFFFF; //0xFFFFFFFFʱ���ʾû����Ϣ
	ExitFlag = TRUE;	//��ʾ����
	m_MsgQueue = pIMsgQueue;
	DWORD dwThreadID = 0;
	
	//��������ͬ����Ϣ�¼����
	m_synMsgHandle = ::CreateEvent(NULL,TRUE,TRUE,NULL);

	m_hHandle = ::CreateThread(NULL, 0, 
		StaticThreadFunc, 
		this, 0, &dwThreadID);
	
    if (!m_hHandle || !dwThreadID) //�̴߳���ʧ��
    {
		::MessageBoxW(NULL,_T("�����߳�ʧ��"),_T("IThreadUnit Error"),MB_OK);
		return 1;
    }

	return 0;
}

BOOL IThreadUnit::Terminated()
{
	ExitFlag = FALSE;
	if(WaitForSingleObject(m_hHandle,200)==WAIT_TIMEOUT)
	{
		m_CurMsg.EnterTime = 0xFFFFFFFF; 
		//�߳�ֹͣ��ʱ ǿ��ֹͣ
		if(TerminateThread(m_hHandle,0)==FALSE)
			return FALSE;
	}
	CloseHandle(m_hHandle);

	//�ͷ�����ͬ����Ϣ�¼����
	SetEvent(m_synMsgHandle);
	CloseHandle(m_synMsgHandle);

	return TRUE;
}

DWORD WINAPI IThreadUnit::StaticThreadFunc(LPVOID lpParam)
{
	CoInitialize(NULL);
	IThreadUnit * pThread = (IThreadUnit *)lpParam;
    if (pThread)
    {
		if(pThread->m_isRest == FALSE)
			pThread->CreateThread();
		else
			pThread->RestThread();
		while(pThread->ExitFlag)
		{
			pThread->DefaultProc();
			if(pThread->m_dwThreadType == 0xFFFF) //�̵߳�Ԫ�����޷�ʹ��
				return 0;
			//Ĭ��һ���̴߳���5����Ϣ�����һ���߳��л�
			for(int i=0;i<3;i++)
			{
				pThread->m_MsgQueue->lock();
				T_Message *pMsg = pThread->m_MsgQueue->GetMsg(pThread->m_dwThreadType);
				pThread->m_MsgQueue->unlock();
				if(pMsg!=NULL)
				{
					::CopyMemory(&(pThread->m_CurMsg),pMsg,sizeof(T_Message));
					 
					DWORD dwRet = pThread->DispatchMessage(pMsg); //����Ϣ���з�������
		 
					if(pMsg->dwMsgType == 1)
						::SetEvent(pMsg->hSynHandle);//ͬ����Ϣ�������¼�����

					if(dwRet == -1) //����-1����ʾû�д�����Ϣ���ٽ���Ϣ���ص�������
					{
						pThread->m_MsgQueue->lock();
						pThread->m_MsgQueue->PostFrontMsg(pMsg);
						pThread->m_MsgQueue->unlock();
					}
					else
					{
						pThread->m_CurMsg.EnterTime = 0xFFFFFFFF; //0xFFFFFFFFʱ���ʾû����Ϣ
						IMsgQueue::Free_Message(pMsg); //��������֮������ͷ�
					}

				}
				else	//û����Ϣ��ϵͳ�л��߳�
				{
					break;
				}
			}
			Sleep(60);
		}
		pThread->DestoryThread();
    } 
	CoUninitialize ();
	return 0;
}
//�첽��Ϣ����
void IThreadUnit::PostMessage(DWORD dwDestWork,DWORD dwMsgParam, DWORD dwData,DWORD dwLevel)
{
	m_ThreadManage->PostMessage(m_dwThreadType,dwDestWork,dwMsgParam,dwData,0,0,dwLevel);
}
//ͬ����Ϣ����
void IThreadUnit::SendMessage(DWORD dwDestWork,DWORD dwMsgParam, DWORD dwData)
{
	::ResetEvent(m_synMsgHandle);
	m_ThreadManage->PostMessage(m_dwThreadType,dwDestWork,dwMsgParam,dwData,1,m_synMsgHandle,1);//1����Ϣ
	::WaitForSingleObject(m_synMsgHandle,INFINITE); //ͬ���ȴ�
}