#pragma once
class CScriptErrHandler
{
public:
	CScriptErrHandler(void);
	~CScriptErrHandler(void);

	void ShieldCurrPage(CComPtr<IHTMLDocument2> &doc);
	void ShieldAllChildPages(CComPtr<IHTMLDocument2> &parentDoc);


private:
	BSTR m_bstrScript;

	void WalkAllChildPages(CComPtr<IHTMLDocument2> &parentDoc);
};

