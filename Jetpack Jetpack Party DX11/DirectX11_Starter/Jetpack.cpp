#include "Jetpack.h"
#include "InputManager.h"

Jetpack::Jetpack()
{
	allowInputForces = false;
	maxSpeed = 200;
}

void Jetpack::Update(XMFLOAT3* velocity)
{
	CheckInput(velocity);
}

void Jetpack::CheckInput(XMFLOAT3* velocity)
{
	if (velocity && allowInputForces)
	{
		// TODO: This should use IPMan
		if(GetAsyncKeyState(VK_SPACE))
		{
			velocity->y += 10.0f;
		}
	}
}