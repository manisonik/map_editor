#include "stdafx.h"
#include "CMSKDialogWnd.h"

CMSKDialogWnd::CMSKDialogWnd() {

}

CMSKDialogWnd::CMSKDialogWnd(LPCWSTR title, CMSKWindow* parent)
{
	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = title;
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = L"MSKWindow";
	cs.style = WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)5;
	PreCreateWindow(cs);

	hInst = parent->GetInstanceHandle();

	SetParent(parent);

	Bind(WM_SIZE, (MSGFN)&CMSKDialogWnd::OnSize);
	Bind(WM_DESTROY, (MSGFN)&CMSKDialogWnd::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKDialogWnd::OnCreate);
	Bind(WM_PAINT, (MSGFN)&CMSKDialogWnd::OnPaint);
	Bind(WM_ERASEBKGND, (MSGFN)&CMSKDialogWnd::OnEraseBkgnd);
	Bind(WM_NCCALCSIZE, (MSGFN)&CMSKDialogWnd::OnNcCalcSize);
	Bind(WM_NCPAINT, (MSGFN)&CMSKDialogWnd::OnNcPaint);

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	Fill();

	parent->Add(this);
}

void CMSKDialogWnd::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam) {
	LPNCCALCSIZE_PARAMS pncc = (LPNCCALCSIZE_PARAMS)lParam;
	pncc->rgrc[0].top += 22;
	pncc->rgrc[0].left += 1;
	pncc->rgrc[0].right -= 1;
	pncc->rgrc[0].bottom -= 1;
}

void CMSKDialogWnd::OnNcPaint(HRGN hRgn) {
	HDC hdc;
	RECT rc;
	RECT rcText;
	HBRUSH hbrBrush;
	HBRUSH hbrBrushOld;
	HFONT font;
	HFONT fontOld;
	TCHAR title[256];
	int length;

	GetWindowRect(m_hWnd, &rc);
	rc.top = 0; rc.left = 0; //rc.bottom = 22;
	hdc = GetWindowDC(m_hWnd);
	hbrBrush = CreateSolidBrush(RGB(70, 130, 180));
	hbrBrushOld = (HBRUSH)SelectObject(hdc, hbrBrush);
	Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
	SelectObject(hdc, hbrBrushOld);

	// Create font and select it
	font = CreateFont(16, 0, 0, 0, 300,
		false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
	fontOld = (HFONT)SelectObject(hdc, font);

	// Get window text
	length = GetWindowTextLength(m_hWnd);
	GetWindowText(m_hWnd, title, 256);
	rcText = { 2, 0, rc.right, 22 };

	// Draw text with transparent background
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));
	DrawText(hdc, title, length, &rcText, DT_SINGLELINE | DT_INTERNAL | DT_END_ELLIPSIS | DT_VCENTER);
	SelectObject(hdc, fontOld);
	ReleaseDC(m_hWnd, hdc);
}

CMSKDialogWnd::~CMSKDialogWnd()
{
}