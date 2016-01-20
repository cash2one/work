// IThreadUnit.h: interface for the IThreadUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITHREADUNIT_H__F84C5776_9989_4C7A_A8BF_57769BC5A93E__INCLUDED_)
#define AFX_ITHREADUNIT_H__F84C5776_9989_4C7A_A8BF_57769BC5A93E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMsgQueue.h"
#include "IThreadManage.h"

class CThreadManage;

#define MSG_TIMER 0xFFFE	//定时器消息
#define MSG_RESETTHREAD 0xFFFD //线程重启
 

class IThreadUnit  
{
	friend class CThreadManage;
private:
	BOOL m_isRest;	//is Rest Thread
	T_Message m_CurMsg; //正在处理的当前消息
	IMsgQueue *m_MsgQueue; //消息队列指针
	DWORD m_dwThreadType;	//消息类型，用于区分线程功能
	DWORD m_dwTimeOut; //线程超时时间
	HANDLE m_hHandle;	//线程句柄
	HANDLE m_synMsgHandle; //用于同步消息句柄
	DWORD ExitFlag;		// 退出标志
	BOOL m_Busy; //标志如果为TRUE,哪么，不重启线程，为FALSE否则才重启
	//线程入口点
	static DWORD WINAPI StaticThreadFunc(LPVOID lpParam);
	//创建线程
	DWORD Create(IMsgQueue *pIMsgQueue);
	BOOL Terminated();	//终止线程
	IThreadManage *m_ThreadManage;
public:
	void PostMessage(DWORD dwDestWork,DWORD dwMsgParam, DWORD dwData,DWORD dwLevel=0); //异步消息
	void SendMessage(DWORD dwDestWork,DWORD dwMsgParam, DWORD dwData); //同步消息
	void Lock(){m_Busy=TRUE;}//锁定线程不可以重启
	void UnLock() //解除锁定，可以重启
	{
		m_CurMsg.EnterTime = ::GetTickCount();	//修改出队时间
		m_Busy=FALSE;
	}
	void ChangeTimeOutValue(DWORD dwTimeOut)	//运行时修改超时值，目前只有ExSer使用
	{
		m_dwTimeOut = dwTimeOut;
	}
public:
	IThreadUnit(DWORD dwThreadType=0xFFFF,DWORD dwTimeOut=0xFFFF); //线程类型、超时时间以秒为单位
	virtual ~IThreadUnit();
	//线程初始化
	virtual DWORD CreateThread() = 0;
	//重启线程
	virtual DWORD RestThread() {return 0;}
	//线程退出
	virtual DWORD DestoryThread() = 0;
	//线程运行
	virtual DWORD DispatchMessage(T_Message *Msg) = 0;
	//默认线程处理
	virtual DWORD DefaultProc(){return 0;}

};

#endif // !defined(AFX_ITHREADUNIT_H__F84C5776_9989_4C7A_A8BF_57769BC5A93E__INCLUDED_)
