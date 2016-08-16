#pragma once
#include <Windows.h>

__int64 CompareFileTime(FILETIME time1, FILETIME time2);

class GetCpuUseage
{
public:
	GetCpuUseage();
	~GetCpuUseage();
public:
	double CpuUseage();
private:
	FILETIME m_preidleTime;
	FILETIME m_prekernelTime;
	FILETIME m_preuserTime;

};