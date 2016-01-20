//����Alltask����
#pragma once
#include "Lock.h"
#include "CommServer.h"
#include "TaskData.h"
#include "NetData.h"
#include "StationData.h"
#include "BlockData.h"
#include <map>
#include <queue>
#include <vector>

class CTaskProcess
{
public:
	CTaskProcess(void);
	~CTaskProcess(void);

	/*
	@breif  �Ƿ�ΪObject�������վ
	@param	dwWebid ��վID*/
/*	BOOL IsObjectWeb(const DWORD &dwWebid);*/

private:
	/*
	@breif  �����ںˣ�
	@param Comserver socket���ӣ�
	@param iPort    �˿ں�
	@param fn   �ص�������
	*/
	void ConnectKernel(CCommServer &Comserver,int &iPort, pfnDataHandler fn);  
	/*
	@breif ����4���ں�
	@param Comserver socket���ӣ�
	@param iPort    �˿ں�
	@param hndEngine �ں˽��̾��
	@param lRunEngine	�����е��ںˣ�
	@Return True�����ӳɹ�
	*/
	BOOL RunEngine(CCommServer &Comserver, int iPort,HANDLE &hndEngine,long &lRunEngine);

	/*
	@breif  OBJECT��վ�赥�߳�ִ��
	@param	dwWebid ��վID
	@param  lngRunCnt object������վ����
	@param  lngRuningCnt �����е�����
	@param  iEventCnt	�¼���Ӧ��������������
	*/
	BOOL RunWebBySingle( DWORD dwWebid,long &lngRunCnt,int iEventCnt,long lngRuningCnt,BOOL bIsObject = FALSE);

	/*
	@brief ��ȡ�������ݿ�����
	@param stationData ��ǰ��վ���ݣ�
	@param funData ���ܿ����ݣ�
	@param curFunction �������ͣ�
	@param pTask  �������ݣ�
	@param iStep  ִ�в���������
	@return  ��������ݣ�
	*/
	CTaskData* GetTaskData(CStationData &stationData, CFunctionData &funcData, 
				E_StationFunction &curFunction,T_TaskData *pTask,long iStep,CStdString &strOCR, DWORD dwMailId);

	/*
	@brief ��֯Ҫ���͸��ں˵�����
	@param dwWebID ��վID;
	@param stationData ��ǰ��վ���ݣ�
	@param pTData	��������ݣ�
	@return �ַ�������
	*/
	CStdString GetSendData(DWORD dwWebID, CStationData &stationData, CTaskData *pTData, CStdString &strOCRParam);

	/*
	@brief �滻��������
	@param cParam Ҫ�õ������ݣ�
	@param strTaskData	�������ݣ�
	@param dwFalg; 0:��Ʒ�滻���ƣ�1.ˢ�����̵�ַ
	*/
	void ReplaceSendData(CommonParam &cParam, CStdString &strTaskData);

	/*
	@brief ���߳�����ʱ�����������
	@param iSingle  �Ƿ�����˫�ںˣ�
	@param  lngRunCnt object������վ����
	@param  lngRuningCnt �����е�����
	*/
	void ClearRunObjectCnt(int iSingle, long &lngRunCnt, long &lngRuningCnt);

	/*
	@brief ��������
	@param eTaskType;
	*/
	void ExitMC(E_TASK_TYPE eTaskType);

	/*
	@brief ��ȡ��վʧ�ܽ�����Ͳ�����
	@param pTask	�������ݣ�
	@param pTData   ��������ݣ�
	@param tDataOut �������ݣ�
	@param vResult  ʧ�ܼ��ϣ�
	@return  ������Ҫִ�е����ݣ�
	*/
	int GetFailedResult(T_TaskData *pTask,CTaskData *pTData,T_DATA_OUT &tDataOut,std::vector<CStdString> &vResult);

	/*
	@brief ��ִ�гɹ���վ���ദ��
	@param pTask	�������ݣ�
	@param tDataOut �������ݣ�
	*/
	int GetSuccResult(T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType);

	/*
	@brief ��˾���ظ����滻����
	@param strData  ���滻���ַ�����
	@param strReplace Ҫ�滻���ַ��� 
	@Return TRUE: �滻�ɹ�
	*/
	BOOL ReplaceCompName(CStdString &strData, CStdString &strReplace);

/*private:*/
	/*
	@breif ����ͨ����Ҷ�������
	@param temp  ͨ���
	@return ��������
	*/
	int KeyToVal(CStdString& temp);

	/*
	@brief ���ַ����л��з�����3���ģ�����ֻ��������
	@param  strSource  Դ�ַ�����
	@return 
	*/
	void ReplaceNewLine(CStdString &strSource);

	/*
	@breif ��ȡ�ں˷��صĲ�Ʒ�б�����
	@param strSource Դ����
	@param strResult Ҫ���ص�����
	*/
	void GetProductList( CStdString strSource, CStdString &strResult);

	/*
	@breif  ����ͨ�����ȡ��ֵ
	@param  strData Ҫ���ҵ��ַ���
	@param  strName ͨ�����
	@param  [out ]strValueֵ
	*/
	void GetValueByName(const CStdString &strData, const CStdString &strName, CStdString &strValue);

	/*
	@brief ��ע��ɹ����ݴ���
	@param pTask	�������ݣ�
	@param tDataOut �������ݣ�
	*/
	void ProcessRegData(T_TaskData *pTask,T_DATA_OUT &tDataOut);

	/*
	@brief ��ˢ�³ɹ����ݴ���
	@param pTask	�������ݣ�
	@param tDataOut �������ݣ�
	*/
	void ProcessRefreData(T_TaskData *pTask,T_DATA_OUT &tDataOut);

	/*
	@brief ����ȡ��Ʒ�б�ɹ����ݴ���
	@param pTask	�������ݣ�
	@param tDataOut �������ݣ�
	@param dwTaskType ��������
	*/
	void ProcessGetPdtData(T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType);

	/*
	@brief ��һ����ҳɹ����ݴ���
	@param pTask	�������ݣ�
	@param tDataOut �������ݣ�
	@param dwTaskType ��������
	*/
	void ProcessMigrateData(T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType);

	/*
	@brief ����һ�����ͨ����滻
	@param tDataOut  �ں˷��ص�����
	@param strMigrateData ͨ����ַ���;
	*/
	DWORD ReplaceMigrateData(T_DATA_OUT &tDataOut, CStdString &strMigrateData);

	/*
	@brief ��ȡ����ͼƬ�ϴ���������
	@param strParam   ͨ�����
	@param strParamValue ͨ�����ֵ;
	@param strMigrateData Ҫ�滻���ַ���;
	@param dwCnt �滻ʧ��������
	*/
	DWORD UpLoadPic(CStdString &strParam,CStdString &strParamValue,CStdString &strMigrateData,DWORD &dwCnt);

	/*
	@brief ��Win8����ϵͳ������Ȩ
	*/
	BOOL RaisePrivileges();

	/*
	@brief ����IE����ģʽ
	@param eVer IE�汾
	*/
	void SetIECoreVersion(E_IeLimit eVer);

	/*
	@breif �ж��Ƿ�64λϵͳ
	*/
	BOOL IsWOW64();

	/*
	@breif ��ȡIE�汾��
	*/
	int GetIEVersion();

private:
	/*std::vector<DWORD> vNsCodeOfObject; //�����������վID;*/
	BOOL   bIsWait;     //��ǰ��վ�赥�߳�ִ�У����OBJECT��վ��
	
/*private:*/
	CNetData m_NetData;

	friend class CAllTaskData;
};

