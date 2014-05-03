#ifndef _MENU_H
#define _MENU_H
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
class Menu
{
public:
	typedef enum MENU_OPTIONS
	{
		NEWGAME = 0,
		SETTINGS = 1,
		EXIT= 2
	};

	Menu(FontRenderer* renderer, SpriteRenderer* spRenderer, const short ScreenWidth, const short ScreenHeight);
	~Menu(void);
	GameState Update(float dt);
	void Render();
	void OnClickNewGame();
	void WindowResize();
	GameState KeyboardInputProcess();
	MENU_OPTIONS changeOptions(MENU_OPTIONS currentOption, int changeBy);
	RECT* GetRECTFromRect(Rect* rect);
private:
	FontRenderer* fontRenderer;
	GUIManager* guiMan;
	GameState currstate;
	GUITextBox* b ;	
	RECT* r;	
	

	MENU_OPTIONS menuOptions;
	
};
#endif