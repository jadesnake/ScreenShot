#include "StdAfx.h"
#include "Layers.h"
#include <algorithm>
#include "DrawLayer.h"


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)   
{   
	UINT  num = 0;  
	UINT  size = 0; 
	ImageCodecInfo* pImageCodecInfo = NULL;    
	GetImageEncodersSize(&num, &size);   
	if(size == 0)   
		return -1; 
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));   
	if(pImageCodecInfo == NULL)   
		return -1; 
	GetImageEncoders(num, size, pImageCodecInfo);
	if(NULL!=pImageCodecInfo)
	{
		for(UINT j = 0; j < num; ++j)   
		{   
			if(NULL!=pImageCodecInfo[j].MimeType && wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )   
			{   
				*pClsid = pImageCodecInfo[j].Clsid;   
				free(pImageCodecInfo);   
				return j; 
			}
		}
		free(pImageCodecInfo);  
	}
	return -1; 
} 

Layers::Layer::Layer(long nW,long nH)
	:bVisible_(true),layers_(NULL)
{
	init(nW,nH);
}
Layers::Layer::Layer(long nW,long nH,Layers *layers)
	:bVisible_(true),layers_(layers)
{
	init(nW,nH);
}
void Layers::Layer::init(long nW,long nH)
{
	rclimit_.left = 0;
	rclimit_.top  = 0;
	rclimit_.right = 99999;
	rclimit_.bottom= 99999;

	bitmap_  = NULL;
	path_	 = NULL;
	pt_.x	 = 0;
	pt_.y	 = 0;
	pen_	 = NULL;
	graphic_ = NULL;
	//
	bitmap_ = new Gdiplus::Bitmap(nW,nH);
	graphic_= new Gdiplus::Graphics(bitmap_); 
	Gdiplus::SolidBrush b(Gdiplus::Color(0,0,0,0));
	graphic_->FillRectangle(&b,0,0,nW,nH);
}
void Layers::Layer::limitRc(const RECT &rc )
{
	rclimit_ = rc;
}
void Layers::Layer::clear()
{
	graphic()->Clear(Gdiplus::Color(0,0,0,0));
}
void	Layers::Layer::font(const capture::FontInfo &ftInfo)
{
	ftInfo_ = ftInfo;
}
void	Layers::Layer::paint(Gdiplus::Graphics *g,RECT rc)
{
	if( bVisible_ )
	{
		if( pen_ && path_ )
			graphic()->DrawPath(pen_,path_);
		g->DrawImage(bitmap_,0,0);
	}
}
Gdiplus::Graphics*		Layers::Layer::graphic()
{
	if( graphic_ )
		return graphic_;
	graphic_ = new Gdiplus::Graphics(bitmap_);
	graphic_->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	return graphic_;
}
Gdiplus::GraphicsPath*	Layers::Layer::path()
{
	if( path_ )
		return path_;
	path_ = new Gdiplus::GraphicsPath;
	return path_;
}
Layers::Layer::~Layer()
{
	if( graphic_ )
	{
		delete graphic_;
		graphic_ = NULL;
	}
	if( bitmap_ )
	{
		delete bitmap_;
		bitmap_ = NULL;
	}
	if( path_ )
	{
		delete path_;
		path_ = NULL;
	}
	if( pen_ )
	{
		delete pen_;
		pen_ = NULL;
	}
}

