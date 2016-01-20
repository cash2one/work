#pragma once

#include "TaskProcess.h"

#define THREAD_MAX_COUNT 20

class CAllTaskData
{
public:
	CAllTaskData(void);
	virtual ~CAllTaskData(void);

	/*
	@breif������������������
	@param  dwIndex ��������
	@param  bSingle �Ƿ�Object��վ��
	*/
	void PushTaskIndex(DWORD dwIndex, BOOL bSingle = FALSE,E_IeLimit ieMode = IE8 );  

	/*
	@breif  ��ʼ���������߳�
	*/
	void StartWork(BOOL bIsOneThread = FALSE);  

	/*
	@breif  ����ĳ��վ�������
	@param  dwWebId ��վid
	@param  dwWebId �������
	*/
	void PushResult(DWORD dwWebId, const CStdString &strResult);

	/*
	@breif  ������վ���ݲ���
	@param  dwWebId ��վid
	*/
	void ContinueOperate(DWORD dwWebId);

	/*
	@brief �����еȴ���������վ ���ڵȴ�
	*/
	void ContinueAllOperate();

	/*
	@brief ������Object�ȴ���������վ ���ڵȴ�
	*/
	void ContinueObjectOperate();

	/*
	@breif ����IEģʽ
	*/
	void SetIEMode(E_IeLimit eVer);

private:
	//------------Object���� start-----
	/*
	@breif  ȡ�����������������
	@return ������������
	*/
	DWORD GetObjectTaskIndex(E_IeLimit eVer = IE8);

	//-------------Object���� end ---------

	/*
	@breif  ȡ�����������������
	@return ������������
	*/
	DWORD GetTaskIndex(E_IeLimit eVer=IE8);

	/*
	@breif ����4���ں�
	*/
	BOOL RunEngine(BOOL bSingleKernel = FALSE);

	/*
	@brief  ����������
	@param  dwWebId��վid
	@param  pTaskData��������
	@param *pTData����ִ�п�
	@return 0��ʾ�˲���ʧ�ܲ���Ҫ����ִ����ȥ��1��ʾ�˲���ɹ�����ִ����ȥ��2��ʾ����ʱ�����������ϣ��ص���һ��
	*/
	int HandleRetData(DWORD dwWebId, T_TaskData* pTaskData,const DWORD &dwFunType, CTaskData *pTData = NULL);


	/*
	@breif  ����վ��վid���¼��������
	@param  dwWebId ��վid
	*/
	void LinkWeiIdToEvent(DWORD dwWebId, HANDLE hEvent);

	/*
	@breif  �Ƴ�����վid���¼�����
	@param  dwWebId ��վid
	*/
	void RemoveWebIdEvent(DWORD dwWebId); 

	/*
	@breif  ȥ����վ�������
	@param  dwWebId ��վid
	*/
	void RemoveResult(DWORD dwWebId);

	/*
	@breif  ȡ����վ�������
	@param  dwWebId ��վid
	*/
	CStdString *GetResult(DWORD dwWebId);

	/*
	@breif  ������������󣬺����β����
	@param  dwWebId ��վid
	@param  strWebName  ��վ����
	@param  dwIndex  ��������
	@param  bSendStop ��ǰ�Ƿ���Ҫ��֪�Ĵ���ֹͣ������
	*/
	void HandleCompleteWeb(DWORD dwWebId, const CStdString &strWebName, DWORD dwIndex, bool bSendStop = true,bool bSingleThread = false);

	/*
	@breif  ����ͨ�����ȡ�ö��ֵ
	@param  strData Ҫ���ҵ��ַ���
	@param  strName ͨ�����
	@param  [out ]strValueֵ
	*/
//	void GetValuesByName(const CStdString &strData, const CStdString &strName, CStdString &strValue);

	/*
	@breif  ����ͨ�����ȡ��ֵ*/
	bool CAllTaskData::FindEvent( DWORD dwWebId );

	/*
	@breif  ��ȡ������������
	@param	*/
	void GetCurTaskType(E_TASK_TYPE &eType,E_StationFunction &curFunction,bool &bSendStop);

	/*
	@breif  ִ�з������ݲ�����������
	@param	pTData ִ���������ݣ�
	@param  pTaskData �������ݽṹ��
	@param  curStep ��ǰִ�еڼ������ܿ飻
	@param  iFunCount  ���ܿ�������
	@param	strPreCompName ��˾���ƣ�
	@param  curFunction  ��ǰ�������ͣ�
	return	����Ҫִ�еĲ���
	*/
	DWORD SendTaskData(CTaskData *pTData,T_TaskData* pTaskData,long &curStep,E_StationFunction &curFunction,
		DWORD &dwRet,CStdString &strTaskData, CommonParam &cParam);

