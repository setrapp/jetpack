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

protected:
	void CreateThrusters();

public:
	bool allowInputForces;
	float maxSpeed;
	float forwardAcceleration;
	float backwardAcceleration;
	float strafeAcceleration;
	float ascentAcceleration;
	bool active;
	Entity* player;
	Entity** thrusters;
	int thrusterCount;
};