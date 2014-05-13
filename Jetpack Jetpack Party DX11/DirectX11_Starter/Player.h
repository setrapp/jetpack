#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "Camera.h"
#include "ClientConnectionEntity.h"
#include "UDPClientConnectionEntity.h"
#include "MessageTypes.h"
#include <queue>
#include "Jetpack.h"
#include "ManeuverJetpack.h"
#include "PositionLerp.h"

class Player : public Entity
{
public:
	Player();
	~Player();
	void Update(float dt);
	void Respawn();
	vector<string>* breakIntoParts(string s);

public:
	//ClientConnectionEntity* clientEntity;
	UDPClientConnectionEntity* clientEntity;
	Entity* networkedCube;
	std::map<int,Entity*> networkedEntities;
	std::map<int,XMFLOAT3> networkedEntityVelocities;
	std::map<int,PositionLerp> networkedEntityLerps;
	float networkSendTimer;
	bool loggedIn;
	bool controllable;
	Jetpack* jetpack;
	XMFLOAT3 respawnPosition;
	XMFLOAT3 respawnLocalRotation;
	XMFLOAT3 targetPosition; 
	float updateTimer;

private:
	void CheckInput(float dt);

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