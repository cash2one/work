#pragma once
#include "IThreadUnit.h"
#include "ITaskPackage.h"
#include <map>


enum E_TASKSTATE {e_original,
				  e_dataprepare,
				  e_updatedata,
				  e_updateerr,
				  e_updatefinished,
				  e_dataprepared,
				  e_p2pdoing,
				  e_doing,
				  e_save,	
				  e_succeed,
				  e_fialed,
				  e_except
				};
enum E_SWITCH	{e_open,e_close};	//����״̬



struct T_TaskDesc		//��������
{	
	E_TASKSTATE TaskState;	//��ǰ����״̬
	DWORD dwCount;			//�������Դ���
	E_Task_Level eLevel;	//���ȼ���(1ע�ᣬ2������3������ע�ᣬ4������5������ע�ᣬ6������7���ݻ�δ׼����)
	DWORD dwPrvTaskID;		//������֮ǰ����ִ�е�����
	DWORD dwNextTaskID;		//������֮����Ҫִ�е�����
	DWORD ClientID;			//�ͻ���ID
	DWORD dwTaskIndex;		//���ݱ��(�ڲ�ID)
	DWORD dwWebId;			//��վ���(��Ҫ��������׼��������Ҫ���µ���վ��Ϣ)
	DWORD dwStartTime;		//����ִ�еĿ�ʼʱ��
	T_TaskData *Data;		//��ʵ���ݵ�ַ
	T_TaskDesc()
	{
		TaskState = e_original ;
		dwCount = e_close ;	
		eLevel = ETOtherEmpty ;
		dwPrvTaskID = 0 ;
		dwNextTaskID = 0 ;	
		ClientID = 0 ;
		dwTaskIndex = 0;
		dwWebId = 0;
		dwStartTime = -1;
		Data = NULL;
	}
};

typedef std::pair <DWORD,T_TaskDesc *> Task_Pair;


class CTaskManger:public IThreadUnit 
{
private:
	ITaskFile *m_TaskFile;
protected:
	long m_TaskDoingMax;
	long m_TaskDoingCount;
	std::map<DWORD,T_TaskDesc *> m_TaskDescMap; //����״̬�б�

private:
	BOOL SetTaskStartTime(DWORD dwTaskIndex,DWORD dwTime);						//��������ʼʱ��
	BOOL GetIdleTask(DWORD &dwTaskIndex,DWORD &dwLevel);						//���һ����������
	T_TaskDesc *GetTask(DWORD dwTaskIndex);										//���һ������������
	std::map<DWORD,DWORD> m_mapTaskCount;                                       //�����Ӧ�������͵�����
//	std::vector<DWORD> vNsCodeOfObject;											//�����������վID;
public:
	CTaskManger(void);
	virtual ~CTaskManger(void);
	DWORD BuildTaskList(DWORD dwGUIDataIndex);									//���������б�
	DWORD SchedulerTask();														//�������		
	DWORD TaskResult(DWORD dwTaskIndex);										//����ִ�з���
	DWORD TaskFialed(DWORD dwTaskIndex);										//����ʧ��
	DWORD TaskSucceed(DWORD dwTaskIndex);										//����ɹ�
	DWORD RemoveTask(DWORD dwTaskIndex);										//ɾ��һ������
	BOOL SetTaskState(DWORD dwTaskIndex,E_TASKSTATE eTaskState);				//�ı�����״̬

	void DelPhotoPageFolder();													//ɾ�������ļ��У�
	
};
