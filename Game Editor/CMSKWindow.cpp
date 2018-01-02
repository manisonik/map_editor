#include "stdafx.h"
#include "CMSKWindow.h"

CMSKWindow::CMSKWindow() {}

CMSKWindow::CMSKWindow(CMSKWindow* parent) {

	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = (PCTSTR)NULL;
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = L"MSKWindow";
	cs.style = WS_CHILD | WS_VISIBLE;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)4;
	PreCreateWindow(cs);

	hInst = parent->GetInstanceHandle();

	SetParent(parent);

	Bind(WM_SIZE, (MSGFN)&CMSKWindow::OnSize);
	Bind(WM_DESTROY, (MSGFN)&CMSKWindow::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKWindow::OnCreate);
	Bind(WM_PAINT, (MSGFN)&CMSKWindow::OnPaint);
	Bind(WM_ERASEBKGND, (MSGFN)&CMSKWindow::OnEraseBkgnd);
	Bind(WM_NCCALCSIZE, (MSGFN)&CMSKWindow::OnNcCalcSize);
	Bind(WM_NCPAINT, (MSGFN)&CMSKWindow::OnNcPaint);

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	Fill();

	parent->Add(this);
}

HINSTANCE CMSKWindow::GetInstanceHandle() {
	return hInst;
}

void CMSKWindow::SetText(LPCWSTR text) {
	m_strText = text;
}

LPCWSTR CMSKWindow::GetText() {
	return m_strText;
}

CMSKWindow::CMSKWindow(LPCWSTR title, int x, int y, int width, int height) {
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&iccx);

	hInst = GetModuleHandle(NULL);
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC) RouterProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPDEFAULT));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = MAKEINTRESOURCE(IDC_GAMEEDITOR);
	wc.lpszClassName = L"MSKWindow";

	// Attempt To Register The Window Class
	RegisterClass(&wc);

	Bind(WM_SIZE, (MSGFN)&CMSKWindow::OnSize);
	Bind(WM_DESTROY, (MSGFN)&CMSKWindow::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKWindow::OnCreate);
	Bind(WM_PAINT, (MSGFN)&CMSKWindow::OnPaint);
	Bind(WM_ERASEBKGND, (MSGFN)&CMSKWindow::OnEraseBkgnd);
	Bind(WM_DRAWITEM, (MSGFN)&CMSKWindow::OnDrawItem);

	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW,			// Extended Style For The Window
		L"MSKWindow",									// Class Name
		title,											// Window Title
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW,			// Selected Window Style
		0, 0,											// Window Position
		width,											// Calculate Adjusted Window Width
		height,											// Calculate Adjusted Window Height
		NULL,											// No Parent Window
		NULL,		// No Menu
		hInst,											// Instance
		this);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
}

CMSKWindow::~CMSKWindow() {
	DeleteBrush(m_hbrBackground);
}

void CMSKWindow::Top() {
	m_nFlags = 1;
}

void CMSKWindow::Bottom() {
	m_nFlags = 2;
}

void CMSKWindow::Left() {
	m_nFlags = 3;
}

void CMSKWindow::Right() {
	m_nFlags = 4;
}

void CMSKWindow::Fill() {
	m_nFlags = 5;
}

void CMSKWindow::None() {
	m_nFlags = 0;
}

int CMSKWindow::GetDockState() {
	return m_nFlags;
}

