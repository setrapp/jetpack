#ifndef _LOBBYSCREEN_H
#define _LOBBYSCREEN_H
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
#include <queue>


using namespace std;
using namespace DirectX;

#pragma once
class LobbyScreen
{
public:
	typedef enum MENU_OPTIONS
	{
		SINGLEPLAYER = 0,
		MULTIPLAYER = 1,
		SETTINGS = 2,
		EXIT= 3,
		READY = 4
	};

	LobbyScreen(FontRenderer* renderer, SpriteRenderer* spRenderer, const short ScreenWidth, const short ScreenHeight);;
	~LobbyScreen(void);
	GameState Update(float dt);
	void Render();
	void AddPlayer(string player, bool isUs);
	void OnClickNewGame();
	void WindowResize();
	GameState KeyboardInputProcess();
	queue<string> playerAdditionQueue;
	vector<string*> nameHoldingVector;
	RECT* GetRECTFromRect(Rect* rect);
	string playerNames[4];
	wstring longPlayerNames[4];
	int additionCount;
private:
	FontRenderer* fontRenderer;
	GUIManager* guiMan;
	GameState currstate;
	GUITextBox* b ;	
	RECT* r;	
	bool multiplayerClicked;
	MENU_OPTIONS menuOptions;
	vector<string> addedNameVector;
	vector<wstring> addedLongNameVector;
};

#endif
