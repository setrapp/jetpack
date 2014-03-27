#pragma once

#include "Entity.h"

class Player : public Entity
{
public:
	Player();
	void Update(float dt);

private:
	void CheckInput(float dt);

private:
	XMFLOAT3 velocity;
	float maxSpeed;
	float groundSpeedDampening;
};