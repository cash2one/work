#pragma once

#include "TaskManger.h"


class CTaskMgrThread :public CTaskManger 
{
private:
	E_SWITCH m_DBState;		//本地数据库状态
	DWORD m_dwGUIDataIndex; //GUI压过的数据缓存
	DWORD m_dwFinishCount;  //完成的任务计数
	DWORD m_dwTotalTaskCount;  //页面下压的任务累计总数
	BOOL  bIsSendComplete;

public:
	CTaskMgrThread();
	virtual ~CTaskMgrThread();
	//线程初始化
	virtual DWORD CreateThread(){return 0;}
	//线程退出
	virtual DWORD DestoryThread(){return 0;}	
	//消息处理
	DWORD DispatchMessage(T_Message *Msg);
private:
 
	DWORD RevGuiMessage(DWORD dwData); //Gui过来的数据消息
	
	DWORD RevErrorPrepareData(DWORD dwIndex); //数据准备失败

	DWORD RevPrepareData(DWORD dwIndex); //数据已经准备好的回来的消息

	DWORD RevPrepareNoWebData(DWORD dwIndex);	//数据准备发过来需要更新网站数据，未能准备数据的一种

	DWORD RevPrepareWebUpdateSucceed(DWORD dwWebId); //数据准备发来网站数据更新完成
	
	DWORD RevPrepareWebUpdateFailed(DWORD dwWebId); //数据准备发来网站数据更新失败

	DWORD RevExSerResult(DWORD dwIndex);	//任务从ExSer执行后返回


	DWORD RevTaskSucceed(DWORD dwIndex);	//任务完成从SaveThread确认返回

	DWORD RevTaskFailed(DWORD dwIndex);		//任务失败

	DWORD RevThreadReset(DWORD dwIndex,DWORD dwThreadType);	//线程重启

	DWORD RemoveTaskData(DWORD dwIndex);	//任务做完，清除任务数据结构

    DWORD PushNormalTaskData(DWORD dwGUIDataIndex);	//add by yzh

	CString GetMacAddress();  //获取mac地址；
};


