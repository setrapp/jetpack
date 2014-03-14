#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <WinUser.h>
#include "Response.h"
#include "KeyboardInput.h"
#include "MouseInput.h"

class InputManager	: Input
{
private:
	KeyboardInput k_ip;
	MouseInput m_ip;

public :Response GetKey(int key)
	{
		k_ip.GetKey(key);
		m_ip.GetKey(key);
	}
};
#endif