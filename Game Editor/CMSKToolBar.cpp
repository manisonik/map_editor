#include "stdafx.h"
#include "CMSKToolBar.h"

#define IDM_NEW 0
#define IDM_OPEN 1
#define IDM_SAVE 2

CMSKToolBar::CMSKToolBar(LPCWSTR text, CMSKWindow* parent)
{
	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = text;
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = TOOLBARCLASSNAME;
	cs.style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NORESIZE | WS_CHILD | WS_VISIBLE 
		| TBSTYLE_WRAPABLE | TBSTYLE_FLAT | CCS_NODIVIDER;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)7;
	PreCreateWindow(cs);

	// Set window text
	SetText(text);

	// Store instance
	hInst = parent->GetInstanceHandle();

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	// Set the class pointer
	SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG)this);

	// Set the parent
	SetParent(parent);

	// Subclass the window
	SetWindowSubclass(m_hWnd, SubClassRouter, 0, (DWORD)this);

	m_hImageList = ImageList_Create(16, 16,   // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
		3, 0);

	SendMessage(m_hWnd, TB_SETIMAGELIST,
		(WPARAM)0,
		(LPARAM)m_hImageList);

	// Load the button images.
	SendMessage(m_hWnd, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	TBBUTTON tbButtons[3] =
	{
		{ MAKELONG(STD_FILENEW,  0), IDM_NEW,  TBSTATE_ENABLED, BTNS_AUTOSIZE,{ 0 }, 0, (INT_PTR)L"" },
		{ MAKELONG(STD_FILEOPEN, 0), IDM_OPEN, TBSTATE_ENABLED, BTNS_AUTOSIZE,{ 0 }, 0, (INT_PTR)L"" },
		{ MAKELONG(STD_FILESAVE, 0), IDM_SAVE, 0,               BTNS_AUTOSIZE,{ 0 }, 0, (INT_PTR)L"" }
	};

	SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(m_hWnd, TB_ADDBUTTONS, (WPARAM)3, (LPARAM)&tbButtons);
	SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0);
	SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)&m_nMaxSize);

	// Set the desired height
	m_nHeight = 24;

	// Dock at the top
	Top();

	// Update the parent window
	parent->Add(this);
}


CMSKToolBar::~CMSKToolBar()
{
}
