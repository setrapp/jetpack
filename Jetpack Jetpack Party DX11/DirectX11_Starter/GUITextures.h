#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Common.h"
#include "Rect.h"
#include "SpriteRenderer.h"
#include <functional>
#include "GUIBase.h"
#include "AnimationBase.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include "Material.h"
#include "Common.h"
#include <string>
#include <wchar.h>
#include "AssetManager.h"

using namespace std;
using namespace DirectX;

class GUITexture : public GUIBase
{
public :
	virtual ~GUITexture() { delete fontRenderer; delete rect;}
	ID3D11ShaderResourceView* resourceView;
	ID3D11SamplerState* samplerState;	
	ID3D11Resource* texture;
	RECT* rect;
	SpriteBatch* spBatch; 
	GUITexture(RECT* rect, wchar_t* path,SpriteBatch* spBatch, FXMVECTOR color = Colors::White, XMFLOAT2 Scale =XMFLOAT2(1, 1), float rotation = 0, float depth = 1, SpriteEffects spriteEffects = DirectX::SpriteEffects::SpriteEffects_None)
	{
		CreateWICTextureFromFile(
		DXConnection::Instance()->device, 
		DXConnection::Instance()->deviceContext, 
		path, 
		&texture, 
		&this->resourceView, sizeof(ID3D11ShaderResourceView));

		/*D3D11_SAMPLER_DESC sBufferDesc;
		sBufferDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sBufferDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sBufferDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sBufferDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sBufferDesc.MaxAnisotropy = 16;
		sBufferDesc.MipLODBias = 0;

		DXConnection::Instance()->device->CreateSamplerState(
			&sBufferDesc,
			&this->samplerState);
			*/
			 this->rect = rect;
			 XMStoreFloat4(&this->color, color);
			 this->rotation = rotation;
			 this->depth = depth;
			 this->spriteFX = spriteEffects;		
			 clicked = false;
			 this->scale = Scale;
			 hover = false;
			 
	}
public :
		 void Render() const
		 {			 
			 spBatch->Begin();
			 spBatch->Draw(resourceView, *rect);
			 spBatch->End(); 
		 }

		 const inline bool Clicked()
		 {
			 if(clicked)
			 {
 					clicked = !clicked;
					return !clicked;
			 }
			 else
				 return clicked;
		 }

		 const inline bool IsMouseHovering() const
		 {
			 return hover;
		 }

		 inline void SetColor(const XMFLOAT4 Color)
		 {
			 this->color = Color;
		 }

		 inline void SetRotation(float rot)
		 {
			 this->rotation = rot;
		 }

		 inline void SetSpriteEffects(const SpriteEffects fx)
		 {
			 this->spriteFX = fx;
		 }

		 inline void SetScale(XMFLOAT2 scale)
		 {
			 this->scale = scale;
		 }
};
