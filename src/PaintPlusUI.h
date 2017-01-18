#pragma once

#include "Const.h"
DWORD XmlColorStr2Num(LPCTSTR v);

template<typename theX>
class PaintPlusUI : public theX
{
public:
	typedef enum PaintState
	{
		HotBorder = 1,
		NorBorder = 1<<2,
	};
	PaintPlusUI(void)
	{
		bk_img		 = NULL;
		paintstate_	 = 0;
		w_hot_border_= 0;
		w_nor_border_= 0;
	}
	~PaintPlusUI(void)
	{
		if( bk_img )
		{
			delete bk_img;
			bk_img = NULL;
		}
	}
	void SetBkImage(LPCTSTR pStrImage)
	{
		if( bk_img )
			delete bk_img;
		bk_img = LoadImage(pStrImage);
		__super::SetBkImage(pStrImage);
	}
	Gdiplus::Image *LoadImage(LPCTSTR pStrImage)
	{
		Gdiplus::Image *pRet = NULL;
		if( ::PathFileExists(pStrImage) )
			pRet = new Gdiplus::Image(pStrImage);
		else
		{
			CAtlString strRes(GetManager()->GetResourcePath().GetData());
			strRes += pStrImage;
			pRet = new Gdiplus::Image(strRes);
		}
		return pRet;
	}
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName,_T("bkimage")) == 0 )
			SetBkImage(pstrValue);
		else if( _tcscmp(pstrName,_T("hotborder")) == 0 )
			w_hot_border_=_ttoi(pstrValue);
		else if( _tcscmp(pstrName,_T("hotbordercolor")) == 0 )
			clr_hot_border_= XmlColorStr2Num(pstrValue);
		else if( _tcscmp(pstrName,_T("norborder"))==0 )
			w_nor_border_=_ttoi(pstrValue);
		else if( _tcscmp(pstrName,_T("norbordercolor")) == 0 )
			clr_nor_border_ = XmlColorStr2Num(pstrValue);
		else 
			__super::SetAttribute(pstrName,pstrValue);
	}
	void AddPaintState(PaintState n)
	{
		paintstate_ |= n;
	}
	void DelPaintState(PaintState n)
	{
		if( paintstate_ )
			paintstate_ &= ~n;
	}
protected:
	void PaintBkImage(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		Gdiplus::Rect	  rc(Rc2plus(GetPos()));
		if( bk_img )
			g.DrawImage(bk_img,rc);
	}
	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		Gdiplus::Rect	  rc(Rc2plus(GetPos()));
		Gdiplus::SolidBrush br(GetAdjustColor(m_dwBackColor));
		if( m_dwBackColor )
			g.FillRectangle(&br,rc);
	}
	void PaintBorder(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		__super::PaintBorder(hDC);
		if( IS_HAVE(paintstate_,NorBorder) )
			PaintNorBorder(g);
		if( IS_HAVE(paintstate_,HotBorder) )
			PaintHotBorder(g);
	}
	void PaintNorBorder(Gdiplus::Graphics &g)
	{
		if( w_nor_border_==0 )
			return ;
		Gdiplus::Rect	  rc(Rc2plus(GetPos()));
		Gdiplus::Pen	  pen(clr_nor_border_,(Gdiplus::REAL)w_nor_border_);
		g.DrawRectangle(&pen,rc);
	}
	void PaintHotBorder(Gdiplus::Graphics &g)
	{
		if( w_hot_border_==0 )
			return ;
		Gdiplus::Rect	  rc(Rc2plus(GetPos()));
		Gdiplus::Pen	  pen(clr_hot_border_,(Gdiplus::REAL)w_hot_border_);
		g.DrawRectangle(&pen,rc);
	}
protected:
	Gdiplus::Image	*bk_img;
	Gdiplus::Color	clr_hot_border_;
	int				w_hot_border_;

	Gdiplus::Color	clr_nor_border_;
	int				w_nor_border_;
	//表示绘制状态
	DWORD			paintstate_;
};

