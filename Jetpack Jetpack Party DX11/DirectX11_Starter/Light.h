#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "dxerr.h"

using namespace DirectX;

class Light
{
public:
	Light();
	Light(XMFLOAT3 position, XMFLOAT4 color, bool isDirectional);
	Light GetUsableCopy();

public:
	Transform* transform;
	XMFLOAT4 color;
	bool isDirectional;
};