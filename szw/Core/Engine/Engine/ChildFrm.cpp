
// ChildFrm.cpp : CChildFrame 类的实现
//

#include "stdafx.h"
#include "Engine.h"
#include "EngineDoc.h"
#include "EngineView.h"
#include "ChildFrm.h"
#include "MyEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame
bool CChildFrame::m_sbViewSrcFlag = false;

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_MDIACTIVATE()
	ON_MESSAGE(WM_MAINWND_SPLITE_WINDOW, &CChildFrame::OnViewSourceWnd)

END_MESSAGE_MAP()

// CChildFrame 构造/析构

CChildFrame::CChildFrame()
{
	// TODO: 在此添加成员初始化代码
}

CChildFrame::~CChildFrame()
{
	/*if(m_splitWnd)
	{
		m_splitWnd.DeleteView(0,0);
		m_splitWnd.DeleteView(1,0);
	}*/
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 诊断

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	//创建2个拆分窗口
	if (!m_splitWnd.CreateStatic(this, 2, 1))
		return FALSE;

	CRect rect;
	GetClientRect(&rect);

	if (m_sbViewSrcFlag)
	{
		if (!m_splitWnd.CreateView(0, 0, RUNTIME_CLASS(CMyEditView), CSize(rect.Width(), rect.Height()/4),pContext) ||
			!m_splitWnd.CreateView(1, 0, RUNTIME_CLASS(CSumengine2View), CSize(rect.Width(), rect.Height()/4*3),pContext))
		{
			return FALSE;
		}
	}
	else
	{
		if (!m_splitWnd.CreateView(0, 0, RUNTIME_CLASS(CMyEditView), CSize(rect.Width(), 0),pContext) ||
			!m_splitWnd.CreateView(1, 0, RUNTIME_CLASS(CSumengine2View), CSize(rect.Width(), rect.Height()),pContext))
		{
			return FALSE;
		}
	}

	return TRUE;

	//return CMDIChildWndEx::OnCreateClient(lpcs, pContext);

}

//获取拆分视图
CWnd *CChildFrame::GetSourceWnd()
{
	//防止所有子框架关闭后，再调用时出现异常
	if (!m_splitWnd.m_hWnd)
		return NULL;

	return m_splitWnd.GetPane(0, 0);
}

//获取拆分视图
CWnd *CChildFrame::GetPageWnd()
{
	//防止所有子框架关闭后，再调用时出现异常
	if (!m_splitWnd.m_hWnd)
		return NULL;

	return m_splitWnd.GetPane(1, 0);
}


// CChildFrame 消息处理程序

//切换视图时触发
void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	//将当前视图所显示页面的URL设置到地址栏
	if (bActivate)
	{
		CWnd *pWnd = ((CChildFrame*)pActivateWnd)->GetPageWnd();

		if (pWnd)
		{
			/*CComQIPtr<IHTMLDocument2> spDoc = (IHTMLDocument2*)((CSumengine2View*)pWnd)->GetHtmlDocument();

			if (spDoc)
			{
				_bstr_t bstr;
				spDoc->get_URL(&bstr.GetBSTR());

				AfxGetMainWnd()->SendMessage(WM_MAINWND_UPDATE_ADDRESS_URL, (WPARAM)(LPCTSTR)bstr);	
			}//*/

			_bstr_t bstr;
			((CSumengine2View*)pWnd)->GetViewURL(bstr);
			if(bstr.length() != 0)
			{
				AfxGetMainWnd()->SendMessage(WM_MAINWND_UPDATE_ADDRESS_URL, (WPARAM)(LPCTSTR)bstr);	
			}
		}
	}//*/
}


LRESULT CChildFrame::OnViewSourceWnd(WPARAM wParam, LPARAM lParam)
{
	if (m_sbViewSrcFlag)
	{
		CRect rect;
		GetClientRect(&rect);
		m_splitWnd.SetRowInfo(0, rect.Height() / 4, 0);
	}
	else
		m_splitWnd.SetRowInfo(0, 0, 0);         //首行调度设为0

	m_splitWnd.RecalcLayout();              //重新显示大小

	return 0;
}

BOOL CChildFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CMDIChildWnd::PreTranslateMessage(pMsg);
}

BOOL CChildFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CMDIChildWnd::OnCommand(wParam, lParam);
}

void CChildFrame::OnClose()
{
	/*if(m_splitWnd)
	{
		m_splitWnd.DeleteView(0,0);
		m_splitWnd.DeleteView(1,0);
	}*/
	g_runlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("CChildFrame收到关闭命令"));
	CMDIChildWnd::OnClose();
}
