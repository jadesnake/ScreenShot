#pragma once

#define  MAX_ALPHABUF    1<<16
class CGdiAlpha
{
public:
	typedef struct tagALPHAINFO
	{
		BITMAP bm;
		LPBYTE lpBuf;
		RECT    rc;
		tagALPHAINFO()
		{
			lpBuf=NULL;
			rc.left=rc.top=rc.right=rc.bottom=0;
		}
	} ALPHAINFO,* LPALPHAINFO;
	static BOOL AlphaBackup(HDC hdc,LPCRECT pRect,ALPHAINFO &alphaInfo);

	static void AlphaRestore(ALPHAINFO &alphaInfo);
private:
	static BYTE s_byAlphaBack[MAX_ALPHABUF];
	static LPBYTE ALPHABACKUP(BITMAP *pBitmap,int x,int y,int cx,int cy);
	//恢复位图的Alpha通道
	static void ALPHARESTORE(BITMAP *pBitmap,int x,int y,int cx,int cy,LPBYTE lpAlpha);
};