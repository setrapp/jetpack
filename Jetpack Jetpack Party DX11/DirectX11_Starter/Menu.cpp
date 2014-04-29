#include "Menu.h"
#include <DirectXMath.h>
#include "GUIText.h"
#include "GUITextBox.h"


Menu::Menu(FontRenderer* renderer) 
{
	guiMan = new GUIManager();	
	GUIText* nG = new GUIText(new Rect(0, 0, 0 ,0), L"NEW GAME", 1, AnimationType::TOPTOBOTTOM, renderer, 1000, Colors::Black);
	guiMan->Add("NEWGAME", nG);
	GUIText* settings = new GUIText(new Rect(0, 100, 0 ,0), L"SETTINGS", 3, AnimationType::LEFTTORIGHT, renderer, 1000, Colors::Black);
	guiMan->Add("SETTINGS", settings);
	GUIText* exit = new GUIText(new Rect(0, 200, 0 ,0), L"EXIT", 5, AnimationType::BOTTOMTOTOP, renderer, 1000, Colors::Black);
	guiMan->Add("EXIT", exit);

	
	this->fontRenderer = renderer;
	
GUITextBox* b = new GUITextBox(new Rect(0, 200, 0 ,0), L"EXIT", 5, AnimationType::BOTTOMTOTOP, this->fontRenderer, 1000, Colors::Black);
	currstate = GameState::Started;
}

void Menu::OnClickNewGame()
{
	Menu::currstate = GameState::Playing;
}

GameState Menu::Update(const float dt)
{	
	guiMan->Update(dt);
	if(guiMan->_guiElements["NEWGAME"]->Clicked())
		return GameState::Playing;	

	if(guiMan->_guiElements["EXIT"]->Clicked())
		PostQuitMessage(0);

	if(guiMan->_guiElements["NEWGAME"]->IsMouseHovering())
	{
		guiMan->_guiElements["NEWGAME"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["NEWGAME"]->SetScale(XMFLOAT2(1.105f, 1.105f));
	}
	else
	{
		guiMan->_guiElements["NEWGAME"]->SetColor(XMFLOAT4(0, 0, 0, 1));
		guiMan->_guiElements["NEWGAME"]->SetScale(XMFLOAT2(1, 1));
	}

	if(guiMan->_guiElements["EXIT"]->IsMouseHovering())
	{
		guiMan->_guiElements["EXIT"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["EXIT"]->SetScale(XMFLOAT2(1.105f, 1.105f));
	}
	else
	{
		guiMan->_guiElements["EXIT"]->SetColor(XMFLOAT4(0, 0, 0, 1));
		guiMan->_guiElements["EXIT"]->SetScale(XMFLOAT2(1, 1));
	}

	if(guiMan->_guiElements["SETTINGS"]->IsMouseHovering())
	{
		guiMan->_guiElements["SETTINGS"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["SETTINGS"]->SetScale(XMFLOAT2(1.105f, 1.105f)); 
	}
	else
	{
		guiMan->_guiElements["SETTINGS"]->SetColor(XMFLOAT4(0, 0, 0, 1));
		guiMan->_guiElements["SETTINGS"]->SetScale(XMFLOAT2(1, 1));
	}
	return GameState::Started;
}

void Menu::Render()
{
	guiMan->Render();
}

Menu::~Menu(void)
{
	delete guiMan;
}


void Menu::WindowResize()
{
}
