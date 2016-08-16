
// ChildFrm.h : CChildFrame 类的接口
//

#pragma once
#include "MySplitter.h"

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// 特性
public:
	CWnd *GetSourceWnd();         //返回源码窗口
	CWnd *GetPageWnd();           //返回网页窗口

	static bool m_sbViewSrcFlag;     //是否显示源码窗口

private:
	CMySplitter m_splitWnd;

// 操作
public:

// 重写
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

// 实现
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg LRESULT OnViewSourceWnd(WPARAM wParam, LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg void OnClose();
};
