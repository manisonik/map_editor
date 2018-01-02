#pragma once
#include "CMSKWindow.h"

class CMSKTreeView :
	public CMSKWindow
{
private:
	COLORREF m_colBackColor;
	COLORREF m_colText;
	RECT m_rcItem;
	BOOL m_bFocus;
	HBRUSH m_hBrush;
	HBRUSH m_hFocusBrush;
	HPEN m_hPen;
	HFONT m_hFont;
	HFONT m_hFontNormal;
	HBITMAP m_hBitmap;

public:
	CMSKTreeView(LPCWSTR text, CMSKWindow* parent);
	~CMSKTreeView();

	//LRESULT OnNotify(LPNMTREEVIEW tv);

	HTREEITEM InsertItem(LPCWSTR text, HTREEITEM parent, HTREEITEM insertAfter,
		int imageIdx, int selImageIdx);
	void ExpandItem(HTREEITEM hItem);
	void EnsureVisibility(HTREEITEM item);  
	void SetImageList(HIMAGELIST imageList);
	void DeleteItem(HTREEITEM item);
	void OnContextMenu(HWND hWnd, int x, int y);
	LRESULT OnNotify(LPNMHDR nmHDR);
	LRESULT OnCustomDraw(LPNMTVCUSTOMDRAW lpNMCustomDraw);
};

