#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Common.h"
#include "Rect.h"
#include "FontRenderer.h"
#include <functional>
#include "Menu.h"
#include <utility>
#include "AnimationBase.h"

using namespace std;
using namespace DirectX;

class GUIBase 
{
public :
	virtual ~GUIBase() { };

protected :
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
	float animationSpeed;
	Rect* rect;

public :
		virtual const bool IsMouseHovering() const = 0;
		virtual void Update (LPPOINT point, float dt);
		virtual const inline bool Clicked() = 0;
		virtual void Render() const = 0;
		virtual inline void SetColor(const XMFLOAT4 Color) = 0;
		virtual inline void SetRotation(float rot) = 0;
		virtual inline void SetSpriteEffects(const SpriteEffects fx) = 0;
		virtual inline void SetScale(XMFLOAT2 scale) = 0;
};
