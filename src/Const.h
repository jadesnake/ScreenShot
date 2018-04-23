#pragma once

namespace capture
{
	typedef enum
	{
		AUTO_SEL = 1,	 //自动选择
		MAN_SEL	 = 1<<1, //用户选择		
		MOV_SEL	 = 1<<2, //移动选择区域
		DRAW_SEL = 1<<3, //绘图状态
	}STATE;
	typedef enum 
	{
		Rectangle,		//矩形
		Ellipse,		//椭圆
		Line,			//线条
		Brush,			//画刷
		Arrow,			//箭头
		Gauss,			//模糊
		Text,			//文本
	}DRAW_CAP;			//绘制的图形
};
#define IS_HAVE(a,b)	((a&b)==b)
#define Rc2plus(a)		Gdiplus::Rect(a.left,a.top,a.right-a.left,a.bottom-a.top)
#define RcAHideB(a,b)	(a.left < b.left && a.top < b.top && a.right>b.right && a.bottom>b.bottom)

#define RcWidth(a)		(a.right-a.left)
#define RcHeight(a)		(a.bottom-a.top)

#define TRACK_T			_T("top")
#define TRACK_B			_T("bottom")
#define TRACK_L			_T("left")
#define TRACK_R			_T("right")
#define TRACK_TL		_T("t_left")
#define TRACK_TR		_T("t_right")
#define TRACK_BL		_T("b_left")
#define TRACK_BR		_T("b_right")

//默认背景色
#define DEF_BACK_COLOR   Gdiplus::Color(200,80,80,80)
#define DEF_FRAME_COLOR  Gdiplus::Color(180,120,180,255)
#define DEF_PEN_COLOR	 Gdiplus::Color(255,255,0,0)
#define DEF_FRAME_WIDTH	 5	
namespace capture
{
	typedef enum EVENT_UI
	{
		TRACK_MOVE = UIEVENT__LAST,		//用户移动锚点
		TRACK_MOVE_END,					//用户移动锚点结束
		PrepareDraw,					//准备绘图
		InitTools,						//初始化工具条
		RedoLayer,
		UndoLayer,
		CloseWindow,
		SaveAsFile,
		SendAsFile,
	};
	class FontInfo
	{
	public:
		FontInfo()
		{
			name = _T("宋体");
			size = 12;
			bold = false;
			underline = false;
			italic = false;
			color  = 0;
		}
		CAtlString name;
		long	   size;
		bool	   bold;
		bool	   underline;
		bool	   italic;
		DWORD	   color;
	};
}

RECT Plus2Rc(Gdiplus::Rect &rc);
void GetShowSize(HDC dc,int &nW,int &nH);
Gdiplus::Bitmap* BitBltDesktop(HWND win);