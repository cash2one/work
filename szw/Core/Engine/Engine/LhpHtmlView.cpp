﻿#include "stdafx.h"
#include "LhpHtmlView.h"
#include ".\lhphtmlview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CLhpHtmlView, CHtmlView)

BEGIN_DISPATCH_MAP(CLhpHtmlView, CHtmlView)
END_DISPATCH_MAP()

BEGIN_MESSAGE_MAP(CLhpHtmlView, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

CLhpHtmlView::CLhpHtmlView(BOOL isview)
{
	m_ContextMenuMode=DefaultMenu;
	m_DefaultMsgBoxTitle="LHP HTMLVIEW";
	m_HostInfoFlag = DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_NO3DBORDER;
	m_IsView = isview;
	m_spDoc = NULL;

	EnableAutomation();
}

CLhpHtmlView::~CLhpHtmlView()
{

}

BOOL CLhpHtmlView::CreateFromStatic(UINT nID, CWnd* pParent)
{
	CWnd* pStatic = pParent->GetDlgItem(nID);
	if(pStatic == NULL )
		return FALSE;

	CRect rc;
	pStatic->GetWindowRect(&rc);
	pParent->ScreenToClient(&rc);
	pStatic->DestroyWindow();

	if(!CHtmlView::Create(NULL, NULL, (WS_CHILD | WS_VISIBLE ), rc, pParent, nID, NULL))
		return FALSE;
	m_IsView = FALSE;

	OnInitialUpdate();

	return TRUE;
}


//void CLhpHtmlView::SetContextMenuMode(CONTEXT_MENU_MODE MenuMode)
//{
//	m_ContextMenuMode=MenuMode;
//}
//
//CLhpHtmlView::CONTEXT_MENU_MODE CLhpHtmlView::GetContextMenuMode()
//{
//	return m_ContextMenuMode;
//}
//
//void CLhpHtmlView::SetDefaultMsgBoxTitle(CString strTitle)
//{
//	m_DefaultMsgBoxTitle=strTitle;
//}
//
//void CLhpHtmlView::SetHostInfoFlag(DWORD dwFlag)
//{
//	m_HostInfoFlag = dwFlag;
//}
//
BOOL CLhpHtmlView::CreateControlSite(COleControlContainer * pContainer, COleControlSite ** ppSite, UINT /*nID*/, REFCLSID /*clsid*/)
{
	//*ppSite = new CDocHostSite(pContainer, this);// 创建自己的控制站点实例
	*ppSite = new CDocHostSite(this->GetControlContainer(), this);// 创建自己的控制站点实例

	return (*ppSite) ? TRUE : FALSE;
}

HRESULT CLhpHtmlView::OnShowMessage(HWND hwnd,LPOLESTR lpstrText,LPOLESTR lpstrCaption,DWORD dwType,LPOLESTR lpstrHelpFile,DWORD dwHelpContext,LRESULT * plResult)
{
    // 窗口标题"Microsoft Internet Explorer"的资源标识
	#define IDS_MESSAGE_BOX_TITLE 2213
    //载入Shdoclc.dll 和IE消息框标题字符串
    HINSTANCE hinstSHDOCLC = LoadLibrary(TEXT("SHDOCLC.DLL"));
    if (hinstSHDOCLC == NULL)
        return S_FALSE;

 	CString strBuf,strCaption(lpstrCaption);
 	strBuf.LoadString(hinstSHDOCLC, IDS_MESSAGE_BOX_TITLE);

//      比较IE消息框标题字符串和lpstrCaption
//      如果相同，用自定义标题替换
 	if(strBuf==lpstrCaption)
         strCaption = m_DefaultMsgBoxTitle;
	strCaption = _T("捕获成功");

    // 创建自己的消息框并且显示
    *plResult = MessageBox(CString(lpstrText), strCaption, dwType);

	//卸载Shdoclc.dll并且返回
    FreeLibrary(hinstSHDOCLC);

	return S_OK;
}

//HRESULT CLhpHtmlView::OnGetHostInfo(DOCHOSTUIINFO * pInfo)
//{
//	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
//	pInfo->dwFlags = m_HostInfoFlag;
//	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
//
//	return S_OK;
//}
//
//HRESULT CLhpHtmlView::OnShowContextMenu(DWORD dwID, LPPOINT ppt,LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved)
//{
//	HRESULT result = S_FALSE;
//
//	switch(m_ContextMenuMode)
//	{
//	case NoContextMenu:				// 无菜单
//		result=S_OK;
//		break;
//	case DefaultMenu:				// 默认菜单
//		break;
//	case TextSelectionOnly:			// 仅文本选择菜单
//		if(!(dwID == CONTEXT_MENU_TEXTSELECT || dwID == CONTEXT_MENU_CONTROL))
//			result=S_OK;
//		break;
//	case CustomMenu:				// 自定义菜单
//		if(dwID!=CONTEXT_MENU_TEXTSELECT)
//			result=OnShowCustomContextMenu(ppt,pcmdtReserved,pdispReserved);
//		break;
//	}
//
//	return result;
//}
//
// HRESULT CLhpHtmlView::OnGetDropTarget(LPDROPTARGET pDropTarget, LPDROPTARGET* ppDropTarget )
// {
// 	m_DropTarget.SetIEDropTarget(pDropTarget);
// 
// 	LPDROPTARGET pMyDropTarget;
// 	pMyDropTarget = (LPDROPTARGET)m_DropTarget.GetInterface(&IID_IDropTarget);
// 	if(pMyDropTarget)
// 	{
// 		*ppDropTarget = pMyDropTarget;
// 		pMyDropTarget->AddRef();
// 		return S_OK;
// 	}
// 
// 	return S_FALSE;
// }

//HRESULT CLhpHtmlView::OnShowCustomContextMenu(LPPOINT ppt, LPUNKNOWN pcmdtReserved,LPDISPATCH pdispReserved)
//{
//	return S_OK;
//}
//
//HRESULT CLhpHtmlView::OnGetExternal(LPDISPATCH *lppDispatch)
//{
//	*lppDispatch = GetIDispatch(TRUE);
//	return S_OK;
//}
//
/*
void CLhpHtmlView::ElementSetFocus(CString ElemID)
{
	IHTMLDocument2* pDoc = NULL;
	IHTMLElementCollection* pAllElem = NULL;
	IHTMLElement* pElem = NULL;
	IHTMLControlElement* pControl=NULL;
	HRESULT hr = S_FALSE;

	pDoc = (IHTMLDocument2*)GetHtmlDocument();
	if(pDoc!=NULL)
	{
		hr = pDoc->get_all(&pAllElem);
		if(SUCCEEDED(hr) && pAllElem!=NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(0);
			hr = pAllElem->item(varID, varIdx, (LPDISPATCH*)&pElem);
			if(SUCCEEDED(hr) && pElem != NULL)
			{
				pElem->QueryInterface(&pControl);
				if(pControl!=NULL)
				{
					pControl->focus();
					pControl->Release();
				}
				pElem->Release();
			} // if(SUCCEEDED(hr) && pElem != NULL)
			pAllElem->Release();
		}//if(pAllElem!=NULL)
		pDoc->Release();
	}//if(pDoc!=NULL)
}

IHTMLStyle* CLhpHtmlView::GetElementStyle(CString ElemID)
{
	IHTMLDocument2* pDoc = NULL;
	IHTMLElementCollection* pElemColl = NULL;
	IDispatch* pElemDisp = NULL;
	IHTMLElement* pElem=NULL;
	IHTMLStyle* pStyle = NULL;
	
	BOOL Result=FALSE;

	pDoc = (IHTMLDocument2*)GetHtmlDocument();
	if(pDoc!=NULL)
	{
		pDoc->get_all(&pElemColl);
		if(pElemColl!=NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(0);
			HRESULT hr = pElemColl->item(varID, varIdx, &pElemDisp);
			if(SUCCEEDED(hr))
			{
				if(pElemDisp!=NULL)
				{
					hr = pElemDisp->QueryInterface(IID_IHTMLElement,(void**)&pElem);
					if(SUCCEEDED(hr))
					{
						hr = pElem->get_style(&pStyle);
						if(SUCCEEDED(hr))
							Result=TRUE;
						pElem->Release();
					}
					pElemDisp->Release();
				}
			}
			pElemColl->Release();
		}//if(pAllElem!=NULL)
		pDoc->Release();
	}//if(pDoc!=NULL)

	return pStyle;
}

BOOL CLhpHtmlView::GetElementValue(CString ElemID,CString& Value)
{
	Value.Empty();

	CComQIPtr<IHTMLDocument2> pDoc;
	CComQIPtr<IHTMLElementCollection> pAllElem;
	CComDispatchDriver pElemDisp;
	CComQIPtr<IHTMLElement> pElem;

	CComQIPtr<IHTMLInputElement> pInput;
	CComQIPtr<IHTMLSelectElement> pSelect;
	CComQIPtr<IHTMLTextAreaElement> pTextArea;
	
	
	BOOL Result = FALSE;
	HRESULT hr = S_FALSE;

	pDoc = (IHTMLDocument2*)GetHtmlDocument();
	if(pDoc != NULL)
	{
		hr = pDoc->get_all(&pAllElem);
		if(SUCCEEDED(hr) && pAllElem != NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(0);
			hr = pAllElem->item(varID, varIdx, &pElemDisp);
			if(SUCCEEDED(hr) && pElemDisp != NULL)
			{
				hr = pElemDisp->QueryInterface(IID_IHTMLElement,(void**)&pElem);
				if(SUCCEEDED(hr) && pElem != NULL)
				{
					BSTR bstrTagName;
					hr = pElem->get_tagName(&bstrTagName);
					if(SUCCEEDED(hr))
					{
						CString strTagName(bstrTagName);
						SysFreeString(bstrTagName);
						strTagName.MakeUpper();
						if(strTagName == _T("INPUT"))
						{
							hr = pElem->QueryInterface(&pInput);
							if(SUCCEEDED(hr) && pInput!=NULL)
							{
								BSTR bstrType;
								hr = pInput->get_type(&bstrType);
								if(SUCCEEDED(hr))
								{
									CString strType(bstrType);
									strType.MakeUpper();
									SysFreeString(bstrType);
									if(strType == _T("TEXT") || strType == _T("HIDDEN"))
									{
										BSTR bstrValue;
										hr = pInput->get_value(&bstrValue);
										if(SUCCEEDED(hr))
										{
											CString strValue(bstrValue);
											SysFreeString(bstrValue);
											Value = strValue;
											Result = TRUE;
										}
									} // if(strType == "TEXT" || strType == "HIDDEN")
									else if(strType == _T("CHECKBOX") || strType == _T("RADIO"))
									{
										VARIANT_BOOL Check;
										hr = pInput->get_checked(&Check);
										if(SUCCEEDED(hr))
										{
											if(Check == VARIANT_TRUE)
												Value = _T("1");
											else
												Value = _T("0");
											Result = TRUE;
										}
									} // else if(strType=="CHECKBOX" || strType == "RADIO")
								} // if(SUCCEEDED(hr))
								//pInput->Release();
							} // if(SUCCEEDED(hr) && pInput!=NULL)
						} // if(strTagName == "INPUT")
						else if(strTagName == _T("SELECT"))
						{
							hr = pElem->QueryInterface(&pSelect);
							if(SUCCEEDED(hr) && pSelect!=NULL)
							{
								long index;
								hr = pSelect->get_selectedIndex(&index);
								if(SUCCEEDED(hr))
								{
									Value.Format(_T("%d"),index);
									Result = TRUE;
								}
								//pSelect->Release();
							} // if(SUCCEEDED(hr) && pSelect!=NULL)
						} // else if(strTagName == "SELECT")
						else if(strTagName == _T("TEXTAREA"))
						{
							hr = pElem->QueryInterface(&pTextArea);
							if(SUCCEEDED(hr) && pTextArea != NULL)
							{
								BSTR bstrValue;
								hr = pTextArea->get_value(&bstrValue);
								if(SUCCEEDED(hr))
								{
									CString strValue(bstrValue);
									SysFreeString(bstrValue);
									Value = strValue;
									Result = TRUE;
								}
								//pTextArea->Release();
							}
						} // else if(strTagName == "TEXTAREA")
					}
					//pElem->Release();
				}
			} // if(SUCCEEDED(hr) && pElem != NULL)
			//pAllElem->Release();
		} // if(SUCCEEDED(hr) && pAllElem != NULL)
		//pDoc->Release();
	} // if(pDoc != NULL)

	return Result;
}

BOOL CLhpHtmlView::PutElementValue(CString ElemID,CString Value)
{
	IHTMLDocument2* pDoc = NULL;
	IHTMLElementCollection* pAllElem = NULL;
	IDispatch* pElemDisp = NULL;
	IHTMLElement* pElem = NULL;

	IHTMLInputElement* pInput=NULL;
	IHTMLSelectElement* pSelect=NULL;
	IHTMLTextAreaElement* pTextArea=NULL;
	
	
	BOOL Result = FALSE;
	HRESULT hr = S_FALSE;

	pDoc = (IHTMLDocument2*)GetHtmlDocument();
	if(pDoc != NULL)
	{
		hr = pDoc->get_all(&pAllElem);
		if(SUCCEEDED(hr) && pAllElem != NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(0);
			hr = pAllElem->item(varID, varIdx, &pElemDisp);
			if(SUCCEEDED(hr) && pElemDisp != NULL)
			{
				hr = pElemDisp->QueryInterface(IID_IHTMLElement,(void**)&pElem);
				if(SUCCEEDED(hr) && pElem != NULL)
				{
					BSTR bstrTagName;
					hr = pElem->get_tagName(&bstrTagName);
					if(SUCCEEDED(hr))
					{
						CString strTagName(bstrTagName);
						SysFreeString(bstrTagName);
						strTagName.MakeUpper();
						if(strTagName == _T("INPUT"))
						{
							hr = pElem->QueryInterface(&pInput);
							if(SUCCEEDED(hr) && pInput!=NULL)
							{
								BSTR bstrType;
								hr = pInput->get_type(&bstrType);
								if(SUCCEEDED(hr))
								{
									CString strType(bstrType);
									strType.MakeUpper();
									SysFreeString(bstrType);
									if(strType == _T("TEXT") || strType == _T("HIDDEN"))
									{
										BSTR bstrValue;
										bstrValue = Value.AllocSysString();
										hr = pInput->put_value(bstrValue);
										SysFreeString(bstrValue);
										if(SUCCEEDED(hr))
											Result = TRUE;
									} // if(strType == "TEXT" || strType == "HIDDEN")
									else if(strType == _T("CHECKBOX") || strType == _T("RADIO"))
									{
										VARIANT_BOOL Check;
										if(Value == _T("1"))
											Check = VARIANT_TRUE;
										else
											Check = VARIANT_FALSE;

										hr = pInput->put_checked(Check);
										if(SUCCEEDED(hr))
											Result = TRUE;
									} // else if(strType=="CHECKBOX" || strType == "RADIO")
								} // if(SUCCEEDED(hr))
								pInput->Release();
							} // if(SUCCEEDED(hr) && pInput!=NULL)
						} // if(strTagName == "INPUT")
						else if(strTagName == _T("SELECT"))
						{
							hr = pElem->QueryInterface(&pSelect);
							if(SUCCEEDED(hr) && pSelect!=NULL)
							{
								long index;
								index = _ttoi(Value);
								hr = pSelect->put_selectedIndex(index);
								if(SUCCEEDED(hr))
									Result = TRUE;
								pSelect->Release();
							} // if(SUCCEEDED(hr) && pSelect!=NULL)
						} // else if(strTagName == "SELECT")
						else if(strTagName == _T("TEXTAREA"))
						{
							hr = pElem->QueryInterface(&pTextArea);
							if(SUCCEEDED(hr) && pTextArea != NULL)
							{
								BSTR bstrValue;
								bstrValue = Value.AllocSysString();
								hr = pTextArea->put_value(bstrValue);
								SysFreeString(bstrValue);
								if(SUCCEEDED(hr))
									Result = TRUE;
								pTextArea->Release();
							}
						} // else if(strTagName == "TEXTAREA")
					}
					pElem->Release();
				}
			} // if(SUCCEEDED(hr) && pElem != NULL)
			pAllElem->Release();
		} // if(SUCCEEDED(hr) && pAllElem != NULL)
		pDoc->Release();
	} // if(pDoc != NULL)

	return Result;
}

// 修改网页元素的内容
BOOL CLhpHtmlView::PutElementHtml(CString ElemID,CString Html)
{
	CComQIPtr<IHTMLDocument2> pDoc;
	CComQIPtr<IHTMLElementCollection> pElemColl;
	CComDispatchDriver pElemDisp;
	CComQIPtr<IHTMLElement> pElem;
	
	BOOL Result=FALSE;

	pDoc = (IHTMLDocument2*)GetHtmlDocument();
	if(pDoc!=NULL)
	{
		pDoc->get_all(&pElemColl);
		if(pElemColl!=NULL)
		{
			_variant_t varID(ElemID);
			_variant_t varIdx(0);
			HRESULT hr = pElemColl->item(varID, varIdx, &pElemDisp);
			if(SUCCEEDED(hr))
			{
				if(pElemDisp!=NULL)
				{
					hr = pElemDisp->QueryInterface(IID_IHTMLElement,(void**)&pElem);
					if(SUCCEEDED(hr))
					{
						BSTR bstrHtml=Html.AllocSysString();
						pElem->put_innerHTML(bstrHtml);
						//pElem->Release();
						::SysFreeString(bstrHtml);
						Result=TRUE;
					}
					//pElemDisp->Release();
				}
			}
		}//if(pAllElem!=NULL)
		//pDoc->Release();
	}//if(pDoc!=NULL)
	return Result;
}
*/
BOOL CLhpHtmlView::PreCreateWindow(CREATESTRUCT& cs)
{
	// 消除窗口闪烁
	cs.style |= WS_CLIPSIBLINGS;
	if(m_IsView)
		cs.dwExStyle |= WS_EX_CLIENTEDGE;

	return CWnd::PreCreateWindow(cs);
}



BOOL CLhpHtmlView::Create(CRect rect, CWnd* pParent, UINT nID)
{
	if(!CHtmlView::Create(NULL, NULL, (WS_CHILD | WS_VISIBLE ), rect, pParent, nID, NULL))
		return FALSE;

	m_IsView = FALSE;

	return TRUE;
}

int CLhpHtmlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CHtmlView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_DropTarget.Register(this);

	return 0;
}

