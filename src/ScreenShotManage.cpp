#include "StdAfx.h"
#include "ScreenShotManage.h"
#include "CaptureImage.h"
CScreenShotManage::CScreenShotManage(void)
{
}

CScreenShotManage::~CScreenShotManage(void)
{
}

void CScreenShotManage::OpenScreenShot()
{
	CaptureImage img;
	img.Work();
}
