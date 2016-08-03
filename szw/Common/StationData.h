#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "FunctionData.h"
#include "EnDeCode.h"
#include <vector>

//管理一个网站的数据
class CStationData:public CEnDeCode
{
private:
	DWORD      m_dwNsCode;
	CStdString    m_strNsName;
	CStdString    m_strNsHost;

	std::map<E_TaskType , CTaskData*> m_mapTaskData; //网站支持的所有任务方法与任务数据的对应列表
	std::map<E_StationFunction, CStdString>    m_mapsfTask;     //网站功能与任务的对应列表,值表示网站功能所对应的E_TaskType集合，以'/'分隔

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

	//管理网站中的任务数据
	void AddTaskData(E_TaskType ttType , CTaskData *pTaskData) ;
	void DelTaskData(E_TaskType ttType);
	CTaskData *GetTaskData(E_TaskType ttType);     //返回指定任务数据的指针，若不存在则new
	void GetTaskData(int nIndex , CTaskData &taskData) ;
	int GetTaskCount() ;
	//void GetEmbedInfo(E_StationFunction eFunc, std::vector<T_EmbedItemData> &vecEmbedItem);//特定function下的嵌入网站数据要执行的task，没有就为空
	CStdString GetAllEmbedWedID();
	//添加功能对应的Task列表，已存在则更新
	void SetFunctionRoute(E_StationFunction sfVal, CStdString strTask);
	CStdString GetFunctionRoute(E_StationFunction sfVal) ;
	void GetAllFunction(std::vector<E_StationFunction> &vecAllFunc) ;
	void DeleteFuntion(E_StationFunction sfVal);

	//采编数据与数据结构转换(网站的数据)
	CStdString GetSource(void);
	void SetSource(CStdString &strNsData);

	//根据功能编号初始化功能数据
	bool LoadData(E_StationFunction sfVal, CFunctionData &cfData);

	std::map<E_TaskType, CTaskData*>* get_MapTaskData();
private:
	CStdString TransTargetToSource() ;
	void ExtraTargetFromSource(CStdString &strSourceData) ;

};
