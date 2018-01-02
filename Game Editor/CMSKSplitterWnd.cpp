#include "stdafx.h"
#include "CMSKSplitterWnd.h"

#define ID_FIRSTCHILD 100
#define ID_SECONDCHILD 101

CMSKSplitterWnd::CMSKSplitterWnd() {
}

CMSKSplitterWnd::CMSKSplitterWnd(CMSKWindow* parent, CMSKWindow* first, CMSKWindow* second, BOOL bVertical) {
	// Get instance handle
	hInst = parent->GetInstanceHandle();

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)RouterProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = parent->GetInstanceHandle();
	wc.hIcon = 0;
	if (bVertical) {
		wc.hCursor = LoadCursor(NULL, IDC_SIZENS);
		wc.lpszClassName = L"MSKSplitterVClass";
	} else {
		wc.hCursor = LoadCursor(NULL, IDC_SIZEWE);
		wc.lpszClassName = L"MSKSplitterHClass";
	}
	wc.hbrBackground = NULL;
	wc.lpszMenuName = 0;

	// Attempt To Register The Window Class
	RegisterClass(&wc);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT CALLBACK {
		return DefWindowProc(hWnd, message, wParam, lParam);
	};
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = 0;
	if (bVertical) {
		wc.hCursor = LoadCursor(NULL, IDC_SIZENS);
	} else {
		wc.hCursor = LoadCursor(NULL, IDC_SIZEWE);
	}
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MSKSplitterBar";

	// Attempt To Register The Window Class
	RegisterClass(&wc);

	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = (PCTSTR)NULL;
	cs.hInstance = parent->GetInstanceHandle();
	if (bVertical) {
		cs.lpszClass = L"MSKSplitterVClass";
	} else {
		cs.lpszClass = L"MSKSplitterHClass";
	}
	cs.style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)2;
	PreCreateWindow(cs);

	// Set the parent
	SetParent(parent);

	Bind(WM_DESTROY, (MSGFN)&CMSKSplitterWnd::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKSplitterWnd::OnCreate);
	Bind(WM_PAINT, (MSGFN)&CMSKSplitterWnd::OnPaint);
	Bind(WM_ERASEBKGND, (MSGFN)&CMSKSplitterWnd::OnEraseBkgnd);

	if (bVertical) {
		Bind(WM_SIZE, (MSGFN)&CMSKSplitterWnd::OnSizeV);
		Bind(WM_LBUTTONDOWN, (MSGFN)&CMSKSplitterWnd::OnLButtonDownV);
		Bind(WM_LBUTTONUP, (MSGFN)&CMSKSplitterWnd::OnLButtonUpV);
		Bind(WM_MOUSEMOVE, (MSGFN)&CMSKSplitterWnd::OnMouseMoveV);
	} else {
		Bind(WM_SIZE, (MSGFN)&CMSKSplitterWnd::OnSizeH);
		Bind(WM_LBUTTONDOWN, (MSGFN)&CMSKSplitterWnd::OnLButtonDownH);
		Bind(WM_LBUTTONUP, (MSGFN)&CMSKSplitterWnd::OnLButtonUpH);
		Bind(WM_MOUSEMOVE, (MSGFN)&CMSKSplitterWnd::OnMouseMoveH);
	}

	// Remove from parent
	parent->Remove(first);
	parent->Remove(second);

	// Store child windows
	hWndFirst = first->GetWindowHandle();
	hWndSecond = second->GetWindowHandle();

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	m_ptBarPos = { parent->GetWidth() / 2, parent->GetHeight() / 2 };

	Fill();

	// Add to parent window
	parent->Add(this);
}

CMSKSplitterWnd::~CMSKSplitterWnd() {
	if (m_hWnd) {
		DestroyWindow(m_hWnd);
	}
}

int CMSKSplitterWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	CMSKWindow::OnCreate(lpCreateStruct);

	// Create the bar
	m_hWndBar = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, L"MSKSplitterBar", L"", WS_POPUP, 0, 0, 0, 0,
		m_hWnd, NULL, hInst, 0);

	SetLayeredWindowAttributes(m_hWndBar, 0, (255 * 70) / 100, LWA_ALPHA);

	// Modify second window
	if (hWndSecond != NULL) {
		::SetParent(hWndSecond, m_hWnd);
		DWORD style = GetWindowLong(hWndSecond, GWL_STYLE);
		style &= ~(WS_POPUP | WS_CAPTION);
		style |= WS_CHILD;
		SetWindowLong(hWndSecond, GWL_STYLE, style);
		m_idSecond = GetWindowLong(hWndSecond, GWL_ID); // Save a copy
		SetWindowLong(hWndSecond, GWL_ID, ID_SECONDCHILD);
	}

	// Modify first window
	if (hWndFirst != NULL) {
		::SetParent(hWndFirst, m_hWnd);
		DWORD style = GetWindowLong(hWndFirst, GWL_STYLE);
		style &= ~(WS_POPUP | WS_CAPTION);
		style |= WS_CHILD;
		SetWindowLong(hWndFirst, GWL_STYLE, style);
		m_idFirst = GetWindowLong(hWndFirst, GWL_ID); // Save a copy
		SetWindowLong(hWndFirst, GWL_ID, ID_FIRSTCHILD);
	}

	return 0;
}

