#include "StdAfx.h"
#include "Toolsbar.h"
#include "BuilderCall.h"
#include "IniFile.h"

ToolsbarUI::ToolsbarUI(void)
	:sel_pen_(Gdiplus::Color(DEF_PEN_COLOR),1)
{
	hasInit_ = false;
	cur_cap_ = capture::Rectangle;
}
ToolsbarUI::~ToolsbarUI(void)
{
	SaveShapeINI();
}
void ToolsbarUI::Init()
{
	GetManager()->AddNotifier(this);
}
ARGB ToolsbarUI::GetDefaultColor()
{
	ARGB ret=-1;
	DuiLib::CStdPtrArray *array = GetManager()->GetOptionGroup(_T("color_block"));
	for(int nI=0;nI < array->GetSize();nI++)
	{
		DuiLib::COptionUI *ui = (DuiLib::COptionUI*)(DuiLib::CControlUI*)array->GetAt(nI);
		if( ui->IsSelected() )
		{
			ret = ui->GetBkColor();
			break;
		}
	}
	return ret;
}
LONG ToolsbarUI::GetDefaultSize()
{
	LONG ret = -1;
	DuiLib::CStdPtrArray *array = GetManager()->GetOptionGroup(_T("brush"));
	for(int nI=0;nI < array->GetSize();nI++)
	{
		BrushBlockUI *ui = (BrushBlockUI*)(DuiLib::CControlUI*)array->GetAt(nI);
		if( ui->IsSelected() )
		{
			ret = ui->GetBrushWidth();
			break;
		}
	}
	return ret;
}
void ToolsbarUI::SetUserINI(const CString& ini)
{
	if(!ini.IsEmpty())
		userINI_ = ini;
}
void ToolsbarUI::SetCurShape(capture::DRAW_CAP cap)
{
	cur_cap_ = cap;
}
void ToolsbarUI::SaveShapeINI()
{
	STORE::iterator it = param_.begin();
	for(it;it!=param_.end();it++){
		DrawConfig dc = it->second;
		CString iniSession(_T("ScreenShot"));
		if(it->first==capture::Arrow)
			iniSession += _T("_ARROW");
		else if(it->first==capture::Brush)
			iniSession += _T("_BRUSH");
		else if(it->first==capture::Ellipse)
			iniSession += _T("_ELLIPSE");
		else if(it->first==capture::Line)
			iniSession += _T("_LINE");
		else if(it->first==capture::Rectangle)
			iniSession += _T("_RECT");
		else if(it->first==capture::Gauss)
			iniSession += _T("_GAUSS");
		CIniFile iniFile;
		iniFile.SetFilePath(userINI_);
		iniFile.SetValue(iniSession,_T("Color"),dc.color_.GetValue());
		iniFile.SetValue(iniSession,_T("Size"),dc.size_);
	}
}
void ToolsbarUI::LoadFromINI(capture::DRAW_CAP cap,const CString& ini)
{
	if(!hasInit_)
		return ;
	if(userINI_.IsEmpty())
		return ;
	CString iniSession(_T("ScreenShot"));
	if(cap==capture::Arrow)
		iniSession += _T("_ARROW");
	else if(cap==capture::Brush)
		iniSession += _T("_BRUSH");
	else if(cap==capture::Ellipse)
		iniSession += _T("_ELLIPSE");
	else if(cap==capture::Line)
		iniSession += _T("_LINE");
	else if(cap==capture::Rectangle)
		iniSession += _T("_RECT");
	else if(cap==capture::Gauss)
		iniSession += _T("_GAUSS");
	DrawConfig dc = param_[cap];
	CIniFile iniFile;
	iniFile.SetFilePath(userINI_);
	{	//读取
		DWORD color = iniFile.GetValue(iniSession,_T("Color"),-1L);
		DWORD size  = iniFile.GetValue(iniSession,_T("Size"),-1L);
		if(color==-1L)
			dc.color_ = GetDefaultColor();
		else
			dc.color_ = color;
		if(size==-1L)
			dc.size_ = GetDefaultSize();
		else
			dc.size_ = size;
	}
	dc.shape_ = cap;
	param_[cap] = dc;
}
void ToolsbarUI::AppearFromStore()
{
	DrawConfig cfg = param_[cur_cap_];
	if(cur_cap_==capture::Text){
		return ;
	}
	DuiLib::CContainerUI *bw = (DuiLib::CContainerUI*)GetManager()->FindControl(_T("brush_width"));
	for(int i=0;i<bw->GetCount();i++){
		BrushBlockUI *ctrl = static_cast<BrushBlockUI*>(bw->GetItemAt(i));
		if( cfg.size_==ctrl->GetBrushWidth() ){
			ctrl->Selected(true);
			break;
		}
	}
	DuiLib::CStdPtrArray *array = GetManager()->GetOptionGroup(_T("color_block"));
	for(int nI=0;nI < array->GetSize();nI++)
	{
		ColorBlockUI *ctrl = static_cast<ColorBlockUI*>(array->GetAt(nI));
		if(cfg.color_.GetValue()==ctrl->GetBkColor())
		{
			ctrl->Selected(true);
			break;
		}
	}
	DuiLib::CControlUI *ui = GetManager()->FindControl(_T("selshow"));
	if( ui )
		ui->SetBkColor(cfg.color_.GetValue());
	sel_pen_.SetColor(cfg.color_);
	sel_pen_.SetWidth(cfg.size_);
}
void ToolsbarUI::ShowTools(const CAtlString& showpart)
{
	DuiLib::CControlUI *p= GetManager()->FindControl(_T("operatbar"));
	if( p==NULL )
		return;
	if( showpart.IsEmpty() )
	{
		DuiLib::CControlUI *pC = GetManager()->FindSubControlByName(p,_T("brush_width"));
		pC->SetVisible(true);
	}
	else
	{
		DuiLib::CControlUI *pA = GetManager()->FindSubControlByName(p,showpart);
		DuiLib::CControlUI *pB = GetManager()->FindSubControlByName(p,_T("range_box"));
		DuiLib::CControlUI *pC = GetManager()->FindSubControlByName(p,_T("brush_width"));
		DuiLib::CControlUI *pD = GetManager()->FindSubControlByName(p,_T("text_select"));		
		if( pA!=pB )
			pB->SetVisible(false);
		if( pA!=pC )
			pC->SetVisible(false);
		if( pA!=pD )
			pD->SetVisible(false);
		if(pA==NULL)
			p->SetVisible(false);
		else
			pA->SetVisible(true);
		if(pA==pD)
		{
			DropBox *drop = static_cast<DropBox*>( GetManager()->FindSubControlByName(p,_T("droplist")) );
			if( drop )
				drop->SelectItem(0);
		}
	}
	RECT rcRoot = GetManager()->GetRoot()->GetPos();

	RECT rcPos = this->GetPos();
	rcPos.top	 = GetPos().bottom + 1;
	rcPos.bottom = rcPos.top +p->GetFixedHeight();
	rcPos.left   += 1;
	rcPos.right  = rcPos.left+p->GetFixedWidth();
	if( rcPos.bottom > rcRoot.bottom )
	{
		rcPos.bottom = GetPos().top - 1;
		rcPos.top = rcPos.bottom - p->GetFixedHeight();
	}
	if( rcPos.right >rcRoot.right )
	{
		rcPos.right = rcRoot.right;
		rcPos.left  = rcPos.right-p->GetFixedWidth();
	}
	p->SetPos(rcPos);
	//从配置中读取信息更新到ui
	AppearFromStore();
}
void ToolsbarUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
}
void ToolsbarUI::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);
	if(!bVisible)
	{
		DuiLib::CControlUI *p= GetManager()->FindControl(_T("operatbar"));
		if( p->IsVisible() )
			p->SetVisible(false);
	}
}
void ToolsbarUI::Event(DuiLib::TEventUI& event)
{
	if( event.Type==capture::InitTools )
	{
		hasInit_ = true;
		LoadFromINI(capture::Arrow,NULL);
		LoadFromINI(capture::Brush,NULL);
		LoadFromINI(capture::Ellipse,NULL);
		LoadFromINI(capture::Line,NULL);
		LoadFromINI(capture::Rectangle,NULL);
		LoadFromINI(capture::Gauss,NULL);
		DrawConfig dc = param_[cur_cap_];
		sel_pen_.SetColor(dc.color_);
		sel_pen_.SetWidth(dc.size_);
	}
	__super::Event(event);
}
void ToolsbarUI::Notify(DuiLib::TNotifyUI& msg)
{
	if( msg.sType==DUI_MSGTYPE_ITEMSELECT && msg.pSender )
	{
		DropBox *box = static_cast<DropBox*>(msg.pSender);
		DuiLib::CControlUI *item = box->GetItemAt(box->GetCurSel());
		if( item ){
			sel_ft_.size = _ttoi(item->GetText().GetData());
			param_[cur_cap_].size_ = sel_ft_.size;
		}
		return ;
	}
	else if( msg.sType==DUI_MSGTYPE_CLICK && msg.pSender )
	{
		DuiLib::CControlUI *p= GetManager()->FindControl(_T("operatbar"));
		if( p==NULL )
			return;
		if( msg.pSender->GetName()==_T("rectangle_option")|| 
			msg.pSender->GetName()==_T("ellipse_option")  ||
			msg.pSender->GetName()==_T("line_option")	  ||
			msg.pSender->GetName()==_T("brush_option")	  ||
			msg.pSender->GetName()==_T("arrow_option")	  ||
			msg.pSender->GetName()==_T("text_option")	  ||
			msg.pSender->GetName()==_T("gauss_option")
			)
		{
			p->SetVisible(true);
			//通知处于绘图状态
			DuiLib::TEventUI ev;
			ev.Type	  = capture::PrepareDraw;	
			ev.pSender= this;
			ev.ptMouse= msg.ptMouse;
			if( msg.pSender->GetName()==_T("rectangle_option") )
			{
				ev.wParam = capture::Rectangle;
				cur_cap_  = capture::Rectangle;
				ShowTools(_T("brush_width"));
			}
			else if( msg.pSender->GetName()==_T("ellipse_option") )
			{
				ev.wParam = capture::Ellipse;
				cur_cap_  = capture::Ellipse;
				ShowTools(_T("brush_width"));
			}
			else if( msg.pSender->GetName()==_T("line_option") )
			{
				ev.wParam = capture::Line;
				cur_cap_  = capture::Line;
				ShowTools(_T("brush_width"));
			}
			else if( msg.pSender->GetName()==_T("brush_option") )
			{
				ev.wParam = capture::Brush;
				cur_cap_  = capture::Brush;
				ShowTools(_T("brush_width"));				
			}
			else if( msg.pSender->GetName()==_T("arrow_option") )
			{
				ev.wParam = capture::Arrow;
				cur_cap_  = capture::Arrow;
				ShowTools(_T("brush_width"));
			}
			else if( msg.pSender->GetName()==_T("gauss_option")){
				ev.wParam = capture::Gauss;
				cur_cap_  = capture::Gauss;
				ShowTools(_T("brush_width"));
			}
			else if( msg.pSender->GetName()==_T("text_option") )
			{
				ev.wParam = capture::Text;
				cur_cap_  = capture::Text;
				ShowTools(_T("text_select"));
				DropBox *drop = static_cast<DropBox*>(GetManager()->FindControl(_T("droplist")));
				Gdiplus::Color clr;
				sel_pen_.GetColor(&clr);
				DuiLib::CControlUI *sel = drop->GetItemAt(drop->GetCurSel());
				sel_ft_.bold	= false;
				sel_ft_.italic	= false;
				sel_ft_.underline=false;
				sel_ft_.size    = _ttoi(sel->GetText().GetData());
				sel_ft_.color	= clr.GetValue();
			}
			GetManager()->GetRoot()->Event(ev);
			return ;
		}
		if( _tcscmp(msg.pSender->GetClass(),_T("ColorBlock"))==0 )
		{
			sel_pen_.SetColor(msg.pSender->GetBkColor());
			sel_ft_.color	= msg.pSender->GetBkColor();
			param_[cur_cap_].color_ = msg.pSender->GetBkColor();
			DuiLib::CControlUI *ui = GetManager()->FindControl(_T("selshow"));
			if( ui )
				ui->SetBkColor(msg.pSender->GetBkColor());
			return ;
		}
		if( _tcscmp(msg.pSender->GetClass(),_T("BrushBlock"))==0 )
		{
			BrushBlockUI *bb = static_cast<BrushBlockUI*>(msg.pSender);
			sel_pen_.SetWidth(bb->GetBrushWidth());
			param_[cur_cap_].size_ = bb->GetBrushWidth();
			return ;
		}
		if( _tcscmp(msg.pSender->GetName(),_T("undo_btn"))==0 )
		{
			DuiLib::TEventUI ev;
			ev.Type	  = capture::UndoLayer;	
			ev.pSender= this;
			ev.ptMouse= msg.ptMouse;
			GetManager()->GetRoot()->Event(ev);
			return ;
		}
		if( _tcscmp(msg.pSender->GetName(),_T("redo_btn"))==0 )
		{
			DuiLib::TEventUI ev;
			ev.Type	  = capture::RedoLayer;
			ev.pSender= this;
			ev.ptMouse= msg.ptMouse;
			GetManager()->GetRoot()->Event(ev);
			return ;
		}
		if( _tcscmp(msg.pSender->GetName(),_T("cancel_btn"))==0 )
		{
			DuiLib::TEventUI ev;
			ev.Type	  = capture::CloseWindow;
			ev.pSender= this;
			ev.ptMouse= msg.ptMouse;
			GetManager()->GetRoot()->Event(ev);
			return ;
		}
		if( _tcscmp(msg.pSender->GetName(),_T("save_btn"))==0 )
		{
			DuiLib::TEventUI ev;
			ev.Type	  = capture::SaveAsFile;
			ev.pSender= this;
			ev.ptMouse= msg.ptMouse;
			GetManager()->GetRoot()->Event(ev);
			return ;
		}
		if( _tcscmp(msg.pSender->GetName(),_T("send_btn"))==0 )
		{
			DuiLib::TEventUI ev;
			ev.Type	  = capture::SendAsFile;
			ev.pSender= this;
			ev.ptMouse= msg.ptMouse;
			GetManager()->GetRoot()->Event(ev);
			return ;
		}
	}
}
void ToolsbarUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	DuiLib::CHorizontalLayoutUI::DoPaint(hDC,rcPaint);
}