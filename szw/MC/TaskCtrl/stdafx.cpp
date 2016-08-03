// stdafx.cpp : 只包括标准包含文件的源文件
// TaskCtl.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

CLogTrace g_SuccAcc(_T("SuccAccount.log"),NULL);
CLogTrace g_log(_T("TaskCtrl.log"), NULL);
IGlobalDataBlock *g_GlobalDataBlock = NULL;
IServerSocket *g_Socket = NULL;
IXMLRW *g_Config = NULL;