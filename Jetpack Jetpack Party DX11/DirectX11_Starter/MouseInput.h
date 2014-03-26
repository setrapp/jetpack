#ifndef MOUSEINPUT_H
#define MOUSEINPUT_H
#include "Input.h"

class MouseInput: Input
{
private: Response mouseResponse;
public :MouseInput();
		Response GetKey(int key);
		{
			return mouseResponse;
		}

};
#endif