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
#include <stdlib.h>
#include <memory>
#include <memory.h>
#include <xmemory>
#include <map>
#include "Debug.h"


using namespace DirectX;
using namespace std;

//Max int value in XINPUTVALUES + 1
#define xMap 20 + 1

//Max int value in KeyType
#define maxMappedControls 6 + 1


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

typedef enum SpecialKeys {
    //Gamepad A or Keyboard F
    FIRE,  
    //GamePad B or Space
    BRAKE,
    //Gamepad X or E
    COLLECT,

    ACCELERATE,
};

typedef enum XINPUTVALUES {
    LX = 1,
    LY = 2,
    RX = 3,
    RY = 4,
    LT = 5,
    RT = 6,
    _DPAD_UP = 7,
    _DPAD_DOWN = 8,
    _DPAD_LEFT = 9,
    _DPAD_RIGHT = 10,
    _START= 11,
    _BACK = 12,
    _LEFT_THUMB = 13,
    _RIGHT_THUMB = 14,
    _LEFT_SHOULDER =15,
    _RIGHT_SHOULDER=16,
    _A =17,
    _B =18,
    _X =19,
    _Y =20,
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
		
		return result;
	}
	
	bool GetSpecialKeyboardState(int key)
	{
		if(keyboard)
		{
			if(GetAsyncKeyState(key))
				return true;
			else
				return false;
		}		
	}

	inline bool GetStartKey()  
	{
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

	inline bool GetBack()  
	{
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

	inline bool GetFire() {
		if(keyboard)
			if(GetAsyncKeyState('F') || GetAsyncKeyState('f'))
				return true;

		if(xAvailable())
		{
			auto state = xState() ;
			if(state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
				return true;
		}
	}


	XBOX360CONTROLLER GetXController()
	{
		return *xcontroller;
	}

	bool vibrate(int left = 0, int right = 0) 
	{
		if(xAvailable())
			return xcontroller->Vibrate(left, right);
		else
			false;
	}

	void DumpAll()
	{
		if(keyboard)
		for(int i = 0 ; i < maxMappedControls; i++) {
			keyStates[i] = 0;
		}

		if(xcontroller)
		for(int i = 0; i < xMap; i++) {
			xVals[i] = 0;
		}
	}
	
protected:
	inline bool xAvailable()  {
		if(xinput)
			if(xcontroller)
				if(xcontroller->isConnected())
					return true;

		return false;
	}

	inline XINPUT_STATE xState()  {		
		return xcontroller->GetState();
	}

	inline void SetXDefaults() {
		if(xAvailable())
		{
			auto state = xState();
			/*xDefaultVals[LX] = state.*/
		}
	}

private:
	int* keyStates;
	float* xVals;
	float* xDefaultVals;
	XBOX360CONTROLLER* xcontroller;
};
#endif