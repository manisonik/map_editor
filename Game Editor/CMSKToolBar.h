#pragma once
#include "CMSKWindow.h"

class CMSKToolBar : public CMSKWindow
{
private:
	HIMAGELIST m_hImageList;
	SIZE m_nMaxSize;

public:
	CMSKToolBar(LPCWSTR text, CMSKWindow* parent);
	~CMSKToolBar();
};

