#if !defined(__SERVYOU_IM_SCREENSHOT_EXPORTS_SYMBOL_H_)
#define __SERVYOU_IM_SCREENSHOT_EXPORTS_SYMBOL_H_

#if defined(_AFXEXT)
#	if defined(_MSC_VER)
#		define SVY_SCREENSHOT_API __declspec(dllexport)
#	else
#		define SVY_SCREENSHOT_API
#	endif
#else
#	if defined(_MSC_VER)
#		define SVY_SCREENSHOT_API __declspec(dllimport)
#	else
#		define SVY_SCREENSHOT_API 
#	endif
#endif
#include "ScreenShotManage.h"
#endif