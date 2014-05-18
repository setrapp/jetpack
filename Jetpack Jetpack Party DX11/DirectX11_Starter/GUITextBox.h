#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Common.h"
#include "Rect.h"
#include "FontRenderer.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include <functional>
#include <utility>
#include "AnimationBase.h"
#include "GUIBase.h"

using namespace std;
using namespace DirectX;

class GUITextBox : public GUIBase
{
public :  

	~GUITextBox()
	{
		delete rect;
	}

	wchar_t* str;
	ID3D11ShaderResourceView* backgroundResourceView;
	ID3D11Resource* backgroundTexture;
	SpriteRenderer* spBatch; 

	//You can have the width declared as 0. It will be taken care of considering the size of string.
	//0 Height will make the rect only as long as the font size
	//Item count represents how late you want the animation to affect for the item. Set it to 0 to have all items show up at once.
	GUITextBox(Rect* rect, wchar_t* str, int itemCount, AnimationType anim, FontRenderer* renderer, float animationSpeed, wchar_t* path,  SpriteRenderer* spBatch, FXMVECTOR color = Colors::White, XMFLOAT2 Scale =XMFLOAT2(1, 1), float rotation = 0, float depth = 0, SpriteEffects spriteEffects = DirectX::SpriteEffects::SpriteEffects_None)
	{
		this->str = str;
		this->rect = new Rect();
		this->fontRenderer = renderer;
		auto sp = this->fontRenderer->GetSpriteFont();

		if(rect->width == 0)
		{
		rect->width = sp->MeasureString(str).m128_f32[0];
		}

		if(rect->height == 0)
		{
		rect->height = sp->MeasureString(str).m128_f32[1];
		}

		CreateWICTextureFromFile(
		DXConnection::Instance()->device, 
		DXConnection::Instance()->deviceContext, 
		path, 
		&backgroundTexture, 
		&this->backgroundResourceView);

		SetRect(rect);
		XMStoreFloat4(&this->color, color);
		this->rotation = rotation;
		this->depth = depth;
		this->spriteFX = spriteEffects;		
		clicked = false;
		this->scale = Scale;
		this->spBatch = spBatch;
		hover = false;
		offset = 250;
		this->animation = anim;
		RECT rect1;
		GetWindowRect(GetActiveWindow(), &rect1);
		if(animation == AnimationType::LEFTTORIGHT)
		{
			basePosition = XMFLOAT2(this->rect->x - rect1.left - rect1.right - offset * itemCount, this->rect->y);
		}
		else
			if(animation == AnimationType::RIGHTTOLEFT)
			{					 
				basePosition = XMFLOAT2(this->rect->x + rect1.left + rect1.right + offset * itemCount, this->rect->y);
			}
			else
				if(animation == AnimationType::TOPTOBOTTOM)
				{
					basePosition = XMFLOAT2(this->rect->x, this->rect->y - rect1.top - rect1.bottom- offset * itemCount);
				}
				else
				{
					basePosition = XMFLOAT2(this->rect->x, this->rect->y + offset + rect1.top + rect1.bottom + offset * itemCount);
				}
	startPosition = basePosition;
		Origin = XMFLOAT2();	
		this->animationSpeed = animationSpeed;
	}

	void Update (LPPOINT point, float dt)
	{			
	Animate(dt * animationSpeed);
	RECT rect1;
	GetWindowRect(GetActiveWindow(), &rect1);
		//Inside the rect or not ?					MOUSE LEFT IS PRESSED
	int x, y;
			x = point->x - rect1.left;
			y = point->y - rect1.top - 21;

		if(rect->Contains(x, y))
		{
			hover = true;
			if(((GetKeyState(VK_LBUTTON) & 0x80) != 0))
			{			 
			clicked = true;
			}
		}
		else
			hover = false;
	}

	void Render() const
	{
		if(backgroundResourceView)
			spBatch->GetSpriteBatch()->Draw(backgroundResourceView, *rect->GetRECTFromRect(rect), nullptr, XMLoadFloat4(&this->color), 0, XMFLOAT2(0, 0), DirectX::SpriteEffects::SpriteEffects_None, 0.75f);

		auto sp = this->fontRenderer->GetSpriteFont();
		sp->DrawString(fontRenderer->GetSpriteBatch(), str, XMLoadFloat2(&XMFLOAT2(basePosition.x, basePosition.y)), XMLoadFloat4(&this->color), rotation, XMLoadFloat2(&XMFLOAT2(0, 0)), XMLoadFloat2(&scale), spriteFX, depth);
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

	inline Rect GetRect()
	{
		return *rect;
	}

private :
	inline void Animate(float dt)
	{
		if(animation == AnimationType::LEFTTORIGHT || animation == AnimationType::RIGHTTOLEFT)
		{
			if(basePosition.x == startPosition.x || abs(rect->x - startPosition.x) > abs(basePosition.x - startPosition.x))
			{
				if(animation == AnimationType::LEFTTORIGHT)
					basePosition.x+=dt;
				else
					basePosition.x-=dt;
			}
		}
		else
		{
			if(basePosition.y == startPosition.y || abs(rect->y - startPosition.y) > abs(basePosition.y - startPosition.y))
			{
				if(animation == AnimationType::TOPTOBOTTOM)
						basePosition.y+=dt;
					else
						basePosition.y-=dt;
			}
		}
	}
};
