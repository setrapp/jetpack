#pragma once
#include "Common.h"

class Jetpack
{
public:
	Jetpack();
	virtual void Update(float dt, XMFLOAT3* velocity);
	virtual void CheckInput(float dt, XMFLOAT3* velocity);

public:
	bool allowInputForces;
	float maxSpeed;
	float forwardAcceleration;
	float backwardAcceleration;
	float strafeAcceleration;
	float ascentAcceleration;
	bool active;
};