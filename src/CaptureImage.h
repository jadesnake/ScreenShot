#pragma once

#include "BuilderCall.h"
class CaptureImage : BuilderCall<DuiLib::WindowImplBase>
{
public:
	CaptureImage(void);
	virtual ~CaptureImage(void);
	void	Work(LPCTSTR userINI);
protected:
	//duilib
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	virtual DuiLib::CControlUI*	LoadOperatbar();
	CString m_userINI;
};