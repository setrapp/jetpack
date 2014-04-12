#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "DemoGame.h"
#include "Camera.h"
#include "ClientConnectionEntity.h"
#include "ServerConnectionEntity.h"
#include <queue>

class Player : public Entity
{
public:
	Player();
	Player(DemoGame* v_game);
	void Update(float dt);

public:
	Camera* camera;
	XMFLOAT3 cameraPos;
	DemoGame* game;
	ClientConnectionEntity* clientEntity;
	ServerConnectionEntity* serverEntity;
	Entity* networkedCube;
	std::map<int,Entity*> networkedEntities;

private:
	void CheckInput(float dt);

private:
	XMFLOAT3 velocity;
	float maxSpeed;
	float groundSpeedDampening;

};