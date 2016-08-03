#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "BlockData.h"
#include "EnDeCode.h"
#include <vector>

//任务数据类
class CTaskData: public CEnDeCode
{
public:
	//这个变量是专门用来存储嵌入网站数据信息的，
	std::vector<T_EmbedItemData> m_vecEmbedItem;
	//是否是嵌入网站信息类的task，如果是，只有嵌入网站信息可用，下面的变量都不会用到,getsource和setsource都需要再编写格式
	bool m_bIsEmbedWeb;
	void SetEmbedWebFlag(bool flag);
	bool GetIsEmbedWeb();
public:
	std::vector<CBlockData> m_vobjBlockData;   //该任务全部页面对象
private:
	long m_lngActiveObject;          //当前活跃数据块
	E_TaskType m_ttTaskType;     //本任务方法属性
	long m_lngPerWait;               //前置延时
	bool m_bFaultTolerant;           //是否容错

	//任务过滤特征，使用MAP可以不关心采编时特征的个数及名称变化
	//Key为特征值，Value为特征值（通过与或非组合匹配）
	std::map<E_RESULT_FILTER_TYPE, T_FILTER_STRUCTURE> m_mapTaskFilters;

public:
	CTaskData() ; 
	void CopyByPointer(CTaskData *pTaskData) ;
	void Clear() ;

	void AddObject(CBlockData &obj);                //添加网页对象
	void DelObject();                   //移除当前的活跃对象
	bool GetCurObject(CBlockData &obj) ;
	CBlockData *GetCurObject() ;
	void SwapBlockData(int nObjIndex1 , int nObjIndex2) ;  //交换两个位置的数据块

	int GetCount();                  //获取页面对象的个数

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

	//移动操作对象
	void MoveForward();
	void MoveNext();

	//采编数据与数据结构转换
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
