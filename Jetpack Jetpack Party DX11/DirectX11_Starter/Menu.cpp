#include "Menu.h"
#include <DirectXMath.h>
#include "GUIText.h"
#include "GUITextBox.h"
#include "GUITextures.h"
#include <d3d11.h>
#include "DXGame.h"

#define TICKER_WIDTH			50
#define TICKER_TOP_OFFSET		40
#define TICKER_HEIGHT			145 - TICKER_TOP_OFFSET

#define MENUBUTTONS_TOP_OFFSET  screenHeight / 2	+ 200
#define MENUBUTTONS_LEFT_OFFSET screenWidth / 2		- 300

Menu::Menu(FontRenderer* renderer, SpriteRenderer* spRenderer, const short ScreenWidth, const short ScreenHeight) 
{
	guiMan = new GUIManager();	

	auto p = Rect::GetRECTFromRect(new Rect(0, 0, ScreenWidth, ScreenHeight));
	guiMan->Add("Background", new GUITexture(p, L"../Assets/Menu_bg.png", spRenderer, true));
	delete p;

	GUIText* nG = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 0 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"NEW GAME", 1, AnimationType::RIGHTTOLEFT, renderer, 4000, Colors::Black);
	guiMan->Add("NEWGAME", nG);
	GUIText* settings = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 100 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"SETTINGS", 3, AnimationType::RIGHTTOLEFT, renderer, 4000, Colors::Black);
	guiMan->Add("SETTINGS", settings);
	GUIText* exit = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 200 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"EXIT", 5, AnimationType::RIGHTTOLEFT, renderer, 4000, Colors::Black);
	guiMan->Add("EXIT", exit);

	if(!r)
		r = new RECT();

	r->left		= nG->GetRect().x	-	TICKER_WIDTH;
	r->top		= nG->GetRect().y	+	TICKER_TOP_OFFSET;
	r->right	= nG->GetRect().x;						//End it where the text starts
	r->bottom	= nG->GetRect().y	+	TICKER_HEIGHT; //End its y where the 

	guiMan->Add("Texture", new GUITexture(r, L"../Assets/jetpackimage.png", spRenderer));	

	this->fontRenderer = renderer;	
	
	currstate = GameState::MenuState;
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
		menuOptions = MENU_OPTIONS::NEWGAME;
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		RECT* rectBig = GetRECTFromRect(guiMan->_guiElements["NEWGAME"]->GetRect());
		tex->SetRect(rectBig);
		delete rectBig;
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
		menuOptions = MENU_OPTIONS::EXIT;
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);		
		RECT* rectBig = GetRECTFromRect(guiMan->_guiElements["EXIT"]->GetRect());
		tex->SetRect(rectBig);
		delete rectBig;
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
		menuOptions = MENU_OPTIONS::SETTINGS;		
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		RECT* rectBig = GetRECTFromRect(guiMan->_guiElements["SETTINGS"]->GetRect());
		tex->SetRect(rectBig);
		delete rectBig;
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
		r->top += 100;
		r->bottom += 100;
		if(r->top > 200 + TICKER_TOP_OFFSET)
			{
				r->top = TICKER_TOP_OFFSET;
				r->bottom = guiMan->_guiElements["NEWGAME"]->GetRect()->y + TICKER_HEIGHT; 
			}
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		tex->SetRect(r);	
		menuOptions = changeOptions(menuOptions, 1);
	}
	else 
		if(IPMan::GetIPMan()->GetKeyDown(KeyType::FORWARD))
	{
		//Because the height of the fonts is 100
		r->top -= 100;
		r->bottom -= 100;
		float x = guiMan->_guiElements["NEWGAME"]->GetRect()->y	+ TICKER_TOP_OFFSET;
		if(r->top < x)
			{
				r->top = guiMan->_guiElements["EXIT"]->GetRect()->y+ TICKER_TOP_OFFSET;
				r->bottom = guiMan->_guiElements["EXIT"]->GetRect()->y+TICKER_HEIGHT;
			}
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		tex->SetRect(r);	
		menuOptions = changeOptions(menuOptions, -1);
	}

	
		if(IPMan::GetIPMan()->GetKey(VK_RETURN))
			{
				switch(menuOptions)
				{
				case MENU_OPTIONS::NEWGAME: return GameState::Playing;
					break;
				case MENU_OPTIONS::SETTINGS: return GameState::MenuState;
					break;
				case MENU_OPTIONS::EXIT: 
					exit(1);
					return GameState::Lost;
					break;
				default: return GameState::MenuState;
					break;
				}
			}
		return GameState::MenuState;
}

void Menu::Render()
{
	guiMan->Render();
}

Menu::~Menu(void)
{
	delete b ;	
	delete r;	
	delete guiMan;
}


void Menu::WindowResize()
{
	RECT rect;
	GetClientRect(GetActiveWindow(), &rect);
	if(guiMan)
	{
		auto p = static_cast<GUITexture*>(guiMan->_guiElements["Background"]);
		if(p)
		{
			RECT* rectBig = Rect::GetRECTFromRect(new Rect(0, 0, rect.left + rect.right, rect.top + rect.bottom));
			p->SetRect(rectBig);
			delete rectBig;
		}
	}
}

Menu::MENU_OPTIONS Menu::changeOptions(MENU_OPTIONS currentOption, int changeBy)
{
	if(currentOption + changeBy < 0)
		return MENU_OPTIONS::EXIT;
	return (MENU_OPTIONS)((currentOption + changeBy) % 3);
}



RECT* Menu::GetRECTFromRect(Rect* rect)
{
	RECT* r = new RECT();
	r->left		= rect->x	-	TICKER_WIDTH;
	r->top		= rect->y	+	TICKER_TOP_OFFSET;
	r->right	= rect->x;						//End it where the text starts
	r->bottom	= rect->y	+	TICKER_HEIGHT; //End its y where the 
	return r;
}
