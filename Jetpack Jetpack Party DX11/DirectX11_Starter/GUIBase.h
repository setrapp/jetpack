#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Common.h"
#include "Rect.h"
#include "FontRenderer.h"
#include <functional>
#include "Menu.h"
#include <utility>

enum AnimationType
{
	LEFTTORIGHT,
	RIGHTTOLEFT
};

class GUIBase
{
public : Rect* rect;
		 wchar_t* str;
		 ~GUIBase()
		 {
			 delete rect;
			 delete str;
			 delete fontRenderer;
		 }

		 //You can have the width declared as 0. It will be taken care of considering the size of string.
		 //0 Height will make the rect only as long as the font size

		 GUIBase(Rect* rect, wchar_t* str, AnimationType anim, FontRenderer* renderer, FXMVECTOR color = Colors::White, XMFLOAT2 Scale =XMFLOAT2(1, 1), float rotation = 0, float depth = 1, SpriteEffects spriteEffects = DirectX::SpriteEffects::SpriteEffects_None)
		 {
			 this->str = str;
			 this->rect = rect;
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

			 XMStoreFloat4(&this->color, color);
			 this->rotation = rotation;
			 this->depth = depth;
			 this->spriteFX = spriteEffects;		
			 clicked = false;
			 this->scale = Scale;
			 hover = false;
			 offset = 250;
			 this->animation = anim;
			 if(animation == AnimationType::LEFTTORIGHT)
			 {
				 basePosition = XMFLOAT2(this->rect->x - offset, this->rect->y);
			 }
			 else
			 {
				 RECT rect1;
				 GetWindowRect(GetActiveWindow(), &rect1);
				 basePosition = XMFLOAT2(this->rect->x + offset + rect1.left + rect1.right, this->rect->y);
			 }
			 Origin = XMFLOAT2();		
		 }

		 void Update (LPPOINT point, float dt)
		 {			
			Animate();
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

		 void Render()
		 {
			 auto sp = this->fontRenderer->GetSpriteFont();
			 sp->DrawString(fontRenderer->GetSpriteBatch(), str, XMLoadFloat2(&XMFLOAT2(basePosition.x, basePosition.y)), XMLoadFloat4(&this->color), rotation, XMLoadFloat2(&XMFLOAT2(0, 0)), XMLoadFloat2(&scale), spriteFX, depth);
		 }

		 inline bool Clicked()	const
		 {
			 return clicked;
		 }

		 inline bool IsMouseHovering() const
		 {
			 return hover;
		 }

		 inline void SetColor(XMFLOAT4 Color)
		 {
			 this->color = Color;
		 }

		 inline void SetRotation(float rot)
		 {
			 this->rotation = rot;
		 }

		 inline void SetSpriteEffects(SpriteEffects fx)
		 {
			 this->spriteFX = fx;
		 }

		 inline void SetScale(XMFLOAT2 scale)
		 {
			 this->scale = scale;
		 }

private :
	XMFLOAT2 scale;
	FontRenderer* fontRenderer;
	XMFLOAT4 color;
	float rotation;
	DirectX::SpriteEffects spriteFX;
	float depth;
	bool clicked;
	bool hover;
	XMFLOAT2 basePosition;
	XMFLOAT2 Origin;
	float offset;
	AnimationType animation;

	inline void Animate()
	{
		if((int)basePosition.x != (int)rect->x)
		{
			if(animation == AnimationType::LEFTTORIGHT)
				basePosition.x++;
			else
				basePosition.x--;
		}
	}
};
