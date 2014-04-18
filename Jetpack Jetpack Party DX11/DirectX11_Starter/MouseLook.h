#pragma once

#include <iostream>
#include<d3d11.h>
#include<DirectXMath.h>
#include "Transform.h"
#include "Camera.h"
#include "ControllableCamera.h"
#include "Player.h"
#include "Debug.h"

using namespace std;
using namespace DirectX;

class MouseLook
{
public :
	//Camera* activeCamera;
	Player* activeCamera;
	XMFLOAT2 rotationValue;
	XMFLOAT2 speed;
	//MouseLook(ControllableCamera* cam, XMFLOAT2 speed) {
	MouseLook(Player* cam, XMFLOAT2 speed) {
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

			XMFLOAT3 eye = activeCamera->transform->GetTranslation();
			XMFLOAT3 lookAt;
			XMStoreFloat3(&lookAt, XMVectorAdd(XMLoadFloat3(&eye), XMLoadFloat3(&activeCamera->transform->GetForward())));
			XMFLOAT3 up = activeCamera->transform->GetUp();

			activeCamera->transform->Rotate(activeCamera->transform->InverseTransformDirection(XMFLOAT3(deltaRotation.y * this->speed.y, 0, 0)));
			eye = activeCamera->transform->GetTranslation();
			XMStoreFloat3(&lookAt, XMVectorAdd(XMLoadFloat3(&eye), XMLoadFloat3(&activeCamera->transform->GetForward())));
			up = activeCamera->transform->GetUp();
			activeCamera->transform->LookAt(eye, lookAt, up); 
			//activeCamera->transform->Rotate(activeCamera->transform->InverseTransformDirection(XMFLOAT3(0, deltaRotation.x * this->speed.x, 0)));
			//activeCamera->transform->LookAt(eye, lookAt, up); 


			SetCursorPos((r.right  + r.left) / 2, (r.bottom + r.top)/ 2 + 23 / 2);
		}
		
		this->rotationValue = XMFLOAT2(x, y);
		return this->rotationValue;
	}
	
};
