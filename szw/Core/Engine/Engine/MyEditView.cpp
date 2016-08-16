// MyEditView.cpp : 实现文件
//

#include "stdafx.h"
#include "Engine.h"
#include "MyEditView.h"


// CMyEditView

IMPLEMENT_DYNCREATE(CMyEditView, CEditView)

CMyEditView::CMyEditView()
{

}

CMyEditView::~CMyEditView()
{
}

BEGIN_MESSAGE_MAP(CMyEditView, CEditView)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


// CMyEditView 诊断

#ifdef _DEBUG
void CMyEditView::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyEditView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyEditView 消息处理程序

//处理反射消息 修改背景为白色（即便只读时也起作用）
HBRUSH CMyEditView::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkMode(TRANSPARENT);   
	pDC->SetTextColor(RGB(0, 0, 0));                     //字的颜色   
	HBRUSH hBr = CreateSolidBrush(RGB(255, 255, 255));   //背景色   

	return   (HBRUSH)hBr;   
}


void CMyEditView::OnInitialUpdate()
{
	CEditView::OnInitialUpdate();

	//禁止修改源码
	GetEditCtrl().SetReadOnly(TRUE);
}
