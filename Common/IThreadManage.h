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
	virtual DWORD Create()=0; //����
	virtual DWORD Destory()=0; //�ͷ�
	virtual BOOL InsertThread(IThreadUnit *pThread)=0; //�����߳�
	virtual void RemoveThread(IThreadUnit *pThread)=0; //ɾ���߳�
	virtual void PostMessage(DWORD dwSourWork,DWORD dwDestWork,
						DWORD dwMsgParam, DWORD dwData,
							DWORD dwMsgType = 0,HANDLE hSynHandle = 0,
									DWORD dwLevel=0)=0;//ѹ����Ϣ
};

#endif // !defined(AFX_ITHREADMANAGE_H__AA3B1BD7_F99D_4830_A853_6550C823D954__INCLUDED_)
