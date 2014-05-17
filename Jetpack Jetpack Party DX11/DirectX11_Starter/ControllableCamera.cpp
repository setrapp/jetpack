#include "ControllableCamera.h"
#include "InputManager.h"

ControllableCamera::ControllableCamera()
{
	velocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 1000;
	groundSpeedDampening = 0.95f;
	controllable = false;
}

void ControllableCamera::Update(float dt, VertexShaderModelConstantBuffer* vsConstantBufferdata)
{
	if (controllable)
	{
		CheckInput(dt);	

		// Slow the character a bit so that it comes to a nice stop over time.
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), groundSpeedDampening));
		transform.Translate(XMFLOAT3(velocity.x * dt, velocity.y * dt, velocity.z * dt));
	}

	Camera::Update(dt, vsConstantBufferdata);
}

void ControllableCamera::CheckInput(float dt)
{
	
	if(IPMan::GetIPMan()->GetKey('S'))
	{
		velocity.z -= 100000 * dt;
	}
	if(IPMan::GetIPMan()->GetKey('W'))
	{
		velocity.z += 100000 * dt;
	}
	if(IPMan::GetIPMan()->GetKey('D'))
	{
		velocity.x += 100000 * dt;
	}
	if(IPMan::GetIPMan()->GetKey('A'))
	{
		velocity.x -= 100000 * dt;
	}
	if(IPMan::GetIPMan()->GetKey('Q'))
	{
		velocity.y -= 100000 * dt;
	}
	if(IPMan::GetIPMan()->GetKey('E'))
	{
		velocity.y += 100000 * dt;
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

	if(IPMan::GetIPMan()->GetKey('Y'))
	{
		transform.Rotate(XMFLOAT3(0, -1 * dt, 0));
	}
	if(IPMan::GetIPMan()->GetKey('H'))
	{
		transform.Rotate(XMFLOAT3(0, 1 * dt, 0));
	}
	
	if(IPMan::GetIPMan()->GetKey('T'))
	{
		transform.Rotate(XMFLOAT3(-1 * dt, 0 , 0));
	}
	if(IPMan::GetIPMan()->GetKey('G'))
	{
		transform.Rotate(XMFLOAT3(1 * dt, 0, 0));
	}
	
	if(IPMan::GetIPMan()->GetKey('U'))
	{
		transform.Rotate(XMFLOAT3(0, 0, -1 * dt));
	}
	if(IPMan::GetIPMan()->GetKey('O'))
	{
		transform.Rotate(XMFLOAT3(0, 0, 1 * dt));
	}
}