#include "ControllableCamera.h"

ControllableCamera::ControllableCamera()
{
	velocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 20;
	groundSpeedDampening = 0.95f;
}

void ControllableCamera::Update(float dt, VertexShaderModelConstantBuffer* vsConstantBufferdata)
{
	CheckInput(dt);
	

	// Slow the character a bit so that it comes to a nice stop over time.
	XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), groundSpeedDampening));
	transform->Translate(XMFLOAT3(velocity.x * dt, velocity.y * dt, velocity.z * dt));

	Camera::Update(dt, vsConstantBufferdata);
}

void ControllableCamera::CheckInput(float dt)
{
	if(GetAsyncKeyState('K'))
	{
		velocity.z += 0.8f;
	}
	if(GetAsyncKeyState('I'))
	{
		velocity.z -= 0.8f;
	}
	if(GetAsyncKeyState('L'))
	{
		velocity.x -= 0.8f;
	}
	if(GetAsyncKeyState('J'))
	{
		velocity.x += 0.8f;
	}
	if(GetAsyncKeyState('N'))
	{
		velocity.y -= 0.8f;
	}
	if(GetAsyncKeyState('M'))
	{
		velocity.y += 0.8f;
	}

	// Clamp to max speed.
	XMVECTOR velocityVec = XMLoadFloat3(&velocity);
	XMFLOAT3 velocityMag3;
	XMStoreFloat3(&velocityMag3, XMVector3Length(velocityVec));
	float velocityMag = velocityMag3.x;
	if (velocityMag > maxSpeed) {
		velocity.x = velocity.x * (maxSpeed / velocityMag);
		velocity.y = velocity.y * (maxSpeed / velocityMag);
		velocity.z = velocity.z * (maxSpeed / velocityMag);
	}

	if(GetAsyncKeyState('A'))
	{
		transform->Rotate(XMFLOAT3(0, -1 * dt, 0));
	}
	if(GetAsyncKeyState('D'))
	{
		transform->Rotate(XMFLOAT3(0, 1 * dt, 0));
	}
	if(GetAsyncKeyState('W'))
	{
		transform->Rotate(XMFLOAT3(-1 * dt, 0 , 0));
	}
	if(GetAsyncKeyState('S'))
	{
		transform->Rotate(XMFLOAT3(1 * dt, 0, 0));
	}
	if(GetAsyncKeyState('Q'))
	{
		transform->Rotate(XMFLOAT3(0, 0, -1 * dt));
	}
	if(GetAsyncKeyState('E'))
	{
		transform->Rotate(XMFLOAT3(0, 0, 1 * dt));
	}

	if(GetAsyncKeyState('X'))
	{
		transform->Scale(XMFLOAT3(1 + dt, 1 + dt, 1 + dt));
	}
	if(GetAsyncKeyState('Z'))
	{
		transform->Scale(XMFLOAT3(1 - dt, 1 - dt, 1 - dt));
	}
}