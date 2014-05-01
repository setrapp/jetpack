#pragma once
#include "Common.h"

class Entity;

class Jetpack
{
public:
	Jetpack();
	~Jetpack();
	virtual void Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	virtual void CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);

public:
	bool allowInputForces;
	float maxSpeed;
	float forwardAcceleration;
	float backwardAcceleration;
	float strafeAcceleration;
	float ascentAcceleration;
	bool active;
	Entity** thrusters;
	int thrusterCount;
};