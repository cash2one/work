#pragma once
#include "IThreadUnit.h"
#include "Sysparam.h"
#include "PrepareData.h"


class CPrePareDataThread :public IThreadUnit
{
public:
	CPrePareDataThread(void);
	~CPrePareDataThread(void);
	//线程初始化
	DWORD CreateThread();
	//线程退出
	virtual DWORD DestoryThread();	
	//消息处理函数
	virtual DWORD DispatchMessage(T_Message *Msg);
	//处理与webservice消息
	void ProcessServerDataMsg(DWORD dwNetId, DWORD dwIndex);

	//获取系统信息函数
	DWORD GetSysparam(TRapalceDataMsg& tRapalceData, DWORD dwIndex);


	//系统信息测试函数
	DWORD GetSysparamForTest(TRapalceDataMsg& tRapalceData);
	//获取服务器网站数据更新时间
	int GetServerDbTime(DWORD dwIndex);
	int GetServerEnineDataDbTime(DWORD dwIndex);
	//判断网站数据是不是最新
	int NetDataIsNew(DWORD dwNetId);
	//获取帐号密码
	int GetIdAndPsd(DWORD dwNetId, T_TaskData* pTask);

	//处理获取服务器数据时间消息
	int ProcessGetServerDataTimeMsg(DWORD dwIndex);

	//处理更新netstation表消息
	int ProcessUpDateNetStationMsg(DWORD dwIndex);

	//处理获取更新数据失败消息
	int ProcessUpDateErrMsg(DWORD dwIndex);
	//处理线程重起消息
	int ProcessResetThreadMsg(T_Message *Msg);

	//任务预数据
	BOOL PreProcessTaskData(DWORD dwIndex,TRapalceDataMsg& tTRapalceDataMsg, BOOL bUpdateDataFlag);
	//处理任务处理
	int PrecessTaskData(DWORD dwIndex, TRapalceDataMsg &tTRapalceDataMsg);

	//释放内存和变量清空
	int EmptyFun();

	int GetEngineDataFileLastUpdateALL(const CStdString &strData,T_ServerNetStationInfoGroup &ServerLastupdateGroup);
	bool CheckConnDBfinished(void);

	//判断网站是否最新，否的话直接下载；
	DWORD CheckWebIsNewAndDown(DWORD dwWebId, DWORD dwIndex, BOOL bUpdateFlag, BOOL bAlreadyUpdate = TRUE);

private:
	CSysparam m_Sysparam;//系统信息变量
	CPrepareData m_PrepareData;//准备类变量
	BOOL m_bAllNetStationLastUpdateTimeGetted; //所有网站的最后更新时间已获取

//	std::map<DWORD,DWORD> m_NetUpdate ; //m_NetUpdateinfo 子集
	std::map<DWORD,std::vector<DWORD>> m_NetUpdateinfo;	//更新信息列表，用于存需更新网站数据的WEBID与TASKID的对应关系
	std::map<DWORD,DWORD> m_DBRefrshTimeInfo;	//最新更新时间，用于存WEBID与ServerTime对应关系
    std::map<DWORD,DWORD> m_EngineDBRefrshTimeInfo; //专供三代引擎执行的任务的数据文件更新时间

	std::map<DWORD,std::vector<DWORD>> m_mapCachedTask;  //记录因模板更新时间未准备好而缓存起来的任务

	int   m_nLocalTest;       //本地调试开关，为1时不从服务器更新网站数据

// 	//保存参数的真实值
// 	std::vector<TParamItem> m_ParamItemTable;

	//将暂时不能执行的任务缓存到列表中，以便在条件成熟时重新发起数据准备的消息
	void CacheTask(DWORD dwNetID, DWORD dwTaskIndex);

}; 
