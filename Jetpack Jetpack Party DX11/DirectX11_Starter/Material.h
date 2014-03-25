#pragma once
#include "Vertex.h"
#include <d3d11.h>
#include <conio.h>
#include <DirectXMath.h>


class Material
{
public:
	Material();
	Material(XMFLOAT4 color);
	Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* path);
	~Material(void);	
	void ApplyTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* path);
	XMFLOAT4 color;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* resourceView;
	ID3D11SamplerState* samplerState;
};

