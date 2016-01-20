#pragma once

#include "IThreadUnit.h"
#include "AllTaskData.h"

class CKernelProcessThread :
	public IThreadUnit
{
public:
	CKernelProcessThread(void);
	~CKernelProcessThread(void);

public:
	//�̳߳�ʼ��
	virtual DWORD CreateThread();
	virtual DWORD RestThread();
	//�߳��˳�
	virtual DWORD DestoryThread();
	virtual DWORD DispatchMessage(T_Message *pMsg);


	/*
	@breif  ��ʼ����
	@param  ��������
	*/
	DWORD DoWorkToKernel(DWORD dwIndex);

private:
	bool  bIsFirst;  //�Ƿ��һ��������
	bool  bIsObjectFirst; //�Ƿ��̵߳�һ��������
	bool  bIsSetIE; //�Ƿ�����IEģʽ
	CAllTaskData   taskData;     //��������
};

