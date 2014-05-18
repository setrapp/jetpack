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

public:
	
	XMFLOAT3 oldPos;

	Skybox(float farPlane,  Entity* player)
	{
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_FRONT;
		DXConnection::Instance()->device->CreateRasterizerState(&rastDesc, &noCull);
		AddModel(AssetManager::Instance()->GetModel("skybox"));
		farPlane /= 2;
		transform.Scale(XMFLOAT3(farPlane, farPlane, farPlane));
		SetBaseMaterial("skybox");
		GetBaseMaterial()->pixelShader = AssetManager::Instance()->GetPixelShader("texture");
		//transform.Translate(XMFLOAT3(0, 0, 20));		
		__super::LoadTexture(L"../Assets/Textures/Skyboxnew1.png");
		transform.SetLocalRotation(XMFLOAT3(PI / 2, 0, 0));
		//oldPos = playerCurrentPosition;
		Finalize();	
		//transform.SetParent(&player->transform);
	}

	~Skybox()
	{
		ReleaseMacro(noCull);
	}

	void Update(float dt, XMFLOAT3 newPosition)
	{
		//XMVECTOR diff = XMVector3Length(XMVectorSubtract(XMLoadFloat3(&newPosition), XMLoadFloat3(&oldPos)));
	//	XMFLOAT3 translationReqd;
		//diff = XMVector4Normalize(diff);
		//XMStoreFloat3(&translationReqd, diff);
		//
		////translationReqd = this->transform.GetTranslation();
		//Debug::Log(Debug::ToString(XMFLOAT4(translationReqd.x, translationReqd.y, translationReqd.z, 0)));
		//btClamp<float>(translationReqd.z, -1, 1);
		//this->transform.Translate(XMFLOAT3(0, translationReqd.z, 0));
		////this->transform.Translate(XMFLOAT3(0, translationReqd.z, 0));
		//XMFLOAT3 temp ;
		//XMStoreFloat3(&temp, XMVector4Normalize(XMLoadFloat3(&this->transform.GetTranslation())));
		//this->transform.Translate(temp);
		//translationReqd = this->transform.GetTranslation();
		//Debug::Log("POS::\n");
		//Debug::Log(Debug::ToString(XMFLOAT4(translationReqd.x, translationReqd.y, translationReqd.z, 0)));
		//this->transform.Translate(translationReqd);

		XMFLOAT3 translationNeeded ;
		translationNeeded = newPosition;
		translationNeeded.y -= 1000;
		this->transform.SetTranslation(translationNeeded);
		XMFLOAT3 translationReqd = this->transform.GetTranslation();
		Debug::Log("POS::\n");
		Debug::Log(Debug::ToString(XMFLOAT4(translationReqd.x, translationReqd.y, translationReqd.z, 0)));
	}

public:
	void Draw(EntityDrawArgs const* drawArgs, XMFLOAT4X4 const* view = NULL, XMFLOAT4X4 const* projection = NULL)
	{				
		//D3D11_DEPTH_STENCIL_DESC desc;
		D3D11_DEPTH_STENCIL_DESC dssDesc;
		ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		dssDesc.DepthEnable = true;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		DXConnection::Instance()->device->CreateDepthStencilState(&dssDesc, &DSLessEqual);
		DXConnection::Instance()->deviceContext->OMSetDepthStencilState(DSLessEqual, 0);
		DXConnection::Instance()->deviceContext->RSSetState(noCull);

		__super::Draw(drawArgs, view, projection);

		DXConnection::Instance()->deviceContext->RSSetState(NULL);
		DXConnection::Instance()->deviceContext->OMSetDepthStencilState(NULL, 0);
	}
};