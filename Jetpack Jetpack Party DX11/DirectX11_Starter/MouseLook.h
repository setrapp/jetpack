#pragma once
#include "Camera.h"
#include "ControllableCamera.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <math.h>

class MouseLook
{
public:
	XMFLOAT3 rotation;
	XMFLOAT2 prevPosition;

	MouseLook(Camera* activeCameraInstance)
	{
		this->activeCamera = activeCameraInstance;
	}

	void OnMouseDown(WPARAM btnState, int x, int y){ 
		
	}
	void OnMouseUp(WPARAM btnState, int x, int y)  { 
	
	}
	void OnMouseMove(WPARAM btnState, int x, int y){ 
		XMFLOAT2 currentPosition = XMFLOAT2(x, y);
		if(currentPosition.x == prevPosition.x && currentPosition.y == prevPosition.y)
		{

		}
		else
		{
			XMMATRIX xRot = XMMatrixRotationX(x * X_ROTATION_SPEED);
			XMMATRIX yRot = XMMatrixRotationY( y * Y_ROTATION_SPEED);
		
			XMMATRIX viewMatrix = XMLoadFloat4x4(&activeCamera->view) * yRot * xRot;
			OrthoNormalize();
		}
	}
	void OnMouseWheel(WPARAM btnState, int x, int y) {
	
	}

	private:
		Camera* activeCamera;
		float xRotation ;
		float yRotation ;
		float X_ROTATION_SPEED, Y_ROTATION_SPEED;

		void MouseLook::OrthoNormalize()
		{
			XMFLOAT3 zAxis = activeCamera->transform->GetForward();
			XMVECTOR z = XMVector3Normalize(XMLoadFloat3(&zAxis));
			auto x = XMVector3Cross(XMLoadFloat3(&activeCamera->transform->GetUp()), z);
			x = XMVector3Normalize(x);
			
			XMVECTOR y = XMVector3Cross(x, z); // Tried reversing these a dozen times as well
			y = XMVector3Normalize(y);

			
			activeCamera->transform->Se
			viewMatrix.Forward = zAxis;
			viewMatrix.Up = yAxis;
			viewMatrix.Right = xAxis;
		}
};

