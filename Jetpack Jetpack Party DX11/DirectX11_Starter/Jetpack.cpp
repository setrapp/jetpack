#include "Jetpack.h"
#include "InputManager.h"
#include "Entity.h"
#include "AssetManager.h"
#include "HUD.h"

Jetpack::Jetpack(Entity* player)
{
	this->player = player;
	active = false;
	allowInputForces = false;
	maxSpeed = 1000;
	forwardAcceleration = 100.0f;
	backwardAcceleration = 100.0f;
	strafeAcceleration = 100.0f;
	ascentAcceleration = 200.0f;
	thrusterCount = 0;
	thrusters = NULL;
	maxFuel = 100;
	fuel = maxFuel;
	fuelUseRate = 1;
}

Jetpack::~Jetpack()
{
	if (thrusters)
	{
		// Deleted by game
		//for(int i = 0; i < thrusterCount; i++)
		//{
		//	delete thrusters[i];
		//	thrusters[i] = NULL;
		//}
		delete thrusters;
	}
}

void Jetpack::Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	active = false;
	if (fuel > 0)
	{
		CheckInput(dt, velocity, angularVelocity);
	}
	if (active)
	{
		fuel -= fuelUseRate * dt;
		Debug::Log(Debug::ToString(fuel));
		if (fuel < 0)
		{
			fuel == 0;
		}
	}
	
	HUD::fuel = this->fuel;
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

void Jetpack::CreateThrusters()
{
	if (thrusterCount <= 0)
	{
		thrusters = NULL;
		return;
	}

	thrusters = new Entity*[thrusterCount];
	for(int i = 0; i < thrusterCount; i++)
	{
		thrusters[i] = new Entity();
		thrusters[i]->AddModel(AssetManager::Instance()->GetModel("fireball"));
		thrusters[i]->Finalize();
		thrusters[i]->transform.SetParent(&player->transform);
		thrusters[i]->transform.SetLocalTranslation(XMFLOAT3(0, 0, 0));
	}
}

void Jetpack::Refuel(int fuelAdd)
{
	if (fuelAdd == MAX_FUEL)
	{
		fuelAdd = maxFuel;
	}

	fuel += fuelAdd;

	if (fuel > maxFuel)
	{
		fuel = maxFuel;
	}
}