#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__

class CCircleProgressUI : public CProgressUI
{
	DECLARE_DUICONTROL(CCircleProgressUI)
public:
	CCircleProgressUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CircleProgress" );
	}

	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwBackColor);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);
		g.ReleaseHDC(hDC);
	}

	void PaintForeColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwForeColor);
		int nStartDegree = 90;
		int nSweepDegree = (int)(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree, nSweepDegree);

		Gdiplus::SolidBrush brushbk(0xFFFFFFFF);
		g.FillPie(&brushbk, rcPos.left + 10, rcPos.top + 10, rcPos.right - rcPos.left - 20, rcPos.bottom - rcPos.top - 20, 0, 360);

		g.ReleaseHDC(hDC);
	}

	void PaintForeImage(HDC hDC)
	{
		
	}
};

#define CHARTVIEW_PIE		0x0
#define CHARTVIEW_HISTOGRAM 0x01

typedef struct tagCHARTITEM
{
	CDuiString name;
	double value;
} CHARTITEM, *PCHARTITEM;

class CChartViewUI : 
	public CControlUI
{
public:
	DECLARE_DUICONTROL(CChartViewUI)
public:
	CChartViewUI(void);
	~CChartViewUI(void);

	bool Add(LPCTSTR name, double value);
	bool AddAt(LPCTSTR name, double value, int iIndex);

public:
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void DoPaint(HDC hDC, const RECT& rcPaint);
	virtual void DoPaintPie(HDC hDC, const RECT& rcPaint);
	virtual void DoPaintHistogram(HDC hDC, const RECT& rcPaint);
private:
	vector<CHARTITEM> m_items;
	int m_ViewStyle;
	CDuiString m_sShadowImage;
	int m_ShadowImageHeight;
	CDuiString m_sPillarImage;
	int m_PillarImageWidth;
	DWORD m_dwTextColor;
	DWORD m_dwDisabledTextColor;
	bool m_bShowHtml;
	bool m_bShowText;
	int m_iFont;
};

class CCircleProgressExUI : public CProgressUI
{
	DECLARE_DUICONTROL(CCircleProgressExUI)
public:
	CCircleProgressExUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CircleProgressEx" );
	}

	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwBackColor);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);
		g.ReleaseHDC(hDC);
	}

	void PaintForeColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwForeColor);
		int nStartDegree = 90;
		int nSweepDegree = (int)(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree, nSweepDegree);

		Gdiplus::SolidBrush brushbk(0xFFFFFFFF);
		g.FillPie(&brushbk, rcPos.left + 10, rcPos.top + 10, rcPos.right - rcPos.left - 20, rcPos.bottom - rcPos.top - 20, 0, 360);

		g.ReleaseHDC(hDC);
	}

	void PaintForeImage(HDC hDC)
	{
		
	}
};

#endif __CONTROLEX_H__
