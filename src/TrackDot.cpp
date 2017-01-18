#include "StdAfx.h"
#include "TrackDot.h"
#include "Const.h"
TrackDot::TrackDot(void)
{
	memset(&cur_pt_,-1,sizeof(POINT));
}

TrackDot::~TrackDot(void)
{
	
}
void TrackDot::DoEvent(DuiLib::TEventUI& event)
{
	if( event.Type == UIEVENT_SETCURSOR )
	{
		if( 0==GetName().CompareNoCase(TRACK_T) || 0==GetName().CompareNoCase(TRACK_B) )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		}
		else if( 0==GetName().CompareNoCase(TRACK_L) || 0==GetName().CompareNoCase(TRACK_R) )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
		}
		else if( 0==GetName().CompareNoCase(TRACK_TL) || 0==GetName().CompareNoCase(TRACK_BR) )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
		}
		else if( 0==GetName().CompareNoCase(TRACK_BL) || 0==GetName().CompareNoCase(TRACK_TR) )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
		}
		return;
	}
	if( event.Type==UIEVENT_BUTTONDOWN )
	{
		cur_pt_ = event.ptMouse;
		return ;
	}
	if( event.Type==UIEVENT_BUTTONUP )
	{
		DuiLib::CControlUI *uiRoot = GetManager()->GetRoot();		
		event.pSender = this;
		event.Type	 = capture::TRACK_MOVE_END;
		uiRoot->Event(event);
	}
	if( event.Type==UIEVENT_MOUSEMOVE && IS_HAVE(event.wParam,MK_LBUTTON) )
	{
		if( event.ptMouse.x==cur_pt_.x && event.ptMouse.y==cur_pt_.y )
			return ;
		POINT pt;
		pt.x = event.ptMouse.x - cur_pt_.x;
		pt.y = event.ptMouse.y - cur_pt_.y;
		memcpy(&cur_pt_,&event.ptMouse,sizeof(POINT));
		if(pt.x==0 && pt.y==0)
			return ;
		DuiLib::CControlUI *uiRoot = GetManager()->GetRoot();		
		event.pSender = this;
		event.Type	 = capture::TRACK_MOVE;
		event.ptMouse= pt;
		uiRoot->Event(event);
	}
	CLabelUI::DoEvent(event);
}
