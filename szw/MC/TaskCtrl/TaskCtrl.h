// TaskCtl.h : TaskCtl DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTaskCtlApp
// �йش���ʵ�ֵ���Ϣ������� TaskCtl.cpp
//

class CTaskCtlApp : public CWinApp
{
public:
	CTaskCtlApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
