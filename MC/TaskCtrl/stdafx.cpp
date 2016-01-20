// stdafx.cpp : 只包括标准包含文件的源文件
// TaskMgr.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

CLogTrace g_tskLog(_T("TaskCtrl.log"), NULL);
IGlobalDataBlock *g_GlobalDataBlock = NULL;
IServerSocket *g_Socket = NULL;
IConfigInfo *g_Config = NULL;



// TODO: 在 STDAFX.H 中
//引用任何所需的附加头文件，而不是在此文件中引用
