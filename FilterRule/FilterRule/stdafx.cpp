
// stdafx.cpp : 只包括标准包含文件的源文件
// FilterRule.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

CLogTrace g_log("FilterRule.log", NULL);

TCHAR *GetCurPath(void)
{
	static TCHAR s_szCurPath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, s_szCurPath, MAX_PATH);

	TCHAR *p = _tcsrchr(s_szCurPath, '\\');
	*p = 0;

	return s_szCurPath;
}