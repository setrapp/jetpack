#ifndef _MENU_H
#define _MENU_H
#include<d3d11.h>
#include<DirectXMath.h>
#include"Entity.h"
#include"GameState.h"
#include "FontRenderer.h"
#include "Rect.h"
#include "Common.h"

#pragma once
class Menu
{
public:
	Menu(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Menu(void);
	void setRenderers(FontRenderer* fontRenderer);
	GameState Update(float dt);
	void Render(ID3D11DeviceContext* deviceContext);
	void ProcessMouseInput(WPARAM btnState, int x, int y);

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	Rect* newGameArea;
	Rect* exitGameArea;
	FontRenderer* fontRenderer;
	DirectX::SpriteBatch* spriteBatch;
	bool newGameFlag;
};
#endif