#include "StdAfx.h"
#include "CaptureImage.h"
#include <WindowsX.h>
#include "ScreenUI.h"
#include "ShowRC.h"
#include "TrackDot.h"
CaptureImage::CaptureImage(void)
{

}
CaptureImage::~CaptureImage(void)
{

}
void CaptureImage::InitWindow()
{
	DuiLib::CControlUI *root = m_PaintManager.GetRoot();
	root->Init();	//由于duilib不通知root节点初始化事件因此手动通知
	ScreenUI *sceen = reinterpret_cast<ScreenUI*>(root);
	sceen->SetUserINI(m_userINI);
	LoadOperatbar();
}
void CaptureImage::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}
DuiLib::CDuiString CaptureImage::GetSkinFolder()
{
	return _T("skin");
}
DuiLib::CDuiString CaptureImage::GetSkinFile()
{
	return _T("capture_image/capture_wnd.xml");
}
LPCTSTR CaptureImage::GetWindowClassName(void) const
{
	return _T("Servyou_captureimage");
}
void CaptureImage::Work(LPCTSTR userINI)
{
	m_userINI = userINI;

	HDC hdcScreen = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	int nW=0,nH=0;
	GetShowSize(hdcScreen,nW,nH);
	::DeleteDC(hdcScreen);
#if defined(_DEBUG)
	this->Create(NULL,_T("抓取屏幕"),UI_WNDSTYLE_FRAME|WS_POPUP|WS_EX_TOOLWINDOW,0,DuiLib::CRect(0,0,nW,nH));
#else
	this->Create(NULL,_T("抓取屏幕"),WS_VISIBLE |WS_POPUP|WS_EX_TOOLWINDOW,WS_EX_TOPMOST,DuiLib::CRect(0,0,nW,nH));
#endif
	this->ShowModal();
}
 DuiLib::CControlUI*	CaptureImage::LoadOperatbar()
{
	DuiLib::CDialogBuilder builder;
	DuiLib::CControlUI	   *pRoot = builder.Create(_T("capture_image/color_palette.xml"),(UINT)0,this,&m_PaintManager);
	pRoot->SetVisible(false);
	DuiLib::CContainerUI *layout = static_cast<DuiLib::CContainerUI*>(m_PaintManager.GetRoot());
	if( layout )
	{
		DuiLib::TEventUI ev;
		ev.Type = capture::InitTools;
		layout->Add(pRoot);
		layout->Event(ev);
	}
	return pRoot;
}
LRESULT CaptureImage::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool	bHandled=false;
	if( uMsg==WM_MOUSEMOVE && IS_HAVE(wParam,MK_LBUTTON) )
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		DuiLib::CControlUI *ui = m_PaintManager.FindControl(pt);
		DuiLib::DUI__Trace(_T("x %d ,y %d name %s"),pt.x,pt.y,ui?ui->GetName().GetData():_T("null"));
	}
	if( uMsg==WM_LBUTTONUP )
	{
		DuiLib::DUI__Trace(_T("end"));
	}
	if( uMsg==WM_RBUTTONUP )
	{
		this->Close();
	}
	switch (uMsg)
	{
	case WM_KEYDOWN :
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				{
					DuiLib::CControlUI *ui = m_PaintManager.GetFocus();
					if( ui )
					{
						CAtlString strClass = ui->GetClass();
						strClass.MakeUpper();
						if( 0==strClass.CompareNoCase(DUI_CTR_RICHEDIT)||-1!=strClass.Find(_T("EDIT"))||
							0==strClass.CompareNoCase(DUI_CTR_EDIT) )
						{
							m_PaintManager.SetFocus(m_PaintManager.GetRoot());
							break;
						}
					}
					this->Close();
				}
				break;
			default:
				break;
			}
		}
		break;
	case WM_CLOSE:
		if (!::IsWindowEnabled(m_hWnd))
		{
			::SetForegroundWindow(m_hWnd);
			return FALSE;
		}
		break;
	case WM_PAINT:
		{
		
		}
		break;
	default:
		break;
	}
	if( bHandled )
		return lRes;
	return __super::HandleMessage(uMsg,wParam,lParam);
}