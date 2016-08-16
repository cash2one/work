#include "stdafx.h"

#if _MFC_VER >= 0x0700
#include <afxocc.h>
#else
#ifdef _AFXDLL
#undef AFX_DATA
#define AFX_DATA AFX_DATA_IMPORT
#endif
#include <..\src\occimpl.h>
#ifdef _AFXDLL
#undef AFX_DATA
#define AFX_DATA AFX_DATA_EXPORT
#endif
#endif

#include <mshtmhst.h>

#include "DocHostSite.h"
#include "LhpHtmlView.h"

BEGIN_INTERFACE_MAP(CDocHostSite, COleControlSite)
	INTERFACE_PART(CDocHostSite, IID_IOleCommandTarget, OleCommandTarget)
	INTERFACE_PART(CDocHostSite, IID_IDocHostShowUI, DocHostShowUI)
	INTERFACE_PART(CDocHostSite, IID_IDocHostUIHandler, DocHostUIHandler)
#if (_WIN32_IE >= 0x0501) // IE 5.5 and higher
	INTERFACE_PART(CDocHostSite, IID_IDocHostUIHandler2, DocHostUIHandler2)
#endif
	INTERFACE_PART(CDocHostSite, IID_IInternetSecurityManager, InternetSecurityManager)
	INTERFACE_PART(CDocHostSite, IID_IServiceProvider, ServiceProvider)
END_INTERFACE_MAP()

CDocHostSite::CDocHostSite(COleControlContainer * pOcc, CLhpHtmlView* pView)
	: COleControlSite( pOcc )
{
	m_pView = pView;
}

CDocHostSite::~CDocHostSite()
{
}

ULONG CDocHostSite::XDocHostShowUI::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XDocHostShowUI::Release()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XDocHostShowUI::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->ExternalQueryInterface( &riid, ppvObj );
}

ULONG CDocHostSite::XOleCommandTarget::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, OleCommandTarget)
		return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XOleCommandTarget::Release()
{
	METHOD_PROLOGUE(CDocHostSite, OleCommandTarget)
		return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XOleCommandTarget::QueryInterface(
	REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, OleCommandTarget)
		return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

HRESULT CDocHostSite::XOleCommandTarget::QueryStatus(
	const GUID* pguidCmdGroup,
	ULONG cCmds,    						 
	OLECMD *prgCmds,    
	OLECMDTEXT *pCmdText)
{
	METHOD_PROLOGUE(CDocHostSite, OleCommandTarget)
		return S_FALSE;
}

HRESULT CDocHostSite::XOleCommandTarget::Exec(
	const GUID* pguidCmdGroup, 
	DWORD nCmdID,
	DWORD nCmdexecopt, 
	VARIANTARG* pvaIn, 
	VARIANTARG* pvaOut)
{
	HRESULT hr = S_OK;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{

		switch (nCmdID) 
		{
      
	    //屏蔽脚本错误
		case OLECMDID_SHOWSCRIPTERROR:
			{
				IHTMLDocument2*             pDoc = NULL;
				IHTMLWindow2*               pWindow = NULL;
				IHTMLEventObj*              pEventObj = NULL;
				BSTR                        rgwszNames[5] = 
				{ 
					SysAllocString(L"errorLine"),
					SysAllocString(L"errorCharacter"),
					SysAllocString(L"errorCode"),
					SysAllocString(L"errorMessage"),
					SysAllocString(L"errorUrl")
				};
				DISPID                      rgDispIDs[5];
				VARIANT                     rgvaEventInfo[5];
				DISPPARAMS                  params;
				BOOL                        fContinueRunningScripts = true;
				int	                        i;
				
				params.cArgs = 0;
				params.cNamedArgs = 0;

				// Get the document that is currently being viewed.
				hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc);				
				// Get document.parentWindow.
				hr = pDoc->get_parentWindow(&pWindow);
#ifdef xiaoqiang
				pDoc->Release();
#endif
				// Get the window.event object.
				hr = pWindow->get_event(&pEventObj);
				// Get the error info from the window.event object.
				for (i = 0; i < 5; i++) 
				{  
					// Get the property's dispID.
					hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1, 
						LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
					// Get the value of the property.
					hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
						LOCALE_SYSTEM_DEFAULT,
						DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
						NULL, NULL);
					SysFreeString(rgwszNames[i]);
				}

				// At this point, you would normally alert the user with 
				// the information about the error, which is now contained
				// in rgvaEventInfo[]. Or, you could just exit silently.

				
				g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__, \
					_T("捕获到脚本错误:\r\nerrorLine:%d\r\nerrorMessage:%s\r\nerrorUrl:%s\r\n\r\n"), \
					rgvaEventInfo[0].intVal, rgvaEventInfo[3].bstrVal, rgvaEventInfo[4].bstrVal);

				(*pvaOut).vt = VT_BOOL;
				if (fContinueRunningScripts)
				{
					// Continue running scripts on the page.
					(*pvaOut).boolVal = VARIANT_TRUE;
				}
				else
				{
					// Stop running scripts on the page.
					(*pvaOut).boolVal = VARIANT_FALSE;			
				} 
				break;
			}
		case OLECMDID_ENABLE_INTERACTION:
			g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获到:Exec OLECMDID_ENABLE_INTERACTION"));

		case OLECMDID_COPY:
			g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获到:Exec OLECMDID_COPY"));

			break;
		case OLECMDID_PASTE:
			g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获到:Exec OLECMDID_PASTE"));

			break;
		case OLECMDID_SELECTALL:

			break;
		case OLECMDID_SHOWMESSAGE:
			g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获到:Exec OLECMDID_SHOWMESSAGE"));

			break;
		case OLECMDID_PAGEACTIONBLOCKED:

			break;
		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}
	else
	{
		hr = OLECMDERR_E_UNKNOWNGROUP;
	}
	return (hr);
}