LRESULT CMSKWindow::RouterProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CMSKWindow *view;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
		view = (CMSKWindow*)cs->lpCreateParams;
		view->m_hWnd = hWnd;

		SetLastError(0);
		if (SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)view) == 0) {
			if (GetLastError() != 0)
				return FALSE;
		}
	} else {
		view = (CMSKWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
	}

	if (view) {
		return view->WndProc(message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

std::wstring CMSKWindow::GetLastErrorStr()
{
	DWORD error = GetLastError();
	if (error)
	{
		LPVOID lpMsgBuf;
		DWORD bufLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		if (bufLen)
		{
			LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
			std::wstring result(lpMsgStr, lpMsgStr + bufLen);

			LocalFree(lpMsgBuf);

			return result;
		}
	}
	return std::wstring();
}

HWND CMSKWindow::GetWindowHandle() {
	return m_hWnd;
}

void CMSKWindow::Bind(UINT msg, MSGFN msgCb) {
	MSGINFO info;
	info.msgCb = msgCb;
	
	if (msg == WM_CREATE || msg == WM_NOTIFY) {
		info.type = 9;
		m_CBMsg.emplace(msg, info);
	} else if (msg == WM_DESTROY || msg == WM_PAINT || msg == WM_MOUSELEAVE) {
		info.type = 0;
		m_CBMsg.emplace(msg, info);
	} else if (msg == WM_SIZE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_MOUSEMOVE ||
		msg == WM_CONTEXTMENU) {
		info.type = 4;
		m_CBMsg.emplace(msg, info);
	} else if (msg == WM_ERASEBKGND) {
		info.type = 8;
		m_CBMsg.emplace(msg, info);
	} else if (msg == WM_DRAWITEM) {
		info.type = 3;
		m_CBMsg.emplace(msg, info);
	} else if (msg == WM_NCCALCSIZE) {
		info.type = 3;
		m_CBMsg.emplace(msg, info);
	} else if (msg == WM_NCPAINT) {
		info.type = 1;
		m_CBMsg.emplace(msg, info);
	} else if (msg == NM_CUSTOMDRAW) {
		info.type = 8;
		GetParent()->m_CBNotify.emplace(msg, info);
	} else if (msg == WM_COMMAND) {
		info.type = 10;
		m_CBMsg.emplace(msg, info);
	} else if (msg == WM_MOVE) {
		info.type = 14;
		m_CBMsg.emplace(msg, info);
	} else if (msg == WM_ACTIVATE) {
		info.type = 5;
		m_CBMsg.emplace(msg, info);
	}
}

CMSKWindow* CMSKWindow::GetParent() {
	return m_pParent;
}

void CMSKWindow::SetParent(CMSKWindow* parent) {
	::SetParent(m_hWnd, parent->GetWindowHandle());
	m_pParent = parent;
}

void CMSKWindow::Add(CMSKWindow* window) {
	// Add window as a child
	m_pChildWnds.push_back(window);

	// Set the unique ID for each window
	HWND hWnd = window->GetWindowHandle();
	SetWindowLongPtr(hWnd, GWL_ID, m_pChildWnds.size() - 1);

	// Update window size
	SendMessage(m_hWnd, WM_SIZE, 0, MAKELPARAM(m_nWidth, m_nHeight));
	//UpdateWindow(window->GetWindowHandle());
}

void CMSKWindow::Remove(CMSKWindow* window) {
	m_pChildWnds.erase(std::remove(m_pChildWnds.begin(), m_pChildWnds.end(), window), m_pChildWnds.end());
}

int CMSKWindow::GetWidth() {
	return m_nWidth;
}

int CMSKWindow::GetHeight() {
	return m_nHeight;
}

void CMSKWindow::Update() {
	SendMessage(m_hWnd, WM_SIZE, 0, MAKELPARAM(m_nWidth, m_nHeight));
	UpdateWindow(m_hWnd);
}

void CMSKWindow::SetBorder(int left, int top, int right, int bottom) {
	m_rcBorder = { left, top, right, bottom };
	SendMessage(m_hWnd, WM_SIZE, 0, MAKELPARAM(m_nWidth, m_nHeight));
	UpdateWindow(m_hWnd);
}

BOOL CMSKWindow::PreCreateWindow(CREATESTRUCT &cs) {
	return TRUE;
}

void CMSKWindow::OnContextMenu(HWND hWnd, int x, int y)
{
}

void CMSKWindow::OnMouseLeave()
{
}

void CMSKWindow::OnDestroy() {
	DeleteBrush(m_hbrBackground);
	PostQuitMessage(0);
}

int CMSKWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	m_rcBorder = { 0, 0, 0, 0 };
	m_hbrBackground = CreateSolidBrush(RGB(250, 250, 250));
	return 0;
}

void CMSKWindow::OnPaint() {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);
	EndPaint(m_hWnd, &ps);
}

void CMSKWindow::OnActivate(int nState, bool bActivated, HWND hWnd){}

// Causing flickering
bool CMSKWindow::OnEraseBkgnd(HDC hdc) {
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	FillRect(hdc, &rc, m_hbrBackground);
	return TRUE;
}

void CMSKWindow::OnMove(int x, int y)
{
}

void CMSKWindow::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {
	SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);
	SetTextColor(lpDrawItemStruct->hDC, RGB(255, 255, 255));
	DrawText(lpDrawItemStruct->hDC, (LPWSTR)lpDrawItemStruct->itemData, 
		lstrlen((LPWSTR)lpDrawItemStruct->itemData), &lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER);
}

