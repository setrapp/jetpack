#pragma once

#include "Jetpack.h"

class ManeuverJetpack : public Jetpack
{
public:
	ManeuverJetpack(Entity* player);
	void Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	void CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	void ApproachTarget(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	void ApplyForces(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	void updateThrusters(int thrusterData);

public:
	float backSpin;
	float frontSpin;
	float sideSpin;
	float bottomSpin;
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