HRESULT CDocHostSite::XDocHostShowUI::ShowHelp(HWND hwnd,
											   LPOLESTR pszHelpFile,
											   UINT nCommand,
											   DWORD dwData,
											   POINT ptMouse,
											   IDispatch * pDispatchObjectHit)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return E_NOTIMPL;
}

HRESULT CDocHostSite::XDocHostShowUI::ShowMessage(HWND hwnd,
												  LPOLESTR lpstrText,
												  LPOLESTR lpstrCaption,
												  DWORD dwType,
												  LPOLESTR lpstrHelpFile,
												  DWORD dwHelpContext,
												  LRESULT * plResult)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->m_pView->OnShowMessage(hwnd,lpstrText,lpstrCaption,dwType,lpstrHelpFile,dwHelpContext,plResult);
}

ULONG CDocHostSite::XDocHostUIHandler::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XDocHostUIHandler::Release()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XDocHostUIHandler::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->ExternalQueryInterface( &riid, ppvObj );
}

HRESULT CDocHostSite::XDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO * pInfo)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnGetHostInfo( pInfo );
}

HRESULT CDocHostSite::XDocHostUIHandler::ShowUI(DWORD dwID,
											  IOleInPlaceActiveObject * pActiveObject,
											  IOleCommandTarget * pCommandTarget,
											  IOleInPlaceFrame * pFrame,
											  IOleInPlaceUIWindow * pDoc)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnShowUI( dwID, pActiveObject, pCommandTarget,
		pFrame, pDoc );
}

HRESULT CDocHostSite::XDocHostUIHandler::HideUI()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnHideUI();
}

HRESULT CDocHostSite::XDocHostUIHandler::UpdateUI()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnUpdateUI();
}

HRESULT CDocHostSite::XDocHostUIHandler::EnableModeless(BOOL fEnable)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnEnableModeless( fEnable );
}

HRESULT CDocHostSite::XDocHostUIHandler::OnDocWindowActivate(BOOL fEnable)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnDocWindowActivate( fEnable );
}

HRESULT CDocHostSite::XDocHostUIHandler::OnFrameWindowActivate(BOOL fEnable)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnFrameWindowActivate( fEnable );
}

HRESULT CDocHostSite::XDocHostUIHandler::ResizeBorder(LPCRECT prcBorder,
													IOleInPlaceUIWindow * pUIWindow,
													BOOL fFrameWindow)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnResizeBorder( prcBorder, pUIWindow, fFrameWindow );
}

HRESULT CDocHostSite::XDocHostUIHandler::ShowContextMenu(DWORD dwID,
													   POINT * ppt,
													   IUnknown * pcmdtReserved,
													   IDispatch * pdispReserved)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnShowContextMenu( dwID, ppt, pcmdtReserved,
		pdispReserved );
}

HRESULT CDocHostSite::XDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg,
															const GUID * pguidCmdGroup,
															DWORD nCmdID)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return E_NOTIMPL;
	return pThis->m_pView->OnTranslateAccelerator( lpMsg,
		pguidCmdGroup, nCmdID );
}

