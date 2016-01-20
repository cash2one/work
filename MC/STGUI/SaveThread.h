#pragma once

#include "IThreadUnit.h"
#include "IServerSocket.h"
#include "IWebOper.h"

#define TIMER_TASK_SIGNAL_PREFIX  _T("_BIZ_EXPRESS_TIMER_TASK_ID_")    //��ʱ����ִ�б�ǵ�ǰ׺����������ں���Ӷ�ʱ����ID��


class CSaveThread  :public IThreadUnit
{
public:
	CSaveThread();
	virtual ~CSaveThread();
	DWORD m_SendCount;
	DWORD m_dwRevCount;
private:
	HINSTANCE m_histWeb;
	IWebOper *m_pIWebOper;
public:
	//��Ϣ�ַ�
    DWORD DispatchMessage(T_Message *Msg);

	DWORD CreateThread();

	DWORD RestThread()
	{
		DestoryThread();
		CreateThread();
		return 0;
	}

    DWORD DestoryThread();

private:
	
    //����������ӿ�
    DWORD SaveTaskResult(DWORD dwIndex);

	//�����������������ϲ�
	DWORD SaveResultToGui(DWORD dwIndex);

	//���ݽ���֤��Ϣת���ɷ����ϲ��ʽ
	void GetCodeOcrInfo(T_TaskData *pTaskData, CStdString &strInfo);
	
	//����׼������ʧ�ܵ�����
	DWORD PreDataFail(DWORD dwIndex);


    //������GUI
	DWORD SendDataToGUI(CStdString strInDtat);


	//������GUI,��Ҫ��������������ݵ��������ݿ�
	DWORD SendDataToGUI(CStdString strInDtat, T_TaskData& TaskData);

private:
      
	std::map<DWORD,T_SaveToServerAccount*> *m_pMapSaveAccountPass;

	CStdString m_strTimerTaskID;         //��ʱ����ID
	DWORD m_dwCurrentTaskItemCount;      //��������
	CStdString m_strTimerTaskFlag;       //��Ƕ�ʱ��������
	HANDLE m_hTimerTaskMutex;            //�Զ�ʱ����IDΪ��׺��ȫ��Mutex��ǣ���NULL��ʾ��ʱ��������ִ��


	DWORD GetWebSiteName(T_TaskData *pTaskData,CStdString &strWebName, CStdString &strItemName);
};


