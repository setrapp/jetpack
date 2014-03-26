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
	bool Rect::Contains(int x, int y)
	{
		if(x > this->x && x < this->width && y > this->y && y < this->height)
		{
			//delete p;
			return true;
		}
		//delete p;
		return false;
	}

};
//class Point
//{
//public :
//	Point();
//	~Point();
//	int x, y;
//};
#endif