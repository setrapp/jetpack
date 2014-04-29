#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "Camera.h"
#include "ClientConnectionEntity.h"
#include "MessageTypes.h";
#include <queue>
#include "Jetpack.h"
#include "ManeuverJetpack.h"

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
	vector<string>* breakIntoParts(string s);
	bool loggedIn;

private:
	void CheckInput(float dt);

private:
	XMFLOAT3 angularVelocity;
	XMFLOAT3 velocity;
	float maxSpeed;
	float forwardAcceleration;
	float backwardAcceleration;
	float strafeAcceleration;
	float gravityAcceleration;
	float terminalVelocity;
	float groundSpeedDampening;
	float airSpeedDampening;
	bool grounded;
	Jetpack* jetpack;
};