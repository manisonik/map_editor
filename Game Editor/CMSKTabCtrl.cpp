#include "stdafx.h"
#include "CMSKTabCtrl.h"


CMSKTabCtrl::CMSKTabCtrl(CMSKWindow* parent)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC) RouterProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = parent->GetInstanceHandle();
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MSKTabControl";

	// Attempt To Register The Window Class
	RegisterClass(&wc);

	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = L"";
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = L"MSKTabControl";
	cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)6;
	PreCreateWindow(cs);

	// Store instance
	hInst = parent->GetInstanceHandle();

	Bind(WM_SIZE, (MSGFN)&CMSKTabCtrl::OnSize);
	Bind(WM_DESTROY, (MSGFN)&CMSKTabCtrl::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKTabCtrl::OnCreate);
	Bind(WM_PAINT, (MSGFN)&CMSKTabCtrl::OnPaint);
	Bind(WM_MOUSEMOVE, (MSGFN)&CMSKTabCtrl::OnMouseMove);
	Bind(WM_LBUTTONDOWN, (MSGFN)&CMSKTabCtrl::OnLButtonDown);
	Bind(WM_LBUTTONUP, (MSGFN)&CMSKTabCtrl::OnLButtonUp);
	Bind(WM_MOUSELEAVE, (MSGFN)&CMSKTabCtrl::OnMouseLeave);
	Bind(WM_ERASEBKGND, (MSGFN)&CMSKTabCtrl::OnEraseBkgnd);

	// Set the tab height
	m_nTabHeight = 20;

	// Set mouse tracking to FALSE
	m_bMouseTracking = FALSE;

	// Set selected item to NULL
	m_selItem = NULL;

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	// Set the parent
	SetParent(parent);

	// Dock at the bottom
	Fill();

	// Update the parent window
	parent->Add(this);
}

CMSKTabCtrl::~CMSKTabCtrl()
{
}

void CMSKTabCtrl::Add(CMSKWindow* window) {
	HWND hWnd = window->GetWindowHandle();

	// Remove top-level window styles
	window->SetParent(window);
	::SetParent(hWnd, m_hWnd);
	DWORD style = GetWindowLong(hWnd, GWL_STYLE);
	style &= ~(WS_POPUP | WS_CAPTION);
	style |= WS_CHILD;
	SetWindowLong(hWnd, GWL_STYLE, style);

	// Create the tab item
	TABITEM* tabItem = new TABITEM();
	tabItem->pstrText = window->GetText();
	tabItem->idChild = 0;
	tabItem->state = 0;
	tabItem->window = window;

	// Add tab item
	m_vTabItem.push_back(tabItem);

	// Select the first tab
	if (m_vTabItem.size() == 1) {
		m_vTabItem.at(0)->state = 2;
		m_curItem = m_vTabItem.at(0);
		m_selItem = m_vTabItem.at(0);
	}
}


void CMSKTabCtrl::OnSize(UINT nType, int cx, int cy) {
	// Get the client rect
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	rc.bottom -= 5;

	// Calculate the line
	m_ptLine[0] = { rc.left,  rc.bottom - m_nTabHeight - 5 };
	m_ptLine[1] = { rc.right, rc.bottom - m_nTabHeight - 5 };

	if (m_curItem) {
		HWND hWnd = m_curItem->window->GetWindowHandle();
		MoveWindow(hWnd, rc.left, rc.top, rc.right, rc.bottom - m_nTabHeight - 8, TRUE);
	}

	// Calculate the tabs
	HDC hdc = GetDC(m_hWnd);
	for (std::vector<TABITEM*>::iterator it = m_vTabItem.begin(); it != m_vTabItem.end(); ++it) {
		// Get the text dimensions
		RECT rcText = { 0, 0, 0, 0 };
		DrawText(hdc, (*it)->pstrText, -1, &rcText, DT_CALCRECT | DT_SINGLELINE | DT_CENTER);

		// Create the tab dimensions
		RECT rcTab;
		rcTab.left = rc.left;
		rcTab.top = rc.bottom - m_nTabHeight - 5;
		rcTab.right = rc.left + rcText.right + 15;
		rcTab.bottom = rc.bottom;

		// Store the rect for hit testing and drawing
		(*it)->rc = rcTab;

		// Adjust dimensions for next tab
		rc.left = rcTab.right - 1;
	}
	ReleaseDC(m_hWnd, hdc);
}

void CMSKTabCtrl::AddTab(LPCWSTR text, CMSKWindow* window) {
	TABITEM* tabItem = new TABITEM();
	tabItem->pstrText = text;
	tabItem->idChild = 0;
	tabItem->state = 0;

	m_vTabItem.push_back(tabItem);
	m_pChildWnds.push_back(window);

	// Select the first tab
	if (m_vTabItem.size() == 1) {
		m_vTabItem.at(0)->state = 2;
		m_curItem = m_vTabItem.at(0);
	}
}

