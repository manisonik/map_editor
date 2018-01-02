#pragma once

#include "resource.h"
#include "CMSKCmd.h"

// Application Defined Messages
#define MTC_INSERTTAB WM_USER + 100

class CMSKWindow
{
protected:
	HWND m_hWnd;
	LPCWSTR m_strText;
	DWORD m_nFlags;
	HINSTANCE hInst;
	RECT m_AdjustedClientRect;
	HBRUSH m_hbrBackground;
	RECT m_rcBorder;
	BOOL m_bResizing = false;
	BOOL m_bSubClass;
	int m_nXPos, m_nYPos, m_nWidth, m_nHeight;
	std::vector<CMSKWindow*> m_pChildWnds;

	template<class T>
	using MSGCB = std::variant<
		void (T::*)(void),
		void (T::*)(UINT_PTR),
		void (T::*)(LONG_PTR),
		void (T::*)(UINT_PTR, LONG_PTR),
		void (T::*)(UINT_PTR, WORD, WORD),
		void (T::*)(WORD, WORD, LONG_PTR),
		void (T::*)(WORD, WORD, WORD, WORD),
		LRESULT(T::*)(void),
		LRESULT(T::*)(UINT_PTR),
		LRESULT(T::*)(LONG_PTR),
		LRESULT(T::*)(UINT_PTR, LONG_PTR),
		LRESULT(T::*)(UINT_PTR, WORD, WORD),
		LRESULT(T::*)(WORD, WORD, LONG_PTR),
		LRESULT(T::*)(WORD, WORD, WORD, WORD),
		void (T::*)(UINT, int, int),
		int (T::*)(LPCREATESTRUCT),
		bool (T::*)(HDC),
		bool (T::*)(int, NMHDR, LRESULT*),
		void (T::*)(int, LPDRAWITEMSTRUCT),
		void (T::*)(BOOL, LPARAM),
		void (T::*)(HRGN)
	>;

	typedef void (CMSKWindow::* MSGFN)(void);

	typedef struct MSGINFO {
		MSGFN msgCb;
		UINT type;
	} MSGINFO;

	std::unordered_map<UINT, MSGINFO> m_CBMsg;
	std::unordered_map<UINT, MSGINFO> m_CBNotify;
private:
	CMSKWindow* m_pParent;

public:
	CMSKWindow();
	CMSKWindow(CMSKWindow* parent);
	CMSKWindow(LPCWSTR title, int x, int y, int width, int height);
	~CMSKWindow();

	static LRESULT CALLBACK SubClassRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK RouterProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT CALLBACK SubClassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	void SetText(LPCWSTR text);
	void SetParent(CMSKWindow* parent);
	void Update();
	void Top();
	void Left();
	void Right();
	void Bottom();
	void Fill();
	void SetBorder(int left, int top, int right, int bottom);
	void SetBkColor(COLORREF color);
	void None();
	int GetWidth();
	int GetHeight();
	int GetDockState();
	HWND GetWindowHandle();
	HINSTANCE GetInstanceHandle();
	RECT GetAdjustedRect();
	LPCWSTR GetText();
	CMSKWindow* GetParent();
	std::wstring GetLastErrorStr();

	virtual void Add(CMSKWindow* window);
	virtual void Remove(CMSKWindow* window);
protected:
	void Bind(UINT msg, MSGFN msgCb);
	BOOL PreCreateWindow(CREATESTRUCT &cs);
	void OnContextMenu(HWND hWnd, int x, int y);
	void OnMouseLeave();
	void OnNcPaint(HRGN hRgn);
	void OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
	bool OnCommand(WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	void OnPaint();
	void OnActivate(int nState, bool bActivated, HWND hWnd);
	bool OnEraseBkgnd(HDC hdc);
	void OnLButtonDown(UINT nFlags, POINT point);
	void OnLButtonUp(UINT nFlags, POINT point);
	void OnMouseMove(UINT nFlags, POINT point);
	void OnMove(int x, int y);
	BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	LRESULT OnNotify(LPNMHDR nmHDR);
	void OnNcDestroy();
	BOOL OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
	void OnMoving(UINT nSide, LPRECT lpRect);
	BOOL OnNcActivate(BOOL bActive);
	BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnNcHitTest(POINT point);
	void OnNcLButtonDblClk(UINT nHitTest, POINT point);
};

