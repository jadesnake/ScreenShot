#include "StdAfx.h"
#include "ScreenShot/ScreenShotLib.h"
#include "CaptureImage.h"

static bool s_bScreenShotOpened = false;
int __stdcall OpenScreenShot(LPCTSTR userINI)
{
	if (s_bScreenShotOpened)
	{
		return 0;
	}
	s_bScreenShotOpened = true;
	//CVirtualScreenDlg pVSDlg;
	//pVSDlg.Create(NULL/*m_hWnd*/, NULL,UI_WNDSTYLE_FRAME | WS_POPUP,0,0,0,600,450);
	//UINT unRet = pVSDlg.ShowModal();
	CaptureImage img;
	img.Work(userINI);
	
	s_bScreenShotOpened = false;
	return 0;
}