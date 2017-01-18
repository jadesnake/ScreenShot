#pragma once

#include "PaintPlusUI.h"
class ColorBlockUI : public OptionPlusUI
{
public:
	ColorBlockUI(void);
	~ColorBlockUI(void);
	LPCTSTR GetClass() const
	{
		return _T("ColorBlock");
	}
};
