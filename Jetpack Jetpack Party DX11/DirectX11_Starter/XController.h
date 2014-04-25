#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "XInput.lib")

#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE	7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 7849
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD	30

class XBOX360CONTROLLER
{
private:
	XINPUT_STATE _controllerState;
	int _controllerNum;
public :
	XBOX360CONTROLLER(int playerNumber) {
		// Set the Controller Number
		_controllerNum = playerNumber - 1;
	}

	XINPUT_STATE GetState() {
		ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

		// Get the state
		XInputGetState(_controllerNum, &_controllerState);

		return _controllerState;
	}

	bool isConnected() {
		ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

		// Get the state
		DWORD Result = XInputGetState(_controllerNum, &_controllerState);

		if(Result == ERROR_SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Vibrate(int left = 0, int right = 0) const {
			// Create a Vibraton State
		XINPUT_VIBRATION Vibration;

		// Zeroise the Vibration
		ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

		// Set the Vibration Values
		Vibration.wLeftMotorSpeed = left;
		Vibration.wRightMotorSpeed = right;

		// Vibrate the controller
		XInputSetState(_controllerNum, &Vibration);

		return true;
	}
};