#pragma once
#include "Vertex.h"
#include <d3d11.h>
#include <conio.h>
#include <DirectXMath.h>


class Material
{
public:
	Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* path);
	~Material(void);	
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* resourceView;
	ID3D11SamplerState* samplerState;
};

