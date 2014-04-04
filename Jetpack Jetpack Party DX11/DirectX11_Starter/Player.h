#pragma once

#include "Entity.h"
#include "Camera.h"

class Player : public Entity
{
public:
	Player();
	void Update(float dt);

public:
	Camera* camera;
	XMFLOAT3 cameraPos;

private:
	void CheckInput(float dt);

private:
	XMFLOAT3 velocity;
	float maxSpeed;
	float groundSpeedDampening;

};