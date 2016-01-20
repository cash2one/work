// IMsgQueue.h: interface for the IMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMSGQUEUE_H__80456B97_1A94_4133_B9E6_235810D22751__INCLUDED_)
#define AFX_IMSGQUEUE_H__80456B97_1A94_4133_B9E6_235810D22751__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



struct T_Message{
	DWORD dwSourWork;	//�����ߣ�������������˭����
	DWORD dwDestWork;	//Ŀ�ĵأ�������������˭����
	DWORD dwMsgParam;	//��Ϣ����(��������·��)
	DWORD dwData;		//��Ϣ����(���ڴ�������)
private:
	DWORD EnterTime;	//������Ϣʱ��
	DWORD dwMsgType; //��Ϣ����,1��ʾͬ����Ϣ��0��ʾ�첽��Ϣ
	HANDLE hSynHandle;	//����ͬ����Ϣ����
	friend class CThreadManage;
	friend class IThreadUnit;
	friend class CMsgQueue;
};

class IMsgQueue  
{
public:
	virtual void lock()=0; //��Ϣ��������
	virtual void unlock()=0; //�ͷ���Ϣ��������
	virtual void PostBackMsg(T_Message *pMsg)=0; //ѹ����Ϣ
	virtual void PostFrontMsg(T_Message *pMsg)=0; //ѹ����Ϣ
	virtual	void PostMsg(DWORD dwSourWork,DWORD dwDestWork,
							DWORD dwMsgParam, DWORD dwData,
								DWORD dwMsgType,HANDLE hSynHandle,DWORD dwLevel=0) = 0;//ѹ����Ϣ
	virtual T_Message *GetMsg(DWORD dwDestWork)=0; //��⵱ǰ��Ϣ����
	virtual bool Empty()=0;  //�ж��Ƿ�Ϊ��
	static T_Message* New_Message();	//�½���Ϣ�ڴ�
	static void Free_Message(T_Message *pMsg); //�ͷ���Ϣ�ڴ�
};

#endif // !defined(AFX_IMSGQUEUE_H__80456B97_1A94_4133_B9E6_235810D22751__INCLUDED_)
