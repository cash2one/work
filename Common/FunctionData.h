#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "TaskData.h"
#include "EnDeCode.h"
#include <vector>

class CFunctionData:public CEnDeCode
{
private:
	std::vector<CTaskData> m_vobjTaskData;
	long m_lngActiveObject;

public:
	CFunctionData(){ clear() ;} ;
	~CFunctionData(){} ;

	void clear() ;

	void AddObject(CTaskData &obj);     //����������
	void DelObject();                   //�Ƴ���ǰ�Ļ�Ծ����
	void GetCurTask(CTaskData &obj) ;
	E_TaskType GetCurTaskType() const;

	int GetCount();                  //��ȡҳ�����ĸ���

	long GetActiveIndex() const;
	void SetActiveIndex(long lIndex);

	//�ɱ����������ݽṹת��
	CStdString GetSource();
	void SetSource(CStdString &strSourceData);
	bool GetEmbedInfo(E_StationFunction eFunc, T_EmbedItemData &vecEmbedItem,const CStdString& strWebID);//�ض�function�µ�Ƕ����վ����Ҫִ�е�task��������Ϊ�ɹ�

private:
	CStdString TransTasksToSource();
	void ExtraTasksFromSource(CStdString &strData);

};
