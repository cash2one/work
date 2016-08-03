#pragma once
#include "common_stru.h"

//任务文件
class ITaskFile
{
public:
	//参数说明：保存文件名，任务产品图片，任务结构
	virtual DWORD WritePackageFile(TCHAR *strFileName,TCHAR *strPicFile,T_TaskData *pTaskData) = 0;
	virtual DWORD ReadPackageFile(TCHAR *strFileName,T_TaskData *pTaskData) = 0;
};

class IOCRFile
{
public:
	virtual DWORD AddFileToList(TCHAR *strFile) = 0;
	virtual DWORD PackFile(TCHAR *strFile,TCHAR *strDir) = 0;	//打包
	virtual DWORD UnPackFile(TCHAR *strFile,TCHAR *strDir) = 0; //解包
};
