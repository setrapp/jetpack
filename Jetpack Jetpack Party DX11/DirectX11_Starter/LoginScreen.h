#ifndef _LOGINSCREEN_H
#define _LOGINSCREEN_H
#include<d3d11.h>
#include<DirectXMath.h>
#include"Entity.h"
#include"GameState.h"
#include "FontRenderer.h"
#include "Rect.h"
#include "Common.h"
#include "GUIBase.h"
#include "GUIManager.h"
#include "GUITextBox.h"
#include "SpriteRenderer.h"


using namespace std;
using namespace DirectX;
#pragma once
class LoginScreen
{
public:
	LoginScreen(FontRenderer* renderer, SpriteRenderer* spRenderer, const short ScreenWidth, const short ScreenHeight);
	~LoginScreen(void);
	GameState Update(float dt);
	void Render();
	void WindowResize();
	GameState KeyboardInputProcess();

private:
	FontRenderer* fontRenderer;
	GUIManager* guiMan;
	GameState currstate;
	GUITextBox* b ;	
	RECT* r;
};
	

#endif
