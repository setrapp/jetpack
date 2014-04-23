#include "Menu.h"
#include <DirectXMath.h>

Menu::Menu(int noOfRenderers, FontRenderer* renderer, ...) 
{
	va_list ap;
	va_start(ap, noOfRenderers);
	for(int i = 0; i < noOfRenderers; i++)
	{
		auto p = va_arg(ap, FontRenderer*);
		this->fontRenderer.push_back(p);
	}
	va_end(ap);
	guiMan = new GUIManager();	
	GUIBase* nG = new GUIBase(new Rect(0, 0, 0 ,0), L"NEW GAME", AnimationType::TOPTOBOTTOM, fontRenderer.at(0), 1000, 1, Colors::Black);
	guiMan->Add("NEWGAME", nG);
	GUIBase* settings = new GUIBase(new Rect(0, 100, 0 ,0), L"SETTINGS", AnimationType::LEFTTORIGHT, fontRenderer.at(0), 1000, 3, Colors::Black);
	guiMan->Add("SETTINGS", settings);
	GUIBase* exit = new GUIBase(new Rect(0, 200, 0 ,0), L"EXIT", AnimationType::BOTTOMTOTOP, fontRenderer.at(0), 1000, 5, Colors::Black);
	guiMan->Add("EXIT", exit);			
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
		exit(1);

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
