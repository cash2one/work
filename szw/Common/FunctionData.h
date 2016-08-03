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

	void AddObject(CTaskData &obj);     //添加任务对象
	void DelObject();                   //移除当前的活跃对象
	void GetCurTask(CTaskData &obj) ;
	E_TaskType GetCurTaskType() const;

	int GetCount();                  //获取页面对象的个数

	long GetActiveIndex() const;
	void SetActiveIndex(long lIndex);

	//采编数据与数据结构转换
	CStdString GetSource();
	void SetSource(CStdString &strSourceData);
	bool GetEmbedInfo(E_StationFunction eFunc, T_EmbedItemData &vecEmbedItem,const CStdString& strWebID);//特定function下的嵌入网站数据要执行的task，返回真为成功

private:
	CStdString TransTasksToSource();
	void ExtraTasksFromSource(CStdString &strData);

};
