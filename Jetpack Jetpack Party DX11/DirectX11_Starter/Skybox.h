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
	ID3D11DepthStencilState* DSLessEqual;
	float currentScale;

public:
	
	XMFLOAT3 oldPos;

	Skybox(const float farPlane,  const Entity* player)
	{
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_FRONT;
		DXConnection::Instance()->device->CreateRasterizerState(&rastDesc, &noCull);
		AddModel(AssetManager::Instance()->GetModel("skybox"));

		currentScale = farPlane * 20;

		transform.Scale(XMFLOAT3(currentScale, currentScale, currentScale));
		SetBaseMaterial("skybox");
		GetBaseMaterial()->pixelShader = AssetManager::Instance()->GetPixelShader("texture");	
		__super::LoadTexture(L"../Assets/Textures/Skyboxnew1.png");

		transform.SetLocalRotation(XMFLOAT3(PI / 2, 0, 0));

		oldPos = player->transform.GetTranslation();
		Finalize();	
	}

	~Skybox()
	{
		ReleaseMacro(noCull);
	}

public:
	FORCEINLINE void Draw(EntityDrawArgs const* drawArgs, XMFLOAT4X4 const* view = NULL, XMFLOAT4X4 const* projection = NULL)
	{				
		//D3D11_DEPTH_STENCIL_DESC desc;
		D3D11_DEPTH_STENCIL_DESC dssDesc;
		ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		dssDesc.DepthEnable = true;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
		DXConnection::Instance()->device->CreateDepthStencilState(&dssDesc, &DSLessEqual);
		DXConnection::Instance()->deviceContext->OMSetDepthStencilState(DSLessEqual, 0);
		DXConnection::Instance()->deviceContext->RSSetState(noCull);

		__super::Draw(drawArgs, view, projection);

		DXConnection::Instance()->deviceContext->RSSetState(NULL);
		DXConnection::Instance()->deviceContext->OMSetDepthStencilState(NULL, 0);
	}
};