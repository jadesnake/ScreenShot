#pragma once

#include "PaintPlusUI.h"
class ToolsbarUI : public PaintPlusUI<DuiLib::CHorizontalLayoutUI>
				 , public DuiLib::INotifyUI
{
public:
	ToolsbarUI(void);
	~ToolsbarUI(void);
	void Init();
	void SetVisible(bool bVisible = true);
	void SetPos(RECT rc);
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void Event(DuiLib::TEventUI& event);
public:
	Gdiplus::Pen		sel_pen_;	//选择的画笔
	capture::FontInfo	sel_ft_;	//选择的字体
protected:
	void Notify(DuiLib::TNotifyUI& msg);
	void ShowTools(const CAtlString& showpart);
};