#pragma once
#include "CMSKWindow.h"

class CMSKStatusBar :
	public CMSKWindow
{
private:
	COLORREF m_crBgColor;

public:
	CMSKStatusBar(CMSKWindow* parent);
	~CMSKStatusBar();

	BOOL SetParts(int* nParts);
	BOOL SetText(
		LPCTSTR lpszText,
		UINT nPane,
		UINT nType,
		COLORREF bgColor
	);
	void ShowBorder(BOOL bShow);
	void SetBkColor(COLORREF color);
};

