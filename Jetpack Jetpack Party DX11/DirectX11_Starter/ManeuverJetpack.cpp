#include "ManeuverJetpack.h"
#include "Common.h"
#include "Entity.h"
//#include "Player.h"

ManeuverJetpack::ManeuverJetpack(Entity* player) : Jetpack(player)
{
	forwardAcceleration = 100.0f;
	backwardAcceleration = 100.0f;
	strafeAcceleration = 100.0f;
	ascentAcceleration = 400.0f;
	backSpin = 10 * (PI / 180);
	frontSpin = 10 * (PI / 180);
	sideSpin = 20 * (PI / 180);
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
		velocity->z -= backwardAcceleration * dt;
		angularVelocity->y -= frontSpin * dt;
		thrusters[Thruster::FRONT_LEFT]->SetVisible(true);
		active = true;
	}
	if(GetAsyncKeyState('A'))//(('S'))
	{
		velocity->x += strafeAcceleration * dt;
		angularVelocity->z -= sideSpin * dt;
		thrusters[Thruster::SIDE_LEFT]->SetVisible(true);
		active = true;
	}
	if(GetAsyncKeyState('S'))//(('D'))
	{
		velocity->z += forwardAcceleration * dt;
		angularVelocity->y += backSpin * dt;
		thrusters[Thruster::BACK_LEFT]->SetVisible(true);
		active = true;
	}
	if(GetAsyncKeyState('D'))//(('F'))
	{
		velocity->y += ascentAcceleration * dt;
		angularVelocity->z -= bottomSpin * dt;
		thrusters[Thruster::BOTTOM_LEFT]->SetVisible(true);
		active = true;
	}
	// Right Side
	if(GetAsyncKeyState('J'))//(('J'))
	{
		velocity->y += ascentAcceleration * dt;
		angularVelocity->z += bottomSpin * dt;
		thrusters[Thruster::BOTTOM_RIGHT]->SetVisible(true);
		active = true;
	}
	if(GetAsyncKeyState('K'))//(('K'))
	{
		velocity->z += forwardAcceleration * dt;
		angularVelocity->y -= backSpin * dt;
		thrusters[Thruster::BACK_RIGHT]->SetVisible(true);
		active = true;
	}
	if(GetAsyncKeyState('L'))//(('L'))
	{
		velocity->x -= strafeAcceleration * dt;
		angularVelocity->z += sideSpin * dt;
		thrusters[Thruster::SIDE_RIGHT]->SetVisible(true);
		active = true;
	}
	if(GetAsyncKeyState('I'))//((VK_OEM_1)) // colon on US standard keyboards
	{
		velocity->z -= backwardAcceleration * dt;
		angularVelocity->y += frontSpin * dt;
		thrusters[Thruster::FRONT_RIGHT]->SetVisible(true);
		active = true;
	}
}