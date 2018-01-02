#pragma once
#include "CMSKWindow.h"

class CMSKSplitterWnd : public CMSKWindow
{
protected:
	POINT m_ptBarPos;

private:
	POINT m_ptCurrentPos;
	POINT m_ptOldPos;
	INT m_idFirst;
	INT m_idSecond;
	BOOL m_fDrag;
	HDC m_hDC;
	int m_nSize;
	HWND hWndFirst;
	HWND hWndSecond;
	HWND m_hWndBar;

	void DrawXorBar(HDC hdc, int x1, int y1, int width, int height);

public:
	CMSKSplitterWnd();
	CMSKSplitterWnd(CMSKWindow* parent, CMSKWindow* first, CMSKWindow* second, BOOL bVertical);
	~CMSKSplitterWnd();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSizeH(UINT nType, int cx, int cy);
	void OnSizeV(UINT nType, int cx, int cy);
	void OnLButtonDownH(UINT nFlags, POINT point);
	void OnLButtonUpH(UINT nFlags, POINT point);
	void OnMouseMoveH(UINT nFlags, POINT point);
	void OnLButtonDownV(UINT nFlags, POINT point);
	void OnLButtonUpV(UINT nFlags, POINT point);
	void OnMouseMoveV(UINT nFlags, POINT point);
};