HRESULT CDocHostSite::XDocHostUIHandler::GetOptionKeyPath(LPOLESTR * pchKey,
														DWORD dw)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnGetOptionKeyPath( pchKey, dw );
}

HRESULT CDocHostSite::XDocHostUIHandler::GetDropTarget(IDropTarget * pDropTarget,
													 IDropTarget ** ppDropTarget)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnGetDropTarget( pDropTarget, ppDropTarget );
}

HRESULT CDocHostSite::XDocHostUIHandler::GetExternal(IDispatch ** ppDispatch)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnGetExternal( ppDispatch );
}

HRESULT CDocHostSite::XDocHostUIHandler::TranslateUrl(DWORD dwTranslate,
													OLECHAR * pchURLIn,
													OLECHAR ** ppchURLOut)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnTranslateUrl( dwTranslate, pchURLIn, ppchURLOut );
}

HRESULT CDocHostSite::XDocHostUIHandler::FilterDataObject(IDataObject * pDO,
														IDataObject ** ppDORet)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnFilterDataObject( pDO, ppDORet );
}

#if (_WIN32_IE >= 0x0501) // IE 5.5 and higher

ULONG CDocHostSite::XDocHostUIHandler2::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler2);

	return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XDocHostUIHandler2::Release()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler2);

	return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XDocHostUIHandler2::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler2);

	return pThis->ExternalQueryInterface( &riid, ppvObj );
}

HRESULT CDocHostSite::XDocHostUIHandler2::GetHostInfo(DOCHOSTUIINFO *)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::ShowUI(DWORD /*dwID*/, 
												 IOleInPlaceActiveObject * /*pActiveObject*/,
												 IOleCommandTarget * /*pCommandTarget*/,
												 IOleInPlaceFrame * /*pFrame*/,
												 IOleInPlaceUIWindow * /*pDoc*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::HideUI()
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::UpdateUI()
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::EnableModeless(BOOL /*fEnable*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::OnDocWindowActivate(BOOL /*fActivate*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::OnFrameWindowActivate(BOOL /*fActivate*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::ResizeBorder(LPCRECT /*prcBorder*/, 
													   IOleInPlaceUIWindow* /*pUIWindow*/,
													   BOOL /*fRameWindow*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::ShowContextMenu(DWORD /*dwID*/, 
														  POINT* /*pptPosition*/,
														  IUnknown* /*pCommandTarget*/,
														  IDispatch* /*pDispatchObjectHit*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::TranslateAccelerator(LPMSG /*lpMsg*/,
															   const GUID * /*pguidCmdGroup*/,
															   DWORD /*nCmdID*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::GetOptionKeyPath(BSTR* /*pbstrKey*/, DWORD)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::GetDropTarget(IDropTarget * /*pDropTarget*/,
														IDropTarget ** /*ppDropTarget*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::GetExternal(IDispatch ** /*ppDispatch*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::TranslateUrl(DWORD /*dwTranslate*/,
													   OLECHAR * /*pchURLIn*/,
													   OLECHAR ** /*ppchURLOut*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::FilterDataObject(IDataObject * /*pDO*/,
														   IDataObject ** /*ppDORet*/)
{
	ASSERT(FALSE);

	return S_FALSE;
}

HRESULT CDocHostSite::XDocHostUIHandler2::GetOverrideKeyPath(LPOLESTR * pchKey, DWORD dw)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler2);

	//return pThis->m_pView->OnGetOverrideKeyPath( pchKey, dw );
	return S_OK;
}
#endif

// InternetSecurityManager Methods
HRESULT FAR EXPORT CDocHostSite::XInternetSecurityManager
										::QueryInterface(REFIID riid, void** ppvObj)
{

	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
    HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}

ULONG FAR EXPORT CDocHostSite::XInternetSecurityManager::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return pThis->ExternalAddRef();
}

