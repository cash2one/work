#pragma once
#include "stdafx.h"

class CMySplitter :public CSplitterWnd
{
	DECLARE_DYNAMIC(CMySplitter)
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
};