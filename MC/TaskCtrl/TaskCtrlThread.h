#pragma once

#include "TaskManger.h"


class CTaskMgrThread :public CTaskManger 
{
private:
	E_SWITCH m_DBState;		//�������ݿ�״̬
	DWORD m_dwGUIDataIndex; //GUIѹ�������ݻ���
	DWORD m_dwFinishCount;  //��ɵ��������
	DWORD m_dwTotalTaskCount;  //ҳ����ѹ�������ۼ�����
	BOOL  bIsSendComplete;

public:
	CTaskMgrThread();
	virtual ~CTaskMgrThread();
	//�̳߳�ʼ��
	virtual DWORD CreateThread(){return 0;}
	//�߳��˳�
	virtual DWORD DestoryThread(){return 0;}	
	//��Ϣ����
	DWORD DispatchMessage(T_Message *Msg);
private:
 
	DWORD RevGuiMessage(DWORD dwData); //Gui������������Ϣ
	
	DWORD RevErrorPrepareData(DWORD dwIndex); //����׼��ʧ��

	DWORD RevPrepareData(DWORD dwIndex); //�����Ѿ�׼���õĻ�������Ϣ

	DWORD RevPrepareNoWebData(DWORD dwIndex);	//����׼����������Ҫ������վ���ݣ�δ��׼�����ݵ�һ��

	DWORD RevPrepareWebUpdateSucceed(DWORD dwWebId); //����׼��������վ���ݸ������
	
	DWORD RevPrepareWebUpdateFailed(DWORD dwWebId); //����׼��������վ���ݸ���ʧ��

	DWORD RevExSerResult(DWORD dwIndex);	//�����ExSerִ�к󷵻�


	DWORD RevTaskSucceed(DWORD dwIndex);	//������ɴ�SaveThreadȷ�Ϸ���

	DWORD RevTaskFailed(DWORD dwIndex);		//����ʧ��

	DWORD RevThreadReset(DWORD dwIndex,DWORD dwThreadType);	//�߳�����

	DWORD RemoveTaskData(DWORD dwIndex);	//�������꣬����������ݽṹ

    DWORD PushNormalTaskData(DWORD dwGUIDataIndex);	//add by yzh

	CString GetMacAddress();  //��ȡmac��ַ��
};


