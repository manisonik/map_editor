#include "stdafx.h"
#include "CMSKRebar.h"

CMSKRebar::CMSKRebar(CMSKWindow* parent)
{
	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = (PCTSTR)NULL;
	cs.dwExStyle = WS_EX_TOOLWINDOW;
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = REBARCLASSNAME;
	cs.style = WS_CHILD | WS_VISIBLE | RBS_VARHEIGHT | WS_CLIPSIBLINGS | CCS_NODIVIDER;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)3;
	PreCreateWindow(cs);

	// Store instance
	hInst = parent->GetInstanceHandle();

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	// Set the class pointer
	SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG)this);

	// Subclass the window
	SetWindowSubclass(m_hWnd, SubClassRouter, 0, (DWORD)this);

	// Set the parent
	SetParent(parent);

	m_rbBand = { sizeof(REBARBANDINFO) };
	m_rbBand.fMask =
		RBBIM_STYLE       // fStyle is valid.
		| RBBIM_TEXT        // lpText is valid.
		| RBBIM_CHILD       // hwndChild is valid.
		| RBBIM_CHILDSIZE   // child size members are valid.
		| RBBIM_SIZE;       // cx is valid
	m_rbBand.fStyle = RBBS_GRIPPERALWAYS;

	// Set the desired height
	m_nHeight = 24;

	// Dock at the top
	Top();

	// Add window
	parent->Add(this);
}

void CMSKRebar::Add(CMSKWindow* bar) {
	// Get the height of the toolbar.
	DWORD dwBtnSize = (DWORD)SendMessage(bar->GetWindowHandle(), TB_GETBUTTONSIZE, 0, 0);

	// Set values unique to the band with the toolbar.
	m_rbBand.cbSize = REBARBANDINFO_V3_SIZE;
	m_rbBand.lpText = TEXT("");
	m_rbBand.hwndChild = bar->GetWindowHandle();
	m_rbBand.cyChild = LOWORD(dwBtnSize);
	m_rbBand.cxMinChild = 3 * HIWORD(dwBtnSize);
	m_rbBand.cyMinChild = LOWORD(dwBtnSize);

	// The default width is the width of the buttons.
	m_rbBand.cx = 0;

	// Add the band that has the toolbar.
	SendMessage(m_hWnd, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&m_rbBand);

	CMSKWindow::Add(bar);
}

CMSKRebar::~CMSKRebar()
{
}