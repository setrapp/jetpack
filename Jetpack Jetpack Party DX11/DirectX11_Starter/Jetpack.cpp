#include "Jetpack.h"
#include "InputManager.h"
#include "Entity.h"
#include "AssetManager.h"

Jetpack::Jetpack()
{
	active = false;
	allowInputForces = false;
	maxSpeed = 1000;
	forwardAcceleration = 100.0f;
	backwardAcceleration = 100.0f;
	strafeAcceleration = 100.0f;
	ascentAcceleration = 200.0f;
	thrusterCount = 0;
	thrusters = NULL;

	/*for(int i = 0; i < thrusterCount; i++)
	{
		thrusters[i] = new Entity();
		thrusters[i]->AddModel(AssetManager::Instance()->GetModel("fireball"));
	}*/
}

Jetpack::Jetpack()
{
	/*for(int i = 0; i < thrusterCount; i++)
	{
		delete thrusters[i];
	}
	delete thrusters;*/
}

void Jetpack::Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	active = false;
	CheckInput(dt, velocity, angularVelocity);
}

void Jetpack::CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	if (velocity && allowInputForces)
	{
		if(IPMan::GetIPMan()->GetKey(KeyType::FORWARD))
		{
			velocity->z += forwardAcceleration * dt;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::BACKWARD))
		{
			velocity->z -= backwardAcceleration * dt;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::LEFT))
		{
			velocity->x -= strafeAcceleration * dt;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::RIGHT))
		{
			velocity->x += strafeAcceleration * dt;
		}

		// TODO: This should use IPMan
		if(GetAsyncKeyState(VK_SPACE))
		{
			velocity->y += ascentAcceleration * dt;
			active = true;
		}
	}
}