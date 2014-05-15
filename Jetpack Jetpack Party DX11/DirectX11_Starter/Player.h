#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "Camera.h"
#include "ClientConnectionEntity.h"
#include "MessageTypes.h"
#include <queue>
#include "Jetpack.h"
#include "FuelStation.h"
#include "ManeuverJetpack.h"

class Player : public Entity
{
public:
	Player();
	~Player();
	void Update(float dt);
	void Respawn();
	vector<string>* breakIntoParts(string s);

public:
	ClientConnectionEntity* clientEntity;
	Entity* networkedCube;
	std::map<int,Entity*> networkedEntities;
	float networkSendTimer;
	bool loggedIn;
	bool controllable;
	Jetpack* jetpack;
	XMFLOAT3 respawnPosition;
	XMFLOAT3 respawnLocalRotation;
	XMFLOAT3 targetPosition; 
	XMFLOAT3 targetUp;
	FuelStation* targetCheckpoint;
	FuelStation* targetFuelStation;

private:
	void CheckInput(float dt);
	void ComputeJetpackTargets(XMFLOAT3 currentVelocity, XMFLOAT3 currentAngularVelocity);

private:
	float minPosture;
	XMFLOAT3 angularVelocity;
	XMFLOAT3 worldVelocity;
	float maxSpeed;
	float forwardAcceleration;
	float backwardAcceleration;
	float strafeAcceleration;
	float gravityAcceleration;
	float terminalVelocity;
	float groundSpeedDampening;
	float airSpeedDampening;
	bool grounded;
};