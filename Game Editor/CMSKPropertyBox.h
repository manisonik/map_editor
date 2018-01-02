#pragma once
#include "CMSKWindow.h"

class CMSKPropertyBox :
	public CMSKWindow
{
protected:
	HWND m_hWndButton;
	HWND m_hWndEdit;
	HBITMAP m_hBitmap;
	HWND m_hWndListBox;
	HFONT m_hFont;

public:
	CMSKPropertyBox(CMSKWindow* parent);
	~CMSKPropertyBox();

	void OnDestroy();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void SetText(LPCWSTR text);
	VOID SetType(UINT type);
	bool OnCommand(WPARAM wParam, LPARAM lParam);
	void OnSize(UINT nType, int cx, int cy);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};

