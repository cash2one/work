#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

//#include "resource.h"		// ������


// CPrePareDLLApp
// �йش���ʵ�ֵ���Ϣ������� CPrePareDLLApp.cpp
//

class CPrePareDLLApp : public CWinApp
{
public:
	CPrePareDLLApp();

	// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};