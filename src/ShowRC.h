#pragma once

class ShowRC : public DuiLib::CLabelUI
{
public:
	ShowRC(void);
	virtual ~ShowRC(void);
protected:
	void PaintBkColor(HDC hDC);
	void SetPos(RECT rc);
};