void CLhpHtmlView::OnDestroy()
{
	if(m_IsView)
        CHtmlView::OnDestroy();
	else
	{
		if (m_pBrowserApp)
		{
			m_pBrowserApp.Release();
			m_pBrowserApp = NULL;
		}
		CWnd::OnDestroy();
	}
}

int CLhpHtmlView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	if(m_IsView)
		return CHtmlView::OnMouseActivate(pDesktopWnd, nHitTest, message);
	else
		return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}



void CLhpHtmlView::PostNcDestroy()
{
	if(m_IsView)
		CHtmlView::PostNcDestroy();
}

void CLhpHtmlView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndBrowser.m_hWnd)) 
	{ 
		CRect rect; 
		GetClientRect(rect); 
		::AdjustWindowRectEx(rect, GetStyle(), FALSE, WS_EX_CLIENTEDGE); 
		m_wndBrowser.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW| SWP_NOACTIVATE | SWP_NOZORDER);
	} 
}

const CString CLhpHtmlView::GetSystemErrorMessage(DWORD dwError)
{
	CString strError;
	LPTSTR lpBuffer;

	if(!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,  dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			(LPTSTR) &lpBuffer, 0, NULL))

	{
		strError = "FormatMessage Netive Error" ;
	}
	else
	{
		strError = lpBuffer;
		LocalFree(lpBuffer);
	}
	return strError;
}

