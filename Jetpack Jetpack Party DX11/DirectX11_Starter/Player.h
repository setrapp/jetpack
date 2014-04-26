#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "Camera.h"
#include "ClientConnectionEntity.h"
#include "MessageTypes.h";
#include <queue>

class Player : public Entity
{
public:
	Player();
	~Player();
	void Update(float dt);

public:
	XMFLOAT3 cameraPos;
	ClientConnectionEntity* clientEntity;
	Entity* networkedCube;
	std::map<int,Entity*> networkedEntities;
	float networkSendTimer;

private:
	void CheckInput(float dt);

private:
	XMFLOAT3 velocity;
	float maxSpeed;
	float groundSpeedDampening;

};