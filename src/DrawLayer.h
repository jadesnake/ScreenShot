#pragma once

#include "Layers.h"
#include "PaintPlusUI.h"

template<typename theX>
class DrawBase : public theX
{
public:
	DrawBase(long nW,long nH)
		:theX(nW,nH)
	{
		cur_pt_.x=0;
		cur_pt_.y=0;
		::SetRectEmpty(&paint_range_);
	}
	~DrawBase()
	{

	}
	void handleMsg(HWND win,DuiLib::TEventUI *ev,long wDef,long hDef)
	{
		if( ev->Type==DuiLib::UIEVENT_BUTTONDOWN )
		{
			cur_pt_	 = ev->ptMouse;
			return ;
		}
		if( ev->Type==DuiLib::UIEVENT_BUTTONUP && !::IsRectEmpty(&paint_range_) )
		{
			::InvalidateRect(win,&paint_range_,FALSE);
			return ;
		}
		if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE )
		{
			rcDraw_.X = cur_pt_.x;
			rcDraw_.Y = cur_pt_.y;
			if( ev->ptMouse.x > cur_pt_.x )
				rcDraw_.Width = ev->ptMouse.x - cur_pt_.x;
			else
			{
				rcDraw_.X = ev->ptMouse.x;
				rcDraw_.Width = cur_pt_.x - ev->ptMouse.x;
			}
			if( ev->ptMouse.y > cur_pt_.y )
				rcDraw_.Height= ev->ptMouse.y - cur_pt_.y;
			else
			{
				rcDraw_.Y     = ev->ptMouse.y;
				rcDraw_.Height= cur_pt_.y-ev->ptMouse.y;
			}
			if( rcDraw_.Height==0 && rcDraw_.Width==0 )
				return ;
			if( rcDraw_.Height==0 )
				rcDraw_.Height = wDef;
			else if( rcDraw_.Width==0 )
				rcDraw_.Width  = hDef;
			paint_range_ = Plus2Rc(rcDraw_);
			if( !::IsRectEmpty(&paint_range_) )
				::InvalidateRect(win,&paint_range_,FALSE);
			return ;
		}
	}
	void draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
	{
		if( pen )
			handleMsg(win,ev,pen->GetWidth(),pen->GetWidth());
		else if( ft )
			handleMsg(win,ev,ft->GetSize()*1.5,ft->GetSize()*1.5);
	}
protected:
	Gdiplus::Rect	rcDraw_;
	POINT			cur_pt_;
	RECT			paint_range_;	//»æÍ¼·¶Î§
};


class DrawPoints : public DrawBase<Layers::Layer>
{
public:
	DrawPoints(long nW,long nH);
	virtual ~DrawPoints();
	virtual void draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft);
protected:
	void	paint(Gdiplus::Graphics *g,RECT rc);
	void	checklimit(Gdiplus::Point &pt);
protected:
	DWORD			tmLast_;
	POINT			ptLast_;
	MOUSEMOVEPOINT	mp_out[64];
	MOUSEMOVEPOINT	mp_in;
	Gdiplus::Pen	*pen_;
	Gdiplus::Point	pt_track[64];
};

class DrawRect : public DrawBase<Layers::Layer>
{
public:
	DrawRect(long nW,long nH);
	virtual ~DrawRect();
	virtual void draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft);
protected:

};

class DrawEllipse : public DrawBase<Layers::Layer>
{
public:
	DrawEllipse(long nW,long nH);
	virtual ~DrawEllipse();
	virtual void draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft);
protected:

};

class DrawArrow : public DrawBase<Layers::Layer>
{
public:
	DrawArrow(long nW,long nH);
	virtual ~DrawArrow();
	virtual void draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft);
protected:
	long pen_limit_w_;
};

class DrawGauss : public DrawBase<Layers::Layer>
{
public:
	DrawGauss(long nW,long nH);
	virtual ~DrawGauss();
	virtual void draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft);
protected:
};

class DrawTexture : public DrawBase<Layers::Layer>
{
public:
	DrawTexture(long nW,long nH);
	virtual ~DrawTexture();
	virtual void draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft);
	void	DoPaint(HDC dc,RECT rc);
	AutoRichDocUI*	GetRE(DuiLib::CPaintManagerUI *pm=NULL);
protected:
	void	paint(Gdiplus::Graphics *g,RECT rc);
	bool	InputChange(void*);
	bool	KillFocus(void*);
	void	paintText(RECT rc);
protected:
	bool				bStartEdit_;
	AutoRichDocUI		*re_;
};