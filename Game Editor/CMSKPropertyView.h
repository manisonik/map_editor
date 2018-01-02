#pragma once
#include "CMSKWindow.h"
#include "CMSKPropertyBox.h"

class CMSKPropertyView :
	public CMSKWindow
{
private:
	int m_nIndent;
	int m_nItemHeight;
	HBRUSH m_hBrush;
	HBRUSH m_hSelectedBrush;
	HFONT m_hFont;
	HFONT m_hFontBold;
	HICON m_hIconPlus;
	HICON m_hIconMinus;
	HPVITEM m_hItemSelected;
	std::vector<HPVITEM> nodes;

public:
	CMSKPropertyView(LPCWSTR text, CMSKWindow * parent);
	~CMSKPropertyView();

	HPVITEM AddCategory(LPCWSTR text);
	HPVITEM AddProperty(LPCWSTR text, UINT type, HPVITEM category);
	void AddValue(float value);
	void AddValue(LPCWSTR value);
	void AddValue(int value);

protected:
	LRESULT OnNotify(LPNMHDR nmHDR);
	void OnLButtonDown(UINT nFlags, POINT point);
	void OnDestroy();
	void OnPaint();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
};

