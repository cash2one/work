#pragma once

#include "stdstring.h"
#include <atlstr.h>
#include "enumdef.h"

CStdString ComBineUrl(CStdString strBase, CStdString strRelative);
CString ComBineUrl(CString strBase, CString strRelative);

//????????????,???html??,??code???
DWORD WINAPI ComBineHtml(CStringArray &ImagePathArray,EPicShowType ePicShowType,
	CString strIndex,CString &strRetPath, CString strInputPrompt = _T(""));

/*
add by zhumingxing 20150619---���ô���͸�����ԣ�0��ʾȫ͸����255��ʾ��͸��
����0֮���޷���������ؼ�������Ĭ������Ϊ1
*/
bool setWndTransparent(HWND hwnd, byte wTransparency=1);

//��ȡ����͸������ֵ
void GetWndTransparentValue(HWND hwnd, BYTE& balpha);

//��flash object�ؼ��н�������Ӧ��flash��ַ
bool GetFlashURL(CString& strURL, const CString& strOutHtml);

//ƴ��flash��֤��HTML
CString comBineFlashHtml(const CString& strURL, const CString& strUniID);