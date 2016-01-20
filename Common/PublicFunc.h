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
add by zhumingxing 20150619---设置窗口透明属性，0表示全透明，255表示不透明
设置0之后无法操作界面控件，所以默认设置为1
*/
bool setWndTransparent(HWND hwnd, byte wTransparency=1);

//获取窗口透明属性值
void GetWndTransparentValue(HWND hwnd, BYTE& balpha);

//从flash object控件中解析出对应的flash地址
bool GetFlashURL(CString& strURL, const CString& strOutHtml);

//拼接flash验证码HTML
CString comBineFlashHtml(const CString& strURL, const CString& strUniID);