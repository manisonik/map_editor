#include "stdafx.h"
#include "CMSKPropertyList.h"
#include "CMSKPropertyBox.h"

CMSKPropertyList::CMSKPropertyList(LPCWSTR text, CMSKWindow* parent) {
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

	Bind(WM_SIZE, (MSGFN)&CMSKPropertyList::OnSize);
	Bind(WM_DESTROY, (MSGFN)&CMSKPropertyList::OnDestroy);
	Bind(WM_CREATE, (MSGFN)&CMSKPropertyList::OnCreate);
	Bind(WM_NOTIFY, (MSGFN)&CMSKPropertyList::OnNotify);
	Bind(WM_LBUTTONDOWN, (MSGFN)&CMSKPropertyList::OnLButtonDown);

	m_hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

	SetText(text);

	SetParent(parent);

	None();

	parent->Add(this);
}

CMSKPropertyList::~CMSKPropertyList()
{
}

HTREEITEM CMSKPropertyList::InsertItem(LPCWSTR text, HTREEITEM parent, HTREEITEM insertAfter,
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

	return (HTREEITEM)TreeView_InsertItem(m_hWndTreeView, &insertStruct);
}

void CMSKPropertyList::ExpandItem(HTREEITEM hItem)
{
	TreeView_Expand(m_hWndTreeView, hItem, TVE_EXPAND);
}

void CMSKPropertyList::EnsureVisibility(HTREEITEM item)
{
	TreeView_EnsureVisible(m_hWndTreeView, item);
}

void CMSKPropertyList::SetImageList(HIMAGELIST imageList) {
	SendMessage(m_hWndTreeView, TVM_SETIMAGELIST,
		(WPARAM)TVSIL_NORMAL, (LPARAM)imageList);
}

void CMSKPropertyList::DeleteItem(HTREEITEM item) {
	SendMessage(m_hWndTreeView, TVM_DELETEITEM, 0, (LPARAM)item);
}

LRESULT CMSKPropertyList::OnNotify(LPNMHDR nmHDR)
{
	LPNMTREEVIEW pnmtv;
	TVITEM item;
	TVITEM child;
	HTREEITEM hTreeItem;
	HWND hWnd;
	CMSKPropertyBox* pb;
	int bShow;
	TVHITTESTINFO info;
	POINT pt;

	switch (nmHDR->code) {
	case PBN_SELECTED:
		GetCursorPos(&pt);
		ScreenToClient(m_hWndTreeView, &pt);

		info.pt = pt;
		TreeView_HitTest(m_hWndTreeView, &info);

		if (info.flags == TVHT_ONITEMRIGHT) {
			TreeView_Select(m_hWndTreeView, info.hItem, TVGN_CARET);
		}
		break;
	case NM_CUSTOMDRAW:
		if (nmHDR->hwndFrom == m_hWndTreeView) {
			return OnCustomDraw(LPNMTVCUSTOMDRAW(nmHDR));
		}
		break;
	case TVN_ITEMEXPANDED:
		pnmtv = (LPNMTREEVIEW)nmHDR;

		switch (pnmtv->action) {
		case TVE_COLLAPSE:
			bShow = SW_HIDE;
			break;
		case TVE_EXPAND:
			bShow = SW_SHOW;
			break;
		}

		item = pnmtv->itemNew;
		hTreeItem = TreeView_GetNextItem(m_hWndTreeView, item.hItem, TVGN_CHILD);

		do {
			child.mask = TVIF_PARAM;
			child.hItem = hTreeItem;
			TreeView_GetItem(m_hWndTreeView, &child);

			pb = (CMSKPropertyBox*)child.lParam;
			hWnd = pb->GetWindowHandle();
			ShowWindow(hWnd, bShow);

			hTreeItem = TreeView_GetNextSibling(m_hWndTreeView, hTreeItem);
		} while (hTreeItem != NULL);

		return TRUE;
	}

	return FALSE;
}
void CMSKPropertyList::OnDestroy()
{
	DeleteFont(m_hFontNormal);
	DeleteBrush(m_hBrush);
	DeleteBrush(m_hFocusBrush);
	DeleteBitmap(m_hBitmap);
	DeleteFont(m_hFont);
}

