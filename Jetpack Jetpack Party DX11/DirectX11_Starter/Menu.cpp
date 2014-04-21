#include "Menu.h"
#include <DirectXMath.h>

Menu::Menu(FontRenderer* renderer) 
{
	guiMan = new GUIManager();
	/*GUIBase* title = new GUIBase(new Rect(0, 0, 0 ,0), L"JETPACK JETPACK PARTY", renderer);
	guiMan->Add("title", title);*/
	
	GUIBase* nG = new GUIBase(new Rect(0, 0, 0 ,0), L"NEW GAME", renderer, Colors::Black);
	guiMan->Add("NEWGAME", nG);
	GUIBase* settings = new GUIBase(new Rect(0, 100, 0 ,0), L"SETTINGS", renderer, Colors::Black);
	guiMan->Add("title", settings);
	GUIBase* exit = new GUIBase(new Rect(0, 200, 0 ,0), L"EXIT", renderer, Colors::Black);
	guiMan->Add("EXIT", exit);		
	
	this->fontRenderer = renderer;
	currstate = GameState::Started;
}

void Menu::OnClickNewGame()
{
	Menu::currstate = GameState::Playing;
}

GameState Menu::Update(float dt)
{	
	guiMan->Update(dt);
	if(guiMan->_guiElements["NEWGAME"]->Contains())
		return GameState::Playing;	
	else
		return GameState::Started;

	if(guiMan->_guiElements["EXIT"]->Contains())
		exit(1);
}

void Menu::Render()
{
	guiMan->Render();
}

Menu::~Menu(void)
{
	delete fontRenderer;
	delete guiMan;
}