ULONG FAR EXPORT CDocHostSite::XInternetSecurityManager::Release()
{                            

	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XInternetSecurityManager
							::SetSecuritySite (IInternetSecurityMgrSite *pSite)
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

HRESULT CDocHostSite::XInternetSecurityManager
							::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{

	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

HRESULT CDocHostSite::XInternetSecurityManager
							::MapUrlToZone(LPCWSTR pwszUrl,DWORD *pdwZone,DWORD dwFlags)
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}


HRESULT  FAR EXPORT CDocHostSite::XInternetSecurityManager
										::GetSecurityId(LPCWSTR pwszUrl,
										BYTE *pbSecurityId,
										DWORD *pcbSecurityId, 
										DWORD dwReserved)
{

	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

 
STDMETHODIMP CDocHostSite::XInternetSecurityManager
				::ProcessUrlAction(	/* [in] */ LPCWSTR pwszUrl,
									/* [in] */ DWORD dwAction,
									/* [size_is][out] */ BYTE __RPC_FAR *pPolicy,
									/* [in] */ DWORD cbPolicy,
									/* [in] */ BYTE __RPC_FAR *pContext,
									/* [in] */ DWORD cbContext,
									/* [in] */ DWORD dwFlags,
									/* [in] */ DWORD dwReserved)
{

	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)

	DWORD dwPolicy=URLPOLICY_ALLOW;
	
	BOOL bNoActiveX=FALSE;
	BOOL bNoJava=FALSE;
	BOOL bNoScript=FALSE;
	BOOL bCrossData=FALSE;

	if (dwAction <= URLACTION_ACTIVEX_MAX && dwAction >= URLACTION_ACTIVEX_MIN)
		dwPolicy = bNoActiveX ? URLPOLICY_DISALLOW : URLPOLICY_ALLOW;
	else if ((dwAction <= URLACTION_JAVA_MAX && dwAction >= URLACTION_JAVA_MIN) ||
				URLACTION_HTML_JAVA_RUN == dwAction)
		if (bNoJava)
			dwPolicy = URLPOLICY_JAVA_PROHIBIT;
		else
			return INET_E_DEFAULT_ACTION;
	else if (dwAction <= URLACTION_SCRIPT_MAX && dwAction >= URLACTION_SCRIPT_MIN)
		dwPolicy = bNoScript ? URLPOLICY_DISALLOW : URLPOLICY_ALLOW;
// !! If the compiler can't find URLACTION_CROSS_DOMAIN_DATA, make sure you are building with
// !! the latest version of the IE headers -- URLMON.H specifically -- from MSDN Downloads for the 
// !! Web Workshop or the Platform SDK
	else if (URLACTION_CROSS_DOMAIN_DATA == dwAction)
		dwPolicy = bCrossData ? URLPOLICY_ALLOW : URLPOLICY_DISALLOW;
	else
		return INET_E_DEFAULT_ACTION;

	if ( cbPolicy >= sizeof (DWORD))
	{
		*(DWORD*) pPolicy = dwPolicy;
		return S_OK;
	} 
	else 
	{
		return S_FALSE;
	}
}

HRESULT CDocHostSite::XInternetSecurityManager
							::QueryCustomPolicy(LPCWSTR pwszUrl,
												REFGUID guidKey,
												BYTE **ppPolicy,
												DWORD *pcbPolicy,
												BYTE *pContext,
												DWORD cbContext,
												DWORD dwReserved)
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

HRESULT CDocHostSite::XInternetSecurityManager
							::SetZoneMapping(DWORD dwZone,
											LPCWSTR lpszPattern,
											DWORD dwFlags)
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}

HRESULT CDocHostSite::XInternetSecurityManager
							::GetZoneMappings(DWORD dwZone,
											IEnumString **ppenumString, 
											DWORD dwFlags)
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
	return INET_E_DEFAULT_ACTION;
}


// ServiceProvider Methods
ULONG FAR EXPORT CDocHostSite::XServiceProvider::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, ServiceProvider)
	return pThis->ExternalAddRef();
}

ULONG FAR EXPORT CDocHostSite::XServiceProvider::Release()
{                            
	METHOD_PROLOGUE(CDocHostSite, ServiceProvider)
	return pThis->ExternalRelease();
}

HRESULT FAR EXPORT CDocHostSite::XServiceProvider
										::QueryInterface(REFIID riid, 
														void** ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, ServiceProvider)
    HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}
STDMETHODIMP CDocHostSite::XServiceProvider
								::QueryService(REFGUID guidService,  
												REFIID riid,
												void** ppvObject)
{

	if (guidService == SID_SInternetSecurityManager && 
					riid == IID_IInternetSecurityManager)
	{
		METHOD_PROLOGUE(CDocHostSite, ServiceProvider)
		HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObject);
		return hr;
	} 
	else 
	{
		*ppvObject = NULL;

	}
	return E_NOINTERFACE;
}