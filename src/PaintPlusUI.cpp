#include "stdafx.h"
#include "PaintPlusUI.h"

DWORD XmlColorStr2Num(LPCTSTR v)
{
	while( *v > _T('\0') && *v <= _T(' ') )
		v = ::CharNext(v);
	if( *v == _T('#')) v = ::CharNext(v);
	LPTSTR pstr = NULL;
	return _tcstoul(v, &pstr, 16);
}

ButtonPlusUI::ButtonPlusUI()
{
	m_dwHotBkColor=0;
	fore_img=NULL;
	hot_img =NULL;
	dis_img =NULL;
	push_img=NULL;
	nor_img =NULL;
	focus_img=NULL;
	hot_fore_img =NULL;
}
ButtonPlusUI::~ButtonPlusUI()
{
	if(fore_img)
	{
		delete fore_img;
		fore_img=NULL;
	}
	if(hot_img)
	{
		delete hot_img;
		hot_img=NULL;
	}
	if(dis_img )
	{
		delete dis_img;
		dis_img=NULL;
	}
	if(push_img)
	{
		delete push_img;
		push_img=NULL;
	}
	if(nor_img)
	{
		delete nor_img;
		nor_img=NULL;
	}
	if(focus_img)
	{
		delete focus_img;
		focus_img=NULL;
	}
	if(hot_fore_img)
	{
		delete hot_fore_img;
		hot_fore_img=NULL;
	}
}
void ButtonPlusUI::SetForeImage(LPCTSTR pstrValue)
{
	if( fore_img )
		delete fore_img;
	fore_img = LoadImage(pstrValue);
	__super::SetForeImage(pstrValue);
}
void ButtonPlusUI::SetHotImage(LPCTSTR pstrValue)
{
	if( hot_img )
		delete hot_img;
	hot_img = LoadImage(pstrValue);
	__super::SetHotImage(pstrValue);
}
void ButtonPlusUI::SetNormalImage(LPCTSTR pStrImage)
{
	if( nor_img )
		delete nor_img;
	nor_img = LoadImage(pStrImage);
	__super::SetNormalImage(pStrImage);
}
void ButtonPlusUI::SetPushedImage(LPCTSTR pStrImage)
{
	if(push_img)
		delete push_img;
	push_img = LoadImage(pStrImage);
	__super::SetPushedImage(pStrImage);
}
void ButtonPlusUI::SetDisabledImage(LPCTSTR pStrImage)
{
	if(dis_img)
		delete dis_img;
	dis_img=LoadImage(pStrImage);
	__super::SetDisabledImage(pStrImage);
}
void ButtonPlusUI::SetFocusedImage(LPCTSTR pStrImage)
{
	if(focus_img)
		delete focus_img;
	focus_img=LoadImage(pStrImage);
	__super::SetFocusedImage(pStrImage);
}
void ButtonPlusUI::SetHotForeImage(LPCTSTR pStrImage)
{
	if(hot_fore_img)
		delete hot_fore_img;
	hot_fore_img = LoadImage(pStrImage);
	__super::SetHotForeImage(pStrImage);
}
void ButtonPlusUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("normalimage")) == 0 ) 
		SetNormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) 
		SetHotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("pushedimage")) == 0 )
		SetPushedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("focusedimage")) == 0 )
		SetFocusedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) 
		SetDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, _T("foreimage")) == 0 )
		SetForeImage(pstrValue);
	else if( _tcscmp(pstrName, _T("hotforeimage")) == 0 ) 
		SetHotForeImage(pstrValue);
	else 
		__super::SetAttribute(pstrName,pstrValue);
}
void ButtonPlusUI::PaintStatusImage(HDC hDC)
{
	Gdiplus::Graphics g(hDC);
	Gdiplus::Rect	  rc(Rc2plus(GetPos()));
	if( IsFocused() )
		m_uButtonState |= UISTATE_FOCUSED;
	else
		m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() )
		m_uButtonState |= UISTATE_DISABLED;
	else
		m_uButtonState &= ~ UISTATE_DISABLED;
	if( (m_uButtonState & UISTATE_DISABLED) != 0 ) 
	{
		if( dis_img )
		{
			if( Gdiplus::Ok != g.DrawImage(dis_img,rc) )
			{
				delete dis_img;
				dis_img=NULL;
			}
			else 
				goto Label_ForeImage;
		}
	}
	else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) 
	{
		if(push_img)
		{
			if( Gdiplus::Ok!=g.DrawImage(push_img,rc) )
			{
				delete push_img;
				push_img=NULL;
			}
		}
		else
			goto Label_ForeImage;
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 )
	{
		if(m_dwHotBkColor != 0) 
		{
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
			return;
		}
		if( hot_img )
		{
			if( Gdiplus::Ok!=g.DrawImage(hot_img,rc) )
			{
				delete hot_img;
				hot_img=NULL;
			}
		}
		if(hot_fore_img)
		{
			if(Gdiplus::Ok!=g.DrawImage(hot_fore_img,rc))
			{
				delete hot_fore_img;
				hot_fore_img=NULL;
			}
		}
		else
			goto Label_ForeImage;
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) 
	{
		if( focus_img )
		{
			if( Gdiplus::Ok!=g.DrawImage(focus_img,rc) )
			{
				delete focus_img;
				focus_img=NULL;
			}
			else goto Label_ForeImage;
		}
	}
	if(nor_img)
	{
		if( Gdiplus::Ok!=g.DrawImage(nor_img,rc) )
		{
			delete nor_img;
			nor_img=NULL;
		}
		else
			goto Label_ForeImage;

	}
	if(fore_img)
		goto Label_ForeImage;
	return;
