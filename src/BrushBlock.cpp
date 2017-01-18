#include "StdAfx.h"
#include "BrushBlock.h"

BrushBlockUI::BrushBlockUI(void)
{
	point_w   = 4;
	bit_brush_= NULL;
}
BrushBlockUI::~BrushBlockUI(void)
{
	if( bit_brush_ )
	{
		delete bit_brush_;
		bit_brush_ = NULL;
	}
}
void BrushBlockUI::DoInit()
{
	SetBrushWidth(point_w);
}
long BrushBlockUI::GetBrushWidth()
{
	return point_w;
}
void BrushBlockUI::SetBrushWidth(long w)
{
	if( bit_brush_ )
		delete bit_brush_;
	//首先生成原图
	Gdiplus::Rect rcPos(0,0,GetFixedWidth(),GetFixedHeight());
	Gdiplus::Bitmap bit(rcPos.Width,rcPos.Height);
	{
		Gdiplus::Graphics	 g(&bit);
		Gdiplus::SolidBrush  br(Gdiplus::Color(255,250,250,250));
		g.FillRectangle(&br,0,0,GetFixedWidth(),GetFixedHeight());
	}
	Gdiplus::TextureBrush ttbr(&bit,rcPos);
	ttbr.TranslateTransform(0,0);

	bit_brush_ = new Gdiplus::Bitmap(GetFixedWidth(),GetFixedHeight());

	Gdiplus::Rect		rcCenter(0,0,w,w);
	rcCenter.Offset(GetFixedWidth()/2-w/2,GetFixedHeight()/2-w/2);

	Gdiplus::GraphicsPath path;
	path.AddEllipse(rcCenter);

	Gdiplus::Graphics	g(bit_brush_);	
	g.SetSmoothingMode(SmoothingModeHighQuality);
	g.FillPath(&ttbr,&path);
}
void BrushBlockUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName,_T("brush_width"))==0 )
		point_w = _ttol(pstrValue);
	else
		__super::SetAttribute(pstrName,pstrValue);
}
void BrushBlockUI::PaintStatusImage(HDC hDC)
{
	Gdiplus::Graphics g(hDC);
	Gdiplus::Rect	  rc(Rc2plus(GetPos()));
	__super::PaintStatusImage(hDC);
	if( bit_brush_ )
		g.DrawImage(bit_brush_,rc);
}