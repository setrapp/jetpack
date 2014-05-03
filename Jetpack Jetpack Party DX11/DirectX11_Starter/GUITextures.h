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
	virtual ~GUITexture() 
	{  	
		delete rect;
		ReleaseMacro(texture);
		ReleaseMacro(resourceView);
	}
	ID3D11ShaderResourceView* resourceView;
	RECT* rect;
	SpriteRenderer* spBatch; 
	bool isStatic;
	ID3D11Resource* texture;
	
	GUITexture(RECT* rect, wchar_t* path, SpriteRenderer* spBatch, bool isStatic = false, FXMVECTOR color = Colors::White, XMFLOAT2 Scale =XMFLOAT2(1, 1), float rotation = 0, float depth = 1, SpriteEffects spriteEffects = DirectX::SpriteEffects::SpriteEffects_None)
	{
		CreateWICTextureFromFile(
		DXConnection::Instance()->device, 
		DXConnection::Instance()->deviceContext, 
		path, 
		&texture, 
		&this->resourceView);
		this->rect = new RECT();
		SetRect(rect);
		
		XMStoreFloat4(&this->color, color);
		this->rotation = rotation;
		this->depth = depth;
		this->spriteFX = spriteEffects;		
		clicked = false;
		this->scale = Scale;
		hover = false;			
		this->spBatch = spBatch;
		this->isStatic = isStatic;
		XMStoreFloat4(&this->color, color);
		if(!isStatic)
			this->color.w = 0;
	}

public :
		 void Render() const
		 {			 
			 if(resourceView)
				 spBatch->GetSpriteBatch()->Draw(resourceView, *rect, nullptr, XMLoadFloat4(&this->color), 0, XMFLOAT2(0, 0), DirectX::SpriteEffects::SpriteEffects_None, depth);
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

		 inline void SetRect(RECT* rect)
		 {
			 *this->rect = *rect;
			 //this->rect = rect;
		 }

		 


		 
		 void Update (LPPOINT point, float dt)
		 {		
			 if(!isStatic)
			 {
				 this->color.w += dt;

				RECT rect1;
				GetWindowRect(GetActiveWindow(), &rect1);
				 //Inside the rect or not ?					MOUSE LEFT IS PRESSED
				int x, y;
					 x = point->x - rect1.left;
					 y = point->y - rect1.top - 21;
				 

				Rect* rectLittle = Rect::GetRectFromRECT(rect);
				if(rectLittle->Contains(x, y))
				 {
					 hover = true;
					 if(((GetKeyState(VK_LBUTTON) & 0x80) != 0))
					 {			 
						clicked = true;
					 }
				 }
				 else
					 hover = false;
				delete rectLittle;
			}
		 }

		 private:
			 XMFLOAT4 color;
};
