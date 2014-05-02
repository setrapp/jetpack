#include <DirectXMath.h>
#ifndef _RECT_H
#define _RECT_H
class Rect	
{
public:
	int x, y, width, height;	
	Rect(void)
	{
		x =0;
		y = 0;
		width = 0;
		height = 0;
	}

	Rect(int x, int y, int width, int height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	~Rect(void)
	{
	}

	const inline bool Rect::Contains(const int x, const int y)
	{
		if(x >= this->x && x <= this->width + this->x && y > this->y && y < this->height + this->y)
		{
			return true;
		}
		return false;
	}

	static RECT ConvertToRECT(Rect* rect)
	{
		RECT r;
		r.left = rect->x;
		r.top = rect->y ;
		r.right = rect->width - r.left;
		r.bottom = rect->height -  r.top;
		return r;
	}
};
#endif