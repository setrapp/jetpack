#pragma once
#include <d3d11.h>
#include <conio.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "Common.h"

using namespace DirectX;

class Material
{
public:
	Material();
	Material(XMFLOAT4 ambient, XMFLOAT4 diffuse, float specular, UINT shininess);
	Material(wchar_t* path);
	~Material(void);	
	void ApplyTexture(wchar_t* path, bool isDDS = false);
	ShaderMaterial GetShaderMaterial();
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	float specular;
	UINT shininess;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* resourceView;
	ID3D11SamplerState* samplerState;
private:
	void Init();
};

