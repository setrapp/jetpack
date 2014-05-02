#include "Menu.h"
#include <DirectXMath.h>
#include "GUIText.h"
#include "GUITextBox.h"
#include "GUITextures.h"
#include <d3d11.h>

#define TICKER_WIDTH 80
#define TICKER_TOP 25
#define TICKER_HEIGHT 80


Menu::Menu(FontRenderer* renderer, SpriteRenderer* spRenderer) 
{
	guiMan = new GUIManager();	
	GUIText* nG = new GUIText(new Rect(80, 0, 0 ,0), L"NEW GAME", 1, AnimationType::TOPTOBOTTOM, renderer, 1000, Colors::Black);
	guiMan->Add("NEWGAME", nG);
	GUIText* settings = new GUIText(new Rect(80, 100, 0 ,0), L"SETTINGS", 3, AnimationType::LEFTTORIGHT, renderer, 1000, Colors::Black);
	guiMan->Add("SETTINGS", settings);
	GUIText* exit = new GUIText(new Rect(80, 200, 0 ,0), L"EXIT", 5, AnimationType::BOTTOMTOTOP, renderer, 1000, Colors::Black);
	guiMan->Add("EXIT", exit);

	r.left		= nG->GetRect().x	-	TICKER_WIDTH;
	r.top		= nG->GetRect().y	+	TICKER_TOP;
	r.right		= nG->GetRect().x;						//End it where the text starts
	r.bottom	= nG->GetRect().y	+	TICKER_HEIGHT; //End its y where the 
	auto p = Rect::ConvertToRECT(&nG->GetRect());
	guiMan->Add("Texture", new GUITexture(&r, L"../Assets/jetpackimage.png", spRenderer));
	
	this->fontRenderer = renderer;	
	GUITextBox* b = new GUITextBox(new Rect(0, 500, 0 ,0), L"EXIT", 5, AnimationType::BOTTOMTOTOP, this->fontRenderer, 1000, Colors::Black);
	currstate = GameState::Started;
	menuOptions = MENU_OPTIONS::NEWGAME;
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

	if(guiMan->_guiElements["NEWGAME"]->IsMouseHovering() || menuOptions == MENU_OPTIONS::NEWGAME)
	{
		guiMan->_guiElements["NEWGAME"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["NEWGAME"]->SetScale(XMFLOAT2(1.105f, 1.105f));
	}
	else
	{
		guiMan->_guiElements["NEWGAME"]->SetColor(XMFLOAT4(0, 0, 0, 1));
		guiMan->_guiElements["NEWGAME"]->SetScale(XMFLOAT2(1, 1));
	}

	if(guiMan->_guiElements["EXIT"]->IsMouseHovering()|| menuOptions == MENU_OPTIONS::EXIT)
	{
		guiMan->_guiElements["EXIT"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["EXIT"]->SetScale(XMFLOAT2(1.105f, 1.105f));
	}
	else
	{
		guiMan->_guiElements["EXIT"]->SetColor(XMFLOAT4(0, 0, 0, 1));
		guiMan->_guiElements["EXIT"]->SetScale(XMFLOAT2(1, 1));
	}

	if(guiMan->_guiElements["SETTINGS"]->IsMouseHovering()|| menuOptions == MENU_OPTIONS::SETTINGS)
	{
		guiMan->_guiElements["SETTINGS"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["SETTINGS"]->SetScale(XMFLOAT2(1.105f, 1.105f)); 
	}
	else
	{
		guiMan->_guiElements["SETTINGS"]->SetColor(XMFLOAT4(0, 0, 0, 1));
		guiMan->_guiElements["SETTINGS"]->SetScale(XMFLOAT2(1, 1));
	}

	currstate = KeyboardInputProcess();
	return currstate;
}

GameState Menu::KeyboardInputProcess()
{
	if(IPMan::GetIPMan()->GetKeyDown(KeyType::BACKWARD))
	{
		//Because the height of the fonts is 100
		r.top += 100;
		r.bottom += 100;
		if(r.top > 200 + TICKER_TOP)
			{
				r.top = TICKER_TOP;
				r.bottom = guiMan->_guiElements["NEWGAME"]->GetRect()->y + TICKER_HEIGHT; 
			}
		auto tex = static_cast<GUITexture*>(guiMan->_guiElements["Texture"]);
		tex->SetRect(r);	
		menuOptions = changeOptions(menuOptions, 1);
	}
	else 
		if(IPMan::GetIPMan()->GetKeyDown(KeyType::FORWARD))
	{
		//Because the height of the fonts is 100
		r.top -= 100;
		r.bottom -= 100;
		if(r.top <= guiMan->_guiElements["NEWGAME"]->GetRect()->x-TICKER_WIDTH)
			{
				r.top = guiMan->_guiElements["NEWGAME"]->GetRect()->x-TICKER_WIDTH;
				r.bottom = guiMan->_guiElements["NEWGAME"]->GetRect()->y	+TICKER_HEIGHT;
			}
		auto tex = static_cast<GUITexture*>(guiMan->_guiElements["Texture"]);
		tex->SetRect(r);	
		menuOptions = changeOptions(menuOptions, -1);
	}

	
		if(IPMan::GetIPMan()->GetKey(VK_RETURN))
			{
				switch(menuOptions)
				{
				case MENU_OPTIONS::NEWGAME: return GameState::Playing;
					break;
				case MENU_OPTIONS::SETTINGS: return GameState::Started;
					break;
				case MENU_OPTIONS::EXIT: 
					exit(1);
					return GameState::Lost;
					break;
				default: return GameState::Started;
					break;
				}
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

Menu::MENU_OPTIONS Menu::changeOptions(MENU_OPTIONS currentOption, int changeBy)
{
	if(currentOption + changeBy < 0)
		return MENU_OPTIONS::EXIT;
	return (MENU_OPTIONS)((currentOption + changeBy) % 3);
}
