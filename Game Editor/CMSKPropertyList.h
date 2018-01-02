#pragma once
#include "CMSKWindow.h"

class CMSKPropertyList :
	public CMSKWindow
{
private:
	HWND		m_hWndTreeView;
	COLORREF	m_crBgColor;
	HBRUSH		m_hBrush;
	HBRUSH		m_hFocusBrush;
	HBITMAP		m_hBitmap;
	HPEN		m_hPen;
	HFONT		m_hFont;
	HFONT		m_hFontNormal;
	//BOOL		m_bFocus;
	RECT		m_rcItem;
	RECT		m_rcClient;

public:
	CMSKPropertyList(LPCWSTR text, CMSKWindow* parent);
	~CMSKPropertyList();

	HTREEITEM InsertItem(LPCWSTR text, HTREEITEM parent, HTREEITEM insertAfter,
		int imageIdx, int selImageIdx);
	void ExpandItem(HTREEITEM hItem);
	void EnsureVisibility(HTREEITEM item);
	void SetImageList(HIMAGELIST imageList);
	void DeleteItem(HTREEITEM item);
	LRESULT OnNotify(LPNMHDR nmHDR);
	LRESULT OnCustomDraw(LPNMTVCUSTOMDRAW lpNMCustomDraw);
	void OnLButtonDown(UINT nFlags, POINT point);
	void OnDestroy();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
};

