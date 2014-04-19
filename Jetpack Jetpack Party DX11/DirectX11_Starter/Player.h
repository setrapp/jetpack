#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "Camera.h"
#include "ClientConnectionEntity.h"
#include <queue>

class Player : public Entity
{
public:
	Player();
	void Update(float dt);

public:
	Camera* camera;
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