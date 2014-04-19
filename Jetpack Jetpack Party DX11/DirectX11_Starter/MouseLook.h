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
	Transform* activeCamera;
	XMFLOAT3 rotationValue;
	XMFLOAT2 speed;

	//MouseLook(ControllableCamera* cam, XMFLOAT2 speed) {
	MouseLook(Transform* cam, XMFLOAT2 speed) {
		activeCamera = cam;
		rotationValue = XMFLOAT3(0, 0, 0);		
		this->speed = speed;
		cam->Rotate(XMFLOAT3());
	}

	void MouseLook::Update(float dt) {
		
	}	

	XMFLOAT3 MouseLook::MouseMove(WPARAM btnState, float x, float y) {
		XMFLOAT2 screenCenter = GetClientCenter();
		XMFLOAT2 deltaRotation = XMFLOAT2(x - screenCenter.x, y - screenCenter.y);
		if (deltaRotation.x != 0 || deltaRotation.y != 0) 
		{
			rotationValue = XMFLOAT3(rotationValue.x + (deltaRotation.y * this->speed.y), rotationValue.y + (deltaRotation.x * this->speed.x), 0);
			
			// Keep x rotation between 0 and (+/-)360
			if (rotationValue.x >= 2 * PI)
			{
				rotationValue.x -= 2 * PI;
			} 
			else if (rotationValue.x <= -2 * PI)
			{
				rotationValue.x += 2 * PI;
			}

			// Keep y rotation between 0 and (+/-)360
			if (rotationValue.y >= 2 * PI)
			{
				rotationValue.y -= 2 * PI;
			}
			else if (rotationValue.y <= -2 * PI)
			{
				rotationValue.y += 2 * PI;
			}

			activeCamera->SetLocalRotation(rotationValue);
		}
		
		ResetCursor();

		return this->rotationValue;
	}

private:
	void MouseLook::ResetCursor()
	{
		XMFLOAT2 center = GetScreenCenter();
		SetCursorPos(center.x, center.y);
	}

	XMFLOAT2 MouseLook::GetScreenCenter()
	{
		RECT rect;
		GetWindowRect(GetActiveWindow(), &rect);
		return XMFLOAT2 ((rect.right  + rect.left) / 2, (rect.bottom + rect.top) / 2);
	}

	XMFLOAT2 MouseLook::GetClientCenter()
	{
		RECT rect;
		GetClientRect(GetActiveWindow(), &rect);
		return XMFLOAT2((rect.right  + rect.left) / 2, (rect.bottom + rect.top) / 2 - 11);
	}

	
};
