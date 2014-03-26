#ifndef KEYBOARDINPUT_H
#define KEYBOARDINPUT_H
#include <WinUser.h>
#include "Input.h"
class KeyboardInput		
	: Input
{
public:
	Response KeyboardInput::GetKey(int key)
	{
		Response* current = new Response();

		if(GetAsyncKeyState(key))
		{
			current->keyPressed = true;
			current->keyCode = (char)key;
			current->mResponse = NULL;
		}

		this->previousResponse = currentResponse;
		return *current;
	}
};
#endif