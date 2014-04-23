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
#include <vector>

using namespace std;
using namespace DirectX;

#pragma once
class Menu
{
public:
	//You can pass multiple FontRenderers to it
	Menu(int no, FontRenderer* renderer, ...);
	~Menu(void);
	GameState Update(float dt);
	void Render();
	void OnClickNewGame();
	void WindowResize();

private:
	vector<FontRenderer*> fontRenderer;
	GUIManager* guiMan;
	GameState currstate;
	
};
#endif