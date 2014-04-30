#pragma once
#include "Common.h"

class Jetpack
{
public:
	Jetpack();
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
};