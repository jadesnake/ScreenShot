#include "StdAfx.h"
#include "Cursor.h"
#include "Public.h"

Cursor::Cursor(void)
{
	cur_ = NULL;
}
Cursor::~Cursor(void)
{
	empty();
}
void	Cursor::empty()
{
	if(cur_)
	{
		::DestroyCursor(cur_);
		cur_ = NULL;
	}
}
void	Cursor::change(long n)
{
	if( state_==n && cur_ )
	{
		::SetCursor(cur_);
		return ;
	}
	state_ = n;
	empty();
	if( state_==capture::AUTO_SEL || IS_HAVE(state_,capture::DRAW_SEL) )
	{
		cur_ = ::LoadCursorFromFile(GetAppPath() + _T("image\\cut.cur"));
		if( cur_==NULL )
			cur_ = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
	}
	else if( IS_HAVE(state_,capture::MOV_SEL) )
	{
		cur_ = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEALL));
	}
	else if( IS_HAVE(state_,capture::AUTO_SEL) )
	{
		cur_ = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
	}
	if( cur_ )
		::SetCursor(cur_);
}