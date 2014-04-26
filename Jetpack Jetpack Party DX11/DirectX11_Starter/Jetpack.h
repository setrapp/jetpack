#pragma once
#include "Common.h"

class Jetpack
{
public:
	Jetpack();
	virtual void Update(XMFLOAT3* velocity);
	virtual void CheckInput(XMFLOAT3* velocity);

public:
	bool allowInputForces;
	float maxSpeed;
};