class ButtonPlusUI : public PaintPlusUI<DuiLib::CButtonUI>
{
public:
	ButtonPlusUI();
	virtual ~ButtonPlusUI();
	void SetForeImage(LPCTSTR pstrValue);
	void SetHotImage(LPCTSTR pstrValue);
	void SetNormalImage(LPCTSTR pStrImage);
	void SetPushedImage(LPCTSTR pStrImage);
	void SetDisabledImage(LPCTSTR pStrImage);
	void SetFocusedImage(LPCTSTR pStrImage);
	void SetHotForeImage(LPCTSTR pStrImage);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
protected:
	void PaintStatusImage(HDC hDC);
private:
	Gdiplus::Image	*fore_img;
	Gdiplus::Image	*hot_img;
	Gdiplus::Image	*dis_img;
	Gdiplus::Image	*push_img;
	Gdiplus::Image  *nor_img;
	Gdiplus::Image	*hot_fore_img;
	Gdiplus::Image	*focus_img;
};

class OptionPlusUI : public PaintPlusUI<DuiLib::COptionUI>
{
public:
	OptionPlusUI();
	virtual ~OptionPlusUI();
	void SetSelectedHotImage(LPCTSTR pstrValue);
	void SetForeImage(LPCTSTR pstrValue);
	void SetHotImage(LPCTSTR pstrValue);
	void SetSelectedImage(LPCTSTR pstrValue);
	void SetNormalImage(LPCTSTR pstrValue);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
protected:
	void PaintStatusImage(HDC hDC);
protected:
	Gdiplus::Image  *nor_img;
	Gdiplus::Image	*fore_img;
	Gdiplus::Image	*selected_img;
	Gdiplus::Image	*hot_img;
	Gdiplus::Image  *sel_hot_img;
};

class LayoutH : public PaintPlusUI<DuiLib::CHorizontalLayoutUI>
{
public:
	LayoutH(){ }
	virtual ~LayoutH(){ }
};

class LayoutV : public PaintPlusUI<DuiLib::CVerticalLayoutUI>
{
public:
	LayoutV(){ }
	virtual ~LayoutV(){ }
};

class Layout : public PaintPlusUI<DuiLib::CContainerUI>
{
public:
	Layout(){ }
	virtual ~Layout(){ }
};

class PlaceUI : public PaintPlusUI<DuiLib::CControlUI>
{
public:
	PlaceUI(){ }
	virtual ~PlaceUI(){ }
};

class DropBox : public PaintPlusUI<DuiLib::CComboUI>
{
public:
	DropBox(){ }
	virtual ~DropBox(){ }
};

class EditUI : public PaintPlusUI<DuiLib::CEditUI>
{
public:
	//layer window 下edit的每次操作都需要主动发出paint事件
	EditUI(){ }
	virtual ~EditUI(){ }
protected:
	void DoInit();
	LRESULT OnEditKeyDown(WPARAM key);
};

class AutoRichDocUI : public PaintPlusUI<DuiLib::CRichEditUI>
{
public:
	AutoRichDocUI();
	virtual ~AutoRichDocUI();
	void Clear();
	inline void SetExternRc(const RECT &rc)
	{
		rcExtern_ = rc;
	}
	const RECT&	GetExternRc()
	{
		return rcExtern_;
	}
	inline void SetLimitRc(const RECT &rc)
	{
		rcLimit_ = rc;
	}
	void OnDraw(HDC hDC, const RECT& rcPaint);
	const capture::FontInfo& GetCurFont();
	void SetFont(const capture::FontInfo ft);
	void UpdateFont();
	CEventSource OnChange;
	CEventSource OnKillFocus;
protected:
	void DoInit();
	void OnTxNotify(DWORD iNotify, void *pv);
	void DoPaint(HDC hDC, const RECT& rcPaint)
	{	

	}
	void Event(DuiLib::TEventUI &ev);
protected:
	long nTotal_;
	RECT rcExtern_;
	RECT rcLimit_;
	capture::FontInfo cur_ft_;
};