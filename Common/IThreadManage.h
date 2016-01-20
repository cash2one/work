// IThreadManage.h: interface for the IThreadManage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITHREADMANAGE_H__AA3B1BD7_F99D_4830_A853_6550C823D954__INCLUDED_)
#define AFX_ITHREADMANAGE_H__AA3B1BD7_F99D_4830_A853_6550C823D954__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IThreadUnit;

class IThreadManage  
{
public:
	IThreadManage();
	virtual ~IThreadManage();
	virtual DWORD Create()=0; //创建
	virtual DWORD Destory()=0; //释放
	virtual BOOL InsertThread(IThreadUnit *pThread)=0; //增加线程
	virtual void RemoveThread(IThreadUnit *pThread)=0; //删除线程
	virtual void PostMessage(DWORD dwSourWork,DWORD dwDestWork,
						DWORD dwMsgParam, DWORD dwData,
							DWORD dwMsgType = 0,HANDLE hSynHandle = 0,
									DWORD dwLevel=0)=0;//压入消息
};

#endif // !defined(AFX_ITHREADMANAGE_H__AA3B1BD7_F99D_4830_A853_6550C823D954__INCLUDED_)
