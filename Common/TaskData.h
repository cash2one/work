#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "BlockData.h"
#include "EnDeCode.h"
#include <vector>

//����������
class CTaskData: public CEnDeCode
{
public:
	//���������ר�������洢Ƕ����վ������Ϣ�ģ�
	std::vector<T_EmbedItemData> m_vecEmbedItem;
	//�Ƿ���Ƕ����վ��Ϣ���task������ǣ�ֻ��Ƕ����վ��Ϣ���ã�����ı����������õ�,getsource��setsource����Ҫ�ٱ�д��ʽ
	bool m_bIsEmbedWeb;
	void SetEmbedWebFlag(bool flag);
	bool GetIsEmbedWeb();
public:
	std::vector<CBlockData> m_vobjBlockData;   //������ȫ��ҳ�����
private:
	long m_lngActiveObject;          //��ǰ��Ծ���ݿ�
	E_TaskType m_ttTaskType;     //�����񷽷�����
	long m_lngPerWait;               //ǰ����ʱ
	bool m_bFaultTolerant;           //�Ƿ��ݴ�

	//�������������ʹ��MAP���Բ����Ĳɱ�ʱ�����ĸ��������Ʊ仯
	//KeyΪ����ֵ��ValueΪ����ֵ��ͨ���������ƥ�䣩
	std::map<E_RESULT_FILTER_TYPE, T_FILTER_STRUCTURE> m_mapTaskFilters;

public:
	CTaskData() ; 
	void CopyByPointer(CTaskData *pTaskData) ;
	void Clear() ;

	void AddObject(CBlockData &obj);                //�����ҳ����
	void DelObject();                   //�Ƴ���ǰ�Ļ�Ծ����
	bool GetCurObject(CBlockData &obj) ;
	CBlockData *GetCurObject() ;
	void SwapBlockData(int nObjIndex1 , int nObjIndex2) ;  //��������λ�õ����ݿ�

	int GetCount();                  //��ȡҳ�����ĸ���

	E_TaskType GetType() const;
	void SetType(E_TaskType pType);

	bool GetFaultTolerant() const;
	void SetFaultTolerant(bool &bFaultTolerant);

	long GetActiveIndex() const;
	void SetActiveIndex(long lIndex);

	long GetPerWait() const;
	void SetPerWait(long lngWait);

	T_FILTER_STRUCTURE *GetFilterStructure(E_RESULT_FILTER_TYPE eType) ;
	void SetFilterStructure(E_RESULT_FILTER_TYPE eType , T_FILTER_STRUCTURE &filterStructure) ;
	void GetFilterStructure(std::map<E_RESULT_FILTER_TYPE , T_FILTER_STRUCTURE> &mapFilterStruct) ;
	void SetFilterStructure( std::map<E_RESULT_FILTER_TYPE , T_FILTER_STRUCTURE> &mapFilterStruct ) ;
	void DelFilterStructure(E_RESULT_FILTER_TYPE eType) ;

	//�ƶ���������
	void MoveForward();
	void MoveNext();

	//�ɱ����������ݽṹת��
	CStdString GetSource();
	void SetSource(CStdString &strSource);

private:
	CStdString TransTaskDataToSource();
	void ExtraTaskDataFromSource(CStdString strData);

	CStdString TransTaskFiltersToSource(std::map<E_RESULT_FILTER_TYPE, T_FILTER_STRUCTURE> &mapTaskFilters) ;
	void ExtraTaskFiltersFromSource(CStdString &strSourceData , std::map<E_RESULT_FILTER_TYPE, T_FILTER_STRUCTURE> &mapTaskFilters) ;

	CStdString TransFilterStructureToSource(T_FILTER_STRUCTURE &tFilterStructure) ;
	void ExtraFilterStructureFromSource(CStdString &strSourceData , T_FILTER_STRUCTURE &tFilterStructure) ;
};
