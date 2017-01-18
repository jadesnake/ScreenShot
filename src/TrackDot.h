#pragma once

#include "PaintPlusUI.h"
class TrackDot : public OptionPlusUI
{
public:
	TrackDot(void);
	virtual ~TrackDot(void);
protected:
	void DoEvent(DuiLib::TEventUI& event);
private:
	POINT			cur_pt_;
};