CString CLhpHtmlView::GetNextToken(CString& strSrc, const CString strDelim,BOOL bTrim, BOOL bFindOneOf)
{
	CString strToken;
	int idx = bFindOneOf? strSrc.FindOneOf(strDelim) : strSrc.Find(strDelim);
	if(idx != -1)
	{
		strToken  = strSrc.Left(idx);
		strSrc = strSrc.Right(strSrc.GetLength() - (idx + 1) );
	}
	else
	{
		strToken = strSrc;
		strSrc.Empty();
	}
	if(bTrim)
	{
		strToken.TrimLeft();
		strToken.TrimRight();
	}
	return strToken;
}

BOOL CLhpHtmlView::JScriptOK()
{
	return (m_spDoc != NULL); 
}

BOOL CLhpHtmlView::GetJScript(CComPtr<IDispatch>& spDisp)
{
	if( m_spDoc==NULL )
		return FALSE;

	HRESULT hr = m_spDoc->get_Script(&spDisp);
	ATLASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

BOOL CLhpHtmlView::GetJScripts(CComPtr<IHTMLElementCollection>& spColl)
{
	if( m_spDoc==NULL )
		return FALSE;

	HRESULT hr = m_spDoc->get_scripts(&spColl);
	ATLASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

BOOL CLhpHtmlView::CallJScript(const CString strFunc,_variant_t* pVarResult)
{
	CStringArray paramArray;
	return CallJScript(strFunc,paramArray,pVarResult);
}

BOOL CLhpHtmlView::CallJScript(const CString strFunc,const CString strArg1,_variant_t* pVarResult)
{
	CStringArray paramArray;
	paramArray.Add(strArg1);
	return CallJScript(strFunc,paramArray,pVarResult);
}

BOOL CLhpHtmlView::CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,_variant_t* pVarResult)
{
	CStringArray paramArray;
	paramArray.Add(strArg1);
	paramArray.Add(strArg2);
	return CallJScript(strFunc,paramArray,pVarResult);
}

BOOL CLhpHtmlView::CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,const CString strArg3,_variant_t* pVarResult)
{
	CStringArray paramArray;
	paramArray.Add(strArg1);
	paramArray.Add(strArg2);
	paramArray.Add(strArg3);
	return CallJScript(strFunc,paramArray,pVarResult);
}

