
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// FilterRule.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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