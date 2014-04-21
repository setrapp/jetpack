#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Common.h"
#include "Rect.h"
#include "FontRenderer.h"
#include <functional>
#include "Menu.h"
#include <utility>


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

		 GUIBase(Rect* rect, wchar_t* str, FontRenderer* renderer, FXMVECTOR color = Colors::White, float rotation = 0, float depth = 1, SpriteEffects spriteEffects = DirectX::SpriteEffects::SpriteEffects_None)
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
			 	rect->height = sp->MeasureString(str).m128_i16[1];
			 }

			 XMStoreFloat4(&this->color, color);
			 this->rotation = rotation;
			 this->depth = depth;
			 this->spriteFX = spriteEffects;		
			 contains = false;
		 }

		 void Update (LPPOINT point, float dt)
		 {			
			 //Inside the rect or not ?					MOUSE LEFT IS PRESSED
			 if(rect->Contains(point->x, point->y) && ((GetKeyState(VK_LBUTTON) & 0x80) != 0))
				 contains = true;
		 }

		 void Render()
		 {
			 auto sp = this->fontRenderer->GetSpriteFont();
			 sp->DrawString(fontRenderer->GetSpriteBatch(), str, XMFLOAT2(rect->x, rect->y), XMLoadFloat4(&this->color), rotation, XMFLOAT2(0, 0), 1, spriteFX, depth);
		 }

		 bool Contains()
		 {
			 return contains;
		 }

private :
	FontRenderer* fontRenderer;
	XMFLOAT4 color;
	float rotation;
	DirectX::SpriteEffects spriteFX;
	float depth;
	bool contains;
};
