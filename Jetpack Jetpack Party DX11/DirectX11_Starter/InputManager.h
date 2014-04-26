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
#include <stdlib.h>
#include <map>
#include "Debug.h"
#include <math.h>
#include "XInputValues.h"
#include "XNew.h"

//Max int value in XINPUTVALUES + 1
#define xMap 20 + 1

//Max int value in KeyType
#define maxMappedControls 6 + 1


using namespace DirectX;
using namespace std;


typedef enum INPUTMODES {
	KEYBOARD = 1,
	XCONTROLLER = 2,
	BOTH = 0
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


class InputManager
{
private:
	INPUTMODES mode;
	bool keyboard ;
	bool xinput ;

public:
	//Give the input modes. You can pass KEYBOARD | XCONTROLLER
	InputManager(INPUTMODES mode){
		
		keyStates = NULL;
		xVals = NULL;
		xDefaultVals = NULL;			
		xcontroller = NULL;

		this->mode = mode;
		{
			if(mode == 0) {
				keyboard = true;
				xinput = true;
			}
			else
				if(mode == INPUTMODES::KEYBOARD) {
					keyboard = true;
					xinput = false;
				}
				else {
					xinput = true;
					keyboard = false;
				}

				if(keyboard)
				{
					keyStates = new int[maxMappedControls];		
					xVals = new float[xMap];
					xDefaultVals = new float[xMap];			
					xcontroller = new XBOX360CONTROLLER(1);						
					SetXDefaults();
				}
				if(xinput)
				{
					xVals = new float[xMap];
					xDefaultVals = new float[xMap];			
					xcontroller = new XBOX360CONTROLLER(1);						
					SetXDefaults();
				}
				DumpAll();
		}
	}	


	~InputManager(void)
	{
		delete [] xVals;
		delete [] xDefaultVals;
		delete [] keyStates;
		delete xcontroller;
	}

	//Returns 1 if a current key is pressed. does not keep a track of the state. If a button is pressed it will constantly return 1; 
	//EG: if(GetKey('k')) dosomething();
	double GetKey(const int key)  
	{
		double result = 0;
		if(keyboard)
		{
			if(GetAsyncKeyState(tolower(key)) || GetAsyncKeyState(toupper(key)))
				return 1;
		}
		return 0;
	}

	//Returns 1 if a current key is pressed. does not keep a track of the state. If a button is pressed it will constantly return 1;
	double GetKey(const KeyType key)  
	{
		double result = 0;
		if(keyboard)
		{
			if(key == FORWARD)
				{
					if((GetAsyncKeyState('w') || GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP)))				
							return 1;
				}
				if(key == BACKWARD) 
				{
					if(GetAsyncKeyState('s') || GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN))					
							return 1;
				}
				if(key == LEFT)
				{
					if(GetAsyncKeyState('a') || GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT))
						return 1;
				}
				if(key == RIGHT)
				{
					if(GetAsyncKeyState('d') || GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT))
						return 1;
				}		

		}
		return 0;
	}

	//Returns a number between 0 and 1, 0 means no input, 1 means full input. Gets absolute value of Gamepad. 
	//Eg: if up is pressed in gamepad, it will return 1 only once. 
	double GetKeyDown(KeyType key)
	{
		double result = 0.0;
		if(keyboard)
		{			
				if(key == FORWARD)
				{
					if((GetAsyncKeyState('w') || GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP)))
					{
						if(!keyStates[FORWARD])
						{
							keyStates[FORWARD] = 1;
							return 1;
						}
					}
					else
					{
						keyStates[FORWARD] = 0;
					}
				}
				if(key == BACKWARD) 
				{
					if(GetAsyncKeyState('s') || GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN))
					{
						if( !keyStates[BACKWARD])
						{
							keyStates[BACKWARD] = 1;
							return 1;
						}
					}
					else
					{
						keyStates[BACKWARD] = 0;
					}
				}
				if(key == LEFT)
				{
					if(GetAsyncKeyState('a') || GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT))
					{
						if(!keyStates[LEFT])
						{
							keyStates[LEFT] = 1;
							return 1;
						}
					}
					else
					{
						keyStates[LEFT] = 0;
					}
				}
				if(key == RIGHT)
				{
					if(GetAsyncKeyState('d') || GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT))
					{
						if(!keyStates[RIGHT])
						{
							keyStates[RIGHT] = 1;
							return 1;
						}
					}
					else
					{
						keyStates[RIGHT] = 0;
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
						result = abs(state.Gamepad.sThumbLY)  / abs(xVals[LY]) >= 0 ? 1 : 0;
						xVals [LY] = state.Gamepad.sThumbLY;	

						if(state.Gamepad.sThumbLY == xDefaultVals[LY])
							result = 0;

						return result;
					}
					if(key == BACKWARD)
					{					
						result = state.Gamepad.sThumbLY  / xVals[LY] < 0 ? 1 : 0;
						xVals [LY] = state.Gamepad.sThumbLY;

						if(state.Gamepad.sThumbLY == xDefaultVals[LY])
							result = 0;

						return result;
					}
					if(key == RIGHT)
					{
						result = state.Gamepad.sThumbLX  / xVals[LX] >= 0 ? 1 : 0;
						xVals [LX] = state.Gamepad.sThumbLY;

						if(state.Gamepad.sThumbLX == xDefaultVals[LX])
							result = 0;

						return result;
					}
					if(key == LEFT)
					{
						result = state.Gamepad.sThumbLX  / xVals[LX] < 0 ? 1 : 0;
						xVals [LX] = state.Gamepad.sThumbLY;

						if(state.Gamepad.sThumbLX == xDefaultVals[LX])
							result = 0;

						return result;
					}						
				}
			}
		}		

		return result;
	}	

	////For all keyboard keys. Returns true only once.
	//const bool GetKeyDown(const int key) const
	//{
	//	if(keyboard)
	//	{
	//		if(GetAsyncKeyState(key))
	//			{
	//				if(keyStates)
	//					if(keyStates[key] == 0)
	//					{
	//						keyStates[key] = 1;
	//						return true;
	//					}
	//			}			
	//		else
	//			{
	//				if(keyStates)
	//					if(keyStates[key])
	//						keyStates[key] = 0;

	//						return false;
	//			}
	//	}		
	//}

	//Returns the X Gamepad state just once
	double GetXKeyDown(KeyType key)
	{
		double result = 0.0;
		if(xinput)
		{
			if(xcontroller)
			{
				if(xcontroller->isConnected())
				{
					XINPUT_STATE state = xcontroller->GetState();
					if(key == FORWARD)
					{//					
						return state.Gamepad.sThumbLY > xDefaultVals[LY] ? 1: 0;
					}
					if(key == BACKWARD)
					{									
						return state.Gamepad.sThumbLY < xDefaultVals[LY]? 1 : 0;
					}
					if(key == RIGHT)
					{						
						return state.Gamepad.sThumbLX > xDefaultVals[LX] ? 1 : 0;
					}
					if(key == LEFT)
					{
						return state.Gamepad.sThumbLX < -xDefaultVals[LX] ? 1 : 0;
					}						
				}				
			}			
		}		
	return result;
	}	

	//Trying to multiplex start of gamepad and enter in keyboard.
	inline bool GetStartKey() const 
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

	inline bool GetBack() const 
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

	inline bool GetFire() const {
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

	XBOX360CONTROLLER* GetXController() const
	{
		return xcontroller;
	}

	bool Vibrate(int left = 0, int right = 0) const
	{
		if(xAvailable())
			return xcontroller->Vibrate(left, right);
		else
			return false;
	}

	inline INPUTMODES GetModes() const {
		if(keyboard && xinput)
			return (INPUTMODES)0;
		if(keyboard)
			return INPUTMODES::KEYBOARD;
		if(xinput)
			return INPUTMODES::XCONTROLLER;

	}

protected:
	//Dont delete this.
	inline void DumpAll()
	{
		if(keyboard)
		for(int i = 0 ; i < maxMappedControls; i++) {
			keyStates[i] = 0;
		}

		if(xinput)
		for(int i = 0; i < xMap; i++) {
			xVals[i] = 0;
			xDefaultVals[i] = 0;
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

	inline void SetXDefaults() {
		if(xAvailable())
		{
			auto state = xState();
			for(int i = 0; i < xMap; i++)
				xDefaultVals[i] = 0;
			xDefaultVals[LX] = state.Gamepad.sThumbLX;
			xDefaultVals[RX] = state.Gamepad.sThumbRX;
			xDefaultVals[LY] = state.Gamepad.sThumbLY;
			xDefaultVals[RY] = state.Gamepad.sThumbRY;
		}
	}			


private:
	int* keyStates;
	float* xVals;
	float* xDefaultVals;
	XBOX360CONTROLLER* xcontroller;
};


class IPMan
{
private : static InputManager* inputManager;
public: 
	IPMan(INPUTMODES mode)
	{
		if(!inputManager)
		{
			inputManager = new InputManager(mode);
		}
	}

	~IPMan()
	{
		delete inputManager;
		inputManager = NULL;
	}
	
	static InputManager* GetIPMan()
	{
		if(!inputManager)
			throw "No modes defined";
		return inputManager;
	}
};


#endif 