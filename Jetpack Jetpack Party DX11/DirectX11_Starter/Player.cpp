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
	XMFLOAT3 transformedVel;
	//XMVECTOR velVec;
	XMStoreFloat3(&transformedVel, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(velocity.x * dt, velocity.y * dt, velocity.z * dt)), XMLoadFloat4x4(&transform->rot)));
	transform->Translate(transformedVel);
	
	if (camera)
	{
		// TODO do this with XMVector math
		/*XMFLOAT3 cameraRight(transform->right.x * cameraPos.x, transform->right.y * cameraPos.x, transform->right.y * cameraPos.x);
		XMFLOAT3 cameraUp(transform->up.x * cameraPos.y, transform->up.y * cameraPos.y, transform->up.y * cameraPos.y);
		XMFLOAT3 cameraForward(transform->forward.x * cameraPos.z, transform->forward.y * cameraPos.z, transform->forward.y * cameraPos.z);
		camera->LookAt(XMLoadFloat3(&XMFLOAT3(transform->trans._41 + transform->forward.x, transform->trans._42 + transform->forward.y, transform->trans._43 + transform->forward.z)),
			XMLoadFloat3(&XMFLOAT3(transform->trans._41 + cameraRight.x + cameraUp.x + cameraForward.x, transform->trans._42 + cameraRight.y + cameraUp.y + cameraForward.y, transform->trans._43 + cameraRight.z + cameraUp.z + cameraForward.z)),
			XMLoadFloat3(&transform->up));*/
	}

	Entity::Update(dt);
}

void Player::CheckInput(float dt)
{
	if(GetAsyncKeyState(VK_UP))
	{
		velocity.z += 0.8f;
	}
	if(GetAsyncKeyState(VK_DOWN))
	{
		velocity.z -= 0.8f;
	}
	if(GetAsyncKeyState(VK_LEFT))
	{
		velocity.x -= 0.8f;
	}
	if(GetAsyncKeyState(VK_RIGHT))
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

	if(GetAsyncKeyState('Q'))
	{
		transform->Rotate(XMFLOAT3(0, -1 * dt, 0));
	}
	if(GetAsyncKeyState('E'))
	{
		transform->Rotate(XMFLOAT3(0, 1 * dt, 0));
	}
}