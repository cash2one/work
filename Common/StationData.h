#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "FunctionData.h"
#include "EnDeCode.h"
#include <vector>

//����һ����վ������
class CStationData:public CEnDeCode
{
private:
	DWORD      m_dwNsCode;
	CStdString    m_strNsName;
	CStdString    m_strNsHost;

	std::map<E_TaskType , CTaskData*> m_mapTaskData; //��վ֧�ֵ��������񷽷����������ݵĶ�Ӧ�б�
	std::map<E_StationFunction, CStdString>    m_mapsfTask;     //��վ����������Ķ�Ӧ�б�,ֵ��ʾ��վ��������Ӧ��E_TaskType���ϣ���'/'�ָ�

	BOOL m_bIsNoImage ;
	DWORD m_dwViewCount ;
public:
	CStationData(){ Clear() ; };
	~CStationData(){};

	void Clear() ;

	DWORD GetNsCode() const ;
	void SetNsCode(DWORD &dwNsCode) ;

	CStdString GetNsName() const ;
	void SetNsName(const CStdString &strNsName) ;

	CStdString GetNsHost() const ;
	void SetNsHost(const CStdString &strNsHost) ;

	BOOL GetIsNoImages() ;
	void SetIsNoImages(BOOL bIsNoImages) ;

	DWORD GetViewCount() ;
	void SetViewCount(DWORD dwViewCount) ;

	//������վ�е���������
	void AddTaskData(E_TaskType ttType , CTaskData *pTaskData) ;
	void DelTaskData(E_TaskType ttType);
	CTaskData *GetTaskData(E_TaskType ttType);     //����ָ���������ݵ�ָ�룬����������new
	void GetTaskData(int nIndex , CTaskData &taskData) ;
	int GetTaskCount() ;
	//void GetEmbedInfo(E_StationFunction eFunc, std::vector<T_EmbedItemData> &vecEmbedItem);//�ض�function�µ�Ƕ����վ����Ҫִ�е�task��û�о�Ϊ��
	CStdString GetAllEmbedWedID();
	//��ӹ��ܶ�Ӧ��Task�б��Ѵ��������
	void SetFunctionRoute(E_StationFunction sfVal, CStdString strTask);
	CStdString GetFunctionRoute(E_StationFunction sfVal) ;
	void GetAllFunction(std::vector<E_StationFunction> &vecAllFunc) ;
	void DeleteFuntion(E_StationFunction sfVal);

	//�ɱ����������ݽṹת��(��վ������)
	CStdString GetSource(void);
	void SetSource(CStdString &strNsData);

	//���ݹ��ܱ�ų�ʼ����������
	bool LoadData(E_StationFunction sfVal, CFunctionData &cfData);

	std::map<E_TaskType, CTaskData*>* get_MapTaskData();
private:
	CStdString TransTargetToSource() ;
	void ExtraTargetFromSource(CStdString &strSourceData) ;

};
