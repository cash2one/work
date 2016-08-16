#include "StdAfx.h"
#include "ScriptErrHandler.h"


CScriptErrHandler::CScriptErrHandler(void)
{
// 	CString strJavaScriptCode = _T("function fnOnError(msg,url,lineno){alert('script error:\\n\\nURL:'+url"
// 		"+'\\n\\nMSG:'+msg +'\\n\\nLine:'+lineno+'\\n\\nframes:' + window.frames.length);return true;}window.onerror=fnOnError;");

// 	CString strJavaScriptCode = _T("function fnOnError(msg,url,lineno){return true;}window.onerror=fnOnError;");
	CString strJavaScriptCode = _T("function fnOnError(){return true;}window.onerror=fnOnError;");

	//屏蔽的脚本,可以改进为从文本里读取
	m_bstrScript = strJavaScriptCode.AllocSysString();
}

CScriptErrHandler::~CScriptErrHandler(void)
{
	SysFreeString(m_bstrScript);
}

void CScriptErrHandler::ShieldCurrPage(CComPtr<IHTMLDocument2> &doc)
{
	CComPtr<IHTMLWindow2>  spIhtmlwindow2;
	doc->get_parentWindow(reinterpret_cast<IHTMLWindow2**>(&spIhtmlwindow2));
	if(spIhtmlwindow2 != NULL)
	{
		CString strLanguage("JavaScript");
		BSTR bstrLanguage = strLanguage.AllocSysString();
		long lTime = 1 * 1000;
		long lTimeID = 0;
		VARIANT varLanguage;
		varLanguage.vt = VT_BSTR;
		varLanguage.bstrVal = bstrLanguage;
		VARIANT pRet;
		//把window.onerror函数插入入当前页面中去
		spIhtmlwindow2->execScript(m_bstrScript, bstrLanguage, &pRet);
		::SysFreeString(bstrLanguage);
	}
}

void CScriptErrHandler::ShieldAllChildPages(CComPtr<IHTMLDocument2> &parentDoc)
{
	WalkAllChildPages(parentDoc);
}

void CScriptErrHandler::WalkAllChildPages(CComPtr<IHTMLDocument2> &parentDoc)
{
	CComPtr<IHTMLFramesCollection2> spFramesCol;
	HRESULT hr = parentDoc->get_frames(&spFramesCol);
	if(SUCCEEDED(hr) && spFramesCol != NULL)
	{
		long lSize = 0;
		hr = spFramesCol->get_length(&lSize);
		if (SUCCEEDED(hr))
		{
			for(int i=0; i<lSize; i++)
			{
				VARIANT frameRequested;
				VARIANT frameOut;
				frameRequested.vt = VT_UI4;
				frameRequested.lVal = i;
				hr = spFramesCol->item(&frameRequested, &frameOut);
				if(SUCCEEDED(hr) && frameOut.pdispVal != NULL)
				{
					CComPtr<IHTMLWindow2> spChildWindow;
					hr = frameOut.pdispVal->QueryInterface(IID_IHTMLWindow2,reinterpret_cast<void**>(&spChildWindow));
					if(SUCCEEDED(hr) && spChildWindow != NULL)
					{
						CComPtr<IHTMLDocument2> spChildDocument;
						hr = spChildWindow->get_document(reinterpret_cast<IHTMLDocument2**>(&spChildDocument));
						if(SUCCEEDED(hr) && spChildDocument != NULL)
						{
							ShieldCurrPage(spChildDocument);
							WalkAllChildPages(spChildDocument);
						}
					}
					frameOut.pdispVal->Release();
				}
			}
		}
	}
}