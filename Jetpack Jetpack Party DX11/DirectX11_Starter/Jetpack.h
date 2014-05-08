#pragma once
#include "Common.h"

class Entity;

class Jetpack
{
public:
	Jetpack(Entity* player);
	~Jetpack();
	virtual void Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	virtual void CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	virtual void ApproachTarget(XMFLOAT3 desiredTranslation, float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	virtual void ApplyForces(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	void Refuel(int fuelAdd);

protected:
	void CreateThrusters();

public:

	static const int MAX_FUEL = -1;


	bool allowInputForces;
	float maxSpeed;
	float forwardAcceleration;
	float backwardAcceleration;
	float strafeAcceleration;
	float ascentAcceleration;
	bool active;
	Entity* player;
	Entity** thrusters;
	bool* thrusterActives;
	int thrusterCount;
	float fuel;
	float maxFuel;
	float fuelUseRate;
	bool playerControlled;
};