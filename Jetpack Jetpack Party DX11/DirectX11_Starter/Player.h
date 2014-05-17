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
#include "NetworkedPlayer.h"

class Player : public Entity
{
public:
	Player();
	~Player();
	void Update(float dt);
	void Respawn();

public:

	bool controllable;
	Jetpack* jetpack;
	XMFLOAT3 respawnPosition;
	XMFLOAT3 respawnLocalRotation;
	XMFLOAT3 targetPosition; 
	

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
	

	enum Thruster
	{
		BOTTOM_LEFT = 0,
		BOTTOM_RIGHT,
		BACK_LEFT,
		BACK_RIGHT,
		FRONT_LEFT,
		FRONT_RIGHT,
		SIDE_LEFT,
		SIDE_RIGHT
	};
};