Label_ForeImage:
	if(fore_img) 
	{
		if( Gdiplus::Ok!=g.DrawImage(fore_img,rc) )
		{
			delete fore_img;
			fore_img=NULL;
		}
	}
}
/*-------------------------------------------------------------------------*/
OptionPlusUI::OptionPlusUI()
{
	m_dwSelectedBkColor = 0;
	fore_img	= NULL;
	selected_img= NULL;
	hot_img		= NULL;
	sel_hot_img = NULL;
	nor_img     = NULL;
}
OptionPlusUI::~OptionPlusUI()
{
	if( fore_img )
	{
		delete fore_img;
		fore_img=NULL;
	}
	if( selected_img )
	{
		delete selected_img;
		selected_img=NULL;
	}
	if( hot_img	)
	{
		delete hot_img;
		hot_img=NULL;
	}
	if( sel_hot_img )
	{
		delete sel_hot_img;
		sel_hot_img=NULL;
	}
	if( nor_img )
	{
		delete nor_img;
		nor_img = NULL;
	}
}
void OptionPlusUI::SetSelectedHotImage(LPCTSTR pstrValue)
{
	if( sel_hot_img )
		delete sel_hot_img;
	sel_hot_img = LoadImage(pstrValue);
	__super::SetSelectedHotImage(pstrValue);
}
void OptionPlusUI::SetForeImage(LPCTSTR pstrValue)
{
	if( fore_img )
		delete fore_img;
	fore_img = LoadImage(pstrValue);
	__super::SetForeImage(pstrValue);
}
void OptionPlusUI::SetHotImage(LPCTSTR pstrValue)
{
	if( hot_img )
		delete hot_img;
	hot_img = LoadImage(pstrValue);
	__super::SetHotImage(pstrValue);
}
void OptionPlusUI::SetSelectedImage(LPCTSTR pstrValue)
{
	if( selected_img )
		delete selected_img;
	selected_img = LoadImage(pstrValue);
	__super::SetSelectedImage(pstrValue);
}
void OptionPlusUI::SetNormalImage(LPCTSTR pstrValue)
{
	if(nor_img)
		delete nor_img;
	nor_img = LoadImage(pstrValue);
	__super::SetNormalImage(pstrValue);
}
void OptionPlusUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("selectedimage"))==0 ) 
		this->SetSelectedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("selectedhotimage"))==0 )
		this->SetSelectedHotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("foreimage"))==0 ) 
		this->SetForeImage(pstrValue);
	else if( _tcscmp(pstrName,_T("hotimage"))==0 )
		this->SetHotImage(pstrValue);
	else if( _tcscmp(pstrName,_T("normalimage"))==0 )
		this->SetNormalImage(pstrValue);
	else
		__super::SetAttribute(pstrName, pstrValue);
}
void OptionPlusUI::PaintStatusImage(HDC hDC)
{
	Gdiplus::Graphics g(hDC);
	Gdiplus::Rect	  rc(Rc2plus(GetPos()));
	m_uButtonState &= ~UISTATE_PUSHED;
	if( IsFocused() )
		m_uButtonState |= UISTATE_FOCUSED;
	else
		m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !IsEnabled() )
		m_uButtonState |= UISTATE_DISABLED;
	else
		m_uButtonState &= ~ UISTATE_DISABLED;
	if( nor_img )
	{
		if( Gdiplus::Ok != g.DrawImage(nor_img,rc) )
		{
			delete nor_img;
			nor_img = NULL;
		}
	}
	if( (m_uButtonState & UISTATE_HOT) != 0 ) 
	{
		if( IsSelected() && sel_hot_img && Gdiplus::Ok != g.DrawImage(sel_hot_img,rc) )
		{
			delete sel_hot_img;
			sel_hot_img = NULL;
		}
		else if( hot_img && Gdiplus::Ok != g.DrawImage(hot_img,rc) )
		{
			delete hot_img;
			hot_img = NULL;
		}
		else if(m_dwHotBkColor != 0) 
		{
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
			return;
		}
		this->AddPaintState(HotBorder);
		goto Label_ForeImage;		
	}
	else if( (m_uButtonState & UISTATE_SELECTED) != 0 ) 
	{
		if( selected_img )
		{
			g.DrawImage(selected_img,rc);
			if( Gdiplus::Ok != g.GetLastStatus() )
				goto Label_ForeImage;
		}
		else if(m_dwSelectedBkColor) 
		{
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
			return;
		}
		this->DelPaintState(HotBorder);
	}
	else
	{
		this->DelPaintState(HotBorder);
	}
