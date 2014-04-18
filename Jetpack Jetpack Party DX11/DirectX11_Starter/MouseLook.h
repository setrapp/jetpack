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
	XMFLOAT3 rotationValue;
	XMFLOAT2 speed;

	//MouseLook(ControllableCamera* cam, XMFLOAT2 speed) {
	MouseLook(Player* cam, XMFLOAT2 speed) {
		activeCamera = cam;
		rotationValue = XMFLOAT3(0, 0, 0);		
		this->speed = speed;
		cam->transform->Rotate(XMFLOAT3());
	}

	void MouseLook::Update(float dt) {
		
	}	

	XMFLOAT3 MouseLook::MouseMove(WPARAM btnState, float x, float y) {
		//if (this->rotationValue.x != 0 || this->rotationValue.y != 0) {
			//activeCamera->transform->SetLocalRotation(XMFLOAT3(0, 0, 1), 0);
			//Debug::Log(Debug::ToString(x));
			//Debug::Log(Debug::ToString(y));
			XMFLOAT2 deltaRotation = XMFLOAT2(x - screenWidth / 2, y - screenHeight / 2);

			XMFLOAT3 eye = activeCamera->transform->GetTranslation();
			XMFLOAT3 lookAt;
			XMStoreFloat3(&lookAt, XMVectorAdd(XMLoadFloat3(&eye), XMLoadFloat3(&activeCamera->transform->GetForward())));
			XMFLOAT3 up = XMFLOAT3(0, 1, 0);//activeCamera->transform->GetUp();

			// Rotation around x-axis.
			/*activeCamera->transform->Rotate(activeCamera->transform->InverseTransformDirection(XMFLOAT3(deltaRotation.y * this->speed.y, 0, 0)));
			eye = activeCamera->transform->GetTranslation();
			XMStoreFloat3(&lookAt, XMVectorAdd(XMLoadFloat3(&eye), XMLoadFloat3(&activeCamera->transform->GetForward())));
			activeCamera->transform->LookAt(eye, lookAt, up);

			// Rotation around y-axis.
			activeCamera->transform->Rotate(activeCamera->transform->InverseTransformDirection(XMFLOAT3(0, deltaRotation.y * this->speed.y, 0)));
			eye = activeCamera->transform->GetTranslation();
			XMStoreFloat3(&lookAt, XMVectorAdd(XMLoadFloat3(&eye), XMLoadFloat3(&activeCamera->transform->GetForward())));
			activeCamera->transform->LookAt(eye, lookAt, up);*/
			rotationValue = XMFLOAT3(rotationValue.x + (deltaRotation.y * this->speed.y), rotationValue.y + (deltaRotation.x * this->speed.x), 0);
			/*TODO: reset rotation value to zero when it passes 360 (may need to convert to degrees)*/
			activeCamera->transform->SetLocalRotation(rotationValue);
		//}
		
		ResetCursor();

		//this->rotationValue = XMFLOAT2(x, y);
		return this->rotationValue;
	}

	void MouseLook::ResetCursor()
	{
		RECT r;
		GetWindowRect(GetActiveWindow(), &r);
		SetCursorPos((r.right  + r.left) / 2, (r.bottom + r.top)/ 2 + 23 / 2);
	}
	
};
