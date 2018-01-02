#include "stdafx.h"
#include "CMSKStatusBar.h"

CMSKStatusBar::CMSKStatusBar(CMSKWindow* parent)
{
	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = L"";
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = STATUSCLASSNAME;
	cs.style = CCS_NORESIZE | CCS_TOP | WS_CHILD | WS_VISIBLE | CCS_NODIVIDER;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)1;
	PreCreateWindow(cs);

	// Store instance
	hInst = parent->GetInstanceHandle();

	//Use(WM_SIZE, (MSGFN)&CMSKStatusBar::OnSize);

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	// Set the class pointer
	SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG)this);

	m_nHeight = 25;

	// Subclass
	SetWindowSubclass(m_hWnd, SubClassRouter, 0, (DWORD)this);

	// Dock at the bottom
	Bottom();

	// Set the parent
	SetParent(parent);

	// Add to parent window
	parent->Add(this);
}

void CMSKStatusBar::ShowBorder(BOOL bShow) {
	SendMessage(m_hWnd, WM_SIZE, 0, 0);
	if (bShow) {
		SendMessage(m_hWnd, SB_SETTEXT, 0, 0);
	} else {
		SendMessage(m_hWnd, SB_SETTEXT, SBT_NOBORDERS, 0);
	}
}

BOOL CMSKStatusBar::SetParts(int* nParts) {
	return SendMessage(m_hWnd, SB_SETPARTS, sizeof(nParts) / sizeof(int), (LPARAM)nParts);
}

BOOL CMSKStatusBar::SetText(
	LPCTSTR lpszText,
	UINT nPane,
	UINT nType,
	COLORREF bgColor
)
{
	if (bgColor != NULL) {
		SendMessage(m_hWnd, SB_SETBKCOLOR, nPane, bgColor);
	}
	return SendMessage(m_hWnd, SB_SETTEXT, LOBYTE(nPane) | nType, (LPARAM)lpszText);
}

void CMSKStatusBar::SetBkColor(COLORREF color) {
	//m_crBgColor = color;
	SendMessage(m_hWnd, SB_SETBKCOLOR, 0, color);
}

CMSKStatusBar::~CMSKStatusBar()
{
}
