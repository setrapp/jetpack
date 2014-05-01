#pragma once

#include <iostream>
#include<d3d11.h>
#include<DirectXMath.h>
#include "Transform.h"
#include "Camera.h"
#include "ControllableCamera.h"
#include "Player.h"
#include "Debug.h"
#include "InputManager.h"
#include "XNew.h"

using namespace std;
using namespace DirectX;

class MouseLook
{
public :
	Transform* looker;
	XMFLOAT3 rotationValue;
	XMFLOAT2 speed;
	float minXDegrees, maxXDegrees;
	float minYDegrees, maxYDegrees;
	bool clampX, clampY;
	bool ignoreMouse;

	MouseLook(Transform* looker, XMFLOAT2 speed) {
		this->looker = looker;
		rotationValue = XMFLOAT3(0, 0, 0);		
		this->speed = speed;
		if (this->looker)
		{
			this->looker->SetLocalRotation(XMFLOAT3());
		}
		ignoreMouse = false;
		minXDegrees = maxXDegrees = 0;
		minYDegrees = maxYDegrees = 0;
		clampX = false;
		clampY = false;
	}

	void MouseLook::Update(float dt) {
		
	}	

	XMFLOAT3 MouseLook::MouseMove(WPARAM btnState, float x, float y) {
		// If mouse looking is being ignored, don't do anything
		if (ignoreMouse || !looker)
		{
			return XMFLOAT3();
		}

		XMFLOAT2 screenCenter = GetClientCenter();
		XMFLOAT2 deltaRotation = XMFLOAT2(x - screenCenter.x, y - screenCenter.y);
		if (deltaRotation.x != 0 || deltaRotation.y != 0) 
		{
			rotationValue = XMFLOAT3(rotationValue.x + (deltaRotation.y * this->speed.y), rotationValue.y + (deltaRotation.x * this->speed.x), 0);
			
			// Keep x rotation either within clamp range or between (+/-)360.
			if (clampX)
			{
				float maxXRadians = maxXDegrees * PI / 180;
				float minXRadians = minXDegrees * PI / 180;
				if (rotationValue.x >= maxXRadians)
				{
					rotationValue.x = maxXRadians;
				} 
				else if (rotationValue.x <= minXRadians)
				{
					rotationValue.x = minXRadians;
				}
			}
			else
			{
				if (rotationValue.x >= 2 * PI)
				{
					rotationValue.x -= 2 * PI;
				} 
				else if (rotationValue.x <= -2 * PI)
				{
					rotationValue.x += 2 * PI;
				}
			}

			// Keep y rotation either within clamp range or between (+/-)360.
			if (clampY)
			{
				float maxYRadians = maxYDegrees * PI / 180;
				float minYRadians = minYDegrees * PI / 180;
				if (rotationValue.y >= maxYRadians)
				{
					rotationValue.y = maxYRadians;
				} 
				else if (rotationValue.y <= minYRadians)
				{
					rotationValue.y = minYRadians;
				}
			}
			else
			{
				if (rotationValue.y >= 2 * PI)
				{
					rotationValue.y -= 2 * PI;
				}
				else if (rotationValue.y <= -2 * PI)
				{
					rotationValue.y += 2 * PI;
				}
			}

			if (looker)
			{
				looker->SetLocalRotation(rotationValue);
			}
		}		

		ResetCursor();

		return this->rotationValue;
	}

	// X Rotation Clamping.
	void UnclampX()
	{
		clampX = false;
	}
	void ClampX()
	{
		clampX = true;
	}
	void ClampX(float minDegrees, float maxDegrees)
	{
		clampX = true;
		minXDegrees = minDegrees;
		maxXDegrees = maxDegrees;
	}

	// Y Rotation Clamping.
	void UnclampY()
	{
		clampY = false;
	}
	void ClampY()
	{
		clampY = true;
	}
	void ClampY(float minDegrees, float maxDegrees)
	{
		clampY = true;
		minYDegrees = minDegrees;
		maxYDegrees = maxDegrees;
	}

	/*XMFLOAT3 MouseLook::XMove(XNEW* xnew) {
		if(xnew)
		{			
			double x = 0, y = 0;
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
			if(xnew->XCHANGED(XINPUTVALUES::LX))
				x+=speed.y;

			if(xnew->XCHANGED(XINPUTVALUES::LY))
				y+=speed.x;

			Debug::Log("\nX : " + Debug::ToString(x));
			Debug::Log("\nY : " + Debug::ToString(y));
			Debug::Log(Debug::ToString(XMFLOAT4(this->rotationValue.x, this->rotationValue.y, this->rotationValue.z, 0)));

			this->rotationValue.x += y / (2 * PI);
			this->rotationValue.y += x / (2 * PI);
			this->rotationValue.x *= 0.1f;
			this->rotationValue.y *= 0.1f;
			looker->Rotate(this->rotationValue);
		}		
		return this->rotationValue;
	}*/

	void MouseLook::ResetCursor()
	{
		XMFLOAT2 center = GetScreenCenter();
		SetCursorPos(center.x, center.y);
	}

private:
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
