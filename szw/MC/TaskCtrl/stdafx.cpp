// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// TaskCtl.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

CLogTrace g_SuccAcc(_T("SuccAccount.log"),NULL);
CLogTrace g_log(_T("TaskCtrl.log"), NULL);
IGlobalDataBlock *g_GlobalDataBlock = NULL;
IServerSocket *g_Socket = NULL;
IXMLRW *g_Config = NULL;