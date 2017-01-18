#pragma once

#include "PaintPlusUI.h"
class BrushBlockUI : public OptionPlusUI
{
public:
	BrushBlockUI(void);
	~BrushBlockUI(void);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetBrushWidth(long w);
	long GetBrushWidth();
	LPCTSTR GetClass() const
	{
		return _T("BrushBlock");
	}
protected:
	void DoInit();
	void PaintStatusImage(HDC hDC);
protected:
	long			 point_w;
	Gdiplus::Bitmap	 *bit_brush_;
};
