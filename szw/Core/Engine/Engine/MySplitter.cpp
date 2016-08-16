#include "stdafx.h"
#include "MySplitter.h"
#include "Engine.h"

IMPLEMENT_DYNAMIC(CMySplitter, CSplitterWnd)

BEGIN_MESSAGE_MAP(CMySplitter, CSplitterWnd)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

extern CSumengine2App theApp;

LRESULT CMySplitter::OnNcHitTest(CPoint point)
{
	//正常模式不允许拖动分隔条
	if (theApp.GetCurrentRunMode() == rmNormal)
	{
		return HTNOWHERE;
	}	
	else
	{
		return CSplitterWnd::OnNcHitTest(point);
	}
}
