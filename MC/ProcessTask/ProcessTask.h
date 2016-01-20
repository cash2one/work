// Kernel4.h : Kernel4 DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CKernel4App
// 有关此类实现的信息，请参阅 Kernel4.cpp
//

class CKernel4App : public CWinApp
{
public:
	CKernel4App();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
