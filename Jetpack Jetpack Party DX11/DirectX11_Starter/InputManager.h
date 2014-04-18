#pragma once
#ifndef _IPMAN_H_
#define _IPMAN_H_
#include <stdio.h>
#include <xkeycheck.h>
#include <Xinput.h>
#include <d3d11.h>
#include <string>
#include <string.h>
#include <DirectXMath.h>
#include "XController.h"
#include <map>

#define maxMappedControls 4 + 1

using namespace DirectX;
using namespace std;

//XINPUT_GAMEPAD_DPAD_UP	 0x0001
//XINPUT_GAMEPAD_DPAD_DOWN	 0x0002
//XINPUT_GAMEPAD_DPAD_LEFT	 0x0004
//XINPUT_GAMEPAD_DPAD_RIGHT	 0x0008
//XINPUT_GAMEPAD_START	 0x0010
//XINPUT_GAMEPAD_BACK	 0x0020
//XINPUT_GAMEPAD_LEFT_THUMB	 0x0040
//XINPUT_GAMEPAD_RIGHT_THUMB	 0x0080
//XINPUT_GAMEPAD_LEFT_SHOULDER	 0x0100
//XINPUT_GAMEPAD_RIGHT_SHOULDER	 0x0200
//XINPUT_GAMEPAD_A	 0x1000
//XINPUT_GAMEPAD_B	 0x2000
//XINPUT_GAMEPAD_X	 0x4000
//XINPUT_GAMEPAD_Y	 0x8000


typedef enum INPUTMODES {
	KEYBOARD = 1,
	XCONTROLLER = 2
};

typedef enum KeyType {
	FORWARD = 1,
	BACKWARD = 2,
	LEFT = 3, 
	RIGHT = 4,
};

class InputManager
{
private:
	INPUTMODES mode;
	bool keyboard ;
	bool xinput ;

public:
	InputManager(INPUTMODES mode){
		keyStates = (int*)malloc(sizeof(int*) * maxMappedControls);
		DumpAll();
		this->mode = mode;
		if(mode) {
			if(mode == 0) {
				keyboard = true;
				xinput = true;
			}
			else
				if(mode == INPUTMODES::KEYBOARD) {
					keyboard = true;
				}
				else {
					xinput = true;
				}

				if(xinput)
				{
					xcontroller = new XBOX360CONTROLLER(1);
				}
		}
	}	

	double GetStandardState(KeyType key) const
	{
		double result = 0.0;
		if(keyboard)
		{			
				if(key == FORWARD)
				{
					if((GetAsyncKeyState('w') || GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP)) && !keyStates[FORWARD])
					{
						keyStates[FORWARD] = 1;
						return 1;
					}
				}
				if(key == BACKWARD) 
				{
					if(GetAsyncKeyState('s') || GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN) && !keyStates[BACKWARD])
					{
						keyStates[BACKWARD] = 1;
						return 1;
					}
				}
				if(key == LEFT)
				{
					if(GetAsyncKeyState('a') || GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT) && !keyStates[LEFT])
					{
						keyStates[LEFT] = 1;
						return 1;
					}
				}
				if(key == RIGHT)
				{
					if(GetAsyncKeyState('d') || GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT) && !keyStates[RIGHT])
					{
						keyStates[RIGHT] = 1;
						return 1;
					}
				}			
			
		}
			if(xinput)
			{
				if(xcontroller)
				{
					if(xcontroller->isConnected())
					{
						XINPUT_STATE state = xcontroller->GetState();
						if(key == FORWARD)
						{
							return state.Gamepad.sThumbLY;
						}
						if(key == BACKWARD)
						{
							return -1 * state.Gamepad.sThumbLY;
						}
						if(key == RIGHT)
						{
							return 1 * state.Gamepad.sThumbLX;
						}
						if(key == LEFT)
						{
							return -1 * state.Gamepad.sThumbLX;
						}						
					}
				}
			}		

			DumpAll();
		return result;
	}
	
	bool GetSpecialKeyboardState(int key) const
	{
		if(keyboard)
		{
			if(GetAsyncKeyState(key))
				return true;
			else
				return false;
		}		
	}

	inline bool GetStartKey() const {
		if(keyboard)
			if(GetAsyncKeyState(VK_RETURN))
				return true;

		if(xAvailable())
		{
			auto state = xState();
			if(state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
				return true;
		}

		return false;
	}

	inline bool GetBack() const {
		if(keyboard)
			if(GetAsyncKeyState(VK_ESCAPE))
				return true;

		if(xAvailable())
		{
			auto state = xState();
			if(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
				return true;
		}

		return false;
	}


	XBOX360CONTROLLER GetXController() const
	{
		return *xcontroller;
	}

	bool vibrate(int left = 0, int right = 0) const
	{
		if(xAvailable())
			return xcontroller->Vibrate(left, right);
		else
			false;
	}

protected:
	inline void DumpAll() const
	{
		for(int i = 0 ; i < maxMappedControls; i++) {
			keyStates[i] = 0;
		}
	}

	inline bool xAvailable() const {
		if(xinput)
			if(xcontroller)
				if(xcontroller->isConnected())
					return true;

		return false;
	}

	inline XINPUT_STATE xState() const {		
		return xcontroller->GetState();
	}

public:
	int* keyStates;
	XBOX360CONTROLLER* xcontroller;
};
#endif