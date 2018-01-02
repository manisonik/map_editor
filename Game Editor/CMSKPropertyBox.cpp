#include "stdafx.h"
#include "CMSKPropertyBox.h"

CMSKPropertyBox::CMSKPropertyBox(CMSKWindow* parent)
{
	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = (PCTSTR)NULL;
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = L"MSKWindow";
	cs.style = WS_CHILD | WS_VISIBLE;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)9;
	PreCreateWindow(cs); 

	// Copy the window instance handler
	hInst = parent->GetInstanceHandle(); 

	Bind(WM_SIZE, (MSGFN)&CMSKPropertyBox::OnSize);
	Bind(WM_DESTROY, (MSGFN)&CMSKPropertyBox::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKPropertyBox::OnCreate);
	Bind(WM_DRAWITEM, (MSGFN)&CMSKPropertyBox::OnDrawItem);
	Bind(WM_COMMAND, (MSGFN)&CMSKPropertyBox::OnCommand);
	Bind(WM_ACTIVATE, (MSGFN)&CMSKPropertyBox::OnActivate);
	Bind(WM_ERASEBKGND, (MSGFN)&CMSKPropertyBox::OnEraseBkgnd);

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	SetParent(parent);

	None();

	parent->Add(this);
}

void CMSKPropertyBox::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {
	if (nIDCtl == 801) {
		BITMAP bm;

		UINT state = lpDrawItemStruct->itemState;
		if (state & ODS_SELECTED) {
			RECT rc = lpDrawItemStruct->rcItem;
			HDC hdc = GetDC(lpDrawItemStruct->hwndItem);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, m_hBitmap);
			GetObject(m_hBitmap, sizeof(bm), &bm);
			BitBlt(hdc, (rc.right - rc.left) / 2 - 5, rc.top + 3, bm.bmWidth - 4, bm.bmHeight - 4, hdcMem, 2, 2, SRCCOPY);
			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
		} else {
			RECT rc = lpDrawItemStruct->rcItem;
			HDC hdc = GetDC(lpDrawItemStruct->hwndItem);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, m_hBitmap);
			GetObject(m_hBitmap, sizeof(bm), &bm);
			int w = (rc.right - rc.left) / 2;
			BitBlt(hdc, (rc.right - rc.left) / 2 - 6, rc.top + 2, bm.bmWidth - 4, bm.bmHeight - 4, hdcMem, 2, 2, SRCCOPY);
			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
		}

	}
}

CMSKPropertyBox::~CMSKPropertyBox()
{
}

void CMSKPropertyBox::OnDestroy()
{
	DeleteFont(m_hFont);
	DeleteBitmap(m_hBitmap);
	CMSKWindow::OnDestroy();
}

int CMSKPropertyBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_hFont = CreateFont(15, 0, 0, 0, FW_NORMAL,
		false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");

	m_hBitmap = (HBITMAP)::LoadImage(NULL,
		MAKEINTRESOURCE(OBM_DNARROW), IMAGE_BITMAP, 0, 0,
		LR_DEFAULTSIZE | LR_SHARED);

	m_hWndEdit = CreateWindowEx(0, L"EDIT", L"Test", WS_VISIBLE | WS_CHILD,
		0, 0, 0, 0, m_hWnd, (HMENU)802, hInst, 0);

	m_hWndButton = CreateWindowEx(0, WC_BUTTON, L"", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		0, 0, 0, 0, m_hWnd, (HMENU)801, hInst, 0);

	m_hWndListBox = CreateWindowEx(0, WC_LISTBOX, L"", WS_POPUP | WS_BORDER,
		0, 0, 100, 100, m_hWnd, (HMENU)NULL, hInst, 0);

	SendMessage(m_hWndEdit, WM_SETFONT, (WPARAM)m_hFont, 0);
	SendMessage(m_hWndListBox, WM_SETFONT, (WPARAM)m_hFont, 0);

	ListBox_AddString(m_hWndListBox, L"True");
	ListBox_AddString(m_hWndListBox, L"False");

	SetWindowSubclass(m_hWndListBox, [](HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
		DWORD_PTR dwRefData)->LRESULT CALLBACK {
		switch (message) {
		int nState;
		case WM_ACTIVATE:
			nState = LOWORD(wParam);
			if (nState == WA_INACTIVE)
				ShowWindow(hWnd, SW_HIDE);
			break;
		case WM_LBUTTONDOWN:
			DefSubclassProc(hWnd, message, wParam, lParam);

			int i = ListBox_GetCurSel(hWnd);
			if (i != LB_ERR) { // No selection
				CMSKPropertyBox* parent = (CMSKPropertyBox*)dwRefData;

				// Get the text length
				int textLen = ListBox_GetTextLen(hWnd, i);
				TCHAR* textBuffer = new TCHAR[textLen + 1];

				// Get the listbox text
				ListBox_GetText(hWnd, i, textBuffer);

				Sleep(100);

				parent->SetText(textBuffer);

				// Free text
				delete[] textBuffer;

				// Avoid dangling references
				textBuffer = NULL;

				// Hide listbox
				ShowWindow(hWnd, SW_HIDE);
			}
			return 0;
		}

		return DefSubclassProc(hWnd, message, wParam, lParam);
	}, 0, (DWORD)this);

	return TRUE;
}

void CMSKPropertyBox::SetText(LPCWSTR text) {
	Edit_SetText(m_hWndEdit, text);
}

void CMSKPropertyBox::SetType(UINT type) {
	if (type & PB_LIST) {
		int value = type & 0x4;
		switch (value) {
		case PB_BOOLEAN:
			break;
		case PB_FLOAT:
			break;
		case PB_INTEGER:
			break;
		case PB_STRING:
			break;
		}
	} else {

	}
}

bool CMSKPropertyBox::OnCommand(WPARAM wParam, LPARAM lParam) {
	UINT code;
	HWND hWndParent;

	switch (LOWORD(wParam)) {
	case 801:
		ShowWindow(m_hWndListBox, SW_SHOWDEFAULT);
		SendMessage(m_hWnd, WM_SIZE, 0, 0);
		return TRUE;
	case 802:
		code = HIWORD(wParam);

		switch (code) {
		case EN_SETFOCUS:
			NMHDR nmh;
			nmh.code = PBN_SELECTED;
			nmh.idFrom = 9;
			nmh.hwndFrom = m_hWnd;
			SendMessage(::GetParent(m_hWnd), WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

void CMSKPropertyBox::OnSize(UINT nType, int cx, int cy) {
	RECT rc;
	GetClientRect(m_hWnd, &rc);

	MoveWindow(m_hWndEdit, rc.left, rc.top, rc.right - 20, rc.bottom, TRUE);
	MoveWindow(m_hWndButton, rc.right - 20, rc.top, 20, rc.bottom, TRUE);

	POINT pt = { rc.left, rc.top };
	ClientToScreen(m_hWnd, &pt);
	MoveWindow(m_hWndListBox, pt.x, pt.y + rc.bottom, rc.right - rc.left, 50, TRUE);
}
