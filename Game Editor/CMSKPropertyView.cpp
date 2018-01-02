#include "stdafx.h"
#include "CMSKPropertyView.h"

#define PVF_COLLAPSED 0
#define PVF_EXPANDED 1
#define PVF_SELECTED 2
#define PVF_DISABLE 4

typedef struct PVITEM {
	LPCWSTR text;
	RECT rcItem;
	UINT flags;
	CMSKPropertyBox* pb;
	std::vector<PVITEM*> children;
} *PPVITEM;

CMSKPropertyView::CMSKPropertyView(LPCWSTR text, CMSKWindow * parent)
{
	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = (PCTSTR)NULL;
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = L"MSKWindow";
	cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)10;
	PreCreateWindow(cs);

	// Copy the window instance handler
	hInst = parent->GetInstanceHandle();

	Bind(WM_CREATE, (MSGFN)&CMSKPropertyView::OnCreate);
	Bind(WM_SIZE, (MSGFN)&CMSKPropertyView::OnSize);
	Bind(WM_DESTROY, (MSGFN)&CMSKPropertyView::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKPropertyView::OnCreate);
	Bind(WM_NOTIFY, (MSGFN)&CMSKPropertyView::OnNotify);
	Bind(WM_LBUTTONDOWN, (MSGFN)&CMSKPropertyView::OnLButtonDown);
	Bind(WM_ERASEBKGND, (MSGFN)&CMSKPropertyView::OnEraseBkgnd);
	Bind(WM_PAINT, (MSGFN)&CMSKPropertyView::OnPaint);

	// Indent amount
	m_nIndent = 16;

	// Set the item height
	m_nItemHeight = 20;

	// Set Background
	SetBkColor(RGB(245, 245, 245));

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	SetText(text);

	SetParent(parent);

	Fill();

	parent->Add(this);
}

CMSKPropertyView::~CMSKPropertyView()
{
}

HPVITEM CMSKPropertyView::AddCategory(LPCWSTR text) {
	PPVITEM pvItem = new PVITEM();
	pvItem->text = text;
	pvItem->pb = new CMSKPropertyBox(this);
	pvItem->rcItem = { 0, 0, 0, 0 };
	pvItem->flags = PVF_COLLAPSED;

	nodes.push_back((HPVITEM)pvItem);

	return (HPVITEM)pvItem;
}

HPVITEM CMSKPropertyView::AddProperty(LPCWSTR text, UINT type, HPVITEM root)
{
	PPVITEM pvItem = new PVITEM();
	pvItem->text = text;
	pvItem->pb = new CMSKPropertyBox(this);
	pvItem->rcItem = { 0, 0, 0, 0 };
	pvItem->flags = PVF_EXPANDED;

	PPVITEM pvRoot = (PPVITEM)root;
	pvRoot->children.push_back(pvItem);

	return (HPVITEM)pvItem;
}

LRESULT CMSKPropertyView::OnNotify(LPNMHDR nmHDR)
{
	POINT pt;
	switch (nmHDR->code) {
	case PBN_SELECTED:
		GetCursorPos(&pt);
		OnLButtonDown(0, pt);
		return TRUE;
	}
	return FALSE;
}

void CMSKPropertyView::OnLButtonDown(UINT nFlags, POINT point)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);

	RECT rc;
	GetClientRect(m_hWnd, &rc);

	bool other = true;
	for (std::vector<HPVITEM>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		PPVITEM node = (PPVITEM)(*it);

		RECT rcIcon;
		rcIcon.left = 2;
		rcIcon.right = rcIcon.left + 9;
		rcIcon.top = node->rcItem.top + (node->rcItem.bottom - node->rcItem.top) / 2 - 4;
		rcIcon.bottom = rcIcon.top + 9;

		if ((node->flags & PVF_EXPANDED) == PVF_EXPANDED) {
			for (std::vector<PPVITEM>::iterator jt = node->children.begin(); jt != node->children.end(); ++jt) {
				PPVITEM child = (PPVITEM)(*jt);

				rc.left = child->rcItem.left;
				rc.top = child->rcItem.top;
				rc.bottom = child->rcItem.bottom;

				if (PtInRect(&rc, pt)) {
					// Keep track of selected item
					if (m_hItemSelected) {
						((PPVITEM)m_hItemSelected)->flags &= ~PVF_SELECTED;
					}

					other = false;
					child->flags |= PVF_SELECTED;
					InvalidateRect(m_hWnd, NULL, TRUE);
					m_hItemSelected = (HPVITEM)child; 
				}
			}
		}

		if (PtInRect(&rcIcon, pt)) { // PVM_SELCHANGED
			other = false;
			node->flags ^= PVF_EXPANDED;
			SendMessage(m_hWnd, WM_SIZE, 0, 0);
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}

	if (other) {
		SetFocus(m_hWnd);
	}
}