LRESULT CMSKWindow::OnNotify(LPNMHDR nmHDR)
{
	std::unordered_map<UINT, MSGINFO>::iterator iter = m_CBNotify.find(nmHDR->code);
	if (iter != m_CBNotify.end()) {
		MSGINFO info = m_CBNotify.at(nmHDR->code);

		if (info.type == nmHDR->idFrom) {
			CMSKWindow* pWnd = (CMSKWindow*)GetWindowLongPtr(nmHDR->hwndFrom, GWL_USERDATA);
			return (pWnd->*(LRESULT(CMSKWindow::*)(LPNMHDR))(info.msgCb))(nmHDR);
		}
	}

	return FALSE;
}

void CMSKWindow::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam) {

}

void CMSKWindow::OnNcPaint(HRGN hRgn) {

}

void CMSKWindow::OnSize(UINT nType, int cx, int cy) {
	// Store the width and height
	m_nWidth = cx; m_nHeight = cy;

	GetClientRect(m_hWnd, &m_AdjustedClientRect);
	m_AdjustedClientRect.left += m_rcBorder.left;
	m_AdjustedClientRect.top += m_rcBorder.top;
	m_AdjustedClientRect.right -= m_rcBorder.right;
	m_AdjustedClientRect.bottom -= m_rcBorder.bottom;
	for (std::vector<CMSKWindow*>::iterator it = m_pChildWnds.begin(); it != m_pChildWnds.end(); ++it) {
		CMSKWindow* child = (*it);
		int x = 0, y = 0, w = 0, h = 0;
		switch ((*it)->GetDockState()) {
		case 1: // Top
			x = m_AdjustedClientRect.left;
			y = m_AdjustedClientRect.top;
			w = m_AdjustedClientRect.right - m_AdjustedClientRect.left;
			h = child->GetHeight();
			MoveWindow(child->GetWindowHandle(), x, y, w, h, TRUE);
			m_AdjustedClientRect.top += h + m_rcBorder.top;
			break;
		case 2:
			x = m_AdjustedClientRect.left;
			y = m_AdjustedClientRect.bottom - child->GetHeight();
			w = m_AdjustedClientRect.right - m_AdjustedClientRect.left;
			h = child->GetHeight();
			MoveWindow(child->GetWindowHandle(), x, y, w, h, TRUE);
			m_AdjustedClientRect.bottom -= h + m_rcBorder.bottom;
			break;
		case 5:
			x = m_AdjustedClientRect.left;
			y = m_AdjustedClientRect.top;
			w = m_AdjustedClientRect.right - m_AdjustedClientRect.left;
			h = m_AdjustedClientRect.bottom - m_AdjustedClientRect.top;
			MoveWindow(child->GetWindowHandle(), x, y, w, h, TRUE);
			break;
		}
	}
}

RECT CMSKWindow::GetAdjustedRect() {
	return m_AdjustedClientRect;
}

void CMSKWindow::SetBkColor(COLORREF color) {
	if (m_hbrBackground != NULL) {
		DeleteBrush(m_hbrBackground);
		m_hbrBackground = NULL;
	}

	m_hbrBackground = CreateSolidBrush(color);
}

LRESULT CALLBACK CMSKWindow::SubClassRouter(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData)
{
	CMSKWindow* pWindow = (CMSKWindow*)dwRefData;
	return pWindow->SubClassProc(hWnd, message, wParam, lParam, uIdSubclass, dwRefData);
}

