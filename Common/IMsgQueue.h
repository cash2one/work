// IMsgQueue.h: interface for the IMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMSGQUEUE_H__80456B97_1A94_4133_B9E6_235810D22751__INCLUDED_)
#define AFX_IMSGQUEUE_H__80456B97_1A94_4133_B9E6_235810D22751__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



struct T_Message{
	DWORD dwSourWork;	//发起者（用于区分是由谁发起）
	DWORD dwDestWork;	//目的地（用于区分是由谁处理）
	DWORD dwMsgParam;	//消息参数(用于命令路由)
	DWORD dwData;		//消息数据(用于传递数据)
private:
	DWORD EnterTime;	//进入消息时间
	DWORD dwMsgType; //消息类型,1表示同步消息，0表示异步消息
	HANDLE hSynHandle;	//用于同步消息类型
	friend class CThreadManage;
	friend class IThreadUnit;
	friend class CMsgQueue;
};

class IMsgQueue  
{
public:
	virtual void lock()=0; //消息队列锁定
	virtual void unlock()=0; //释放消息队列锁定
	virtual void PostBackMsg(T_Message *pMsg)=0; //压入消息
	virtual void PostFrontMsg(T_Message *pMsg)=0; //压入消息
	virtual	void PostMsg(DWORD dwSourWork,DWORD dwDestWork,
							DWORD dwMsgParam, DWORD dwData,
								DWORD dwMsgType,HANDLE hSynHandle,DWORD dwLevel=0) = 0;//压入消息
	virtual T_Message *GetMsg(DWORD dwDestWork)=0; //检测当前消息类型
	virtual bool Empty()=0;  //判断是否为空
	static T_Message* New_Message();	//新建消息内存
	static void Free_Message(T_Message *pMsg); //释放消息内存
};

#endif // !defined(AFX_IMSGQUEUE_H__80456B97_1A94_4133_B9E6_235810D22751__INCLUDED_)
