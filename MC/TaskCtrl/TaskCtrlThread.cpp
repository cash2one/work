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
    Ĭ�����ݿ�ر�
    */
    m_DBState = e_close;

	m_dwGUIDataIndex = 0;
	m_dwFinishCount = 0;
	m_dwTotalTaskCount = 0;
	bIsSendComplete = FALSE;

    printf("����TaskMgrThread\n");
}

CTaskMgrThread::~CTaskMgrThread()
{
	
}


//��ExSer���ؽ�����͵�SaveThread���б�����ж�
DWORD CTaskMgrThread::RevExSerResult(DWORD dwIndex)
{
	TaskResult(dwIndex);
    return 0;
}

/*��SaveThread����������Ϣ
������δȷ�����
��Ҫ��ȷ��������֮���Ƿ������񣬱�������
*/
DWORD CTaskMgrThread::RevTaskSucceed(DWORD dwIndex)
{
	TaskSucceed(dwIndex);
    return 0;
}

//��SaveThread�ж��󷵻�����ʧ��
DWORD CTaskMgrThread::RevTaskFailed(DWORD dwIndex)
{
	TaskFialed(dwIndex);
    return 0;
}

DWORD CTaskMgrThread::DispatchMessage(T_Message *Msg)
{
    switch(Msg->dwMsgParam)
    {
    case MSG_RESETTHREAD:	//�߳�����
        {
            RevThreadReset(Msg->dwData,Msg->dwSourWork);
        }
        break;
    case eTask_PushGUITask: //GUIѹ������
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
  
	case E_4KERNEL_TASK_OUT: //�Ĵ�����ɽ��
        {
			RevExSerResult(Msg->dwData);
        }
        break;
    case eTask_TaskFailed://��Save����ʧ��
        {
            RevTaskFailed(Msg->dwData);
			RemoveTaskData(Msg->dwData);
			m_dwFinishCount++;
        }
        break;
    case eTask_TaskSucceed: //��Save���ػ������������ȷ����Ϣ
        {
            RevTaskSucceed(Msg->dwData); 
			RemoveTaskData(Msg->dwData);
			m_dwFinishCount++;
        }
        break;
    case E_PPD_CONNDBOK_MSG: 
        {
            m_DBState = e_open;
			g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__, _T("����׼����ʼ�����\n"));
            //printf("����׼����ʼ�����\n");
			PushNormalTaskData(m_dwGUIDataIndex); //��GUI���������ֽ��ȥ
        }
        break;
    case E_PPD_GETNETDATAFROMSERVER_MSG:	//û����վ���ݣ���Ҫ����   �ı�����״̬
        {
            RevPrepareNoWebData(Msg->dwData);				 
        }break;
    case E_PPD_SERVERRETURNDATA_MSG:	//��վ���³ɹ�
        {
            RevPrepareWebUpdateSucceed(Msg->dwData);
        }break;
    case E_PPD_SERVERRETURNERR_MSG:		//��վ����ʧ��
        {
            RevPrepareWebUpdateFailed(Msg->dwData);
			PostMessage(E_SAVE_THREAD,E_PPD_SERVERRETURNERR_MSG,Msg->dwData);
        }break;
    case E_PPD_NETCODELESSZERO_MSG:		//��վ�Ѿ��¼�
    case E_NEWERR:
	case E_PPD_NOWEBDATA_MSG:
        {
            RevErrorPrepareData(Msg->dwData);
            PostMessage(E_SAVE_THREAD,Msg->dwMsgParam,Msg->dwData);
        }break;

	case MSG_TIMER:
		{
			g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__,_T("��ʣ�������� = %05d ��ǰ������������ = %5d ��������� = %5d"),m_TaskDescMap.size(),m_TaskDoingCount,m_dwFinishCount);
			
		}
		break;
	}

	if (m_dwTotalTaskCount > 0 
		&& m_dwFinishCount == m_dwTotalTaskCount
		&& bIsSendComplete == FALSE)
	{
		//�����������
		DelPhotoPageFolder();  //�ж��Ƿ���Ҫɾ������
		CString strMac = GetMacAddress();
		g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__,_T("Mac��ַ��%s"),strMac.GetBuffer(strMac.GetLength()));
		CStdString strBack;
		strBack.Format(_T("BackResult(;0)AllTaskComplete(;0)(;0)%s(;0)"),strMac.GetBuffer(strMac.GetLength()));
		strMac.ReleaseBuffer();
		BYTE * pByData = (BYTE *)strBack.c_str();

		g_Socket->SendData(strBack.size() * 2,E_GET_EXCUTE_TASK_RESULT,(char*)pByData);
		g_Socket->UnInit();
		bIsSendComplete = TRUE;

		g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT,__TFILE__, __LINE__,_T("������������������˳�"));

		ExitProcess(0);	//�˳�����
	}

	SchedulerTask();	

    return 0; 
    //����0������Ϣ����������-1������Ϣ��������,��������ᱻ�ٴη�����Ϣ������
}

//GUI ��������
DWORD CTaskMgrThread::RevGuiMessage(DWORD dwData)
{
	//ע�ᷢ��ˢ��ɾ����������

    DWORD Ret = PushNormalTaskData(dwData);	
    g_tskLog.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__,\
        _T("ѹ����...."));

    return Ret;
}

//����׼����������Ҫ������վ���ݣ�δ��׼�����ݵ�һ��
DWORD CTaskMgrThread::RevPrepareNoWebData(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_updatedata);	

	g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__,__LINE__, _T("��վ��Ҫ�������� taskid:%d"),dwIndex);
    return 0;
}
//����׼��������վ���ݸ������
DWORD CTaskMgrThread::RevPrepareWebUpdateSucceed(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_updatefinished);
	g_tskLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__,__LINE__, _T("��վ����������� taskid:%d"),dwIndex);
	
    return 0;
}

//����׼��������վ���ݸ���ʧ��
DWORD CTaskMgrThread::RevPrepareWebUpdateFailed(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_updateerr);

	g_tskLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__,__LINE__, _T("��վ��������ʧ�� taskid:%d"),dwIndex);
    return 0;
}


DWORD CTaskMgrThread::RevErrorPrepareData(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_fialed);
    return 1;

}

//����׼����ɵ����� ����
DWORD CTaskMgrThread::RevPrepareData(DWORD dwIndex)
{
	SetTaskState(dwIndex,e_dataprepared);
    return 0;
}
//�߳��������� 
DWORD CTaskMgrThread::RevThreadReset(DWORD dwIndex,DWORD dwThreadType)
{

    return 0;
}
//�������ṹ
DWORD CTaskMgrThread::RemoveTaskData(DWORD dwIndex)
{
	RemoveTask(dwIndex);
    return 0;
}

//modfiy by yzh
DWORD CTaskMgrThread::PushNormalTaskData(DWORD dwGUIDataIndex)
{
	m_dwGUIDataIndex = dwGUIDataIndex; //���ڻ�������

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
			g_tskLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__,__LINE__, _T("��վ���ݻ�δ׼���á�"));
		}
	}
	

    return 0;
}

/*
@brief ��ȡMac��ַ
@return Mac��ַ
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

		//���������ڴ��С
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
