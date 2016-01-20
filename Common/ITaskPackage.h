#pragma once
#include "common_stru.h"

//�����ļ�
class ITaskFile
{
public:
	//����˵���������ļ����������ƷͼƬ������ṹ
	virtual DWORD WritePackageFile(TCHAR *strFileName,TCHAR *strPicFile,T_TaskData *pTaskData) = 0;
	virtual DWORD ReadPackageFile(TCHAR *strFileName,T_TaskData *pTaskData) = 0;
};

class IOCRFile
{
public:
	virtual DWORD AddFileToList(TCHAR *strFile) = 0;
	virtual DWORD PackFile(TCHAR *strFile,TCHAR *strDir) = 0;	//���
	virtual DWORD UnPackFile(TCHAR *strFile,TCHAR *strDir) = 0; //���
};