	/*
	@brief ����һ����ҳɹ�ʱ����
	@param dwTaskType �������ͣ�
	@param dwPdtCnt   ��Ʒ������
	@param pTask	  �������ݣ�
	@return TRUE :û�в�Ʒ��Ҫ��ȡ��
	*/
	BOOL ProcessSuccMigrate(DWORD dwTaskType, DWORD dwPdtCnt, T_TaskData *pTask,DWORD dwIndex);

	/*
	@brief �����ݳ�ʼ����
	*/
	void InitData();
	/*
	@brief doTaskǰ��׼����
	@param iSingle �Ƿ�Ҫ����2���ںˣ�
	@param eType   �������ͣ�
	@param dwWebId ��վID;
	*/
	BOOL DoPreJob(int iSingle,DWORD dwWebId, E_TASK_TYPE eType,CommonParam &cParam);

	/*
	@brief dotask��DoSingleTask��ͬ�Ĳ��֣�
	@param dwIndex  ����������
	@param hEvent   �¼�����
	@param bSingle  ���߳�����
	*/
	DWORD DoCommonTask(DWORD dwIndex, HANDLE hEvent, BOOL bSingle,bool &isRestartEngine);

	/*
	@breif �жϵ�ǰ�汾��������վ�Ƿ�������ɣ�
	@param eIever ie�汾��
	@return TRUE ����ɣ�False δ���
	*/
	BOOL CheckRunOver(E_IeLimit eIever, BOOL bSingle);

	/*
	@breif �ж϶��߳���վ�Ƿ���������ִ�У�
	*/
	BOOL CheckMultiTask();

private:
	std::map<E_IeLimit,std::queue<DWORD>>  m_allQueueTask;       //�������������������������
	std::map<E_IeLimit,std::queue<DWORD>>	m_objectQueueTask;	  //Object�̵߳�����
	std::map<DWORD, HANDLE>  IndexToEvent;      //ÿ������ͬ���¼�  ��վid��Ӧ�¼�
	std::map<DWORD, CStdString*>   webIdToResult;   //��վid��Ӧ������
	std::map<DWORD, HANDLE> indexToObjectEvent; //Object��վ��Ӧ�¼��źţ�

	HANDLE   hEvents[THREAD_MAX_COUNT];        //�̶߳�Ӧ���¼�
	HANDLE   hThreads[THREAD_MAX_COUNT];       //�����߳�

	HANDLE	 m_hEvent;					//���̶߳�Ӧ�¼�
	HANDLE   m_hThread;					//���������߳�
	CLock  m_TaskLock;                  //������
	CLock  m_EventLock;                 //�¼���
	CLock  m_TaskResultLock;           //������˵����
	HANDLE m_hndEngine;    //�ں˾��
	HANDLE m_hndSingleEngine; //���߳������ں˾����


	long  lIsRunEngine;         //�Ƿ���������4���ˣ�����0Ϊ��������
	int iWaitTime;

	//�ں�ͨѶ begin--
	CCommServer m_Connection ;
	int m_nPort ;   //���ں�ͨѶ�˿ں�
	bool   bRun;      //�߳������Ƿ�����
	//����������
	long lngKernelCnt;	//�ں���
	CCommServer m_ConnectionByObject;
	int m_ObjPort;

	
/*	DWORD  m_dwFunType;       //��������*/
	long   lngRunObject;         //object������վ������
	long   lngRunningCount;     //�����е�������
	long   lngCheckObjectWeb;   //������ڼ��OBJECT�����ȵȴ���

	E_IeLimit m_eCurIeVer;     //��ǰIE�汾
	BOOL	bObjectRunFinish;  //�ж�Object���������Ƿ����

	CStdString strMailBak;		//��������
	CStdString strMail;			//ԭ������

public:

	/*
	@breif  �����߳�
	@param  thisָ��
	*/
	static DWORD WINAPI DoTask(LPVOID lp);       //�����߳�
	static DWORD WINAPI DoSingleTask(LPVOID lp);  //���߳�����

	CNetData m_NetData;
	CTaskProcess m_Taskprocess;  //���������װ��
	int  iOneKernel; //�����Ƿ�Ҫ����˫�ں˲���,Ĭ����˫�ںˣ�0��
	int  iIEVersion; //IE�汾��
};


DWORD GetData(TCHAR *Orgbuf, DWORD dwTotalLen) ;
DWORD GetObjectData(TCHAR *Orgbuf, DWORD dwTotalLen);