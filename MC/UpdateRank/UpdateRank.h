// GetUpdateRank.h : GetUpdateRank DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGetUpdateRankApp
// �йش���ʵ�ֵ���Ϣ������� GetUpdateRank.cpp
//

class CGetUpdateRankApp : public CWinApp
{
public:
	CGetUpdateRankApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
typedef BOOL  (*ProcGetQuickPhoto)(CStdString strWord, IServerSocket *pSocket);