#pragma once
#include "IThreadUnit.h"
#include "Sysparam.h"
#include "PrepareData.h"


class CPrePareDataThread :public IThreadUnit
{
public:
	CPrePareDataThread(void);
	~CPrePareDataThread(void);
	//�̳߳�ʼ��
	DWORD CreateThread();
	//�߳��˳�
	virtual DWORD DestoryThread();	
	//��Ϣ������
	virtual DWORD DispatchMessage(T_Message *Msg);
	//������webservice��Ϣ
	void ProcessServerDataMsg(DWORD dwNetId, DWORD dwIndex);

	//��ȡϵͳ��Ϣ����
	DWORD GetSysparam(TRapalceDataMsg& tRapalceData, DWORD dwIndex);


	//ϵͳ��Ϣ���Ժ���
	DWORD GetSysparamForTest(TRapalceDataMsg& tRapalceData);
	//��ȡ��������վ���ݸ���ʱ��
	int GetServerDbTime(DWORD dwIndex);
	int GetServerEnineDataDbTime(DWORD dwIndex);
	//�ж���վ�����ǲ�������
	int NetDataIsNew(DWORD dwNetId);
	//��ȡ�ʺ�����
	int GetIdAndPsd(DWORD dwNetId, T_TaskData* pTask);

	//�����ȡ����������ʱ����Ϣ
	int ProcessGetServerDataTimeMsg(DWORD dwIndex);

	//�������netstation����Ϣ
	int ProcessUpDateNetStationMsg(DWORD dwIndex);

	//�����ȡ��������ʧ����Ϣ
	int ProcessUpDateErrMsg(DWORD dwIndex);
	//�����߳�������Ϣ
	int ProcessResetThreadMsg(T_Message *Msg);

	//����Ԥ����
	BOOL PreProcessTaskData(DWORD dwIndex,TRapalceDataMsg& tTRapalceDataMsg, BOOL bUpdateDataFlag);
	//����������
	int PrecessTaskData(DWORD dwIndex, TRapalceDataMsg &tTRapalceDataMsg);

	//�ͷ��ڴ�ͱ������
	int EmptyFun();

	int GetEngineDataFileLastUpdateALL(const CStdString &strData,T_ServerNetStationInfoGroup &ServerLastupdateGroup);
	bool CheckConnDBfinished(void);

	//�ж���վ�Ƿ����£���Ļ�ֱ�����أ�
	DWORD CheckWebIsNewAndDown(DWORD dwWebId, DWORD dwIndex, BOOL bUpdateFlag, BOOL bAlreadyUpdate = TRUE);

private:
	CSysparam m_Sysparam;//ϵͳ��Ϣ����
	CPrepareData m_PrepareData;//׼�������
	BOOL m_bAllNetStationLastUpdateTimeGetted; //������վ��������ʱ���ѻ�ȡ

//	std::map<DWORD,DWORD> m_NetUpdate ; //m_NetUpdateinfo �Ӽ�
	std::map<DWORD,std::vector<DWORD>> m_NetUpdateinfo;	//������Ϣ�б����ڴ��������վ���ݵ�WEBID��TASKID�Ķ�Ӧ��ϵ
	std::map<DWORD,DWORD> m_DBRefrshTimeInfo;	//���¸���ʱ�䣬���ڴ�WEBID��ServerTime��Ӧ��ϵ
    std::map<DWORD,DWORD> m_EngineDBRefrshTimeInfo; //ר����������ִ�е�����������ļ�����ʱ��

	std::map<DWORD,std::vector<DWORD>> m_mapCachedTask;  //��¼��ģ�����ʱ��δ׼���ö���������������

	int   m_nLocalTest;       //���ص��Կ��أ�Ϊ1ʱ���ӷ�����������վ����

// 	//�����������ʵֵ
// 	std::vector<TParamItem> m_ParamItemTable;

	//����ʱ����ִ�е����񻺴浽�б��У��Ա�����������ʱ���·�������׼������Ϣ
	void CacheTask(DWORD dwNetID, DWORD dwTaskIndex);

}; 
