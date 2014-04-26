#pragma once
#include "Common.h"

class Jetpack
{
public:
	virtual void Update();
	virtual void CheckInput();

private:
	XMFLOAT3 velocity;
};