#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Entity.h"
#include "Material.h"
#include "Toolkit\Inc\DDSTextureLoader.h"
#include "Transform.h"
#include "Common.h"
#include "DXConnection.h"
#include <d3d11shader.h>
#include "Common.h"

using namespace std;
using namespace DirectX;


class Skybox : public Entity
{
	ID3D11RasterizerState* noCull;

public:
	Skybox(float farPlane)
	{
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_FRONT;
		DXConnection::Instance()->device->CreateRasterizerState(&rastDesc, &noCull);
		AddModel(AssetManager::Instance()->GetModel("skybox"));
		farPlane /= 2;
		transform.Scale(XMFLOAT3(farPlane, farPlane, farPlane));
		SetBaseMaterial("gift");
		GetBaseMaterial()->pixelShader = AssetManager::Instance()->GetPixelShader("texture");
		//transform.Translate(XMFLOAT3(0, 0, 20));		
		__super::LoadTexture(L"../Assets/Textures/skybox.png");
		Finalize();	
	}

	~Skybox()
	{
		ReleaseMacro(noCull);
	}

public:
	void Draw(EntityDrawArgs const* drawArgs, XMFLOAT4X4 const* view = NULL, XMFLOAT4X4 const* projection = NULL)
	{				
		//DXConnection::Instance()->deviceContext->ClearState();/*(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);*/
		DXConnection::Instance()->deviceContext->RSSetState(noCull);
		__super::Draw(drawArgs, view, projection);
		DXConnection::Instance()->deviceContext->RSSetState(NULL);
	}
};