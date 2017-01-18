#include "StdAfx.h"
#include "Toolsbar.h"
#include "BuilderCall.h"
ToolsbarUI::ToolsbarUI(void)
	:sel_pen_(Gdiplus::Color(DEF_PEN_COLOR),1)
{

}
ToolsbarUI::~ToolsbarUI(void)
{

}
void ToolsbarUI::Init()
{
	GetManager()->AddNotifier(this);
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
		DuiLib::CStdPtrArray *array = GetManager()->GetOptionGroup(_T("color_block"));
		for(int nI=0;nI < array->GetSize();nI++)
		{
			DuiLib::COptionUI *ui = (DuiLib::COptionUI*)(DuiLib::CControlUI*)array->GetAt(nI);
			if( ui->IsSelected() )
			{
				DWORD dwC = ui->GetBkColor();
				sel_pen_.SetColor(dwC);
				DuiLib::CControlUI *selshow = GetManager()->FindControl(_T("selshow"));
				if( selshow )
					selshow->SetBkColor(dwC);
				break;
			}
		}
		array = GetManager()->GetOptionGroup(_T("brush"));
		for(int nI=0;nI < array->GetSize();nI++)
		{
			BrushBlockUI *ui = (BrushBlockUI*)(DuiLib::CControlUI*)array->GetAt(nI);
			if( ui->IsSelected() )
			{
				sel_pen_.SetWidth(ui->GetBrushWidth());
				break;
			}
		}
	}
	__super::Event(event);
}
void ToolsbarUI::Notify(DuiLib::TNotifyUI& msg)
{
	if( msg.sType==DUI_MSGTYPE_ITEMSELECT && msg.pSender )
	{
		DropBox *box = static_cast<DropBox*>(msg.pSender);
		DuiLib::CControlUI *item = box->GetItemAt(box->GetCurSel());
		if( item )
			sel_ft_.size = _ttoi(item->GetText().GetData());
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
			msg.pSender->GetName()==_T("text_option")	  
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
				ShowTools(_T("brush_width"));
				ev.wParam = capture::Rectangle;
			}
			else if( msg.pSender->GetName()==_T("ellipse_option") )
			{
				ShowTools(_T("brush_width"));
				ev.wParam = capture::Ellipse;
			}
			else if( msg.pSender->GetName()==_T("line_option") )
			{
				ShowTools(_T("brush_width"));
				ev.wParam = capture::Line;
			}
			else if( msg.pSender->GetName()==_T("brush_option") )
			{
				ShowTools(_T("brush_width"));
				ev.wParam = capture::Brush;
			}
			else if( msg.pSender->GetName()==_T("arrow_option") )
			{
				ShowTools(_T("brush_width"));
				ev.wParam = capture::Arrow;
			}
			else if( msg.pSender->GetName()==_T("text_option") )
			{
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
				ev.wParam = capture::Text;
			}
			GetManager()->GetRoot()->Event(ev);
			return ;
		}
		if( _tcscmp(msg.pSender->GetClass(),_T("ColorBlock"))==0 )
		{
			sel_pen_.SetColor(msg.pSender->GetBkColor());
			sel_ft_.color	= msg.pSender->GetBkColor();
			DuiLib::CControlUI *ui = GetManager()->FindControl(_T("selshow"));
			if( ui )
				ui->SetBkColor(msg.pSender->GetBkColor());
			return ;
		}
		if( _tcscmp(msg.pSender->GetClass(),_T("BrushBlock"))==0 )
		{
			BrushBlockUI *bb = static_cast<BrushBlockUI*>(msg.pSender);
			sel_pen_.SetWidth(bb->GetBrushWidth());
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