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
enum E_SWITCH	{e_open,e_close};	//开关状态



struct T_TaskDesc		//任务描述
{	
	E_TASKSTATE TaskState;	//当前任务状态
	DWORD dwCount;			//任务重试次数
	E_Task_Level eLevel;	//优先级别(1注册，2发布，3发布需注册，4搜索，5搜索需注册，6其它，7数据还未准备的)
	DWORD dwPrvTaskID;		//此任务之前必须执行的任务
	DWORD dwNextTaskID;		//此任务之后需要执行的任务
	DWORD ClientID;			//客户端ID
	DWORD dwTaskIndex;		//数据编号(内部ID)
	DWORD dwWebId;			//网站编号(主要用于数据准备返回需要更新的网站消息)
	DWORD dwStartTime;		//分配执行的开始时间
	T_TaskData *Data;		//真实数据地址
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
	std::map<DWORD,T_TaskDesc *> m_TaskDescMap; //任务状态列表

private:
	BOOL SetTaskStartTime(DWORD dwTaskIndex,DWORD dwTime);						//设置任务开始时间
	BOOL GetIdleTask(DWORD &dwTaskIndex,DWORD &dwLevel);						//获得一个空闲任务
	T_TaskDesc *GetTask(DWORD dwTaskIndex);										//获得一个任务描述符
	std::map<DWORD,DWORD> m_mapTaskCount;                                       //保存对应任务类型的数量
//	std::vector<DWORD> vNsCodeOfObject;											//包含对象的网站ID;
public:
	CTaskManger(void);
	virtual ~CTaskManger(void);
	DWORD BuildTaskList(DWORD dwGUIDataIndex);									//建立任务列表
	DWORD SchedulerTask();														//任务调度		
	DWORD TaskResult(DWORD dwTaskIndex);										//任务执行返回
	DWORD TaskFialed(DWORD dwTaskIndex);										//任务失败
	DWORD TaskSucceed(DWORD dwTaskIndex);										//任务成功
	DWORD RemoveTask(DWORD dwTaskIndex);										//删除一个任务
	BOOL SetTaskState(DWORD dwTaskIndex,E_TASKSTATE eTaskState);				//改变任务状态

	void DelPhotoPageFolder();													//删除快照文件夹；
	
};
