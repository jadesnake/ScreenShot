#pragma once

#include "PaintPlusUI.h"
#include "ShowRC.h"
#include "ScreenUI.h"
#include "TrackDot.h"
#include "Toolsbar.h"
#include "BrushBlock.h"
#include "ColorBlock.h"


template<typename theX>
class  BuilderCall : public theX
{
public:
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		DuiLib::CControlUI *ui=NULL;
		if( _tcscmp(pstrClass,_T("Screen"))==0 )
		{
			ui = new ScreenUI;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("ShowRC"))==0 )
		{
			ui = new ShowRC;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("TrackDot"))==0 )
		{
			ui = new TrackDot;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("OptionPlus"))==0 )
		{
			ui = new OptionPlusUI;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("ButtonPlus"))==0 )
		{
			ui = new ButtonPlusUI;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("Toolsbar"))==0 )
		{
			ui = new ToolsbarUI;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("BrushBlock"))==0 )
		{
			ui = new BrushBlockUI;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("ColorBlock"))==0 )
		{
			ui = new ColorBlockUI;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("Layout"))==0 )
		{
			ui = new Layout;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("LayoutH"))==0 )
		{
			ui = new LayoutH;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("LayoutV"))==0 )
		{
			ui = new LayoutV;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("Place")) ==0 )
		{
			ui = new PlaceUI;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("DropBox"))==0 )
		{
			ui = new DropBox;
			return ui;
		}
		if( _tcscmp(pstrClass,_T("EditUI"))==0 )
		{
			ui = new EditUI;
			return ui;
		}
		return __super::CreateControl(pstrClass);
	}
};