Label_ForeImage:
	if( fore_img )
	{
		g.DrawImage(fore_img,rc);
		if( Gdiplus::Ok != g.GetLastStatus() )
		{
			delete fore_img;
			fore_img = NULL;
		}
	}
}
/*------------------------------------------------------------------------*/
void EditUI::DoInit()
{
	__super::DoInit();
}
LRESULT EditUI::OnEditKeyDown(WPARAM key)
{
	Invalidate();
	return true;
}
/*------------------------------------------------------------------------*/
AutoRichDocUI::AutoRichDocUI()
{
	nTotal_ = 0;
	memset(&rcExtern_,0,sizeof(RECT));	
	memset(&rcLimit_,0,sizeof(RECT));

	cur_ft_.bold  = false;
	cur_ft_.color = 0;
	cur_ft_.italic= false;
	cur_ft_.name  = _T("宋体");
	cur_ft_.size  = 12;
	cur_ft_.underline = false;
}
AutoRichDocUI::~AutoRichDocUI()
{

}
void AutoRichDocUI::Clear()
{
	__super::Clear();
}
void AutoRichDocUI::DoInit()
{
	__super::DoInit();
	DuiLib::TFontInfo *ftinfo = GetManager()->GetFontInfo(__super::GetFont());
	cur_ft_.bold  = ftinfo->bBold;
	cur_ft_.italic= ftinfo->bItalic;
	cur_ft_.name  = ftinfo->sFontName.GetData();
	cur_ft_.size  = ftinfo->iSize;
	cur_ft_.underline = ftinfo->bUnderline;


	DWORD dwMask = 0;
	dwMask = GetEventMask();
	dwMask |= ENM_CHANGE;
	__super::SetEventMask(dwMask);
	__super::SetRich(true);
	__super::SetAttribute(_T("multiline"),_T("true"));
	__super::SetAttribute(_T("wantreturn"),_T("true"));
	SetName(_T("auto rich edit ui"));
}
const capture::FontInfo& AutoRichDocUI::GetCurFont()
{
	return cur_ft_;
}
void AutoRichDocUI::SetFont(const capture::FontInfo ft)
{
	cur_ft_ = ft;
}
void AutoRichDocUI::UpdateFont()
{
	SetTextColor(cur_ft_.color);
	LONG yPixPerInch = GetDeviceCaps(GetManager()->GetPaintDC(), LOGPIXELSY);
	LONG nFont = MulDiv(cur_ft_.size,yPixPerInch,72);
	__super::SetFont(cur_ft_.name,nFont,cur_ft_.bold,cur_ft_.underline,cur_ft_.italic);
	Invalidate();
}
void AutoRichDocUI::OnTxNotify(DWORD iNotify, void *pv)
{
	if( iNotify == EN_CHANGE )
	{
		long nLen = GetTextLength();
		long nW = 0,nH = 0;
		HRESULT hr;
		POINT ptA={0,0},ptB={0,0};
		RECT rc		= GetPos();
		if( nLen > nTotal_ )
		{
			CComPtr<ITextRange2> sptr = this->GetRange(nTotal_,nLen);
			hr = sptr->GetPoint(tomStart|tomAllowOffClient|tomClientCoord|TA_LEFT|TA_BOTTOM,&ptA.x,&ptA.y);
			hr = sptr->GetPoint(tomEnd|tomAllowOffClient|tomClientCoord|TA_RIGHT|TA_BOTTOM,&ptB.x,&ptB.y);
			nW = ptB.x-ptA.x;
			nH = ptB.y-ptA.y;
			if( nW==0 && nH==0 && SUCCEEDED(hr) )
			{
				//用户输入了回车
				//那么就多生成一行空余空间
				rc.bottom += rcExtern_.bottom;
			}
			else if( nW && SUCCEEDED(hr) )
			{
				if( (rc.right - ptB.x) <= rcExtern_.right )
					rc.right += nW;
				if( rc.bottom-ptB.y <= rcExtern_.bottom )
					rc.bottom += rcExtern_.bottom;
			}
		}
		else
			rc = GetPos();
		if( rc.right > rcLimit_.right )
			rc.right = rcLimit_.right;
		if( rc.bottom > rcLimit_.bottom )
			rc.bottom = rcLimit_.bottom;
		if( rc.left < rcLimit_.left )
			rc.left = rcLimit_.left;
		if( rc.top < rcLimit_.top )
			rc.top = rcLimit_.top;
		nTotal_ = nLen;
		SetPos(rc);
		OnChange(NULL);
	}
}
void AutoRichDocUI::OnDraw(HDC hDC, const RECT& rcPaint)
{
	__super::DoPaint(hDC,rcPaint);
}
void AutoRichDocUI::Event(DuiLib::TEventUI &ev)
{
	if( ev.Type==DuiLib::UIEVENT_KILLFOCUS )
		OnKillFocus(NULL);
	__super::Event(ev);
}