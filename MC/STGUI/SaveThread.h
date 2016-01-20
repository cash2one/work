#pragma once

#include "IThreadUnit.h"
#include "IServerSocket.h"
#include "IWebOper.h"

#define TIMER_TASK_SIGNAL_PREFIX  _T("_BIZ_EXPRESS_TIMER_TASK_ID_")    //定时任务执行标记的前缀（完整标记在后面加定时任务ID）


class CSaveThread  :public IThreadUnit
{
public:
	CSaveThread();
	virtual ~CSaveThread();
	DWORD m_SendCount;
	DWORD m_dwRevCount;
private:
	HINSTANCE m_histWeb;
	IWebOper *m_pIWebOper;
public:
	//消息分发
    DWORD DispatchMessage(T_Message *Msg);

	DWORD CreateThread();

	DWORD RestThread()
	{
		DestoryThread();
		CreateThread();
		return 0;
	}

    DWORD DestoryThread();

private:
	
    //任务结果保存接口
    DWORD SaveTaskResult(DWORD dwIndex);

	//保存任务结果并返回上层
	DWORD SaveResultToGui(DWORD dwIndex);

	//根据将验证信息转换成返回上层格式
	void GetCodeOcrInfo(T_TaskData *pTaskData, CStdString &strInfo);
	
	//处理准备数据失败的任务
	DWORD PreDataFail(DWORD dwIndex);


    //发包到GUI
	DWORD SendDataToGUI(CStdString strInDtat);


	//发包到GUI,需要保存任务基本数据到本地数据库
	DWORD SendDataToGUI(CStdString strInDtat, T_TaskData& TaskData);

private:
      
	std::map<DWORD,T_SaveToServerAccount*> *m_pMapSaveAccountPass;

	CStdString m_strTimerTaskID;         //定时任务ID
	DWORD m_dwCurrentTaskItemCount;      //总任务数
	CStdString m_strTimerTaskFlag;       //标记定时任务类型
	HANDLE m_hTimerTaskMutex;            //以定时任务ID为后缀的全局Mutex标记，非NULL表示定时任务正在执行


	DWORD GetWebSiteName(T_TaskData *pTaskData,CStdString &strWebName, CStdString &strItemName);
};


