// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� TROYCONTROLS_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// TROYCONTROLS_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef TROYCONTROLS_EXPORTS
#define TROYCONTROLS_API __declspec(dllexport)
#else
#define TROYCONTROLS_API __declspec(dllimport)
#endif

#define DUI_MSGTYPE_TABINDEXCHANGED			(_T("tabindexchanged"))
#define DUI_MSGTYPE_TABCLOSED				(_T("tabclosed"))

extern "C" TROYCONTROLS_API CControlUI* CreateControl(LPCTSTR pstrType);

class TROYCONTROLS_API CBrowserTab : public COptionUI
{
public:
	CBrowserTab();
	~CBrowserTab();

public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void DoEvent(TEventUI& event);

	void DoInit();
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void DoPostPaint(HDC hDC, const RECT& rcPaint);
	void PaintStatusImage(HDC hDC);
	void PaintIcon(HDC hDC);
	void PaintClose(HDC hDC);

	void SetIconSize(SIZE szIcon);
	void SetIconPadding(RECT rcPadding);
	void SetIconImage(LPCTSTR pStrCursor);

	void SetCloseSize(SIZE szIcon);
	void SetClosePadding(RECT rcPadding);
	void SetCloseImage(LPCTSTR pStrCursor);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	
protected:
	bool TestClose(POINT ptMouse);

protected:
	bool m_bHitClose;
	RECT m_rcOldTextPadding;
	SIZE m_szIcon;
	RECT m_rcIconPadding;
	CDuiString m_sIconImage;
	SIZE m_szClose;
	RECT m_rcClosePadding;
	CDuiString m_sCloseImage;
	POINT m_ptMouse;
};

class TROYCONTROLS_API CBrowserTabBar : public CContainerUI
{
public:
	CBrowserTabBar();
	~CBrowserTabBar();

public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetPos(RECT rc, bool bNeedInvalidate = true);

public:
	bool Add(CControlUI* pControl);
	bool AddAt(CControlUI* pControl, int iIndex);
	void Invalidate();

	void SetStartTab(int nStart);
	void SelectTab(CBrowserTab *pTab);
	void SelectTab(int nIndex);
	void CloseTab(CBrowserTab *pTab, BOOL bPrevSelected = TRUE);
	int GetTabCount();
	int GetVisibleTabCount();
	CBrowserTab* GetTab(int nIndex);
	int GetTabIndex(CBrowserTab *pTab);
	CBrowserTab* GetPrevTab(CBrowserTab *pTab);
	CBrowserTab* GetNextTab(CBrowserTab *pTab);

private:
	void UpdatePostPaint(CBrowserTab *pTab);

private:
	int m_nStartTab;
	int m_nSelectedTab;
};