#include <DirectXMath.h>
#ifndef _RECT_H
#define _RECT_H
class Rect	
{
public:
	int x, y, width, height;	
	Rect(void)
	{
	}
	~Rect(void)
	{
	}
	inline bool Rect::Contains(int x, int y) const
	{
		if(x > this->x && x < this->width && y > this->y && y < this->height)
		{
			return true;
		}
		return false;
	}
};
#endif