#include "stdafx.h"
#include "ScreenUI.h"
#include "Toolsbar.h"
#include <algorithm>
#include <windowsx.h>
#include "dib/DIBAPI.H"

BOOL __stdcall SaveTopWinProc(HWND hwnd,LPARAM lParam)
{
	std::vector<HWND> *wins = (std::vector<HWND>*)(void*)lParam;
	if( !::IsIconic(hwnd) && ::IsWindow(hwnd)  && ::IsWindowVisible(hwnd) )	
		wins->push_back(hwnd);
	return TRUE;
}
void GetAllTopWindows(std::vector<HWND> &wins)
{
	::EnumWindows(SaveTopWinProc,(LPARAM)(void*)&wins);
}
//返回true-表示发生互换，false-反之
bool TrackMove(RECT &rc,const POINT pt,DuiLib::CControlUI *ui,DuiLib::CContainerUI *parent)
{
	RECT rcTmp = rc;
	if( 0==ui->GetName().CompareNoCase(TRACK_T) )
	{
		rcTmp.top += pt.y;
		if( rcTmp.top > rc.bottom )
		{
			rc.top     = rc.bottom;
			rc.bottom += pt.y;
			parent->FindSubControl(TRACK_B)->SetName(TRACK_T);
			ui->SetName(TRACK_B);
			return true;
		}
		else
			rc.top  += pt.y;
	}
	else if( 0==ui->GetName().CompareNoCase(TRACK_B) )
	{
		rcTmp.bottom += pt.y;
		if( rcTmp.bottom < rc.top )
		{
			rc.bottom  = rc.top;
			rc.top	  += pt.y;
			parent->FindSubControl(TRACK_T)->SetName(TRACK_B);
			ui->SetName(TRACK_T);
			return true;
		}
		else
			rc.bottom  += pt.y;
	}
	else if( 0==ui->GetName().CompareNoCase(TRACK_L) )
	{
		rcTmp.left += pt.x;
		if( rcTmp.left > rc.right )
		{
			rc.left   = rc.right;
			rc.right += pt.x;
			parent->FindSubControl(TRACK_R)->SetName(TRACK_L);
			ui->SetName(TRACK_R);
			return true;
		}
		else
		{
			rc.left += pt.x;
		}
	}
	else if( 0==ui->GetName().CompareNoCase(TRACK_R) )
	{
		rcTmp.right += pt.x;
		if( rcTmp.right < rc.left )
		{
			rc.right  = rc.left;
			rc.left  += pt.x;
			parent->FindSubControl(TRACK_L)->SetName(TRACK_R);
			ui->SetName(TRACK_L);
			return true;
		}
		else
		{
			rc.right += pt.x;
		}
	}
	return false;
}
ScreenUI::ScreenUI()
	:cur_screen_(NULL)
	,draw_states_(capture::AUTO_SEL)
	,showrc_(NULL)
	,draw_layer(NULL)
	,ui_move_(NULL)
	,draw_cap_(capture::Rectangle)
	,tools_(NULL)
{
	memset(&cur_sel_,0,sizeof(RECT));
	cur_pt_.x = -1;
	cur_pt_.y = -1;
}
ScreenUI::~ScreenUI()
{
	if( cur_screen_ )
	{
		delete cur_screen_;
		cur_screen_ = NULL;
	}
	top_wins_.clear();
}
void ScreenUI::SetUserINI(const CString& ini)
{
	if(tools_){
		tools_->SetUserINI(ini);
	}
}
void ScreenUI::Init()
{
	cur_screen_ = BitBltDesktop(GetManager()->GetPaintWindow());
	//获得所有顶层窗口并剔除自己
	std::vector<HWND>	top_wins;
	GetAllTopWindows(top_wins);
	std::vector<HWND>::iterator itSlef = std::find(top_wins.begin(),top_wins.end(),GetManager()->GetPaintWindow());
	if( itSlef != top_wins.end() )
		top_wins.erase(itSlef);
	for(size_t nI=0;nI < top_wins.size();nI++)
	{
		RECT rc;
		HWND win = top_wins[nI];
		if( !::IsIconic(win) && ::IsWindow(win)  && ::IsWindowVisible(win)  && GetWindowRect(win,&rc) )
		{
			if( rc.left < 0 || rc.top<0 )
				continue;
			top_wins_.push_back(rc);
		}
	}
	showrc_ = FindSubControl(_T("showrc"));
	tools_  = static_cast<ToolsbarUI*>(FindSubControl(_T("tools")));
	GetManager()->SetFocus(this);
	rcTrue_=DuiLib::CRect(0,0,cur_screen_->GetWidth(),cur_screen_->GetHeight());
	DuiLib::DUI__Trace(_T("Screen init"));
	__super::DoInit();
}
UINT ScreenUI::GetControlFlags() const
{
	return UIFLAG_SETCURSOR;
}
const RECT& ScreenUI::GetPos() const
{
	return rcTrue_;
}
void ScreenUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	if( m_items.IsEmpty() ) return;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
}
void ScreenUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	Gdiplus::Graphics g(hDC);
	if( !cur_screen_ )
	{
		__super::DoPaint(hDC,rcPaint);
		return ;
	}
	RECT rcTemp = { 0 };
	if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) )
		return;
	//先画一层灰图
	Gdiplus::SolidBrush sbr(DEF_BACK_COLOR);
	Gdiplus::Rect rcSel = Rc2plus(cur_sel_);
	g.DrawImage(cur_screen_,0,0);
	if( ::IsRectEmpty(&cur_sel_) )
	{
		g.FillRectangle(&sbr,0,0,cur_screen_->GetWidth(),cur_screen_->GetHeight());
	}
	else 
	{
		Gdiplus::Region rcFull(Gdiplus::Rect(0,0,cur_screen_->GetWidth(),cur_screen_->GetHeight()));
		rcFull.Exclude(rcSel);
		g.FillRegion(&sbr,&rcFull);
		if( IS_HAVE(draw_states_,capture::AUTO_SEL) )
		{
			Gdiplus::Pen pen(DEF_FRAME_COLOR,DEF_FRAME_WIDTH);
			g.DrawRectangle(&pen,rcSel);
		}
		else if( IS_HAVE(draw_states_,capture::MAN_SEL) )
		{
			layers_.paint(&g,cur_sel_);
			DuiLib::CContainerUI::DoPaint(hDC,rcPaint);
		}
	}
}
bool ScreenUI::CanMove( const RECT& rc )
{
	POINT ptA = {rc.left,rc.top};
	POINT ptB = {rc.right,rc.bottom};
	RECT  range={0,0,cur_screen_->GetWidth(),cur_screen_->GetHeight()};
	return (::PtInRect(&range,ptA) && PtInRect(&range,ptB));
}
void ScreenUI::Move(long x,long y)
{
	RECT  rcTmp	   = cur_sel_;
	::OffsetRect(&rcTmp,x,y);
	bool bCanMove = CanMove(rcTmp);
	if( !bCanMove )
	{
		if( rcTmp.left < 0 )
		{
			rcTmp.left = 0;
			rcTmp.right= cur_sel_.right-cur_sel_.left;
		}
		if( rcTmp.top < 0 )
		{
			rcTmp.top   = 0;
			rcTmp.bottom= cur_sel_.bottom-cur_sel_.top;
		}
		if( rcTmp.right > cur_screen_->GetWidth() )
		{
			rcTmp.right = cur_screen_->GetWidth();
			rcTmp.left  = rcTmp.right - (cur_sel_.right-cur_sel_.left);
		}
		if( rcTmp.bottom > cur_screen_->GetHeight() )
		{
			rcTmp.bottom = cur_screen_->GetHeight();
			rcTmp.top    = rcTmp.bottom - (cur_sel_.bottom-cur_sel_.top);
		}
		bCanMove =true;
	}
	cur_sel_ = rcTmp;
	if( showrc_ )
		showrc_->SetPos(cur_sel_);
	UpdateTrackDotPos();
	Invalidate();
}
void ScreenUI::TrackDotMove(const DuiLib::TEventUI& ev)
{
	RECT rcTmp = cur_sel_;
	if( 0==ev.pSender->GetName().CompareNoCase(TRACK_TL) )
	{
		bool a = TrackMove(cur_sel_,ev.ptMouse,FindSubControl(TRACK_T),this);
		bool b = TrackMove(cur_sel_,ev.ptMouse,FindSubControl(TRACK_L),this);
		if( a&&b )
		{
			FindSubControl(TRACK_BR)->SetName(TRACK_TL);
			ev.pSender->SetName(TRACK_BR);
		}
		else if( a )
		{
			FindSubControl(TRACK_BL)->SetName(TRACK_TL);
			ev.pSender->SetName(TRACK_BL);
		}
		else if( b )
		{
			FindSubControl(TRACK_TR)->SetName(TRACK_TL);
			ev.pSender->SetName(TRACK_TR);
		}
	}
	else if( 0==ev.pSender->GetName().CompareNoCase(TRACK_BR) )
	{
		bool a = TrackMove(cur_sel_,ev.ptMouse,FindSubControl(TRACK_R),this);
		bool b = TrackMove(cur_sel_,ev.ptMouse,FindSubControl(TRACK_B),this);
		if( a&&b )
		{
			FindSubControl(TRACK_TL)->SetName(TRACK_BR);
			ev.pSender->SetName(TRACK_TL);
		}
		else if(a)
		{
			FindSubControl(TRACK_BL)->SetName(TRACK_BR);
			ev.pSender->SetName(TRACK_BL);
		}
		else if(b)
		{
			FindSubControl(TRACK_TR)->SetName(TRACK_BR);
			ev.pSender->SetName(TRACK_TR);
		}
	}
	else if( 0==ev.pSender->GetName().CompareNoCase(TRACK_TR) )
	{
		bool a = TrackMove(cur_sel_,ev.ptMouse,FindSubControl(TRACK_T),this);
		bool b = TrackMove(cur_sel_,ev.ptMouse,FindSubControl(TRACK_R),this);
		if( a&&b )
		{
			FindSubControl(TRACK_BL)->SetName(TRACK_TR);
			ev.pSender->SetName(TRACK_BL);
		}
		else if(a)
		{
			FindSubControl(TRACK_BR)->SetName(TRACK_TR);
			ev.pSender->SetName(TRACK_BR);
		}
		else if(b)
		{
			FindSubControl(TRACK_TL)->SetName(TRACK_TR);
			ev.pSender->SetName(TRACK_TL);
		}
	}
	else if( 0==ev.pSender->GetName().CompareNoCase(TRACK_BL) )
	{
		bool a = TrackMove(cur_sel_,ev.ptMouse,FindSubControl(TRACK_L),this);
		bool b = TrackMove(cur_sel_,ev.ptMouse,FindSubControl(TRACK_B),this);
		if( a&&b )
		{
			FindSubControl(TRACK_TR)->SetName(TRACK_BL);
			ev.pSender->SetName(TRACK_TR);
		}
		else if(a)
		{
			FindSubControl(TRACK_BR)->SetName(TRACK_BL);
			ev.pSender->SetName(TRACK_BR);
		}
		else if(b)
		{
			FindSubControl(TRACK_TL)->SetName(TRACK_BL);
			ev.pSender->SetName(TRACK_TL);
		}
	}	
	else
	{
		TrackMove(cur_sel_,ev.ptMouse,ev.pSender,this);
	}
	//对超出边界的数据做极值校验处理
	if( cur_sel_.right > GetPos().right )
		cur_sel_.right = GetPos().right;
	if( cur_sel_.left < GetPos().left )
		cur_sel_.left = GetPos().left;
	if( cur_sel_.top < GetPos().top )
		cur_sel_.top = GetPos().top;
	if( cur_sel_.bottom > GetPos().bottom )
		cur_sel_.bottom = GetPos().bottom;
	UpdateTrackDotPos();
	if( showrc_ )
		showrc_->SetPos(cur_sel_);
	return ;
}
bool ScreenUI::PtInSel(POINT pt)
{
	if(!::IsRectEmpty(&cur_sel_) && ::PtInRect(&cur_sel_,pt) )
		return true;
	return false;
}
void ScreenUI::Event(DuiLib::TEventUI& event)
{
	if( event.Type==DuiLib::UIEVENT_KEYDOWN && IS_HAVE(draw_states_,capture::MOV_SEL))
	{
		//支持键盘操作
		switch(event.chKey)
		{
		case VK_LEFT:
			Move(-1,0);
			return ;
		case VK_RIGHT:
			Move(1,0);
			return ;
		case VK_UP:
			Move(0,-1);
			return ;
		case VK_DOWN:
			Move(0,1);
			return ;
		case VK_ESCAPE:
			DuiLib::CControlUI *ui = GetManager()->GetFocus();
			if( ui )
			{
				CAtlString strClass = ui->GetClass();
				strClass.MakeUpper();
				if( 0==strClass.CompareNoCase(DUI_CTR_RICHEDIT)||-1!=strClass.Find(_T("EDIT"))||
					0==strClass.CompareNoCase(DUI_CTR_EDIT) )
				{
					GetManager()->SetFocus(GetManager()->GetRoot());
					return ;
				}
			}
			::PostMessage(GetManager()->GetPaintWindow(),WM_CLOSE,0,0);
			return ;
		}
	}
	if( event.Type==DuiLib::UIEVENT_SETCURSOR )
	{
		DuiLib::CControlUI *ui=GetManager()->FindSubControlByPoint(this,event.ptMouse);
		if( ui && ui!=this )
			ui->Event(event);
		else if( PtInSel(event.ptMouse) )
			cursor_.change(draw_states_);
		else
			cursor_.change(capture::AUTO_SEL);
		return ;
	}
	if( event.Type==DuiLib::UIEVENT_MOUSEMOVE )
	{
		DuiLib::CControlUI *ui=NULL;
		if( event.wParam==0 && event.lParam==0 )
		{
			if( IS_HAVE(draw_states_,capture::AUTO_SEL) )
			{
				cur_sel_ = FindWinRect(event.ptMouse);
				cur_pt_	 = event.ptMouse;
				this->Invalidate();
			}
			return ;
		}
		if( IS_HAVE(event.wParam,MK_LBUTTON) )
		{
			if( IS_HAVE(draw_states_,capture::MOV_SEL) )
			{
				POINT pt;
				pt.x = event.ptMouse.x - cur_pt_.x;
				pt.y = event.ptMouse.y - cur_pt_.y;
				memcpy(&cur_pt_,&event.ptMouse,sizeof(POINT));
				Move(pt.x,pt.y);
			}
			else if( IS_HAVE(draw_states_,capture::AUTO_SEL) )
			{
				//用户直接画选择框
				ui_move_ = FindSubControl(TRACK_BR);
				draw_states_   = capture::MAN_SEL;
				if( cur_pt_.x  < event.ptMouse.x  )
				{
					cur_sel_.left  = cur_pt_.x;
					cur_sel_.right = event.ptMouse.x;
				}
				else
				{
					cur_sel_.left   = event.ptMouse.x;
					cur_sel_.right = cur_pt_.x;
				}
				if( cur_pt_.y < event.ptMouse.y )
				{
					cur_sel_.top  = cur_pt_.y;
					cur_sel_.bottom= event.ptMouse.y;
				}
				else
				{
					cur_sel_.top  = event.ptMouse.y;
					cur_sel_.bottom= cur_pt_.y;
				}				
				UpdateTrackDotPos();
				DuiLib::TEventUI e=event;
				e.pSender = ui_move_;
				e.Type    = UIEVENT_BUTTONDOWN;
				ui_move_->Event( e );
			}
			else if( draw_states_==capture::MAN_SEL && ui_move_)
			{
				event.pSender = ui_move_;
				ui_move_->Event( event );
			}
			else if( IS_HAVE(draw_states_,capture::DRAW_SEL) && draw_layer && tools_ )
			{
				POINT ptTmp = event.ptMouse;
				if( ptTmp.x > cur_sel_.right  )
					event.ptMouse.x = cur_sel_.right;
				if( ptTmp.x < cur_sel_.left )
					event.ptMouse.x = cur_sel_.left;				
				if( ptTmp.y < cur_sel_.top )
					event.ptMouse.y = cur_sel_.top;
				if( ptTmp.y > cur_sel_.bottom )
					event.ptMouse.y = cur_sel_.bottom;
				draw_layer->draw(GetManager()->GetPaintWindow(),&event,&tools_->sel_pen_);
				event.ptMouse = ptTmp;
			}	
			return ;
		}
	}
	if( event.Type==DuiLib::UIEVENT_BUTTONDOWN )
	{
		cur_pt_	 = event.ptMouse;
		DuiLib::CControlUI *ui = FindSubControl(_T("tools"));
		if( ui )
			ui->SetVisible(false);
		lclicktm_ = ::GetMessageTime();
		if( IS_HAVE(draw_states_,capture::DRAW_SEL) && PtInSel(event.ptMouse) )
		{
			//为了方便测试
			//if(draw_layer==NULL){
				draw_layer = layers_.push(draw_cap_,cur_screen_,cur_sel_);
				draw_layer->limitRc(cur_sel_);
				draw_layer->font(tools_->sel_ft_);
			//}
			draw_layer->draw(GetManager()->GetPaintWindow(),&event,&tools_->sel_pen_);
		}
	}
	if( event.Type==DuiLib::UIEVENT_BUTTONUP )
	{
		if( IS_HAVE(draw_states_,capture::AUTO_SEL) ||  IS_HAVE(draw_states_,capture::MAN_SEL)
			&& !IS_HAVE(draw_states_,capture::DRAW_SEL) )
		{
			//用户在300微妙内做的up说明用户接受了当前sel
			LONG  pertm= ::GetMessageTime() - lclicktm_ ;
			if( pertm<=300 || (cur_pt_.x==event.ptMouse.x && cur_pt_.y==event.ptMouse.y) )
				draw_states_ =  capture::MAN_SEL|capture::MOV_SEL;
			//弄个最小尺寸处理避免弄成0*0矩形
			if(  (cur_sel_.right-cur_sel_.left) <= (DEF_FRAME_WIDTH*2) )
				cur_sel_.right = cur_sel_.left + (DEF_FRAME_WIDTH*2);
			if( (cur_sel_.bottom-cur_sel_.top) <= (DEF_FRAME_WIDTH*2)	)
				cur_sel_.bottom = cur_sel_.top+(DEF_FRAME_WIDTH*2);
			if( showrc_ )
				showrc_->SetPos(cur_sel_);
			UpdateTrackDotPos();
			Invalidate();
		}
		if( draw_states_==capture::MAN_SEL )
		{
			//已完成选择区域动作
			draw_states_ =  capture::MAN_SEL|capture::MOV_SEL;
			ui_move_	 = NULL;
		}
		if( IS_HAVE(draw_states_,capture::DRAW_SEL) && draw_layer )
		{
			draw_layer->draw(GetManager()->GetPaintWindow(),&event,&tools_->sel_pen_);
		}
		DuiLib::CControlUI *ui = FindSubControl(_T("tools"));
		if( ui )
			ui->SetVisible(true);
		UpdateToolsPos();
	}
	if( event.Type==capture::TRACK_MOVE )
	{
		TrackDotMove(event);
		DuiLib::CControlUI *ui = FindSubControl(_T("tools"));
		if( ui )
			ui->SetVisible(false);
		return ;
	}
	if( event.Type==capture::TRACK_MOVE_END )
	{
		DuiLib::CControlUI *ui = FindSubControl(_T("tools"));
		if( ui )
			ui->SetVisible(true);
		UpdateToolsPos();
	}
	if( event.Type==capture::PrepareDraw )
	{
		draw_states_ &= ~capture::MOV_SEL;	//不可移动
		draw_states_ |= capture::DRAW_SEL;	//增加绘制状态
		draw_cap_	 = (capture::DRAW_CAP)event.wParam; 
		VisibleTrackDot(false);				//隐藏掉拖拽点避免引起歧义
	}
	if( event.Type==capture::RedoLayer )
	{
		layers_.redo();
		draw_layer = layers_.current();
		return ;
	}
	if( event.Type==capture::UndoLayer )
	{
		if( draw_layer )
		{
			layers_.undo();
			draw_layer = layers_.current();
		}
		return ;
	}
	if( event.Type==capture::CloseWindow )
	{
		HWND hwnd = GetManager()->GetPaintWindow();
		::PostMessage(hwnd,WM_CLOSE,0,0);
		return;
	}
	if( event.Type==capture::InitTools && tools_ )
	{
		//tools_->SetCurShape(draw_cap_);
		tools_->Event(event);
	}
	if( event.Type==capture::SaveAsFile )
	{
		savepath_ = DoSave(true);
		if( !savepath_.IsEmpty() )
		{
			//保存成功后清除各个绘图层
			layers_.clear();
			draw_layer = NULL;
		}
		return ;
	}
	if( event.Type==capture::SendAsFile || (event.Type==DuiLib::UIEVENT_DBLCLICK&& ::PtInRect(&cur_sel_,event.ptMouse)) )
	{
		if( savepath_.IsEmpty() )
			savepath_ = DoSave(false);
		if( !savepath_.IsEmpty() )
		{
			CopyToClipboard(savepath_);
			CopyBitmapToClipboard();
			::PostMessage(GetManager()->GetPaintWindow(),WM_CLOSE,0,0);
		}
		return ;
	}
	if( __super::OnEvent(&event) ) 
		__super::DoEvent(event);
}
CAtlString ScreenUI::DoSave(bool bShowDlg)
{
	CAtlString fileName;
	CAtlString strTmpPath;
	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	fileName.Format(_T("%4d%02d%02d%02d%02d%02d.png"),sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond);
	if( bShowDlg )
	{
		CAtlString strExt = _T("png (*.png)|*.png|jpg (*.jpg)|*.jpg|bmp (*.bmp)|*.bmp||");
		CustomUI::CFileDialog DlgFile(false,_T("*.png"),fileName,0,strExt,GetManager()->GetPaintWindow(),0,TRUE);
		DlgFile.SetShowName(fileName);
		if(IDOK==DlgFile.DoModal() )
		{
			strTmpPath = DlgFile.GetPathName();
		}
		else
			return strTmpPath;
	}
	else
	{
		strTmpPath.Format(_T("%sSnapShotTemp"),GetAppPath().GetString());
		if (!IsPathFind(strTmpPath))
		{
			::CreateDirectory(strTmpPath.GetBuffer(0),NULL);			
		}
		strTmpPath += _T("\\");
		strTmpPath += fileName;
	}
	if( !layers_.save(cur_screen_,cur_sel_,strTmpPath) )
	{
		::MessageBox(NULL,_T("保存失败，未识别扩展名"),_T("提示"),MB_OK|MB_ICONERROR);
		return _T("");
	}
	return strTmpPath;
}
void ScreenUI::CopyBitmapToClipboard()
{
	HBITMAP	bit=NULL;
	Gdiplus::Bitmap *build = layers_.buildClip(cur_screen_,cur_sel_);
	if( Gdiplus::Ok==build->GetHBITMAP(NULL,&bit) )
	{
		BOOL bOpen = ::OpenClipboard( GetManager()->GetPaintWindow() );
		if( !bOpen )
			return ;
		HDIB dib = BitmapToDIB(bit,NULL);
		::SetClipboardData(CF_BITMAP,bit);
		::SetClipboardData(CF_DIB,dib);
		::CloseClipboard();
		::DeleteObject((HGDIOBJ )bit);
	}
}
void ScreenUI::CopyToClipboard(const CAtlString& file)
{
	DWORD dwLength = (file.GetLength()+2)*sizeof(TCHAR);
	DWORD dwBufferSize = dwLength+sizeof(DROPFILES);
	BOOL bOpen = OpenClipboard( GetManager()->GetPaintWindow() );
	if( !bOpen )
		return ;
	BOOL bEmpty = EmptyClipboard();
	HGLOBAL hMem = GlobalAlloc(GMEM_SHARE|GMEM_ZEROINIT,dwBufferSize);
	if( NULL!=hMem )
	{
		LPSTR lpGblData = (char *)GlobalLock(hMem);

		DROPFILES *pSlxDropFiles = (DROPFILES*)(void*)lpGblData;

		if(NULL!=pSlxDropFiles)
		{
			pSlxDropFiles->fNC = FALSE;
			pSlxDropFiles->pt.x = 0;
			pSlxDropFiles->pt.y = 0;
			pSlxDropFiles->fWide = TRUE;
			pSlxDropFiles->pFiles = sizeof(DROPFILES);
			_tcscpy((LPTSTR)((LPBYTE)lpGblData+sizeof(DROPFILES)),file.GetString());
		}
		GlobalUnlock(hMem);
		if(!SetClipboardData(CF_HDROP,hMem))
			GlobalFree(hMem);
	}
	CloseClipboard();
}
void ScreenUI::UpdateToolsPos()
{
	DuiLib::CControlUI *ui = FindSubControl(_T("tools"));
	RECT	rcTmp=cur_sel_;
	if( !ui )
		return ;
	if( (GetPos().bottom-cur_sel_.bottom) <= ui->GetFixedHeight() )
	{
		rcTmp.top	 = cur_sel_.top;
		rcTmp.bottom = rcTmp.top + ui->GetFixedHeight();
	}
	else
	{
		rcTmp.top	 = cur_sel_.bottom;
		rcTmp.bottom = rcTmp.top + ui->GetFixedHeight();
	}
	if( rcTmp.right==GetPos().left )
	{
		rcTmp.left = GetPos().left;
		rcTmp.right= rcTmp.left + ui->GetFixedWidth();
	}
	else
	{
		if( (cur_screen_->GetWidth()-rcTmp.right) < ui->GetFixedWidth()/2 )
			rcTmp.left = rcTmp.right-ui->GetFixedWidth();
		else
			rcTmp.right = rcTmp.left+ui->GetFixedWidth();
	}
	ui->SetPos(rcTmp);
}
void ScreenUI::VisibleTrackDot(bool bV)
{
	DuiLib::CControlUI *ui=NULL;
	if( ui=FindSubControl(TRACK_TL) )
		ui->SetVisible(bV);
	if( ui=FindSubControl(TRACK_T) )
		ui->SetVisible(bV);
	if( ui=FindSubControl(TRACK_TR) )
		ui->SetVisible(bV);
	if( ui=FindSubControl(TRACK_BL) )
		ui->SetVisible(bV);
	if( ui=FindSubControl(TRACK_B) )
		ui->SetVisible(bV);
	if( ui=FindSubControl(TRACK_BR) )
		ui->SetVisible(bV);
	if( ui=FindSubControl(TRACK_L) )
		ui->SetVisible(bV);
	if( ui=FindSubControl(TRACK_R) )
		ui->SetVisible(bV);
}
void ScreenUI::UpdateTrackDotPos()
{
	DuiLib::CControlUI *ui=NULL;
	RECT	rcTmp=cur_sel_;
	if( ui = FindSubControl(TRACK_TL) )
	{
		rcTmp.left   = cur_sel_.left-ui->GetFixedWidth()/2;
		rcTmp.right  = rcTmp.left	+ui->GetFixedWidth();
		rcTmp.top	 = cur_sel_.top -ui->GetFixedHeight()/2;
		rcTmp.bottom = rcTmp.top    +ui->GetFixedHeight();
		ui->SetPos(rcTmp);
		rcTmp=cur_sel_;
	}
	if( ui=FindSubControl(TRACK_T) )
	{
		rcTmp.left  += (cur_sel_.right-cur_sel_.left)/2 - ui->GetFixedWidth()/2;
		rcTmp.right = rcTmp.left	+ ui->GetFixedWidth();
		rcTmp.top	 = cur_sel_.top -ui->GetFixedHeight()/2;
		rcTmp.bottom = rcTmp.top    +ui->GetFixedHeight();
		ui->SetPos(rcTmp);
		rcTmp=cur_sel_;
	}
	if( ui=FindSubControl(TRACK_TR) )
	{
		rcTmp.right	 = cur_sel_.right+ui->GetFixedWidth()/2;
		rcTmp.left   = rcTmp.right   -ui->GetFixedWidth();
		rcTmp.top	 = cur_sel_.top -ui->GetFixedHeight()/2;
		rcTmp.bottom = rcTmp.top    +ui->GetFixedHeight();
		ui->SetPos(rcTmp);
		rcTmp=cur_sel_;
	}
	if( ui=FindSubControl(TRACK_BL) )
	{
		rcTmp.left   = cur_sel_.left-ui->GetFixedWidth()/2;
		rcTmp.right  = rcTmp.left	 + ui->GetFixedWidth();
		rcTmp.bottom = cur_sel_.bottom+ui->GetFixedHeight()/2;
		rcTmp.top	 = rcTmp.bottom   -ui->GetFixedHeight();		
		ui->SetPos(rcTmp);
		rcTmp=cur_sel_;
	}
	if( ui=FindSubControl(TRACK_B) )
	{
		rcTmp.left  += (cur_sel_.right-cur_sel_.left)/2 - ui->GetFixedWidth()/2;
		rcTmp.right = rcTmp.left	+ ui->GetFixedWidth();
		rcTmp.bottom = cur_sel_.bottom+ui->GetFixedHeight()/2;
		rcTmp.top	 = rcTmp.bottom   -ui->GetFixedHeight();		
		ui->SetPos(rcTmp);
		rcTmp=cur_sel_;
	}
	if( ui=FindSubControl(TRACK_BR) )
	{
		rcTmp.right	 = cur_sel_.right+ui->GetFixedWidth()/2;
		rcTmp.left   = rcTmp.right   -ui->GetFixedWidth();
		rcTmp.bottom = cur_sel_.bottom+ui->GetFixedHeight()/2;
		rcTmp.top	 = rcTmp.bottom   -ui->GetFixedHeight();		
		ui->SetPos(rcTmp);
		rcTmp=cur_sel_;
	}
	if( ui=FindSubControl(TRACK_L) )
	{
		rcTmp.left   = cur_sel_.left-ui->GetFixedWidth()/2;
		rcTmp.right  = rcTmp.left	+ui->GetFixedWidth();
		rcTmp.top	 += (cur_sel_.bottom-cur_sel_.top)/2-ui->GetFixedHeight()/2;
		rcTmp.bottom = rcTmp.top+ui->GetFixedHeight();
		ui->SetPos(rcTmp);
		rcTmp=cur_sel_;
	}
	if( ui=FindSubControl(TRACK_R) )
	{
		rcTmp.right	 = cur_sel_.right+ui->GetFixedWidth()/2;
		rcTmp.left   = rcTmp.right   -ui->GetFixedWidth();
		rcTmp.top	 += (cur_sel_.bottom-cur_sel_.top)/2-ui->GetFixedHeight()/2;
		rcTmp.bottom = rcTmp.top+ui->GetFixedHeight();
		ui->SetPos(rcTmp);
		rcTmp=cur_sel_;
	}
}
RECT ScreenUI::FindWinRect(const POINT &pt)
{	
	RECT rc={0,0,0,0};
	for(size_t nI=0;nI < top_wins_.size();nI++)
	{
		RECT hWin = top_wins_[nI];
		if( ::PtInRect(&hWin,pt) )
			return hWin;
	}
	return rc;
}