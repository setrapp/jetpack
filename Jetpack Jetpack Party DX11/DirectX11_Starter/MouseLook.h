#pragma once

#include <iostream>
#include<d3d11.h>
#include<DirectXMath.h>
#include "Transform.h"
#include "Camera.h"
#include "ControllableCamera.h"

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
		if (this->rotationValue.x != 0 && this->rotationValue.y != 0) {
			XMFLOAT2 deltaRotation = XMFLOAT2(x - this->rotationValue.x, y - this->rotationValue.y);
			activeCamera->transform->Rotate(XMFLOAT3(deltaRotation.y * this->speed.x, deltaRotation.x * this->speed.y, 0));
		}
		this->rotationValue = XMFLOAT2(x, y);
		return this->rotationValue;
	}
	
};
