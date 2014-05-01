#include "ManeuverJetpack.h"
#include "Common.h"



ManeuverJetpack::ManeuverJetpack()
{
	forwardAcceleration = 100.0f;
	backwardAcceleration = 100.0f;
	strafeAcceleration = 100.0f;
	ascentAcceleration = 150.0f;
	backSpin = 50 * (PI / 180);
	frontSpin = 50 * (PI / 180);
	sideSpin = 50 * (PI / 180);
	bottomSpin = 50 * (PI / 180);
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