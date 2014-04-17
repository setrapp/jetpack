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
using namespace DirectX;
using namespace std;
typedef enum INPUTMODES {
	KEYBOARD = 1,
	XCONTROLLER = 2
};

typedef enum KeyType {
	FORWARD = 'W' & 'w' & VK_UP,
	BACKWARD = 's' & 'S' & VK_DOWN,
	LEFT = 'a' & 'A' & VK_LEFT, 
	RIGHT = 8,
	RETURN = 16,
	ESCAPE = 32,
	SPACE = 64,

};

class InputManager
{
private:
	INPUTMODES mode;
	bool keyboard ;
	bool xinput ;

public:
	InputManager(INPUTMODES mode){
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
		}
	}	

	double GetState(KeyType key) {
		double it = 0.0;
		if(mode == INPUTMODES::KEYBOARD)
			if(GetAsyncKeyState(FORWARD))
				it = 1;
		return it;
	}
	
protected:
};
#endif