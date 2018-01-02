#include "stdafx.h"
#include "CMSKTreeView.h"
#include "CMSKPropertyBox.h"

CMSKTreeView::CMSKTreeView(LPCWSTR text, CMSKWindow* parent)
{
	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(CREATESTRUCT));
	cs.hwndParent = parent->GetWindowHandle();
	cs.lpszName = text;
	cs.hInstance = parent->GetInstanceHandle();
	cs.lpszClass = WC_TREEVIEW;
	cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | TVS_HASBUTTONS | TVS_LINESATROOT;
	cs.lpCreateParams = this;
	cs.hMenu = (HMENU)8;
	PreCreateWindow(cs);

	// Store instance
	hInst = parent->GetInstanceHandle();

	// Set window text
	SetText(text);

	// Set the parent
	SetParent(parent);

	// Set background color
	m_colBackColor = RGB(245, 245, 245);
	TreeView_SetBkColor(m_hWnd, m_colBackColor);

	m_hBrush = CreateSolidBrush(RGB(255, 255, 255));
	m_hFocusBrush = CreateSolidBrush(RGB(0, 122, 204));
	m_hPen = CreatePen(PS_SOLID, 1, RGB(210, 210, 225));

	Bind(WM_NOTIFY, (MSGFN)&CMSKTreeView::OnNotify);
	Bind(WM_CONTEXTMENU, (MSGFN)&CMSKTreeView::OnContextMenu);

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	m_hFont = CreateFont(15, 0, 0, 0, FW_BLACK,
		false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");

	m_hFontNormal = CreateFont(15, 0, 0, 0, FW_NORMAL,
		false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");

	m_hBitmap = (HBITMAP)::LoadImage(NULL,
		MAKEINTRESOURCE(OBM_DNARROW), IMAGE_BITMAP, 0, 0,
		LR_DEFAULTSIZE | LR_SHARED);

	SendMessage(m_hWnd, WM_SETFONT, (WPARAM)m_hFontNormal, 0);

	// Set the class pointer
	SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG)this);

	// Subclass
	SetWindowSubclass(m_hWnd, SubClassRouter, 0, (DWORD)this);

	// Dock at the bottom
	Fill();

	// Add to parent window
	parent->Add(this);
}

CMSKTreeView::~CMSKTreeView()
{
	DeleteFont(m_hFontNormal);
	DeleteBrush(m_hBrush);
	DeleteBrush(m_hFocusBrush);
	DeleteBitmap(m_hBitmap);
	DeleteFont(m_hFont);
}

HTREEITEM CMSKTreeView::InsertItem(LPCWSTR text, HTREEITEM parent, HTREEITEM insertAfter,
	int imageIdx, int selImageIdx) {
	TVINSERTSTRUCT insertStruct;
	insertStruct.hParent = parent;
	insertStruct.hInsertAfter = insertAfter;
	insertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	insertStruct.item.pszText = (LPWSTR)text;
	insertStruct.item.cchTextMax = sizeof(text) / sizeof(text[0]);
	insertStruct.item.iImage = imageIdx;
	insertStruct.item.iSelectedImage = selImageIdx;
	insertStruct.item.lParam = (LPARAM)(new CMSKPropertyBox(this));

	return (HTREEITEM)TreeView_InsertItem(m_hWnd, &insertStruct);
}

void CMSKTreeView::ExpandItem(HTREEITEM hItem)
{
	TreeView_Expand(m_hWnd, hItem, TVE_EXPAND);
}

void CMSKTreeView::EnsureVisibility(HTREEITEM item)
{
	TreeView_EnsureVisible(m_hWnd, item);
}

void CMSKTreeView::SetImageList(HIMAGELIST imageList) {
	SendMessage(m_hWnd, TVM_SETIMAGELIST,
		(WPARAM)TVSIL_NORMAL, (LPARAM)imageList);
}

void CMSKTreeView::DeleteItem(HTREEITEM item) {
	SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)item);
}

