#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include "dxerr.h"
#include "DXConnection.h"

using namespace DirectX;
using namespace std;

#define Print(x) { if(x) { printf(x); } }

#define ReleaseMacro(x) { if(x){ x->Release(); x = 0; } }

// Macro for popping up a text box based
// on a failed HRESULT and then quitting (only in debug builds)
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)												\
	{															\
		HRESULT hr = (x);										\
		if(FAILED(hr))											\
		{														\
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);	\
			PostQuitMessage(0);									\
		}														\
	}														
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif

struct ShaderMaterial
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMUINT4 shininess;
};

struct ShaderLight
{
	XMFLOAT4X4 world;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
};

struct VertexShaderModelConstantBuffer
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 inverseTranspose;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

struct MaterialsAndLightsConstantBuffer
{
	ShaderMaterial material;
	ShaderLight light;
};

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};
