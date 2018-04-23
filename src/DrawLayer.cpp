#include "stdafx.h"
#include "DrawLayer.h"
#include "GdiAlpha.h"
#include "Log.h"

int nVirtualWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN) ;
int nVirtualHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) ;
int nVirtualLeft = GetSystemMetrics(SM_XVIRTUALSCREEN) ;
int nVirtualTop = GetSystemMetrics(SM_YVIRTUALSCREEN) ;


DrawEllipse::DrawEllipse(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
{

}
DrawEllipse::~DrawEllipse()
{

}
void DrawEllipse::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE && bitmap_ )
	{
		graphic()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		clear();
		graphic()->DrawEllipse(pen,rcDraw_);
		paint_range_ = Plus2Rc(rcDraw_);
	}
}
/*-------------------------------------------------------------------------*/
DrawRect::DrawRect(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
{

}
DrawRect::~DrawRect()
{

}
void DrawRect::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE && bitmap_ )
	{
		graphic()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		clear();
		graphic()->DrawRectangle(pen,rcDraw_);
	}
}
/*-------------------------------------------------------------------------*/
DrawPoints::DrawPoints(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
	, tmLast_(0)
{
	pen_	= NULL;
	ZeroMemory(&mp_in, sizeof(mp_in));
	ZeroMemory(mp_out, sizeof(mp_out));
}
DrawPoints::~DrawPoints()
{

}
void DrawPoints::paint(Gdiplus::Graphics *g,RECT rc)
{
	__super::paint(g,rc);
}
void DrawPoints::checklimit(Gdiplus::Point &pt)
{
	if( pt.X < rclimit_.left )
		pt.X = rclimit_.left;
	if( pt.X > rclimit_.right )
		pt.X = rclimit_.right;

	if( pt.Y < rclimit_.top )
		pt.Y = rclimit_.top;
	if( pt.Y > rclimit_.bottom )
		pt.Y = rclimit_.bottom;
}
void DrawPoints::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	Gdiplus::Point pt(ev->ptMouse.x,ev->ptMouse.y);
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_BUTTONDOWN )
	{
		tmLast_ = ::GetMessageTime();
		ptLast_.x = ev->ptMouse.x;
		ptLast_.y = ev->ptMouse.y;
		::ClientToScreen(win,&ptLast_);

		paint_range_.left  = pt.X;
		paint_range_.top   = pt.Y;
		paint_range_.right = pt.X + pen->GetWidth();
		paint_range_.bottom= pt.Y + pen->GetWidth();
		pen_ = pen;
		pen_->SetLineJoin(Gdiplus::LineJoinRound);
		pen_->SetEndCap(Gdiplus::LineCapRound);
		pt_track[0].X = ev->ptMouse.x;
		pt_track[0].Y = ev->ptMouse.y;
		graphic()->DrawLine(pen,ev->ptMouse.x,ev->ptMouse.y,pt_track[0].X,pt_track[0].Y);
		::InvalidateRect(win,&paint_range_,FALSE);
	}
	else if( ev->Type==DuiLib::UIEVENT_BUTTONUP )
	{
		::InvalidateRect(win,&rclimit_,FALSE);
	}
	else if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE )
	{
		int mode = GMMP_USE_DISPLAY_POINTS;
		POINT ptScreen = {ev->ptMouse.x,ev->ptMouse.y};
		::ClientToScreen(win,&ptScreen);
		mp_in.x		= ptScreen.x;
		mp_in.y		= ptScreen.y;
		mp_in.time	= ::GetMessageTime();
		int cpt = ::GetMouseMovePointsEx(sizeof(MOUSEMOVEPOINT),&mp_in,mp_out,64,mode);
		int nIn = 0;
		if( cpt==-1 )
		{
			cpt = 1;
			if( ev->ptMouse.x==ptLast_.x && ev->ptMouse.y==ptLast_.y )
			{
				pt_track[0].X = ev->ptMouse.x;
				pt_track[0].Y = ev->ptMouse.y;
				pt_track[0].X += pen_->GetWidth();
				pt_track[0].Y += pen_->GetWidth();
				checklimit(pt_track[0]);
				graphic()->DrawLine(pen_,ev->ptMouse.x,ev->ptMouse.y,pt_track[0].X,pt_track[0].Y);
			}
			else
			{
				pt_track[0].X = ptLast_.x;
				pt_track[0].Y = ptLast_.y;
				pt_track[1].X = ev->ptMouse.x;
				pt_track[1].Y = ev->ptMouse.y;
				graphic()->DrawCurve(pen,pt_track,2);
			}
			tmLast_ = mp_in.time;
			ptLast_ = ev->ptMouse;
		}
		else
		{
			ptScreen = ptLast_;
			::ClientToScreen(win,&ptScreen);
			for(nIn = 0;nIn < cpt;nIn++) 
			{
				if (mp_out[nIn].time < tmLast_)
					break;
				if (mp_out[nIn].time == tmLast_	 &&
					mp_out[nIn].x == ptScreen.x &&
					mp_out[nIn].y == ptScreen.y )
					break;
			}
			nIn+=2;
			cpt = nIn;
			while (--nIn >= 0) 
			{
				ptScreen.x = mp_out[nIn].x;
				ptScreen.y = mp_out[nIn].y;
				::ScreenToClient(win,&ptScreen);

				pt_track[nIn].X = ptScreen.x;
				pt_track[nIn].Y = ptScreen.y;

				checklimit(pt_track[nIn]);
				ptLast_ = ptScreen;
			}
			tmLast_ = mp_in.time;
			graphic()->DrawCurve(pen_,pt_track,cpt);
		}
	}
}
/*-------------------------------------------------------------------------*/
DrawArrow::DrawArrow(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
{
	pen_limit_w_ = 10;
}
DrawArrow::~DrawArrow()
{

}
void DrawArrow::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	Gdiplus::Point pt(ev->ptMouse.x,ev->ptMouse.y);
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_BUTTONDOWN )
	{
		pen_limit_w_ = pen->GetWidth();
		pen->SetStartCap(Gdiplus::LineCapArrowAnchor);
	}
	if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE )
	{
		this->clear();
		long nX = abs(ev->ptMouse.x-cur_pt_.x);
		long nY = abs(ev->ptMouse.y-cur_pt_.y);
		graphic()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		graphic()->DrawLine(pen,pt.X,pt.Y,cur_pt_.x,cur_pt_.y);
	}
}
/*-------------------------------------------------------------------------*/
DrawMosaic::DrawMosaic(long nW,long nH,Layers *layers)
	: DrawBase<Layers::Layer>(nW,nH,layers)
{
	//将所有内容绘制到图层上
	nRadom_=0;
 	mapBit_ = layers->getBuild();
}
DrawMosaic::~DrawMosaic()
{

}
void DrawMosaic::swip(BitmapData *src,BitmapData *dst,int mosaic_w,int mosaic_h)
{
	//src和dst必须相同否则此算法无效
	//处理argb
	if(mosaic_h<=2||mosaic_w<=2) {
		//过小无意义
		return ;
	}
	if(src->Height!=dst->Height || src->Width!=dst->Width) {
		//位图不同大小不同
		return ;
	}
	if(src->PixelFormat!=dst->PixelFormat){
		//格式不同
		return ;
	}
	int xNum = src->Width/mosaic_w + src->Width%mosaic_w;
	int yNum = src->Height/mosaic_h+ src->Height%mosaic_h;
	std::vector<RECT> ranges;
	//生成二维矩阵
	RECT rcZero={0,0,0,0};
	ranges.assign(yNum*xNum,rcZero);
	for(int y=0;y < yNum;y++){
		RECT rcTmp={0,0,0,0};
		rcTmp.top = y*mosaic_h;
		rcTmp.bottom = rcTmp.top+mosaic_h;
		if(rcTmp.bottom>src->Height){
			rcTmp.bottom = src->Height;
		}
		for(int x=0;x < xNum;x++){
			rcTmp.left = x*mosaic_w;
			rcTmp.right= rcTmp.left+mosaic_w;
			if(rcTmp.right>src->Width){
				rcTmp.right = src->Width;
			}
			ranges[(y*xNum)+x]=rcTmp;
		}
		memset(&rcTmp,0,sizeof(RECT));
	}
	//填充二维矩阵fill argb
	UINT bits    = GetPixelFormatSize(src->PixelFormat);
	int  pixByte = bits/8;

	byte *curScan = (byte*)src->Scan0;
	int  curStride= src->Stride;
	int  offset   = curStride-src->Width*pixByte;
	int  dataW	  = src->Width;

	byte *dstScan = (byte*)dst->Scan0;
	int  dstStride= dst->Stride;
	int  dstOffset= dstStride-dst->Width*pixByte;
	srand((unsigned)time(NULL)); 
	//32 bgra
	for(int i=0;i<ranges.size();i++){
		RECT rc = ranges[i];
		int H = rc.bottom-rc.top;
		int W = rc.right -rc.left;
		byte *scan0 = curScan+curStride*rc.top+rc.left*pixByte;
		std::vector<APPEAR> colors;
		for(int y=0;y<H;y++){
			for(int x=0;x<W;x++){
				//统计颜色频率
				APPEAR appear;
				appear.color.value= *(long*)scan0;
				appear.freq  = 0;
				colors.push_back(appear);
				scan0+=pixByte;
			}
			//跳过冗余区域
			scan0 += offset+(dataW-rc.right)*pixByte;
			//偏移
			scan0 += rc.left*pixByte;
		}
		//填充该区域
		byte *scan1 = dstScan+dstStride*rc.top+rc.left*pixByte;
		for(int y=0;y<H;y++){
			for(int x=0;x<W;x++){
				int rIndex = rand()%colors.size();
				APPEAR now = colors[rIndex];
				memcpy(scan1,&now.color.value,pixByte);
				scan1+=pixByte;				
			}
			scan1 += dstOffset+(dataW-rc.right)*pixByte;
			scan1 += rc.left*pixByte;
		}
	}
	return ;
}
void DrawMosaic::handle(BitmapData *src,BitmapData *dst,int mosaic_w,int mosaic_h,bool radeom)
{
	//src和dst必须相同否则此算法无效
	//处理argb
	if(mosaic_h<=2||mosaic_w<=2) {
		//过小无意义
		return ;
	}
	if(src->Height!=dst->Height || src->Width!=dst->Width) {
		//位图不同大小不同
		return ;
	}
	if(src->PixelFormat!=dst->PixelFormat){
		//格式不同
		return ;
	}
	int xNum = src->Width/mosaic_w + src->Width%mosaic_w;
	int yNum = src->Height/mosaic_h+ src->Height%mosaic_h;
	std::vector<RECT> ranges;
	//生成二维矩阵
	RECT rcZero={0,0,0,0};
	ranges.assign(yNum*xNum,rcZero);
	for(int y=0;y < yNum;y++){
		RECT rcTmp={0,0,0,0};
		rcTmp.top = y*mosaic_h;
		rcTmp.bottom = rcTmp.top+mosaic_h;
		if(rcTmp.bottom>src->Height){
			rcTmp.bottom = src->Height;
		}
		for(int x=0;x < xNum;x++){
			rcTmp.left = x*mosaic_w;
			rcTmp.right= rcTmp.left+mosaic_w;
			if(rcTmp.right>src->Width){
				rcTmp.right = src->Width;
			}
			ranges[(y*xNum)+x]=rcTmp;
		}
		memset(&rcTmp,0,sizeof(RECT));
	}
	//填充二维矩阵fill argb
	UINT bits    = GetPixelFormatSize(src->PixelFormat);
	int  pixByte = bits/8;
	
	byte *curScan = (byte*)src->Scan0;
	int  curStride= src->Stride;
	int  offset   = curStride-src->Width*pixByte;
	int  dataW	  = src->Width;

	byte *dstScan = (byte*)dst->Scan0;
	int  dstStride= dst->Stride;
	int  dstOffset= dstStride-dst->Width*pixByte;

	//32 bgra
	for(int i=0;i<ranges.size();i++){
		RECT rc = ranges[i];
		int H = rc.bottom-rc.top;
		int W = rc.right -rc.left;
		byte *scan0 = curScan+curStride*rc.top+rc.left*pixByte;
		//byte *scan0 = dstScan+dstStride*rc.top+rc.left*pixByte;
		RATE rates;
		for(int y=0;y<H;y++){
			for(int x=0;x<W;x++){
				//统计颜色频率
				APPEAR appear;
				appear.color.value= *(long*)scan0;
				appear.freq  = 0;
				RATE::iterator it = rates.find(appear.color.value);
				if(it==rates.end()){
					rates[appear.color.value] = appear;
				}else{
					it->second.freq += 1;
				}
				//scan0[0]=(0xff>>i);
				//scan0[1]=(0xf0>>i);
				//scan0[2]=(0x12<<i);
				//scan0[3]=0xff; 
				scan0+=pixByte;
			}
			//跳过冗余区域
			scan0 += offset+(dataW-rc.right)*pixByte;
			//scan0 += dstOffset+(dataW-rc.right)*pixByte;
			//偏移
			scan0 += rc.left*pixByte;
		}
		APPEAR max;	//出现频率最高的色值
		max.color.value=0;
		max.freq =0;
		for(RATE::iterator it=rates.begin();it!=rates.end();it++){
			if(it->second.freq>max.freq)
				max = it->second;
		}
		//max.color.value=0xff00ff00;
		//填充该区域
		byte *scan1 = dstScan+dstStride*rc.top+rc.left*pixByte;
		for(int y=0;y<H;y++){
			for(int x=0;x<W;x++){
				memcpy(scan1,&max.color.value,pixByte);
				//scan1[0]=(0xff>>i);
				//scan1[1]=(0xf0>>i);
				//scan1[2]=(0x12<<i);
				//scan1[3]=0xff;
				scan1+=pixByte;				
			}
			scan1 += dstOffset+(dataW-rc.right)*pixByte;
			scan1 += rc.left*pixByte;
		}
	}
	//Stride + top-down；- bottom-up
	//每行扫描线都会4byte对齐，如果不对齐那么补0
	//为了跳过补位(24rgb)
	//32Argb 8 bits each;alpha, red, green,blue  
	//Stride - dataIn.Width*3
	return ;
}
void DrawMosaic::drawRect()
{
	BitmapData backData,frontData;
	Gdiplus::Status op1 = mapBit_->LockBits(&rcDraw_,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&backData);
	Gdiplus::Status op2 = bitmap_->LockBits(&rcDraw_,Gdiplus::ImageLockModeWrite,PixelFormat32bppARGB,&frontData);
	//handle(&backData,&frontData,8,8);
	int tile=8,maxV=0;
	maxV = rcDraw_.Width>rcDraw_.Height?rcDraw_.Width:rcDraw_.Height;
	//对高分屏处理
	if(maxV>=500 && maxV<=2500){
		tile = (maxV/10)/4;
	}
	swip(&backData,&frontData,tile,tile);
	op1 = mapBit_->UnlockBits(&backData);
	op2 = bitmap_->UnlockBits(&frontData);
}
void DrawMosaic::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	__super::draw(win,ev,pen,br,ft);
	if( ev->Type==DuiLib::UIEVENT_BUTTONDOWN && ev->pSender )
	{

	}
	if( ev->Type==DuiLib::UIEVENT_MOUSEMOVE )
	{
		graphic()->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		clear();
		graphic()->DrawRectangle(pen,rcDraw_);
	}
	if( ev->Type==DuiLib::UIEVENT_BUTTONUP )
	{
		clear();
		drawRect();
	}
}
/*-------------------------------------------------------------------------*/
DrawTexture::DrawTexture(long nW,long nH)
	: DrawBase<Layers::Layer>(nW,nH)
	, re_(NULL)
	, bStartEdit_(false)
{

}
DrawTexture::~DrawTexture()
{
	
}
void DrawTexture::paintText(RECT rc)
{
	if( re_ )
	{
		HDC hdc = graphic()->GetHDC();
		re_->OnDraw(hdc,re_->GetPos());
		graphic()->ReleaseHDC(hdc);
	}
}
void DrawTexture::paint(Gdiplus::Graphics *g,RECT rc)
{
	if( bStartEdit_ )
	{
		paintText(rc);
	}
	else if( re_ && bStartEdit_!=re_->IsVisible() )
	{
		re_->SetVisible(false);
	}
	Layers::Layer::paint(g,rc);
}
AutoRichDocUI*	DrawTexture::GetRE(DuiLib::CPaintManagerUI *pm)
{
	if( re_==NULL&&pm )
	{
		re_ =  new AutoRichDocUI;
		DuiLib::CContainerUI *root = static_cast<DuiLib::CContainerUI*>(pm->GetRoot());
		root->Add(re_);

		DuiLib::TFontInfo *fInfo = pm->GetFontInfo(re_->GetFont());

		re_->SetAttribute(_T("norborder"),_T("2"));
		re_->SetAttribute(_T("norbordercolor"),_T("#ffabcdef"));
		re_->SetAttribute(_T("bkcolor"),_T("#ffffffff"));
		re_->AddPaintState(AutoRichDocUI::NorBorder);
		re_->SetLimitRc( rclimit_ );
		re_->SetExternRc(DuiLib::CRect(0,0,ftInfo_.size+5,ftInfo_.size+5));
		re_->SetFont(ftInfo_);
		re_->UpdateFont();
		re_->OnChange += DuiLib::MakeDelegate(this,&DrawTexture::InputChange);
		re_->OnKillFocus += DuiLib::MakeDelegate(this,&DrawTexture::KillFocus);
	}
	return re_;
}
bool	DrawTexture::InputChange(void*)
{
	clear();
	return false;
}
bool	DrawTexture::KillFocus(void*)
{
	clear();
	bStartEdit_ = false;
	//取消边框
	if( re_ )
	{
		re_->SetAttribute(_T("norborder"),_T("0"));

		Gdiplus::RectF pos;
		CAtlString	strTxt = re_->GetText().GetData();
		re_->SetAttribute(_T("norborder"),_T("0"));
		strTxt.Replace('\r','\n');
		pos.X = re_->GetPos().left;
		pos.Y = re_->GetPos().top;
		pos.Width = RcWidth(re_->GetPos()) + 2;
		pos.Height= RcHeight(re_->GetPos())+ 2;
		
		Gdiplus::FontFamily	ff(ftInfo_.name);
		Gdiplus::Font		ft(&ff,ftInfo_.size);
		Gdiplus::SolidBrush sb(ftInfo_.color);
		graphic()->SetTextRenderingHint(TextRenderingHint(TextRenderingHintAntiAlias));
		graphic()->DrawString(strTxt, -1, &ft, pos, NULL, &sb);

		DuiLib::CContainerUI *root = static_cast<DuiLib::CContainerUI*>(re_->GetManager()->GetRoot());
		root->Remove(re_);
		re_ = NULL;
	}
	return false;
}
void DrawTexture::draw(HWND win,DuiLib::TEventUI *ev,Gdiplus::Pen *pen,Gdiplus::SolidBrush *br,Gdiplus::Font *ft)
{
	__super::handleMsg(win,ev,ftInfo_.size,ftInfo_.size);
	if( ev->Type==DuiLib::UIEVENT_BUTTONDOWN && ev->pSender )
	{
		if( bStartEdit_==false )
		{
			if( re_==NULL )
			{
				re_ = GetRE(ev->pSender->GetManager());
				Gdiplus::Color clr;
				pen->GetColor(&clr);
				re_->SetTextColor(clr.GetValue());
			}
			else
			{
				re_->Clear();
				re_->SetVisible(true);
			}
			RECT rcPos={ev->ptMouse.x,ev->ptMouse.y,ev->ptMouse.x+ftInfo_.size,ev->ptMouse.y+ftInfo_.size};
			rcPos.left  -= re_->GetExternRc().left;
			rcPos.top   -= re_->GetExternRc().top;
			rcPos.right  += re_->GetExternRc().right;
			rcPos.bottom += re_->GetExternRc().bottom;
			if( rcPos.bottom > rclimit_.bottom )
			{
				long nOff = rcPos.bottom - rclimit_.bottom;
				::OffsetRect(&rcPos,0,-nOff);
			}
			if( rcPos.right > rclimit_.right )
			{
				long nOff = rcPos.right - rclimit_.right;
				::OffsetRect(&rcPos,-nOff,0);
			}
			re_->SetPos(rcPos);
			re_->SetFocus();
			bStartEdit_ = true;
			::InvalidateRect(win,&paint_range_,FALSE);
		}
	}
}