void CMSKSplitterWnd::OnSizeH(UINT nType, int cx, int cy) {
	CMSKWindow::OnSize(nType, cx, cy);

	// Move the first window
	MoveWindow(hWndFirst, m_AdjustedClientRect.left, m_AdjustedClientRect.top,
		m_ptBarPos.x - m_AdjustedClientRect.left, m_AdjustedClientRect.bottom - m_AdjustedClientRect.top, TRUE);
	m_AdjustedClientRect.left = m_ptBarPos.x + 5;


	// Move the second window
	MoveWindow(hWndSecond, m_AdjustedClientRect.left, m_AdjustedClientRect.top,
		m_AdjustedClientRect.right - m_AdjustedClientRect.left, m_AdjustedClientRect.bottom - m_AdjustedClientRect.top, TRUE);
}

void CMSKSplitterWnd::OnLButtonDownH(UINT nFlags, POINT point) {
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_ptCurrentPos = { m_ptBarPos.x,  rc.top };
	ClientToScreen(m_hWnd, &m_ptCurrentPos);

	m_fDrag = TRUE;
	SetCapture(m_hWnd);

	MoveWindow(m_hWndBar, m_ptCurrentPos.x, m_ptCurrentPos.y, 5, rc.bottom - rc.top, TRUE);
	ShowWindow(m_hWndBar, SW_SHOW);

	m_ptOldPos = m_ptCurrentPos;
}

void CMSKSplitterWnd::OnLButtonUpH(UINT nFlags, POINT point) {
	if (m_fDrag != FALSE) {
		m_fDrag = FALSE;
		ReleaseCapture();

		ShowWindow(m_hWndBar, SW_HIDE);

		m_ptBarPos = { m_ptOldPos.x, 0 };
		ScreenToClient(m_hWnd, &m_ptBarPos);

		GetClientRect(m_hWnd, &m_AdjustedClientRect);
		SendMessage(m_hWnd, WM_SIZE, 0, 0);
	}
}

void CMSKSplitterWnd::OnMouseMoveH(UINT nFlags, POINT point) {
	if (m_fDrag != FALSE) {
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		GetCursorPos(&m_ptCurrentPos);
		POINT pt = { rc.left, rc.top };
		ClientToScreen(m_hWnd, &pt);

		if (m_ptCurrentPos.x < pt.x + 10) {
			m_ptCurrentPos.x = pt.x + 10;
		}

		if (m_ptCurrentPos.x > pt.x + (rc.right - 15)) {
			m_ptCurrentPos.x = pt.x + (rc.right - 15);
		}

		MoveWindow(m_hWndBar, m_ptCurrentPos.x, pt.y, 5, rc.bottom - rc.top, TRUE);
		m_ptOldPos = m_ptCurrentPos;
	}
}

void CMSKSplitterWnd::OnSizeV(UINT nType, int cx, int cy) {
	CMSKWindow::OnSize(nType, cx, cy);

	// Move the first window
	MoveWindow(hWndFirst, m_AdjustedClientRect.left, m_AdjustedClientRect.top,
		m_AdjustedClientRect.right - m_AdjustedClientRect.left, m_ptBarPos.y - m_AdjustedClientRect.top, TRUE);
	m_AdjustedClientRect.top = m_ptBarPos.y + 5;

	// Move the second window
	MoveWindow(hWndSecond, m_AdjustedClientRect.left, m_AdjustedClientRect.top,
		m_AdjustedClientRect.right - m_AdjustedClientRect.left, m_AdjustedClientRect.bottom - m_AdjustedClientRect.top, TRUE);
}

void CMSKSplitterWnd::OnLButtonDownV(UINT nFlags, POINT point) {
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_ptCurrentPos = { rc.left, m_ptBarPos.y };
	ClientToScreen(m_hWnd, &m_ptCurrentPos);

	m_fDrag = TRUE;
	SetCapture(m_hWnd);

	MoveWindow(m_hWndBar, m_ptCurrentPos.x, m_ptCurrentPos.y, rc.right - rc.left, 5, TRUE);
	ShowWindow(m_hWndBar, SW_SHOW);

	m_ptOldPos = m_ptCurrentPos;
}

void CMSKSplitterWnd::OnLButtonUpV(UINT nFlags, POINT point) {
	if (m_fDrag != FALSE) {
		m_fDrag = FALSE;
		ReleaseCapture();

		ShowWindow(m_hWndBar, SW_HIDE);

		m_ptBarPos = { 0, m_ptOldPos.y };
		ScreenToClient(m_hWnd, &m_ptBarPos);

		GetClientRect(m_hWnd, &m_AdjustedClientRect);
		SendMessage(m_hWnd, WM_SIZE, 0, 0);
	}
}

void CMSKSplitterWnd::OnMouseMoveV(UINT nFlags, POINT point) {
	if (m_fDrag != FALSE) {
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		GetCursorPos(&m_ptCurrentPos);
		POINT pt = { rc.left, rc.top };
		ClientToScreen(m_hWnd, &pt);

		if (m_ptCurrentPos.y < pt.y + 10) {
			m_ptCurrentPos.y = pt.y + 10;
		}

		if (m_ptCurrentPos.y > pt.y + (rc.bottom - 15)) {
			m_ptCurrentPos.y = pt.y + (rc.bottom - 15);
		}

		MoveWindow(m_hWndBar, pt.x, m_ptCurrentPos.y, rc.right - rc.left, 5, TRUE);
		m_ptOldPos = m_ptCurrentPos;
	}
}