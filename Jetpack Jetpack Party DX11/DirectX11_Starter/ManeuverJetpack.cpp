#include "ManeuverJetpack.h"
#include "Common.h"
#include "Entity.h"
//#include "Player.h"

ManeuverJetpack::ManeuverJetpack(Entity* player) : Jetpack(player)
{
	maxSpeed = 1000;
	//fuelUseRate = 10;
	forwardAcceleration = 400.0f;
	backwardAcceleration = 400.0f;
	strafeAcceleration = 400.0f;
	ascentAcceleration = 600.0f;
	backSpin = 30 * (PI / 180);
	frontSpin = 30 * (PI / 180);
	sideSpin = 15 * (PI / 180);
	bottomSpin = 10 * (PI / 180);
	
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
	for (int i = 0; i < thrusterCount; i++)
	{
		thrusters[i]->SetVisible(false);
	}
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

void ManeuverJetpack::ApproachTarget(XMFLOAT3 desiredTranslation, float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	// Hmmmmm, figure out which steering options would get the jetpacker in the desired direction the fastest
	// Need to account for landing, may need to pass in a desired rotation or a landing flag.

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