#pragma once

#include <iostream>
#include<d3d11.h>
#include<DirectXMath.h>
#include "Transform.h"
#include "Camera.h"
#include "ControllableCamera.h"
#include "Debug.h"

using namespace std;
using namespace DirectX;

class MouseLook
{
public :
	Camera* activeCamera;
	XMFLOAT2 rotationValue;
	XMFLOAT2 speed;
	MouseLook(ControllableCamera* cam, XMFLOAT2 speed) {
		activeCamera = cam;
		rotationValue = XMFLOAT2(0, 0);		
		this->speed = speed;
		cam->transform->Rotate(XMFLOAT3());
	}

	void MouseLook::Update(float dt) {
		
	}	

	XMFLOAT2 MouseLook::MouseMove(WPARAM btnState, float x, float y) {
		RECT r;
		GetWindowRect(GetActiveWindow(), &r);
		if (this->rotationValue.x != 0 && this->rotationValue.y != 0) {
			
			//activeCamera->transform->SetLocalRotation(XMFLOAT3(0, 0, 1), 0);
			Debug::Log(Debug::ToString(x));
			Debug::Log(Debug::ToString(y));
			XMFLOAT2 deltaRotation = XMFLOAT2(x - screenWidth / 2, y - screenHeight / 2);
			activeCamera->transform->Rotate(XMFLOAT3(deltaRotation.y * this->speed.y, deltaRotation.x * this->speed.x, 0));		


			SetCursorPos((r.right  + r.left) / 2, (r.bottom + r.top)/ 2 + 23 / 2);
		}
		
		this->rotationValue = XMFLOAT2(x, y);
		return this->rotationValue;
	}
	
};
