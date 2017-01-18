#include "stdafx.h"
#include "DrawLayer.h"
#include "GdiAlpha.h"
int nVirtualWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN) ;
int nVirtualHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) ;
int nVirtualLeft = GetSystemMetrics(SM_XVIRTUALSCREEN) ;
int nVirtualTop = GetSystemMetrics(SM_YVIRTUALSCREEN) ;


DrawEllipse::DrawEllipse(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
{

}
DrawEllipse::~DrawEllipse()
{

}
void DrawEllipse::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE && bitmap_ )
	{
		graphic()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		clear();
		graphic()->DrawEllipse(pen,rcDraw_);
		paint_range_ = Plus2Rc(rcDraw_);
	}
}
/*-------------------------------------------------------------------------*/
DrawRect::DrawRect(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
{

}
DrawRect::~DrawRect()
{

}
void DrawRect::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE && bitmap_ )
	{
		graphic()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		clear();
		graphic()->DrawRectangle(pen,rcDraw_);
	}
}
/*-------------------------------------------------------------------------*/
DrawPoints::DrawPoints(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
	, tmLast_(0)
{
	pen_	= NULL;
	ZeroMemory(&mp_in, sizeof(mp_in));
	ZeroMemory(mp_out, sizeof(mp_out));
}
DrawPoints::~DrawPoints()
{

}
void DrawPoints::paint(Gdiplus::Graphics *g,RECT rc)
{
	__super::paint(g,rc);
}
void DrawPoints::checklimit(Gdiplus::Point &pt)
{
	if( pt.X < rclimit_.left )
		pt.X = rclimit_.left;
	if( pt.X > rclimit_.right )
		pt.X = rclimit_.right;

	if( pt.Y < rclimit_.top )
		pt.Y = rclimit_.top;
	if( pt.Y > rclimit_.bottom )
		pt.Y = rclimit_.bottom;
}
void DrawPoints::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	Gdiplus::Point pt(ev->ptMouse.x,ev->ptMouse.y);
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_BUTTONDOWN )
	{
		tmLast_ = ::GetMessageTime();
		ptLast_.x = ev->ptMouse.x;
		ptLast_.y = ev->ptMouse.y;
		::ClientToScreen(win,&ptLast_);

		paint_range_.left  = pt.X;
		paint_range_.top   = pt.Y;
		paint_range_.right = pt.X + pen->GetWidth();
		paint_range_.bottom= pt.Y + pen->GetWidth();
		pen_ = pen;
		pen_->SetLineJoin(Gdiplus::LineJoinRound);
		pen_->SetEndCap(Gdiplus::LineCapRound);
		pt_track[0].X = ev->ptMouse.x;
		pt_track[0].Y = ev->ptMouse.y;
		graphic()->DrawLine(pen,ev->ptMouse.x,ev->ptMouse.y,pt_track[0].X,pt_track[0].Y);
		::InvalidateRect(win,&paint_range_,FALSE);
	}
	else if( ev->Type==DuiLib::UIEVENT_BUTTONUP )
	{
		::InvalidateRect(win,&rclimit_,FALSE);
	}
	else if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE )
	{
		int mode = GMMP_USE_DISPLAY_POINTS;
		POINT ptScreen = {ev->ptMouse.x,ev->ptMouse.y};
		::ClientToScreen(win,&ptScreen);
		mp_in.x		= ptScreen.x;
		mp_in.y		= ptScreen.y;
		mp_in.time	= ::GetMessageTime();
		int cpt = ::GetMouseMovePointsEx(sizeof(MOUSEMOVEPOINT),&mp_in,mp_out,64,mode);
		int nIn = 0;
		if( cpt==-1 )
		{
			cpt = 1;
			if( ev->ptMouse.x==ptLast_.x && ev->ptMouse.y==ptLast_.y )
			{
				pt_track[0].X = ev->ptMouse.x;
				pt_track[0].Y = ev->ptMouse.y;
				pt_track[0].X += pen_->GetWidth();
				pt_track[0].Y += pen_->GetWidth();
				checklimit(pt_track[0]);
				graphic()->DrawLine(pen_,ev->ptMouse.x,ev->ptMouse.y,pt_track[0].X,pt_track[0].Y);
			}
			else
			{
				pt_track[0].X = ptLast_.x;
				pt_track[0].Y = ptLast_.y;
				pt_track[1].X = ev->ptMouse.x;
				pt_track[1].Y = ev->ptMouse.y;
				graphic()->DrawCurve(pen,pt_track,2);
			}
			tmLast_ = mp_in.time;
			ptLast_ = ev->ptMouse;
		}
		else
		{
			ptScreen = ptLast_;
			::ClientToScreen(win,&ptScreen);
			for(nIn = 0;nIn < cpt;nIn++) 
			{
				if (mp_out[nIn].time < tmLast_)
					break;
				if (mp_out[nIn].time == tmLast_	 &&
					mp_out[nIn].x == ptScreen.x &&
					mp_out[nIn].y == ptScreen.y )
					break;
			}
			nIn+=2;
			cpt = nIn;
			while (--nIn >= 0) 
			{
				ptScreen.x = mp_out[nIn].x;
				ptScreen.y = mp_out[nIn].y;
				::ScreenToClient(win,&ptScreen);

				pt_track[nIn].X = ptScreen.x;
				pt_track[nIn].Y = ptScreen.y;

				checklimit(pt_track[nIn]);
				ptLast_ = ptScreen;
			}
			tmLast_ = mp_in.time;
			graphic()->DrawCurve(pen_,pt_track,cpt);
		}
	}
}
/*-------------------------------------------------------------------------*/
DrawArrow::DrawArrow(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
{
	pen_limit_w_ = 10;
}
DrawArrow::~DrawArrow()
{

}
void DrawArrow::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	Gdiplus::Point pt(ev->ptMouse.x,ev->ptMouse.y);
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_BUTTONDOWN )
	{
		pen_limit_w_ = pen->GetWidth();
		pen->SetStartCap(Gdiplus::LineCapArrowAnchor);
	}
	if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE )
	{
		this->clear();
		long nX = abs(ev->ptMouse.x-cur_pt_.x);
		long nY = abs(ev->ptMouse.y-cur_pt_.y);
		graphic()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		graphic()->DrawLine(pen,pt.X,pt.Y,cur_pt_.x,cur_pt_.y);
	}
}
/*-------------------------------------------------------------------------*/
DrawGauss::DrawGauss(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
{

}
DrawGauss::~DrawGauss()
{

}
void DrawGauss::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE )
	{

	}
}
/*-------------------------------------------------------------------------*/
DrawTexture::DrawTexture(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
	, re_(NULL)
	, bStartEdit_(false)
{

}
DrawTexture::~DrawTexture()
{
	
}
void DrawTexture::paintText(RECT rc)
{
	if( re_ )
	{
		HDC hdc = graphic()->GetHDC();
		re_->OnDraw(hdc,re_->GetPos());
		graphic()->ReleaseHDC(hdc);
	}
}
void DrawTexture::paint(Gdiplus::Graphics *g,RECT rc)
{
	if( bStartEdit_ )
	{
		paintText(rc);
	}
	else if( re_ && bStartEdit_!=re_->IsVisible() )
	{
		re_->SetVisible(false);
	}
	Layers::Layer::paint(g,rc);
}
AutoRichDocUI*	DrawTexture::GetRE(DuiLib::CPaintManagerUI *pm)
{
	if( re_==NULL&&pm )
	{
		re_ =  new AutoRichDocUI;
		DuiLib::CContainerUI *root = static_cast<DuiLib::CContainerUI*>(pm->GetRoot());
		root->Add(re_);

		DuiLib::TFontInfo *fInfo = pm->GetFontInfo(re_->GetFont());

		re_->SetAttribute(_T("norborder"),_T("2"));
		re_->SetAttribute(_T("norbordercolor"),_T("#ffabcdef"));
		re_->SetAttribute(_T("bkcolor"),_T("#ffffffff"));
		re_->AddPaintState(AutoRichDocUI::NorBorder);
		re_->SetLimitRc( rclimit_ );
		re_->SetExternRc(DuiLib::CRect(0,0,ftInfo_.size+5,ftInfo_.size+5));
		re_->SetFont(ftInfo_);
		re_->UpdateFont();
		re_->OnChange += DuiLib::MakeDelegate(this,&DrawTexture::InputChange);
		re_->OnKillFocus += DuiLib::MakeDelegate(this,&DrawTexture::KillFocus);
	}
	return re_;
}
bool	DrawTexture::InputChange(void*)
{
	clear();
	return false;
}
bool	DrawTexture::KillFocus(void*)
{
	clear();
	bStartEdit_ = false;
	//È¡Ïû±ß¿ò
	if( re_ )
	{
		re_->SetAttribute(_T("norborder"),_T("0"));

		Gdiplus::RectF pos;
		CAtlString	strTxt = re_->GetText().GetData();
		re_->SetAttribute(_T("norborder"),_T("0"));
		strTxt.Replace('\r','\n');
		pos.X = re_->GetPos().left;
		pos.Y = re_->GetPos().top;
		pos.Width = RcWidth(re_->GetPos()) + 2;
		pos.Height= RcHeight(re_->GetPos())+ 2;
		
		Gdiplus::FontFamily	ff(ftInfo_.name);
		Gdiplus::Font			ft(&ff,ftInfo_.size);
		Gdiplus::SolidBrush   sb(ftInfo_.color);
		graphic()->SetTextRenderingHint(TextRenderingHint(TextRenderingHintAntiAlias));
		graphic()->DrawString(strTxt, -1, &ft, pos, NULL, &sb);

		DuiLib::CContainerUI *root = static_cast<DuiLib::CContainerUI*>(re_->GetManager()->GetRoot());
		root->Remove(re_);
		re_ = NULL;
	}
	return false;
}
void DrawTexture::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	__super::handleMsg(win,ev,ftInfo_.size,ftInfo_.size);
	if( ev->Type==DuiLib::UIEVENT_BUTTONDOWN && ev->pSender )
	{
		if( bStartEdit_==false )
		{
			if( re_==NULL )
			{
				re_ = GetRE(ev->pSender->GetManager());
				Gdiplus::Color clr;
				pen->GetColor(&clr);
				re_->SetTextColor(clr.GetValue());
			}
			else
			{
				re_->Clear();
				re_->SetVisible(true);
			}
			RECT rcPos={ev->ptMouse.x,ev->ptMouse.y,ev->ptMouse.x+ftInfo_.size,ev->ptMouse.y+ftInfo_.size};
			rcPos.left  -= re_->GetExternRc().left;
			rcPos.top   -= re_->GetExternRc().top;
			rcPos.right  += re_->GetExternRc().right;
			rcPos.bottom += re_->GetExternRc().bottom;
			if( rcPos.bottom > rclimit_.bottom )
			{
				long nOff = rcPos.bottom - rclimit_.bottom;
				::OffsetRect(&rcPos,0,-nOff);
			}
			if( rcPos.right > rclimit_.right )
			{
				long nOff = rcPos.right - rclimit_.right;
				::OffsetRect(&rcPos,-nOff,0);
			}
			re_->SetPos(rcPos);
			re_->SetFocus();
			bStartEdit_ = true;
			::InvalidateRect(win,&paint_range_,FALSE);
		}
	}
}
