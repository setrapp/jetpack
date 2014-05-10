#pragma once

#include "Camera.h"

class ControllableCamera : public Camera
{
public:
	ControllableCamera();
	void Update(float dt, VertexShaderModelConstantBuffer* vsConstantBufferdata);
	void CheckInput(float dt);

public:
	bool controllable;
private:
	XMFLOAT3 velocity;
	float maxSpeed;
	float groundSpeedDampening;
};