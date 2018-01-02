#pragma once
#include "CMSKWindow.h"

class CMSKRebar :
	public CMSKWindow
{
private:
	REBARBANDINFO m_rbBand;

public:
	CMSKRebar(CMSKWindow* parent);
	~CMSKRebar();

	void Add(CMSKWindow* bar);
};

