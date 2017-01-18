#pragma once

#include "Const.h"
#include "Layers.h"
#include "Cursor.h"
#include "PaintPlusUI.h"

class ToolsbarUI;
class ScreenUI : public Layout
{
public:
	ScreenUI();
	virtual ~ScreenUI();
	void Init();
	const RECT& GetPos() const;
protected:
	//duilib
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void Event(DuiLib::TEventUI& event);
	void SetPos(RECT rc);
protected:
	RECT	FindWinRect(const POINT &pt);
	UINT	GetControlFlags() const;
	bool	CanMove( const RECT& rc );
	void	Move(long x,long y);
	void	UpdateTrackDotPos();
	void	UpdateToolsPos();
	void	TrackDotMove(const DuiLib::TEventUI& ev);
	void	VisibleTrackDot(bool bV);
	bool	PtInSel(POINT pt);
	CAtlString	DoSave(bool bShowDlg);
	void	CopyToClipboard(const CAtlString& file);
	void  CopyBitmapToClipboard();
private:
	CAtlString					savepath_;
	DuiLib::CControlUI	*ui_move_;		//拖拽的控件
	RECT							rcTrue_;		//通过ms得到client rc没有包含任务栏因此引入此值
	Layers						layers_;		//绘制层
	Layers::Layer				*draw_layer;	//绘图层

	Cursor						cursor_;		//指针
	Gdiplus::Bitmap			*cur_screen_;	//当前屏幕bitmap
	RECT							cur_sel_;		//当前选择区域
	std::vector<RECT>	top_wins_;		//所有顶层窗口
	long							draw_states_;	//绘图状态
	POINT						cur_pt_;		//缓存的当前指针用于过滤无用的MOUSEMOVE事件提高运行效率
	capture::DRAW_CAP	draw_cap_;		//绘制图形
	ToolsbarUI				*tools_;			
	//
	DuiLib::CControlUI	*showrc_;		//显示rect区域
	LONG						lclicktm_;		//鼠标按下时间
};