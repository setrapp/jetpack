#include "Jetpack.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "HUD.h"

Jetpack::Jetpack(Entity* player)
{
	this->player = player;
	active = false;
	allowInputForces = false;
	maxSpeed = 10;
	forwardAcceleration = 1.0f;
	backwardAcceleration = 1.0f;
	strafeAcceleration = 1.0f;
	ascentAcceleration = 2.0f;
	thrusterCount = 0;
	thrusters = NULL;
	thrusterActives = NULL;
	thrusterDirections = NULL;
	maxFuel = 100;
	fuel = maxFuel;
	fuelUseRate = 1;
	bool playerControllable = false;
	bool playerAI = false;
	targetAcceleration = XMFLOAT3(0, 0, 0);
	targetAngularAcceleration = XMFLOAT3(0, 0, 0);
	actionsPerSecond = -1;
	isNetworked=false;
	timeSinceAction = 0;
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
		delete thrusterActives;
		delete thrusterDirections;
	}
}

void Jetpack::Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity)
{
	active = false;
	
	if (fuel > 0)
	{
		if (playerControllable)
		{
			OffThrusters();
			CheckInput(dt, velocity, angularVelocity);
		}
		else if (playerAI)
		{
			if (actionsPerSecond <= 0 || timeSinceAction >= 1 / actionsPerSecond)
			{
				OffThrusters();
				ApproachTarget(dt, velocity, angularVelocity);
				timeSinceAction = 0;
			}
			else
			{
				timeSinceAction += dt;
			}
		}
	}

	ApplyForces(dt, velocity, angularVelocity);

	if (active)
	{
		fuel -= fuelUseRate * dt;
		if (fuel < 0)
		{
			fuel == 0;
			OffThrusters();
		}
	}

	if (playerControllable)
	{
		HUD::fuel = this->fuel;
	}
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

void Jetpack::ApproachTarget(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity){}

void Jetpack::ApplyForces(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity){}

void Jetpack::CreateThrusters()
{
	if (thrusterCount <= 0)
	{
		thrusters = NULL;
		return;
	}

	thrusters = new Entity*[thrusterCount];
	thrusterActives = new bool[thrusterCount];
	thrusterDirections = new XMFLOAT3[thrusterCount];
	for(int i = 0; i < thrusterCount; i++)
	{
		thrusters[i] = new Entity();
		thrusters[i]->AddModel(AssetManager::Instance()->GetModel("fireball"));
		thrusters[i]->Finalize();
		thrusters[i]->transform.SetParent(&player->transform);
		thrusters[i]->transform.SetLocalTranslation(XMFLOAT3(0, 0, 0));
		thrusterDirections[i] = XMFLOAT3(0, 1, 0);
	}
	OffThrusters();
	active = false;
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

void Jetpack::OffThrusters()
{
	for (int i = 0; i < thrusterCount; i++)
	{
		thrusters[i]->SetVisible(false);
		thrusterActives[i] = false;
	}
}