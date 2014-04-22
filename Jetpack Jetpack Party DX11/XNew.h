#pragma once
#include "XInputValues.h"
#include "DirectX11_Starter\XController.h"
#include <Xinput.h>
#include <stdio.h>
#include <stdlib.h>

#define maxsize 21

class XNEW
{
public :
	float controllerVals[maxsize];

	XNEW(XBOX360CONTROLLER* controller)
	{
		Init();
		this->controller = controller;
		GetValues();
	}

	bool XCHANGED(XINPUTVALUES val)
	{
		if(controllerValsOld	[val] == this->controllerVals[val]) {
			return 0;
		}
		else
			return 1;
	}

	void Update() {
		GetValues();
	}

private :
	float controllerValsOld[maxsize];
	XNEW() {/*controllerValsOld = (float*)malloc(sizeof(float*) * maxsize);*/ }
	
	//XNEW* oldState;
	inline void GetValues()	{
		if(xAvailable()) {			
			Equate();
			auto state = xState();
			this->controllerVals[LX				 ] = state.Gamepad.sThumbLX								    ;
			this->controllerVals[LY				 ] = state.Gamepad.sThumbLY								    ;
			this->controllerVals[RY				 ] = state.Gamepad.sThumbRY								    ;
			this->controllerVals[RX				 ] = state.Gamepad.sThumbRX								    ;
			this->controllerVals[LT				 ] = state.Gamepad.bLeftTrigger							    ;
			this->controllerVals[RT				 ] = state.Gamepad.bRightTrigger							;
			this->controllerVals[_DPAD_UP		 ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP		;
			this->controllerVals[_DPAD_DOWN		 ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN		;
			this->controllerVals[_DPAD_LEFT      ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT      ;
			this->controllerVals[_DPAD_RIGHT     ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT     ;
			this->controllerVals[_START          ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_START          ;
			this->controllerVals[_BACK           ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK           ;
			this->controllerVals[_LEFT_THUMB     ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB     ;
			this->controllerVals[_RIGHT_THUMB    ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB    ;
			this->controllerVals[_LEFT_SHOULDER  ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER  ;
			this->controllerVals[_RIGHT_SHOULDER ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ;
			this->controllerVals[_A              ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_A              ;
			this->controllerVals[_B              ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_B              ;
			this->controllerVals[_X              ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_X              ;
			this->controllerVals[_Y              ] = state.Gamepad.wButtons & XINPUT_GAMEPAD_Y				;		
		}																							
	}

	inline void Init() {
		for(int i =0; i< maxsize; i++)
			controllerVals[i] = 0;
	}
	inline bool xAvailable() const {		
			if(controller)
				if(controller->isConnected())
					return true;

		return false;
	}

	inline void Equate()
	{
		for(int i = 0; i < maxsize; i++)
			controllerValsOld[i] = static_cast<float>(controllerVals[i]);
	}

	inline XINPUT_STATE xState() const {		
		return controller->GetState();
	}


	XBOX360CONTROLLER* controller;
};