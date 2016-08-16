#include "StdAfx.h"
#include "DocEvtHandlerMgr.h"


CDocEvtHandlerMgr::CDocEvtHandlerMgr(CHtmlView *pView)
	:m_dwDocCookie(0),
	m_pHtmlView(pView),
	m_pDispDoc(NULL),
	m_pEventHandler(NULL)
{
	m_pEventHandler = new CDocEvtHandler(m_pHtmlView);
}


CDocEvtHandlerMgr::~CDocEvtHandlerMgr(void)
{
	UninstallEventHandler();
	if (!m_pEventHandler)
		delete m_pEventHandler;
}

void CDocEvtHandlerMgr::InstallEventHandler()
{
	// 已安装，卸载先。最后一次安装的才有效
	if(m_dwDocCookie)   
		UninstallEventHandler();

	m_pDispDoc = m_pHtmlView->GetHtmlDocument();
	IConnectionPointContainerPtr pCPC = m_pDispDoc;
	IConnectionPointPtr pCP;

	// 找到安装点
	pCPC->FindConnectionPoint(DIID_HTMLDocumentEvents2, &pCP);
	IUnknown* pUnk = m_pEventHandler->GetInterface(&IID_IUnknown);

	//安装
	HRESULT hr = pCP->Advise(pUnk, &m_dwDocCookie);
	if(!SUCCEEDED(hr))  // 安装失败
	{
		m_dwDocCookie = 0;
		g_runlog.Trace(LOGL_TOP,LOGC_ERROR,__TFILE__, __LINE__,_T("安装事件处理函数失败！hr=0x%08x"), hr);
	}
}

void CDocEvtHandlerMgr::UninstallEventHandler()
{
	if(0 == m_dwDocCookie) 
		return;

	IConnectionPointContainerPtr pCPC = m_pDispDoc;
	IConnectionPointPtr pCP;
	pCPC->FindConnectionPoint(DIID_HTMLDocumentEvents2, &pCP);
    HRESULT	hr = pCP->Unadvise(m_dwDocCookie);
	if (!SUCCEEDED(hr))
		g_runlog.Trace(LOGL_TOP,LOGC_ERROR,__TFILE__, __LINE__,_T("卸载事件处理函数失败！hr=0x%08x"), hr);
}