#include "Player.h"

Player::Player()
{
	velocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 20;
	groundSpeedDampening = 0.95f;
}

void Player::Update(float dt)
{
	// Check for user input.
	CheckInput(dt);
	
	// Slow the character a bit so that it comes to a nice stop over time.
	XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), groundSpeedDampening));
	transform->Translate(transform->InverseTransformDirection(XMFLOAT3(velocity.x * dt, velocity.y * dt, velocity.z * dt)));

	Entity::Update(dt);
}

void Player::CheckInput(float dt)
{
	if(GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
	{
		velocity.z += 0.8f;
	}
	if(GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
	{
		velocity.z -= 0.8f;
	}
	if(GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
	{
		velocity.x -= 0.8f;
	}
	if(GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
	{
		velocity.x += 0.8f;
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

	/*if(GetAsyncKeyState('A'))
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
	}*/
}