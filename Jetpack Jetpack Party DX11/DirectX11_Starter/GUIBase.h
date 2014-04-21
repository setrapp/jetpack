#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Common.h"
#include "Rect.h"
#include "FontRenderer.h"
#include <functional>
#include "Menu.h"
#include <utility>

//Alignments are unstable and not been tested yet. Just FYI. 
//Use L and T for basic settings. That is tested.
enum HAlignment
{
	//Left
	L,
	//Right
	R,	
};

enum VAlignment
{
	//Top
	T,
	//Bottom
	B
};

class GUIBase
{
public : Rect* rect;
		 wchar_t* str;
		 ~GUIBase()
		 {

		 }

		 //You can have the width declared as 0. It will be taken care of considering the size of string.
		 //0 Height will make the rect only as long as the font size

		 GUIBase(Rect* rect, wchar_t* str, HAlignment hallign, VAlignment valign, FontRenderer* renderer, FXMVECTOR color = Colors::White, float rotation = 0, float depth = 1, SpriteEffects spriteEffects = DirectX::SpriteEffects::SpriteEffects_None)
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
			 contains = false;
			 this->halign = halign;
			 this->valign = valign;
			 Origin = XMFLOAT2();			 
			 Resize();
		 }

		 void Resize()
		 {
			 if(halign == HAlignment::L)
			 { Origin.x = 0; return ;}
			 
			 if(valign == VAlignment::T)
				{ Origin.y = 0; return ;}

			 
			 RECT rect;
			 GetWindowRect(GetActiveWindow(), &rect);

			 if(halign == HAlignment::R)
				 Origin.x = rect.right;


			 if(valign == VAlignment::B)
				 Origin.y = rect.bottom;
		 }


		 void Update (LPPOINT point, float dt)
		 {			
			 RECT rect1;
			GetWindowRect(GetActiveWindow(), &rect1);
			 //Inside the rect or not ?					MOUSE LEFT IS PRESSED
			 
			 if(((GetKeyState(VK_LBUTTON) & 0x80) != 0))
			 {
				 int x, y;
				 x = point->x - rect1.left;
				 y = point->y - rect1.top - 21;
				 if(rect->Contains(x, y))
					 contains = true;
			 }
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
	XMFLOAT2 Origin;
	HAlignment halign;
	VAlignment valign;
};
