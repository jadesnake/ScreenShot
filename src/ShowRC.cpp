#include "StdAfx.h"
#include "ShowRC.h"
#include "Const.h"

ShowRC::ShowRC(void)
{
}
ShowRC::~ShowRC(void)
{
}
void ShowRC::PaintBkColor(HDC hDC)
{
	Gdiplus::Graphics   g(hDC);
	Gdiplus::Color		cr;
	cr.SetValue( GetBkColor() );
	Gdiplus::SolidBrush br(cr);
	RECT rc = GetPos();
	if( !::IsRectEmpty(&rc) )
		g.FillRectangle(&br,Rc2plus(GetPos()));
}
void ShowRC::SetPos(RECT rc)
{
	RECT rcPos=rc;
	CAtlString txt;
	SIZE szIn={rc.right-rc.left,rc.bottom-rc.top};
	txt.Format(_T(" %d x %d "),szIn.cx,szIn.cy);
	SetText(txt);
	rcPos.top   = rc.top - GetFixedHeight();
	rcPos.bottom= rc.top;
	if( rcPos.top < 0 )
	{
		rcPos.top   = rc.top;
		rcPos.bottom= rc.top + GetFixedHeight();
	}
	rcPos.right = rc.left + GetFixedWidth();
	__super::SetPos( rcPos );
}