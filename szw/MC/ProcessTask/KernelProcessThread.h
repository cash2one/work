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
	//线程初始化
	virtual DWORD CreateThread();
	virtual DWORD RestThread();
	//线程退出
	virtual DWORD DestoryThread();
	virtual DWORD DispatchMessage(T_Message *pMsg);


	/*
	@breif  开始任务
	@param  任务索引
	*/
	DWORD DoWorkToKernel(DWORD dwIndex);

private:
	bool  bIsFirst;  //是否第一次做任务
	bool  bIsObjectFirst; //是否单线程第一次做任务
	bool  bIsSetIE; //是否设置IE模式
	CAllTaskData   taskData;     //任务数据
};

