#include "stdafx.h"
#include "DocEvtHandler.h"
#include "mshtmdid.h"



IMPLEMENT_DYNAMIC(CDocEvtHandler, CCmdTarget)

CDocEvtHandler::CDocEvtHandler(CHtmlView *pView)
{
	m_pNotifyView = pView;

	EnableAutomation();  // 重要：激活 IDispatch
}

CDocEvtHandler::~CDocEvtHandler()
{
}

BEGIN_MESSAGE_MAP(CDocEvtHandler, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDocEvtHandler, CCmdTarget)
	DISP_FUNCTION_ID(CDocEvtHandler,"HTMLELEMENTEVENTS2_ONCLICK",
	DISPID_HTMLELEMENTEVENTS2_ONCLICK, OnClick,	VT_EMPTY, VTS_DISPATCH)

	DISP_FUNCTION_ID(CDocEvtHandler,"HTMLELEMENTEVENTS2_ONMOUSEMOVE",
	DISPID_HTMLELEMENTEVENTS2_ONMOUSEMOVE, OnMouseMove,
	VT_EMPTY, VTS_DISPATCH)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CDocEvtHandler, CCmdTarget)
	INTERFACE_PART(CDocEvtHandler, DIID_HTMLButtonElementEvents2, Dispatch)
END_INTERFACE_MAP()

void CDocEvtHandler::OnClick(MSHTML::IHTMLEventObjPtr pEvtObj)
{
// 	MSHTML::IHTMLElementPtr pElement = pEvtObj->GetsrcElement(); // 事件发生的对象元素
// 
// 	_bstr_t strTagname;
// 	pElement->get_innerHTML(&strTagname.GetBSTR());
// 	CString strMsg = strTagname;
}

void CDocEvtHandler::OnMouseMove(MSHTML::IHTMLEventObjPtr pEvtObj)
{
	MSHTML::IHTMLElementPtr pElement = pEvtObj->GetsrcElement(); // 事件发生的对象元素

	//活动对象改变时进行处理
	if (m_pLastActiveElement != pElement)
	{
		m_pLastActiveElement = pElement;

		_bstr_t strTagname;
		pElement->get_outerHTML(&strTagname.GetBSTR());
		CStdString strMsg = strTagname;

		m_pNotifyView->SendMessage(WM_VIEW_EVENT_ACTIVE_ELEMENT, (WPARAM)(LPCTSTR)strMsg);
	}
}