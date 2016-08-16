#pragma once
#import <mshtml.tlb>

class CDocEvtHandler: public CCmdTarget
{
	DECLARE_DYNAMIC(CDocEvtHandler)

public:
	CDocEvtHandler(CHtmlView *pView);
	virtual ~CDocEvtHandler();

	// 消息处理函数
	void OnClick(MSHTML::IHTMLEventObjPtr pEvtObj);

	void OnMouseMove(MSHTML::IHTMLEventObjPtr pEvtObj);

private:
	CHtmlView *m_pNotifyView;                  //接收当前对象数据的View
	MSHTML::IHTMLElementPtr m_pLastActiveElement;        //上次激活对象

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