Layers::Layers(void)
	:cur_(NULL)
	,cur_build_(NULL)
{

}
Layers::~Layers(void)
{
	clear();
	if( cur_build_ )
	{
		delete cur_build_;
		cur_build_=NULL;
	}
}
Layers::Layer*	Layers::push(capture::DRAW_CAP cap,Gdiplus::Bitmap *draw,RECT rcSel)
{
	//建立和屏幕相等的位图，因为不需要做坐标映射那就浪费点内存
	Layer *cur=NULL;
	buildFull(draw);
	switch(cap)
	{
	case capture::Rectangle:
		cur = new DrawRect(rcSel.right,rcSel.bottom);
		break;
	case capture::Ellipse:
		cur = new DrawEllipse(rcSel.right,rcSel.bottom);
		break;
	case capture::Arrow:
		cur = new DrawArrow(rcSel.right,rcSel.bottom);
		break;
	case capture::Brush:
		cur = new DrawPoints(rcSel.right,rcSel.bottom);
		break;
	case capture::Gauss:
		cur = new DrawMosaic(rcSel.right,rcSel.bottom,this);
		break;
	case capture::Text:
		cur = new DrawTexture(rcSel.right,rcSel.bottom);
		break;
	default:
		cur = new Layer(rcSel.right,rcSel.bottom);
	}
	cur_ = cur;
	all_.push_back(cur_);
	return cur_;
}
Layers::Layer*	Layers::current()
{
	return cur_;
}
void	Layers::paint(Gdiplus::Graphics *g,RECT rc)
{
	for(size_t nI=0;nI < all_.size();nI++)
	{
		Layer *bit = all_[nI];
		bit->paint(g,rc);
	}
}
void	Layers::clear()
{
	lst_redo_.clear();
	for(size_t nI=0;nI < all_.size();nI++)
	{
		Layer *bit = all_[nI];
		delete bit;
	}
	all_.clear();
}
void	Layers::move(long nX,long nY)
{
	for(size_t nI=0;nI < all_.size();nI++)
	{
		Layer *bit = all_[nI];
		bit->pt_.x = nX;
		bit->pt_.y = nY;
	}
}
long	Layers::countLayerVisible()
{
	long nRet = 0;
	for(size_t nI=0;nI < all_.size();nI++)
	{
		if( all_[nI]->bVisible_ )
			nRet++;
	}
	return nRet;
}
Layers::Layer*	Layers::getLayerAtLast()
{
	Layers::Layer* pRet = NULL;
	for(size_t nI=all_.size()-1;true;nI--)
	{
		if( all_[nI]->bVisible_ )
		{
			pRet = all_[nI];
			break;
		}
		if( nI==0 )
			break;
	}
	return pRet;
}
bool	Layers::undo()
{
	if( 0==all_.size() )
		return false;
	Layer *l = getLayerAtLast();
	if( l )
	{
		l->bVisible_ = false;
		lst_redo_.push_back(l);
	}
	//修改当前绘图层
	if( 0==countLayerVisible() )
		cur_ = NULL;
	else
	{
		cur_ = getLayerAtLast();
	}
	return true;
}
bool	Layers::redo()
{
	if( 0==lst_redo_.size() )
		return false;
	Layer *l = lst_redo_.back();
	l->bVisible_ = true;
	lst_redo_.pop_back();
	cur_ = l;
	return true;
}
Gdiplus::Bitmap* Layers::getBuild()
{
	return cur_build_;
}
Gdiplus::Bitmap* Layers::buildFull(Gdiplus::Bitmap *draw)
{
	if( cur_build_ )
		delete cur_build_;
	Gdiplus::Graphics gImg(draw);
	RECT	  rcImg={0,0,draw->GetWidth(),draw->GetHeight()};
	gImg.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	paint(&gImg,rcImg);

	cur_build_ = new Gdiplus::Bitmap(draw->GetWidth(),draw->GetHeight());
	Gdiplus::Graphics g(cur_build_);
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.DrawImage(draw,0,0);
	return cur_build_;
}
Gdiplus::Bitmap* Layers::buildClip(Gdiplus::Bitmap *draw,RECT rc)
{
	if( cur_build_ )
		delete cur_build_;
	Gdiplus::Graphics gImg(draw);
	gImg.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	paint(&gImg,rc);
	
	cur_build_ = new Gdiplus::Bitmap(rc.right-rc.left,rc.bottom-rc.top);
	Gdiplus::Graphics g(cur_build_);
	Gdiplus::RectF	  rcDst(0,0,cur_build_->GetWidth(),cur_build_->GetHeight());
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.DrawImage(draw,rcDst,rc.left,rc.top,RcWidth(rc),RcHeight(rc),Gdiplus::UnitPixel);
	return cur_build_;
}
bool	Layers::save(Gdiplus::Bitmap *draw,RECT rc,const CAtlString &str)
{
	CLSID imgClsid;
	if( -1 != str.Find(_T(".png")) )
		GetEncoderClsid(L"image/png", &imgClsid);
	else if( -1 != str.Find(_T(".jpg")) )
		GetEncoderClsid(L"image/jpeg", &imgClsid);
	else if( -1 != str.Find(_T(".bmp")) )
		GetEncoderClsid(L"image/bmp", &imgClsid);
	else 
		return false;
	buildClip(draw,rc);
	return (Gdiplus::Ok==cur_build_->Save(str,&imgClsid,NULL)?true:false);
}