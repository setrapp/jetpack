#include "ManeuverJetpack.h"
#include "Common.h"
#include "Entity.h"
//#include "Player.h"

ManeuverJetpack::ManeuverJetpack(Entity* player) : Jetpack(player)
{
	maxSpeed = 1000;
	fuelUseRate = 1;//10;
	forwardAcceleration = 400.0f;
	backwardAcceleration = 400.0f;
	strafeAcceleration = 400.0f;
	ascentAcceleration = 600.0f;
	backSpin = 30 * (PI / 180);
	frontSpin = 30 * (PI / 180);
	sideSpin = 15 * (PI / 180);
	bottomSpin = 10 * (PI / 180);
	actionsPerSecond = 1.9f;
	
	// Thrusters
	thrusterCount = 8;
	CreateThrusters();
	// Bottom Left
	thrusters[Thruster::BOTTOM_LEFT]->transform.SetLocalTranslation(XMFLOAT3(-2, -2, -2.5));
	thrusters[Thruster::BOTTOM_LEFT]->transform.SetLocalRotation(XMFLOAT3(0, 0, PI));
	// Bottom Right
	thrusters[Thruster::BOTTOM_RIGHT]->transform.SetLocalTranslation(XMFLOAT3(2, -2, -2.5));
	thrusters[Thruster::BOTTOM_RIGHT]->transform.SetLocalRotation(XMFLOAT3(0, 0, PI));
	// Back Left
	thrusters[Thruster::BACK_LEFT]->transform.SetLocalTranslation(XMFLOAT3(-1, 2, -3));
	thrusters[Thruster::BACK_LEFT]->transform.SetLocalRotation(XMFLOAT3(-PI / 2, 0, 0));
	// Back Right
	thrusters[Thruster::BACK_RIGHT]->transform.SetLocalTranslation(XMFLOAT3(1, 2, -3));
	thrusters[Thruster::BACK_RIGHT]->transform.SetLocalRotation(XMFLOAT3(-PI / 2, 0, 0));
	// Front Left
	thrusters[Thruster::FRONT_LEFT]->transform.SetLocalTranslation(XMFLOAT3(-2, 0, 3));
	thrusters[Thruster::FRONT_LEFT]->transform.SetLocalRotation(XMFLOAT3(PI / 2, 0, 0));
	// Front Right
	thrusters[Thruster::FRONT_RIGHT]->transform.SetLocalTranslation(XMFLOAT3(2, 0, 3));
	thrusters[Thruster::FRONT_RIGHT]->transform.SetLocalRotation(XMFLOAT3(PI / 2, 0, 0));
	// Side Left
	thrusters[Thruster::SIDE_LEFT]->transform.SetLocalTranslation(XMFLOAT3(-3, 2, -1));
	thrusters[Thruster::SIDE_LEFT]->transform.SetLocalRotation(XMFLOAT3(0, 0, PI / 2));
	// Side Right
	thrusters[Thruster::SIDE_RIGHT]->transform.SetLocalTranslation(XMFLOAT3(3, 2, -1));
	thrusters[Thruster::SIDE_RIGHT]->transform.SetLocalRotation(XMFLOAT3(0, 0, -PI / 2));
}

void ManeuverJetpack::Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	// Estimate the propultion direction of each thruster.
	XMFLOAT3 transformRight = player->transform.GetRight(), transformUp = player->transform.GetUp(), transformForward = player->transform.GetForward();
	XMVECTOR right = XMLoadFloat3(&transformRight), up = XMLoadFloat3(&transformUp), forward = XMLoadFloat3(&transformForward);
	XMStoreFloat3(&thrusterDirections[Thruster::BOTTOM_LEFT], XMVector3Normalize(XMVectorAdd(XMVectorScale(up, 10), right)));
	thrusterDirections[Thruster::BOTTOM_RIGHT] = thrusterDirections[Thruster::BOTTOM_LEFT];
	thrusterDirections[Thruster::BOTTOM_RIGHT].x *= -1;
	XMStoreFloat3(&thrusterDirections[Thruster::BACK_LEFT], XMVector3Normalize(XMVectorAdd(XMVectorScale(forward, 2), right)));
	thrusterDirections[Thruster::BACK_RIGHT] = thrusterDirections[Thruster::BACK_LEFT];
	thrusterDirections[Thruster::BACK_RIGHT].x *= -1;
	XMStoreFloat3(&thrusterDirections[Thruster::FRONT_LEFT], XMVector3Normalize(XMVectorAdd(XMVectorScale(-forward, 2), right)));
	thrusterDirections[Thruster::FRONT_RIGHT] = thrusterDirections[Thruster::FRONT_LEFT];
	thrusterDirections[Thruster::FRONT_RIGHT].x *= -1;
	XMStoreFloat3(&thrusterDirections[Thruster::SIDE_LEFT], XMVector3Normalize(XMVectorAdd(XMVectorScale(right, 3), -up)));
	thrusterDirections[Thruster::SIDE_RIGHT] = thrusterDirections[Thruster::SIDE_LEFT];
	thrusterDirections[Thruster::SIDE_RIGHT].x *= -1;

	// Call base update.
	Jetpack::Update(dt, velocity, angularVelocity);
}

