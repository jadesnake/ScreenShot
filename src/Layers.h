//透明图层
#pragma once

#include "const.h"
class Layers
{
public:
	Layers(void);
	virtual ~Layers(void);
	class Layer
	{
	public:
		Layer(long nW,long nH);
		virtual ~Layer();
		virtual void draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br=NULL,Gdiplus::Font *ft=NULL)
		{			}
		virtual void	paint(Gdiplus::Graphics *g,RECT rc);
		virtual void	limitRc(const RECT &rc );
		virtual void	font(const capture::FontInfo &ftInfo);

		Gdiplus::Graphics*		graphic();
		Gdiplus::GraphicsPath*	path();
		void clear();
	public:
		POINT pt_;								//x,y
		capture::FontInfo		ftInfo_;		//
		Gdiplus::Bitmap			*bitmap_;		//图层
		Gdiplus::Pen			*pen_;			//		
		bool					bVisible_;		//显示或隐藏
	protected:
		Gdiplus::GraphicsPath	*path_;			//
		Gdiplus::Graphics		*graphic_;		//
		RECT					rclimit_;
		friend Layers;
	};
	//新建一层
	Layer*	push(capture::DRAW_CAP cap,long nW,long nH);
	//当前图层
	Layer*	current();
	//绘制所有图层
	virtual void	paint(Gdiplus::Graphics *g,RECT rc);
	//撤销
	virtual bool undo();
	//恢复
	virtual bool redo();
	void	clear();
	void	move(long nX,long nY);
	long	countLayerVisible();
	Layer*	getLayerAtLast();
	Gdiplus::Bitmap* build(Gdiplus::Bitmap *draw,RECT rc);
	bool	save(Gdiplus::Bitmap *draw,RECT rc,const CAtlString &str);
protected:
	typedef std::vector<Layer*>	Ones;
private:
	Ones			all_;		//所有的图层
	Ones			lst_redo_;	//恢复列表
	Layer			*cur_;		//当前所使用的图层
	Gdiplus::Bitmap *cur_build_;//当前生成的bitmap图
};