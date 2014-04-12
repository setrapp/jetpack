#pragma once
#include <d3d11.h>
#include <conio.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

using namespace DirectX;

struct ShaderMaterial
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMUINT4 shininess;
};

class Material
{
public:
	Material();
	Material(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, UINT shininess);
	Material(wchar_t* path);
	~Material(void);	
	void ApplyTexture(wchar_t* path);
	ShaderMaterial GetShaderMaterial();
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	UINT shininess;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* resourceView;
	ID3D11SamplerState* samplerState;
};

