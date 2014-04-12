#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Common.h"
#include "Transform.h"

using namespace DirectX;

class Light
{
public:
	Light();
	Light(XMFLOAT3 position, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, bool isDirectional);
	ShaderLight GetShaderLight();

public:
	Transform* transform;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	bool isDirectional;
};