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
	sideSpin = 10 * (PI / 180);
	bottomSpin = 10 * (PI / 180);
	thrusterCount = 8;
	CreateThrusters();
}

void ManeuverJetpack::CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	// TODO: These should use IPMan
	// Left Side
	if(GetAsyncKeyState('W'))//('A'))
	{
		velocity->z -= backwardAcceleration * dt;
		angularVelocity->y -= frontSpin * dt;
		active = true;
	}
	if(GetAsyncKeyState('A'))//(('S'))
	{
		velocity->x += strafeAcceleration * dt;
		angularVelocity->z -= sideSpin * dt;
		active = true;
	}
	if(GetAsyncKeyState('S'))//(('D'))
	{
		velocity->z += forwardAcceleration * dt;
		angularVelocity->y += backSpin * dt;
		active = true;
	}
	if(GetAsyncKeyState('D'))//(('F'))
	{
		velocity->y += ascentAcceleration * dt;
		angularVelocity->z -= bottomSpin * dt;
		active = true;
	}
	// Right Side
	if(GetAsyncKeyState('J'))//(('J'))
	{
		velocity->y += ascentAcceleration * dt;
		angularVelocity->z += bottomSpin * dt;
		active = true;
	}
	if(GetAsyncKeyState('K'))//(('K'))
	{
		velocity->z += forwardAcceleration * dt;
		angularVelocity->y -= backSpin * dt;
		active = true;
	}
	if(GetAsyncKeyState('L'))//(('L'))
	{
		velocity->x -= strafeAcceleration * dt;
		angularVelocity->z += sideSpin * dt;
		active = true;
	}
	if(GetAsyncKeyState('I'))//((VK_OEM_1)) // colon on US standard keyboards
	{
		velocity->z -= backwardAcceleration * dt;
		angularVelocity->y += frontSpin * dt;
		active = true;
	}
}