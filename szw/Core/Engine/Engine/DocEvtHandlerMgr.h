#pragma once

#include "DocEvtHandler.h"
// #include "Sumengine2View.h"

class CDocEvtHandlerMgr
{
public:
	CDocEvtHandlerMgr(CHtmlView *pView);
	virtual ~CDocEvtHandlerMgr(void);

	void InstallEventHandler();
	void UninstallEventHandler();

private:
	CDocEvtHandler *m_pEventHandler;
	DWORD m_dwDocCookie;    // 用于卸载事件响应函数
	IDispatch *m_pDispDoc;  // 用于卸载事件响应函数
	CHtmlView *m_pHtmlView;
};

