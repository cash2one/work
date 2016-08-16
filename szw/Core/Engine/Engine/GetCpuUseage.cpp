#include "stdafx.h"
#include "GetCpuUseage.h"

__int64 CompareFileTime(FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;

	return   (b - a);
}

GetCpuUseage::GetCpuUseage()
{	
	GetSystemTimes(&m_preidleTime, &m_prekernelTime, &m_preuserTime);
	Sleep(100);
}

GetCpuUseage::~GetCpuUseage()
{
	;
}

double GetCpuUseage::CpuUseage()
{
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);


	int idle = CompareFileTime(m_preidleTime, idleTime);
	int kernel = CompareFileTime(m_prekernelTime, kernelTime);
	int user = CompareFileTime(m_preuserTime, userTime);


	if (kernel + user == 0)
	{
		return 0.0;
	}	
	//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
	double cpu = (kernel + user - idle) * 100 / (kernel + user);

	m_preidleTime = idleTime;
	m_prekernelTime = kernelTime;
	m_preuserTime = userTime;
	return cpu;
}

