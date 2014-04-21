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
	inline bool Rect::Contains(int x, int y) const
	{
		if(x >= this->x && x <= this->width + this->x && y > this->y && y < this->height + this->y)
		{
			return true;
		}
		return false;
	}
};
#endif