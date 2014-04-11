#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "dxerr.h"

using namespace DirectX;

struct ShaderLight
{
	XMFLOAT4X4 world;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
};

class Light
{
public:
	Light();
	Light(XMFLOAT3 position, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, bool isDirectional);
	ShaderLight ConvertToShaderLight();

public:
	Transform* transform;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	bool isDirectional;
};