BOOL CLhpHtmlView::CallJScript(const CString strFunc, const CStringArray& paramArray,_variant_t* pVarResult)
{
	CComPtr<IDispatch> spScript;
	if(!GetJScript(spScript))
	{
		MessageBox(_T("函数GetJScrip调用失败！"));
		return FALSE;
	}
	CComBSTR bstrFunc(strFunc);
	DISPID dispid = NULL;
	HRESULT hr = spScript->GetIDsOfNames(IID_NULL,&bstrFunc,1,
											LOCALE_SYSTEM_DEFAULT,&dispid);
	if(FAILED(hr))
	{
		MessageBox(GetSystemErrorMessage(hr));
		return FALSE;
	}

	INT_PTR arraySize = paramArray.GetSize();

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = (UINT)arraySize;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs];
	
	for( int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}
	dispparams.cNamedArgs = 0;

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
   	_variant_t vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg
         
	hr = spScript->Invoke(dispid,IID_NULL,0,
							DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	delete [] dispparams.rgvarg;
	if(FAILED(hr))
	{
		MessageBox(GetSystemErrorMessage(hr));
		return FALSE;
	}
	
	if(pVarResult)
	{
		*pVarResult = vaResult;
	}
	return TRUE;
}

CString CLhpHtmlView::ScanJScript(CString& strAText, CStringArray& args)
{
	args.RemoveAll();
	CString strDelim(_T(" \n\r\t")),strSrc(strAText);
	BOOL bFound = FALSE;
	while(!strSrc.IsEmpty())
	{
		CString strStart = GetNextToken(strSrc,strDelim);
		if(strStart == _T("function"))
		{
			bFound = TRUE;
			break;
		}
		if(strStart == _T("/*"))
		{
			// Skip comments
			while(!strSrc.IsEmpty())
			{
				CString strStop = GetNextToken(strSrc,strDelim);
				if(strStop == _T("*/"))
				{
					break;
				}
			}
		}
	}

	if(!bFound)
		return _T("");
	
	CString strFunc = GetNextToken(strSrc,_T("("),TRUE);
	CString strArgs = GetNextToken(strSrc,_T(")"),TRUE);

	// Parse arguments
	CString strArg;
	while(!(strArg = GetNextToken(strArgs,_T(","))).IsEmpty())
		args.Add(strArg);

	strAText= strSrc;
	return strFunc;
}