int CMSKPropertyList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_crBgColor = RGB(245, 245, 245);
	m_hBrush = CreateSolidBrush(RGB(255, 255, 255));
	m_hFocusBrush = CreateSolidBrush(RGB(0, 122, 204));
	m_hPen = CreatePen(PS_SOLID, 1, RGB(210, 210, 225));

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

	m_hWndTreeView = CreateWindowEx(0, WC_TREEVIEW, L"", 
		WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_LINESATROOT,
		0, 0, 0, 0, m_hWnd, 0, hInst, 0);

	TreeView_SetBkColor(m_hWndTreeView, m_crBgColor);

	SetWindowSubclass(m_hWndTreeView, [](HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
		DWORD_PTR dwRefData)->LRESULT CALLBACK {
		switch (message) {
		case WM_LBUTTONDOWN:
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd, &pt);

			TVHITTESTINFO info;
			info.pt = pt;
			TreeView_HitTest(hWnd, &info);

			if (info.flags == TVHT_ONITEMRIGHT) {

				TreeView_Select(hWnd, info.hItem, TVGN_CARET);
				SetFocus(hWnd);
			}
			break;
		case WM_KILLFOCUS:
			//OutputDebugString(L"KillFocus\n");
			return 0;
		}

		return DefSubclassProc(hWnd, message, wParam, lParam);
	}, 0, (DWORD)this);

	SetFocus(m_hWndTreeView);

	return 0;
} 

void CMSKPropertyList::OnSize(UINT nType, int cx, int cy)
{
	//RECT rc;
	GetClientRect(m_hWnd, &m_rcClient);
	MoveWindow(m_hWndTreeView, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top, TRUE);
}

LRESULT CMSKPropertyList::OnCustomDraw(LPNMTVCUSTOMDRAW lpNMCustomDraw)
{
	HPEN hPenOld;
	HBRUSH hBrushOld;
	HFONT hFontOld;
	TVITEM item;
	static BOOL m_bFocus = FALSE;

	HWND hWnd = lpNMCustomDraw->nmcd.hdr.hwndFrom;
	HDC hdc = lpNMCustomDraw->nmcd.hdc;

	//RECT rc;
	GetClientRect(hWnd, &m_rcClient);
	switch (lpNMCustomDraw->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:
		// Clear to background color
		lpNMCustomDraw->clrText = m_crBgColor;
		lpNMCustomDraw->clrTextBk = m_crBgColor;

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
			TreeView_GetItemRect(hWnd, (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec, &m_rcItem, TRUE);
			m_rcItem.right = m_rcClient.right;

			item.cchTextMax = 255;
			item.mask = TVIF_TEXT;
			item.hItem = (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec;
			item.pszText = (LPWSTR)malloc(sizeof(wchar_t) * 255);
			TreeView_GetItem(hWnd, &item);

			hFontOld = (HFONT)SelectObject(hdc, m_hFont);
			hBrushOld = (HBRUSH)SelectObject(hdc, m_hBrush);
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, item.pszText, -1, &m_rcItem, DT_BOTTOM | DT_SINGLELINE | DT_WORD_ELLIPSIS);
			SelectObject(hdc, hBrushOld);
			SelectObject(hdc, hFontOld);
			break;
		case 1:
			TreeView_GetItemRect(hWnd, (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec, &m_rcItem, TRUE);
			m_rcItem.left -= 20;
			m_rcItem.right = m_rcClient.right / 2;
			m_rcItem.top += 1;

			item.cchTextMax = 255;
			item.mask = TVIF_TEXT | TVIF_PARAM;
			item.hItem = (HTREEITEM)lpNMCustomDraw->nmcd.dwItemSpec;
			item.pszText = (LPWSTR)malloc(sizeof(wchar_t) * 255);
			TreeView_GetItem(hWnd, &item);

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

			CMSKPropertyBox* pb = (CMSKPropertyBox*)lpNMCustomDraw->nmcd.lItemlParam;
			int x = m_rcItem.right + 1;
			int y = m_rcItem.top;
			int w = m_rcClient.right - x;
			int h = m_rcItem.bottom - m_rcItem.top;
			MoveWindow(pb->GetWindowHandle(), x, y, w, h, TRUE);
			break;
		}
		break;
	}

	return 0;
}

void CMSKPropertyList::OnLButtonDown(UINT nFlags, POINT point)
{

}
