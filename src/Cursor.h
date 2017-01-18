#pragma once

#include "Const.h"
class Cursor
{
public:
	Cursor(void);
	virtual ~Cursor(void);	
	void	change(long n);
protected:
	void	empty();
private:
	HCURSOR		cur_;		//当前指针
	long		state_;		//处理状态
};
