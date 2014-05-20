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

		currentScale = farPlane * 100;

		AssetManager::Instance()->StoreMaterial(new Material(XMFLOAT4(1, 1, 1, 1), XMFLOAT4(0, 0, 0, 1), 0, 128), "skybox");
		transform.Scale(XMFLOAT3(currentScale, currentScale, currentScale));
		SetBaseMaterial("skybox");
		GetBaseMaterial()->pixelShader = AssetManager::Instance()->GetPixelShader("texture");	
		__super::LoadTexture(L"../Assets/Textures/skybox3.png");

		transform.SetLocalRotation(XMFLOAT3(PI / 2, 0, 0));

		oldPos = player->transform.GetTranslation();
		Finalize();	

		D3D11_DEPTH_STENCIL_DESC dssDesc;
		ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		dssDesc.DepthEnable = false;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dssDesc.StencilEnable = true;
		dssDesc.StencilReadMask = 0xFF;
		dssDesc.StencilWriteMask = 0xFF;
		dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		
		DXConnection::Instance()->device->CreateDepthStencilState(&dssDesc, &DSLessEqual);
	}

	~Skybox()
	{
		ReleaseMacro(noCull);
	}

public:
	void Draw(EntityDrawArgs const* drawArgs, XMFLOAT4X4 const* view = NULL, XMFLOAT4X4 const* projection = NULL)
	{						

		DXConnection::Instance()->deviceContext->OMSetDepthStencilState(DSLessEqual, 0);
		DXConnection::Instance()->deviceContext->RSSetState(noCull);

		__super::Draw(drawArgs, view, projection);

		DXConnection::Instance()->deviceContext->RSSetState(NULL);
		DXConnection::Instance()->deviceContext->OMSetDepthStencilState(NULL, 0);
	}
};