LRESULT CALLBACK CMSKWindow::SubClassProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData) 
{
	std::unordered_map<UINT, MSGINFO>::iterator iter = m_CBMsg.find(message);
	if (iter != m_CBMsg.end()) {
		MSGINFO info = m_CBMsg.at(message);
		switch (info.type) {
		case 0:
			(this->*(void (CMSKWindow::*)(void))(info.msgCb))();
			break;
		case 1:
			(this->*(void (CMSKWindow::*)(UINT_PTR))(info.msgCb))(wParam);
			break;
		case 2:
			(this->*(void (CMSKWindow::*)(LONG_PTR))(info.msgCb))(lParam);
			break;
		case 3:
			(this->*(void (CMSKWindow::*)(UINT_PTR, LONG_PTR))(info.msgCb))(wParam, lParam);
			break;
		case 4:
			(this->*(void (CMSKWindow::*)(UINT_PTR, WORD, WORD))(info.msgCb))(wParam, HIWORD(lParam), LOWORD(lParam));
			break;
		case 5:
			(this->*(void (CMSKWindow::*)(WORD, WORD, LONG_PTR))(info.msgCb))(HIWORD(lParam), LOWORD(lParam), lParam);
			break;
		case 6:
			(this->*(void (CMSKWindow::*)(WORD, WORD, WORD, WORD))(info.msgCb))(HIWORD(lParam), LOWORD(lParam), HIWORD(lParam), LOWORD(lParam));
			break;
		case 7:
			return (this->*(LRESULT(CMSKWindow::*)(void))(info.msgCb))();
			break;
		case 8:
			return (this->*(LRESULT(CMSKWindow::*)(UINT_PTR))(info.msgCb))(wParam);
			break;
		case 9:
			return (this->*(LRESULT(CMSKWindow::*)(LONG_PTR))(info.msgCb))(lParam);
			break;
		case 10:
			return (this->*(LRESULT(CMSKWindow::*)(UINT_PTR, LONG_PTR))(info.msgCb))(wParam, lParam);
			break;
		case 11:
			return (this->*(LRESULT(CMSKWindow::*)(UINT_PTR, WORD, WORD))(info.msgCb))(wParam, HIWORD(lParam), LOWORD(lParam));
			break;
		case 12:
			return (this->*(LRESULT(CMSKWindow::*)(WORD, WORD, LONG_PTR))(info.msgCb))(HIWORD(lParam), LOWORD(lParam), lParam);
			break;
		case 13:
			return (this->*(LRESULT(CMSKWindow::*)(WORD, WORD, WORD, WORD))(info.msgCb))(HIWORD(lParam), LOWORD(lParam), HIWORD(lParam), LOWORD(lParam));
			break;
		}
	}

	return DefSubclassProc(hWnd, message, wParam, lParam);
}

LRESULT CMSKWindow::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	LPNMHDR notify;

	std::unordered_map<UINT, MSGINFO>::iterator iter = m_CBMsg.find(message);
	if (iter != m_CBMsg.end()) {
		MSGINFO info = m_CBMsg.at(message);

		switch (info.type) {
		case 0:
			(this->*(void (CMSKWindow::*)(void))(info.msgCb))();
			break;
		case 1:
			(this->*(void (CMSKWindow::*)(UINT_PTR))(info.msgCb))(wParam);
			break;
		case 2:
			(this->*(void (CMSKWindow::*)(LONG_PTR))(info.msgCb))(lParam);
			break;
		case 3:
			(this->*(void (CMSKWindow::*)(UINT_PTR, LONG_PTR))(info.msgCb))(wParam, lParam);
			break;
		case 4:
			(this->*(void (CMSKWindow::*)(UINT_PTR, WORD, WORD))(info.msgCb))(wParam, LOWORD(lParam), HIWORD(lParam));
			break;
		case 5:
			(this->*(void (CMSKWindow::*)(WORD, WORD, LONG_PTR))(info.msgCb))(HIWORD(lParam), LOWORD(lParam), lParam);
			break;
		case 6:
			(this->*(void (CMSKWindow::*)(WORD, WORD, WORD, WORD))(info.msgCb))(HIWORD(lParam), LOWORD(lParam), HIWORD(lParam), LOWORD(lParam));
			break;
		case 7:
			return (this->*(LRESULT (CMSKWindow::*)(void))(info.msgCb))();
			break;
		case 8:
			return (this->*(LRESULT (CMSKWindow::*)(UINT_PTR))(info.msgCb))(wParam);
			break;
		case 9:
			return (this->*(LRESULT(CMSKWindow::*)(LONG_PTR))(info.msgCb))(lParam);
			break;
		case 10:
			return (this->*(LRESULT (CMSKWindow::*)(UINT_PTR, LONG_PTR))(info.msgCb))(wParam, lParam);
			break;
		case 11: 
			return (this->*(LRESULT (CMSKWindow::*)(UINT_PTR, WORD, WORD))(info.msgCb))(wParam, HIWORD(lParam), LOWORD(lParam));
			break;
		case 12:
			return (this->*(LRESULT (CMSKWindow::*)(WORD, WORD, LONG_PTR))(info.msgCb))(HIWORD(lParam), LOWORD(lParam), lParam);
			break;
		case 13:
			return (this->*(LRESULT (CMSKWindow::*)(WORD, WORD, WORD, WORD))(info.msgCb))(HIWORD(lParam), LOWORD(lParam), HIWORD(lParam), LOWORD(lParam));
			break;
		case 14:
			return (this->*(LRESULT(CMSKWindow::*)(WORD, WORD))(info.msgCb))(LOWORD(lParam), HIWORD(lParam));
			break;
		}
	}

	/*if (message == WM_NOTIFY) {
		return OnNotify((LPNMHDR)lParam);
	}*/

	m_bResizing = false;

	return DefWindowProc(m_hWnd, message, wParam, lParam);
}