#pragma once


// CMyEditView 视图

class CMyEditView : public CEditView
{
	DECLARE_DYNCREATE(CMyEditView)

protected:
	CMyEditView();           // 动态创建所使用的受保护的构造函数
	virtual ~CMyEditView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	virtual void OnInitialUpdate();
};


