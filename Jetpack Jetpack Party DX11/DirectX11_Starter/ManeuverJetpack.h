#pragma once

#include "Jetpack.h"

class ManeuverJetpack : public Jetpack
{
public:
	ManeuverJetpack(Entity* player);
	void Update(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	void CheckInput(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	void ApproachTarget(XMFLOAT3 desiredTranslation, float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);
	void ApplyForces(float dt, XMFLOAT3* velocity, XMFLOAT3* angularVelocity);

public:
	float backSpin;
	float frontSpin;
	float sideSpin;
	float bottomSpin;

private:
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