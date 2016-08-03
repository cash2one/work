#pragma once

#include "TaskProcess.h"

#define THREAD_MAX_COUNT 20

class CAllTaskData
{
public:
	CAllTaskData(void);
	virtual ~CAllTaskData(void);

	/*
	@breif插入任务索引到队列
	@param  dwIndex 任务索引
	@param  bSingle 是否Object网站；
	*/
	void PushTaskIndex(DWORD dwIndex, BOOL bSingle = FALSE,E_IeLimit ieMode = IE8 );  

	/*
	@breif  开始启动任务线程
	*/
	void StartWork(BOOL bIsOneThread = FALSE);  

	/*
	@breif  保存某网站操作结果
	@param  dwWebId 网站id
	@param  dwWebId 结果描述
	*/
	void PushResult(DWORD dwWebId, const CStdString &strResult);

	/*
	@breif  继续网站数据操作
	@param  dwWebId 网站id
	*/
	void ContinueOperate(DWORD dwWebId);

	/*
	@brief 将所有等待操作的网站 不在等待
	*/
	void ContinueAllOperate();

	/*
	@brief 将所有Object等待操作的网站 不在等待
	*/
	void ContinueObjectOperate();

	/*
	@breif 设置IE模式
	*/
	void SetIEMode(E_IeLimit eVer);

private:
	//------------Object操作 start-----
	/*
	@breif  取得任务索引，并清除
	@return 返回任务索引
	*/
	DWORD GetObjectTaskIndex(E_IeLimit eVer = IE8);

	//-------------Object操作 end ---------

	/*
	@breif  取得任务索引，并清除
	@return 返回任务索引
	*/
	DWORD GetTaskIndex(E_IeLimit eVer=IE8);

	/*
	@breif 启动4代内核
	*/
	BOOL RunEngine(BOOL bSingleKernel = FALSE);

	/*
	@brief  处理返回数据
	@param  dwWebId网站id
	@param  pTaskData任务数据
	@param *pTData任务执行块
	@return 0表示此步骤失败不需要继续执行下去，1表示此步骤成功继续执行下去，2表示发布时遇到完善资料，回到上一步
	*/
	int HandleRetData(DWORD dwWebId, T_TaskData* pTaskData,const DWORD &dwFunType, CTaskData *pTData = NULL);


	/*
	@breif  将网站网站id与事件对象关联
	@param  dwWebId 网站id
	*/
	void LinkWeiIdToEvent(DWORD dwWebId, HANDLE hEvent);

	/*
	@breif  移除该网站id的事件对象
	@param  dwWebId 网站id
	*/
	void RemoveWebIdEvent(DWORD dwWebId); 

	/*
	@breif  去除网站操作结果
	@param  dwWebId 网站id
	*/
	void RemoveResult(DWORD dwWebId);

	/*
	@breif  取得网站操作结果
	@param  dwWebId 网站id
	*/
	CStdString *GetResult(DWORD dwWebId);

	/*
	@breif  处理任务结束后，后继收尾操作
	@param  dwWebId 网站id
	@param  strWebName  网站名称
	@param  dwIndex  任务索引
	@param  bSendStop 当前是否需要告知四代核停止该任务
	*/
	void HandleCompleteWeb(DWORD dwWebId, const CStdString &strWebName, DWORD dwIndex, bool bSendStop = true,bool bSingleThread = false);

	/*
	@breif  根据通配符名取得多个值
	@param  strData 要查找的字符串
	@param  strName 通配符名
	@param  [out ]strValue值
	*/
//	void GetValuesByName(const CStdString &strData, const CStdString &strName, CStdString &strValue);

	/*
	@breif  根据通配符名取得值*/
	bool CAllTaskData::FindEvent( DWORD dwWebId );

	/*
	@breif  获取当期任务类型
	@param	*/
	void GetCurTaskType(E_TASK_TYPE &eType,E_StationFunction &curFunction,bool &bSendStop);

	/*
	@breif  执行发送数据并处理返回数据
	@param	pTData 执行任务数据；
	@param  pTaskData 任务数据结构；
	@param  curStep 当前执行第几个功能块；
	@param  iFunCount  功能块总数；
	@param	strPreCompName 公司名称；
	@param  curFunction  当前任务类型；
	return	接下要执行的步骤
	*/
	DWORD SendTaskData(CTaskData *pTData,T_TaskData* pTaskData,long &curStep,E_StationFunction &curFunction,
		DWORD &dwRet,CStdString &strTaskData, CommonParam &cParam);

