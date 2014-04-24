#include "ControllableCamera.h"
#include "InputManager.h"

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
	transform.Translate(transform.InverseTransformDirection(XMFLOAT3(velocity.x * dt, velocity.y * dt, velocity.z * dt)));

	Camera::Update(dt, vsConstantBufferdata);
}

void ControllableCamera::CheckInput(float dt)
{
	
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('K'))
	{
		velocity.z -= 0.8f;
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('I'))
	{
		velocity.z += 0.8f;
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('L'))
	{
		velocity.x += 0.8f;
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('J'))
	{
		velocity.x -= 0.8f;
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('B'))
	{
		velocity.y -= 0.8f;
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('N'))
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

	if(IPMan::GetIPMan()->GetSpecialKeyboardState('Y'))
	{
		transform.Rotate(XMFLOAT3(0, -1 * dt, 0));
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('H'))
	{
		transform.Rotate(XMFLOAT3(0, 1 * dt, 0));
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('T'))
	{
		transform.Rotate(XMFLOAT3(-1 * dt, 0 , 0));
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('G'))
	{
		transform.Rotate(XMFLOAT3(1 * dt, 0, 0));
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('U'))
	{
		transform.Rotate(XMFLOAT3(0, 0, -1 * dt));
	}
	if(IPMan::GetIPMan()->GetSpecialKeyboardState('O'))
	{
		transform.Rotate(XMFLOAT3(0, 0, 1 * dt));
	}
}