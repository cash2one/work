// Kernel4.h : Kernel4 DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CKernel4App
// �йش���ʵ�ֵ���Ϣ������� Kernel4.cpp
//

class CKernel4App : public CWinApp
{
public:
	CKernel4App();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
