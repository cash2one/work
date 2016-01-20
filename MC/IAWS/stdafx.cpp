// stdafx.cpp : 只包括标准包含文件的源文件
// WebServiceEx.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


IGlobalDataBlock *g_GlobalDataBlock;


IServerSocket *g_ServerSocket;

IConfigInfo *g_Config = NULL;

CLogTrace g_log(_T("IAWS.log"), NULL);
