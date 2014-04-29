#pragma once

#include "Jetpack.h"

class ManeuverJetpack : public Jetpack
{
public:
	ManeuverJetpack();
	void CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);

public:
	float backSpin;
	float frontSpin;
	float sideSpin;
	float bottomSpin;
};