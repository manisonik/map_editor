#pragma once
#include "CMSKWindow.h"

class CMSKTabCtrl :
	public CMSKWindow
{
private:
	struct TABITEM {
		LPCWSTR pstrText;
		UINT idChild;
		UINT state;
		RECT rc;
		CMSKWindow* window;
	};

	POINT m_ptLine[2];
	int m_nTabHeight;

	std::vector<TABITEM*> m_vTabItem;
	TABITEM* m_selItem;
	TABITEM* m_curItem;
	BOOL m_bMouseTracking;
	HBRUSH m_hHoverBrush;
	HBRUSH m_hSelectedBrush;
	HFONT m_hFont;
	HPEN m_hPen;
	HPEN m_hPenNull;
public:
	CMSKTabCtrl(CMSKWindow* parent);
	~CMSKTabCtrl();

	void Add(CMSKWindow * window);

	void AddTab(LPCWSTR text, CMSKWindow* window);
	void OnPaint();
	void OnDestroy();
	void OnMouseMove(UINT nFlags, POINT point);
	void OnMouseLeave();
	void OnLButtonDown(UINT nFlags, POINT point);
	void OnLButtonUp(UINT nFlags, POINT point);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
};