void CMSKTreeView::OnContextMenu(HWND hWnd, int y, int x)
{
	POINT pt;
	GetCursorPos(&pt);

	HMENU hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_POPUP | MF_STRING, 0, L"Test");
	AppendMenu(hMenu, MF_POPUP | MF_STRING, 0, L"Test2");

	int command = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, 0,
		GetParent()->GetWindowHandle(), 0);

	ScreenToClient(m_hWnd, &pt);

	TVHITTESTINFO hitTestInfo;
	hitTestInfo.pt.x = pt.x;
	hitTestInfo.pt.y = pt.y;

	HTREEITEM item = (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST,
		(WPARAM)0, (LPARAM)&hitTestInfo);

	if (item) {
		OutputDebugString(L"Hit!");
	}

	/*if (command == ID_TREEVIEW_ADDITEM)
	{
	// add new item
	}
	else if (command == ID_TREEVIEW_REMOVEITEM)
	{
	// delete the selected item
	}*/

	DestroyMenu(hMenu);
}

LRESULT CMSKTreeView::OnNotify(LPNMHDR nmHDR)
{
	switch (nmHDR->code) {
	case NM_CUSTOMDRAW:
		OnCustomDraw(LPNMTVCUSTOMDRAW(nmHDR));
		break;
	}

	return FALSE;
}

LRESULT CMSKTreeView::OnCustomDraw(LPNMTVCUSTOMDRAW lpNMCustomDraw)
{
	HPEN hPenOld;
	HBRUSH hBrushOld;
	HFONT hFontOld;
	HWND hWnd;
	TVITEM item;

	RECT rc;
	GetClientRect(m_hWnd, &rc);

	HDC hdc = lpNMCustomDraw->nmcd.hdc;
	switch (lpNMCustomDraw->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:

		// Clear to background color
		lpNMCustomDraw->clrText = m_colBackColor;
		lpNMCustomDraw->clrTextBk = m_colBackColor;

		// reset the focus, because it will be drawn of us
		m_bFocus = false;
		if (lpNMCustomDraw->nmcd.uItemState & CDIS_FOCUS) {
			m_bFocus = true;
		}

		lpNMCustomDraw->nmcd.uItemState &= ~CDIS_FOCUS;
		return CDRF_NOTIFYPOSTPAINT;
	case CDDS_ITEMPOSTPAINT:
		switch (lpNMCustomDraw->iLevel) {
		case 0:
			TreeView_GetItemRect(m_hWnd, (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec, &m_rcItem, TRUE);
			m_rcItem.right = rc.right;

			item.cchTextMax = 255;
			item.mask = TVIF_TEXT;
			item.hItem = (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec;
			item.pszText = (LPWSTR)malloc(sizeof(wchar_t) * 255);
			TreeView_GetItem(m_hWnd, &item);

			hFontOld = (HFONT)SelectObject(hdc, m_hFont);
			hBrushOld = (HBRUSH)SelectObject(hdc, m_hBrush);
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, item.pszText, -1, &m_rcItem, DT_BOTTOM | DT_SINGLELINE | DT_WORD_ELLIPSIS);
			SelectObject(hdc, hBrushOld);
			SelectObject(hdc, hFontOld);
			break;
		case 1:
			TreeView_GetItemRect(m_hWnd, (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec, &m_rcItem, TRUE);
			m_rcItem.left -= 20;
			m_rcItem.right = rc.right / 2;
			m_rcItem.top += 1;

			item.cchTextMax = 255;
			item.mask = TVIF_TEXT | TVIF_PARAM;
			item.hItem = (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec;
			item.pszText = (LPWSTR)malloc(sizeof(wchar_t) * 255);
			TreeView_GetItem(m_hWnd, &item);

			if (m_bFocus == true) {
				FillRect(hdc, &m_rcItem, (HBRUSH)m_hFocusBrush);
			} else {
				FillRect(hdc, &m_rcItem, (HBRUSH)m_hBrush);
			}

			m_rcItem.left += 2;
			hBrushOld = (HBRUSH)SelectObject(hdc, m_hBrush);
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, item.pszText, -1, &m_rcItem, DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
			SelectObject(hdc, hBrushOld);

			CMSKPropertyBox* pb = (CMSKPropertyBox*) lpNMCustomDraw->nmcd.lItemlParam;
			int x = m_rcItem.right + 1;
			int y = m_rcItem.top;
			int w = rc.right - x;
			int h = m_rcItem.bottom - m_rcItem.top;
			MoveWindow(pb->GetWindowHandle(), x, y, w, h, TRUE);
			break;
		}
		break;
	}

	return 0;
}
