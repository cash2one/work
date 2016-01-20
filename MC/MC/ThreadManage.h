// ThreadManage.h: interface for the CThreadManage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADMANAGE_H__8925966E_E55A_461C_8D2F_BE420AB5ACB4__INCLUDED_)
#define AFX_THREADMANAGE_H__8925966E_E55A_461C_8D2F_BE420AB5ACB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

 
#include "MsgQueue.h"
#include "IThreadManage.h"

class IThreadUnit;

class CThreadManage :public IThreadManage 
{
private:
	std::list<IThreadUnit*>m_listThread;
	std::map<DWORD,IMsgQueue*> m_MsgQueueMap; //消息队列map
	//HANDLE m_hTimer; //记时器
	HANDLE m_hHandle;	//线程句柄
	BOOL m_ExitFlag;	//退出标志
	BOOL m_PrintLog;   //打印日志
	//线程入口点
	static DWORD WINAPI StaticThreadFunc(LPVOID lpParam);
	//检测线程状态
	void CheckThreadState();
	//广播消息
	void broadcastMsg(DWORD dwMsgParamOne, DWORD dwMsgParam,DWORD dwData);
	//线程运行
	DWORD Run();
public:
	CThreadManage();
	virtual ~CThreadManage();
	DWORD Create(); //创建
	DWORD Destory(); //释放
	BOOL InsertThread(IThreadUnit *pThread); //增加线程
	void RemoveThread(IThreadUnit *pThread); //删除线程
	void PostMessage(DWORD dwSourWork,DWORD dwDestWork,
						DWORD dwMsgParam, DWORD dwData,
							DWORD dwMsgType = 0,HANDLE hSynHandle = 0,
								DWORD dwLevel=0);//压入消息
public:
	DWORD DataIn(BYTE* pData, DWORD dwLen, DWORD dwFlag, void* pReserve=NULL);
};

#endif // !defined(AFX_THREADMANAGE_H__8925966E_E55A_461C_8D2F_BE420AB5ACB4__INCLUDED_)