void CMSKPropertyView::OnDestroy()
{
	DeleteFont(m_hFontBold);
	DeleteBrush(m_hBrush);
	DeleteBrush(m_hSelectedBrush);
	DeleteFont(m_hFont);
}

void CMSKPropertyView::OnPaint()
{
	HFONT hFontOld;
	HBRUSH hBrushOld;
	PAINTSTRUCT ps;

	HDC hdc = BeginPaint(m_hWnd, &ps);
	for (std::vector<HPVITEM>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		PPVITEM node = (PPVITEM)(*it);

		int hh = (node->rcItem.bottom - node->rcItem.top) / 2;
		if ((node->flags & PVF_EXPANDED) == PVF_EXPANDED)
			DrawIconEx(hdc, 2, node->rcItem.top + hh - 4, m_hIconMinus, 9, 9, NULL, NULL, DI_NORMAL);
		else
			DrawIconEx(hdc, 2, node->rcItem.top + hh - 4, m_hIconPlus, 9, 9, NULL, NULL, DI_NORMAL);

		hFontOld = (HFONT)SelectObject(hdc, m_hFontBold);
		hBrushOld = (HBRUSH)SelectObject(hdc, m_hBrush);
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, node->text, -1, &node->rcItem, DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		SelectObject(hdc, hBrushOld);
		SelectObject(hdc, hFontOld);

		if ((node->flags & PVF_EXPANDED) == PVF_EXPANDED) {
			for (std::vector<PPVITEM>::iterator jt = node->children.begin(); jt != node->children.end(); ++jt) {
				PPVITEM child = (PPVITEM)(*jt);

				if ((child->flags & PVF_SELECTED) == PVF_SELECTED) {
					FillRect(hdc, &child->rcItem, m_hSelectedBrush);
				} else {
					FillRect(hdc, &child->rcItem, m_hBrush);
				}

				hFontOld = (HFONT)SelectObject(hdc, m_hFont);
				hBrushOld = (HBRUSH)SelectObject(hdc, m_hBrush);
				SetBkMode(hdc, TRANSPARENT);
				DrawText(hdc, child->text, -1, &child->rcItem, DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
				SelectObject(hdc, hBrushOld);
				SelectObject(hdc, hFontOld);
			}
		}
	}
	EndPaint(m_hWnd, &ps);
}

int CMSKPropertyView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_hBrush = CreateSolidBrush(RGB(255, 255, 255));

	m_hSelectedBrush = CreateSolidBrush(RGB(0, 122, 204));

	m_hFont = CreateFont(16, 0, 0, 0, FW_NORMAL,
		false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");

	m_hFontBold = CreateFont(16, 0, 0, 0, FW_SEMIBOLD,
		false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");

	m_hIconPlus = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PVPLUS));

	m_hIconMinus = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PVMINUS));

	return 0;
}
 
void CMSKPropertyView::OnSize(UINT nType, int cx, int cy)
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int y = 0;
	for (std::vector<HPVITEM>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		PPVITEM node = (PPVITEM)(*it);

		node->rcItem.left = rcClient.left + m_nIndent;
		node->rcItem.right = rcClient.right;
		node->rcItem.top = y;
		node->rcItem.bottom = y + m_nItemHeight;
		y += m_nItemHeight + 2;

		if ((node->flags & PVF_EXPANDED) == PVF_EXPANDED) {
			for (std::vector<PPVITEM>::iterator jt = node->children.begin(); jt != node->children.end(); ++jt) {
				PPVITEM child = (PPVITEM)(*jt);

				child->rcItem.left = rcClient.left + m_nIndent;
				child->rcItem.right = rcClient.right / 2.0f;
				child->rcItem.top = y;
				child->rcItem.bottom = y + m_nItemHeight;
				y += m_nItemHeight + 2;

				int x = child->rcItem.right + 2;
				int y = child->rcItem.top;
				int w = rcClient.right - x;
				int h = child->rcItem.bottom - child->rcItem.top;
				MoveWindow(child->pb->GetWindowHandle(), x, y, w, h, TRUE);
				ShowWindow(child->pb->GetWindowHandle(), SW_SHOW);
			}
		} else {
			for (std::vector<PPVITEM>::iterator jt = node->children.begin(); jt != node->children.end(); ++jt) {
				PPVITEM child = (PPVITEM)(*jt);
				ShowWindow(child->pb->GetWindowHandle(), SW_HIDE);
			}
		}
	}
}


