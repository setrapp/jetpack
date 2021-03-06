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

	inline Rect(int x, int y, int width, int height)
	{
		if(!this)
		{}
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	~Rect(void)
	{
	}

	inline void MoveRect(const int xnew, const int ynew)
	{
		this->x += xnew;
		this->y += ynew;
	}

	const inline bool Rect::Contains(const int x, const int y)
	{
		if(x >= this->x && x <= this->width + this->x && y > this->y && y < this->height + this->y)
		{
			return true;
		}
		return false;
	}

	inline static RECT* GetRECTFromRect(Rect* rect)
	{
		RECT* r = new RECT();
		r->left = rect->x;
		r->top = rect->y ;
		r->right = rect->width + r->left;
		r->bottom = rect->height +  r->top;
		return r;
	}


	inline static Rect* GetRectFromRECT(const RECT* rectOld)
	{
		Rect* rect = new Rect();
		rect->x = rectOld->left;
		rect->y = rectOld->top;
		rect->width = rectOld->left + rectOld->right;
		rect->height = rectOld->top + rectOld->bottom;
		return rect;
	}
};
#endif