void ManeuverJetpack::CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	// TODO: These should use IPMan
	// Left Side
	if(GetAsyncKeyState('W'))//('A'))
	{
		thrusterActives[Thruster::FRONT_LEFT] = true;
	}
	if(GetAsyncKeyState('A'))//(('S'))
	{
		thrusterActives[Thruster::SIDE_LEFT] = true;
	}
	if(GetAsyncKeyState('S'))//(('D'))
	{
		thrusterActives[Thruster::BACK_LEFT] = true;
	}
	if(GetAsyncKeyState('D'))//(('F'))
	{
		thrusterActives[Thruster::BOTTOM_LEFT] = true;
	}
	// Right Side
	if(GetAsyncKeyState('J'))//(('J'))
	{
		thrusterActives[Thruster::BOTTOM_RIGHT] = true;
	}
	if(GetAsyncKeyState('K'))//(('K'))
	{
		thrusterActives[Thruster::BACK_RIGHT] = true;
	}
	if(GetAsyncKeyState('L'))//(('L'))
	{
		thrusterActives[Thruster::SIDE_RIGHT] = true;
	}
	if(GetAsyncKeyState('I'))//((VK_OEM_1)) // colon on US standard keyboards
	{
		thrusterActives[Thruster::FRONT_RIGHT] = true;
	}
}

void ManeuverJetpack::ApproachTarget(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	XMVECTOR desiredAcceleration = XMLoadFloat3(&targetAcceleration);
	XMFLOAT3 thrustDotDir;
	float minDot = 0.7f;

	// Left Side
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::FRONT_LEFT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::FRONT_LEFT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::SIDE_LEFT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::SIDE_LEFT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::BACK_LEFT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::BACK_LEFT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::BOTTOM_LEFT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::BOTTOM_LEFT] = true;
	}
	// Right Side
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::BOTTOM_RIGHT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::BOTTOM_RIGHT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::BACK_RIGHT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::BACK_RIGHT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::SIDE_RIGHT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::SIDE_RIGHT] = true;
	}
	XMStoreFloat3(&thrustDotDir, XMVector3Dot(desiredAcceleration, XMLoadFloat3(&thrusterDirections[Thruster::FRONT_RIGHT])));
	if(thrustDotDir.x >= minDot)
	{
		thrusterActives[Thruster::FRONT_RIGHT] = true;
	}
}

void ManeuverJetpack::ApplyForces(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	// TODO: These should use IPMan
	// Left Side
	if(thrusterActives[Thruster::FRONT_LEFT])
	{
		velocity->z -= backwardAcceleration * dt;
		angularVelocity->y -= frontSpin * dt;
		thrusters[Thruster::FRONT_LEFT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::SIDE_LEFT])
	{
		velocity->x += strafeAcceleration * dt;
		angularVelocity->z -= sideSpin * dt;
		thrusters[Thruster::SIDE_LEFT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::BACK_LEFT])
	{
		velocity->z += forwardAcceleration * dt;
		angularVelocity->y += backSpin * dt;
		thrusters[Thruster::BACK_LEFT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::BOTTOM_LEFT])
	{
		velocity->y += ascentAcceleration * dt;
		angularVelocity->z -= bottomSpin * dt;
		thrusters[Thruster::BOTTOM_LEFT]->SetVisible(true);
		active = true;
	}
	// Right Side
	if(thrusterActives[Thruster::BOTTOM_RIGHT])
	{
		velocity->y += ascentAcceleration * dt;
		angularVelocity->z += bottomSpin * dt;
		thrusters[Thruster::BOTTOM_RIGHT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::BACK_RIGHT])
	{
		velocity->z += forwardAcceleration * dt;
		angularVelocity->y -= backSpin * dt;
		thrusters[Thruster::BACK_RIGHT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::SIDE_RIGHT])
	{
		velocity->x -= strafeAcceleration * dt;
		angularVelocity->z += sideSpin * dt;
		thrusters[Thruster::SIDE_RIGHT]->SetVisible(true);
		active = true;
	}
	if(thrusterActives[Thruster::FRONT_RIGHT])
	{
		velocity->z -= backwardAcceleration * dt;
		angularVelocity->y += frontSpin * dt;
		thrusters[Thruster::FRONT_RIGHT]->SetVisible(true);
		active = true;
	}
}