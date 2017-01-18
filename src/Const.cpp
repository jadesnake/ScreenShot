#include "stdafx.h"
#include "const.h"
#include <D3D9.h>

RECT Plus2Rc(Gdiplus::Rect &rc)
{
	RECT ret;
	ret.left  = rc.X;
	ret.top   = rc.Y;
	ret.right = rc.X+rc.Width;
	ret.bottom= rc.Y+rc.Height;
	return ret;
}
void GetShowSize(HDC dc,int &nW,int &nH){
	typedef IDirect3D9* (WINAPI *CreateDx9)(UINT);
	int nX=0,nY=0;
	OSVERSIONINFO vInfo;
	vInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&vInfo);
	if(vInfo.dwMajorVersion >= 6 && vInfo.dwMinorVersion >= 1)
	{
		IDirect3D9*	pD3D=NULL;
		D3DDISPLAYMODE	ddm;
		HMODULE hlib = LoadLibrary(_T("D3d9"));
		CreateDx9 fun = (CreateDx9)GetProcAddress(hlib,"Direct3DCreate9");
		if(!fun )
		{
			FreeLibrary(hlib);
			goto DEF_PROC;
		}
		if( (pD3D=fun(D3D_SDK_VERSION))==NULL){
			FreeLibrary(hlib);
			goto DEF_PROC;
		}
		if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm)))
		{
			pD3D->Release();
			FreeLibrary(hlib);
			goto DEF_PROC;
		}
		pD3D->Release();
		nX=ddm.Width;
		nY=ddm.Height;
		FreeLibrary(hlib);
	}
DEF_PROC:
	if(nX==0 || nY==0)
	{
		nX = ::GetDeviceCaps(dc,HORZRES);
		nY = ::GetDeviceCaps(dc,VERTRES);
	}
	nW = nX;
	nH = nY;
}