void CMSKTabCtrl::OnPaint() {
	HBRUSH hbrOldBrush;
	HPEN hOldPen;
	HFONT fontOld;
	PAINTSTRUCT ps;

	// Get the client rect
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	rc.bottom -= 5;

	// Begin painting
	HDC hdc = BeginPaint(m_hWnd, &ps);

	// Draw line over tabs
	hOldPen = (HPEN)SelectObject(hdc, m_hPen);
	MoveToEx(hdc, m_ptLine[0].x, m_ptLine[0].y - 2, NULL);
	LineTo(hdc, m_ptLine[1].x, m_ptLine[1].y - 2);
	hOldPen = (HPEN)SelectObject(hdc, m_hPen);

	// Select the tab control font
	fontOld = (HFONT)SelectObject(hdc, m_hFont);

	for (std::vector<TABITEM*>::iterator it = m_vTabItem.begin(); it != m_vTabItem.end(); ++it) {
		// Copy tab
		RECT rcTab = (*it)->rc;

		if ((*it)->state == 0) { // Item default state
			hOldPen = (HPEN)SelectObject(hdc, m_hPenNull);
			hbrOldBrush = (HBRUSH)SelectObject(hdc, m_hbrBackground);
			Rectangle(hdc, rcTab.left, rcTab.top, rcTab.right, rcTab.bottom);
			SelectObject(hdc, hbrOldBrush);
			SelectObject(hdc, hOldPen);

			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, (*it)->pstrText, -1, &rcTab, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		} else if ((*it)->state == 2) { // Item selected
			hOldPen = (HPEN)SelectObject(hdc, m_hPenNull);
			hbrOldBrush = (HBRUSH)SelectObject(hdc, m_hSelectedBrush);
			Rectangle(hdc, rcTab.left, rcTab.top, rcTab.right, rcTab.bottom);
			SelectObject(hdc, hbrOldBrush);
			SelectObject(hdc, hOldPen);

			SetTextColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, (*it)->pstrText, -1, &rcTab, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		else if ((*it)->state == 3) { // Item hover
			hOldPen = (HPEN)SelectObject(hdc, m_hPenNull);
			hbrOldBrush = (HBRUSH)SelectObject(hdc, m_hHoverBrush);
			Rectangle(hdc, rcTab.left, rcTab.top, rcTab.right, rcTab.bottom);
			SelectObject(hdc, hbrOldBrush);
			SelectObject(hdc, hOldPen);

			SetTextColor(hdc, RGB(255, 255, 255));
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, (*it)->pstrText, -1, &rcTab, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
	}

	// Select the old font
	SelectObject(hdc, fontOld);

	// End painting
	EndPaint(m_hWnd, &ps);
}

void CMSKTabCtrl::OnMouseMove(UINT nFlags, POINT point) {
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);

	if (!m_bMouseTracking) {
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;

		if (TrackMouseEvent(&tme)) {
			m_bMouseTracking = TRUE;
		}
	}

	for (std::vector<TABITEM*>::iterator it = m_vTabItem.begin(); it != m_vTabItem.end(); ++it) {
		RECT rc = (*it)->rc;

		if (PtInRect(&rc, pt)) { // Mouse in region
			if ((*it)->state != 2) {
				(*it)->state = 3;
				m_selItem = (*it);
				InvalidateRect(m_hWnd, NULL, NULL);
				UpdateWindow(m_hWnd);
			}
		} else {
			if ((*it)->state == 3) { // Mouse exit region
				(*it)->state = 0;
				m_selItem = NULL;
				InvalidateRect(m_hWnd, NULL, NULL);
				UpdateWindow(m_hWnd);
			}
		}
	}
}

void CMSKTabCtrl::OnMouseLeave()
{
	m_bMouseTracking = FALSE;

	if (m_selItem) {
		if (m_selItem->state != 2) {
			m_selItem->state = 0;
			m_selItem = NULL;
			InvalidateRect(m_hWnd, NULL, NULL);
			UpdateWindow(m_hWnd);
		}
	}
}

void CMSKTabCtrl::OnLButtonDown(UINT nFlags, POINT point)
{
}

void CMSKTabCtrl::OnLButtonUp(UINT nFlags, POINT point)
{
	if (m_selItem) {
		m_curItem->state = 0;
		ShowWindow(m_curItem->window->GetWindowHandle(), SW_HIDE);

		m_selItem->state = 2;
		m_curItem = m_selItem;
		ShowWindow(m_curItem->window->GetWindowHandle(), SW_SHOW);
	}

	SendMessage(m_hWnd, WM_SIZE, 0, 0);
	InvalidateRect(m_hWnd, NULL, NULL);
	UpdateWindow(m_hWnd);
}

void CMSKTabCtrl::OnDestroy() {
	DeleteFont(m_hFont);
	DeletePen(m_hPen);
	DeletePen(m_hPenNull);
	DeleteBrush(m_hSelectedBrush);
	PostQuitMessage(0);
}

int CMSKTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	CMSKWindow::OnCreate(lpCreateStruct);

	m_hFont = CreateFont(16, 0, 0, 0, 300,
		false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");

	m_hPen = CreatePen(PS_SOLID, 3, RGB(210, 210, 225));
	m_hPenNull = CreatePen(PS_NULL, 0, 0);
	m_hSelectedBrush = CreateSolidBrush(RGB(215, 215, 225));
	m_hHoverBrush = CreateSolidBrush(RGB(0, 122, 204));

	return 0;
}