	/*
	@brief 处理一键搬家成功时步骤
	@param dwTaskType 任务类型；
	@param dwPdtCnt   产品数量；
	@param pTask	  任务数据；
	@return TRUE :没有产品需要提取；
	*/
	BOOL ProcessSuccMigrate(DWORD dwTaskType, DWORD dwPdtCnt, T_TaskData *pTask,DWORD dwIndex);

	/*
	@brief 对数据初始化；
	*/
	void InitData();
	/*
	@brief doTask前做准备；
	@param iSingle 是否要启动2个内核；
	@param eType   任务类型；
	@param dwWebId 网站ID;
	*/
	BOOL DoPreJob(int iSingle,DWORD dwWebId, E_TASK_TYPE eType,CommonParam &cParam);

	/*
	@brief dotask和DoSingleTask共同的部分；
	@param dwIndex  任务索引；
	@param hEvent   事件对象；
	@param bSingle  单线程任务；
	*/
	DWORD DoCommonTask(DWORD dwIndex, HANDLE hEvent, BOOL bSingle,bool &isRestartEngine);

	/*
	@breif 判断当前版本的所有网站是否都运行完成；
	@param eIever ie版本号
	@return TRUE 已完成；False 未完成
	*/
	BOOL CheckRunOver(E_IeLimit& eIever, BOOL bSingle);

	/*
	@breif 判断多线程网站是否还有任务在执行；
	*/
	BOOL CheckMultiTask();

private:
	std::map<E_IeLimit,std::queue<DWORD>>  m_allQueueTask;       //任务管理器传过来的任务数据
	std::map<E_IeLimit,std::queue<DWORD>>	m_objectQueueTask;	  //Object线程的数据
	std::map<DWORD, HANDLE>  IndexToEvent;      //每个任务同步事件  网站id对应事件
	std::map<DWORD, CStdString*>   webIdToResult;   //网站id对应任务结果
	std::map<DWORD, HANDLE> indexToObjectEvent; //Object网站对应事件信号；

	HANDLE   hEvents[THREAD_MAX_COUNT];        //线程对应的事件
	HANDLE   hThreads[THREAD_MAX_COUNT];       //任务线程

	HANDLE	 m_hEvent;					//单线程对应事件
	HANDLE   m_hThread;					//单个任务线程
	CLock  m_TaskLock;                  //任务锁
	CLock  m_EventLock;                 //事件锁
	CLock  m_TaskResultLock;           //任务结果说明锁
	CLock  m_CheckLock;				   //检测是否有网站在运行的锁
	HANDLE m_hndEngine;    //内核句柄
	HANDLE m_hndSingleEngine; //单线程运行内核句柄；


	long  lIsRunEngine;         //是否正在启动4代核，大于0为正在启动
	int iWaitTime;

	//内核通讯 begin--
	CCommServer m_Connection ;
	int m_nPort ;   //与内核通讯端口号
	bool   bRun;      //线程任务是否运行
	//单任务运行
	long lngKernelCnt;	//内核数
	CCommServer m_ConnectionByObject;
	int m_ObjPort;

	
/*	DWORD  m_dwFunType;       //功能类型*/
	long   lngRunObject;         //object类型网站数量；
	long   lngRunningCount;     //运行中的数量；
	long   lngCheckObjectWeb;   //如果有在检测OBJECT，就先等待；

	BOOL	bObjectRunFinish;  //判断Object对象运行是否结束
	BOOL	m_bCheckRun;	   //检测是否当前正在执行网站运行中检测

	CStdString strMailBak;		//备份邮箱
	CStdString strMail;			//原本邮箱

public:

	/*
	@breif  任务线程
	@param  this指针
	*/
	static DWORD WINAPI DoTask(LPVOID lp);       //任务线程
	static DWORD WINAPI DoSingleTask(LPVOID lp);  //单线程任务

	CNetData m_NetData;
	CTaskProcess m_Taskprocess;  //处理任务封装；
	int  iOneKernel; //配置是否要启动双内核参数,默认是双内核（0）
	int  iIEVersion; //IE版本；
	E_IeLimit m_eCurIeVer;     //当前IE版本
};


DWORD GetData(TCHAR *Orgbuf, DWORD dwTotalLen) ;
DWORD GetObjectData(TCHAR *Orgbuf, DWORD dwTotalLen);