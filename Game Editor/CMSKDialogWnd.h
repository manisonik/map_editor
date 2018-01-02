#pragma once
#include "CMSKWindow.h"

class CMSKDialogWnd :
	public CMSKWindow
{
public:
	CMSKDialogWnd();
	CMSKDialogWnd(LPCWSTR title, CMSKWindow* parent);
	~CMSKDialogWnd();

	void OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);
	void OnNcPaint(HRGN hRgn);


};

