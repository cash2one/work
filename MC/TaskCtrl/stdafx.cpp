// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// TaskMgr.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

CLogTrace g_tskLog(_T("TaskCtrl.log"), NULL);
IGlobalDataBlock *g_GlobalDataBlock = NULL;
IServerSocket *g_Socket = NULL;
IConfigInfo *g_Config = NULL;



// TODO: �� STDAFX.H ��
//�����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