BOOL CLhpHtmlView::SetScriptDocument()
{
	LPDISPATCH pDisp = GetHtmlDocument();
	if(pDisp==NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_spDoc = NULL;

	CComPtr<IDispatch> spDisp = pDisp;

	HRESULT hr = spDisp->QueryInterface(IID_IHTMLDocument2,(void**)&m_spDoc);
	if(FAILED(hr))
	{
		MessageBox(_T("Failed to get HTML document COM object"));
		pDisp->Release();
		return FALSE;
	}
	pDisp->Release();
	return TRUE;
}

void CLhpHtmlView::OnDocumentComplete(LPCTSTR lpszURL)
{
	CHtmlView::OnDocumentComplete(lpszURL);
	SetScriptDocument();
}

//HRESULT CLhpHtmlView::GetElement(LPCTSTR szElementId, IDispatch **ppdisp, 
//								 BOOL *pbCollection /*= NULL*/)
//{
//	CComPtr<IHTMLElementCollection> sphtmlAll;
//	CComPtr<IHTMLElementCollection> sphtmlColl;
//	CComPtr<IDispatch> spdispElem;
//	CComVariant varName;
//	CComVariant varIndex;
//	HRESULT hr = S_OK;
//	CComPtr<IHTMLDocument2> sphtmlDoc;
//	USES_CONVERSION;
//
//	*ppdisp = NULL;
//
//	if (pbCollection)
//		*pbCollection = FALSE;
//
//	sphtmlDoc = (IHTMLDocument2*)GetHtmlDocument();
//	if (sphtmlDoc == NULL)
//		return hr;
//
//	varName.vt = VT_BSTR;
//	varName.bstrVal = T2BSTR(szElementId);
//	if (!varName.bstrVal)
//	{
//		hr = E_OUTOFMEMORY;
//		goto __Error;
//	}
//
//	hr = sphtmlDoc->get_all(&sphtmlAll);
//	if (sphtmlAll == NULL)
//		goto __Error;
//	hr = sphtmlAll->item(varName, varIndex, &spdispElem);
//	if (spdispElem == NULL)
//	{
//		hr = E_NOINTERFACE;
//		goto __Error;
//	}
//
//	spdispElem->QueryInterface(__uuidof(IHTMLElementCollection), (void **) &sphtmlColl);
//	if (sphtmlColl)
//	{
//		if (pbCollection)
//			*pbCollection = TRUE;
//#ifdef _DEBUG
//		else
//		{
//			TRACE(traceHtml, 0, _T("Warning: duplicate IDs or NAMEs.\n"));
//			ATLASSERT(FALSE);
//		}
//#endif
//
//	}
//__Error:
//	if (SUCCEEDED(hr))
//	{
//		*ppdisp = spdispElem;
//		if (spdispElem)
//			(*ppdisp)->AddRef();
//	}
//	return hr;
//}
//
//HRESULT CLhpHtmlView::GetElement(LPCTSTR szElementId, IHTMLElement **pphtmlElement)
//{
//	return GetElementInterface(szElementId, __uuidof(IHTMLElement), (void **) pphtmlElement);
//}
//
//HRESULT CLhpHtmlView::GetElementInterface(LPCTSTR szElementId, REFIID riid, void **ppvObj)
//{
//	HRESULT hr = E_NOINTERFACE;
//	*ppvObj = NULL;
//	CComPtr<IDispatch> spdispElem;
//
//	hr = GetElement(szElementId, &spdispElem);
//
//	if (spdispElem)
//		hr = spdispElem->QueryInterface(riid, ppvObj);
//	return hr;
//}
