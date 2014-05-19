#include "LobbyScreen.h"
#include <DirectXMath.h>
#include "GUIText.h"
#include "GUITextBox.h"
#include "GUITextures.h"
#include <d3d11.h>
#include "DXGame.h"

#define TICKER_WIDTH			50
#define TICKER_TOP_OFFSET		0
#define TICKER_HEIGHT			80 - TICKER_TOP_OFFSET

#define MENUBUTTONS_TOP_OFFSET  screenHeight / 2	+ 200
#define MENUBUTTONS_LEFT_OFFSET screenWidth / 2		- 300


#include <DirectXMath.h>
#include "GUIText.h"
#include "GUITextBox.h"
#include "GUITextures.h"
#include <d3d11.h>
#include "DXGame.h"

#define TICKER_WIDTH			50
#define TICKER_TOP_OFFSET		0
#define TICKER_HEIGHT			80 - TICKER_TOP_OFFSET

#define MENUBUTTONS_TOP_OFFSET  screenHeight / 2	+ 200
#define MENUBUTTONS_LEFT_OFFSET screenWidth / 2		- 300

LobbyScreen::LobbyScreen(FontRenderer* renderer, SpriteRenderer* spRenderer, const short ScreenWidth, const short ScreenHeight) 
{
	additionCount=0;
	multiplayerClicked=false;
	guiMan = new GUIManager();	

	GUIText* sp = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, -100 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"SINGLEPLAYER", 1, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::White);
	guiMan->Add("SINGLEPLAYER", sp);
	GUIText* mp = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 0 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"MULTIPLAYER", 3, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::White);
	guiMan->Add("MULTIPLAYER", mp);
	GUIText* settings = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 100 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"SETTINGS", 5, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::White);
	guiMan->Add("SETTINGS", settings);
	GUIText* exit = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 200 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"EXIT", 7, AnimationType::BOTTOMTOTOP, renderer, 4000, Colors::White);
	guiMan->Add("EXIT", exit);	

	if(!r)
		r = new RECT();

	r->left		= sp->GetRect().x	-	TICKER_WIDTH;
	r->top		= sp->GetRect().y	+	TICKER_TOP_OFFSET;
	r->right	= sp->GetRect().x;						//End it where the text starts
	r->bottom	= sp->GetRect().y	+	TICKER_HEIGHT; //End its y where the 


	guiMan->_guiElements["SINGLEPLAYER"]->SetDepth(0.5f);
	guiMan->_guiElements["MULTIPLAYER"]->SetDepth(0.5f);
	guiMan->_guiElements["EXIT"]->SetDepth(0.5f);

	this->fontRenderer = renderer;	
	
	currstate = GameState::GameLobby;
	menuOptions = MENU_OPTIONS::SINGLEPLAYER;
}

void LobbyScreen::OnClickNewGame()
{
	LobbyScreen::currstate = GameState::Playing;
}

GameState LobbyScreen::Update(const float dt)
{	
	guiMan->Update(dt);
	if(guiMan->_guiElements["SINGLEPLAYER"]->Clicked())
		return GameState::Playing;	

	if(guiMan->_guiElements["MULTIPLAYER"]->Clicked()){
		if(!multiplayerClicked){
			additionCount++;
			addedLongNameVector.push_back(L"Player ");
			addedLongNameVector.at(addedLongNameVector.size()-1)+= to_wstring(additionCount);
			addedNameVector.push_back("Player ");
			addedNameVector.at(addedNameVector.size()-1)+= to_string(additionCount);
			guiMan->Add((char*)addedNameVector.at(addedNameVector.size()-1).c_str(), new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, (-100 + (-100* additionCount)) + MENUBUTTONS_TOP_OFFSET, 0 ,0),(wchar_t*)addedLongNameVector.at(addedLongNameVector.size()-1).c_str(), 1, AnimationType::BOTTOMTOTOP, this->fontRenderer, 4000, Colors::White));
			guiMan->_guiElements[(char*)addedNameVector.at(addedNameVector.size()-1).c_str()]->SetDepth(0.5f);
			multiplayerClicked=true;
		}
	}
	else{
		multiplayerClicked=false;
	}

	if(guiMan->_guiElements["EXIT"]->Clicked())
		PostQuitMessage(0);

	/*if(guiMan->_guiElements["SINGLEPLAYER"]->IsMouseHovering() || menuOptions == MENU_OPTIONS::SINGLEPLAYER)
	{
		guiMan->_guiElements["SINGLEPLAYER"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["SINGLEPLAYER"]->SetScale(XMFLOAT2(1.105f, 1.105f));
		menuOptions = MENU_OPTIONS::SINGLEPLAYER;
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		RECT* rectBig = GetRECTFromRect(guiMan->_guiElements["SINGLEPLAYER"]->GetRect());
		tex->SetRect(rectBig);
		delete rectBig;
	}
	else
	{
		guiMan->_guiElements["SINGLEPLAYER"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["SINGLEPLAYER"]->SetScale(XMFLOAT2(1, 1));

	}
	*/
	currstate = KeyboardInputProcess();
	return currstate;
}

GameState LobbyScreen::KeyboardInputProcess()
{
	if(IPMan::GetIPMan()->GetKeyDown(KeyType::BACKWARD))
	{
		//Because the height of the fonts is 100
		r->top += 100;
		r->bottom += 100;
		if(r->top > 200 + TICKER_TOP_OFFSET)
			{
				r->top = TICKER_TOP_OFFSET;
				r->bottom = guiMan->_guiElements["SINGLEPLAYER"]->GetRect()->y + TICKER_HEIGHT; 
			}
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		tex->SetRect(r);	
	}
	else 
		if(IPMan::GetIPMan()->GetKeyDown(KeyType::FORWARD))
	{
		//Because the height of the fonts is 100
		r->top -= 100;
		r->bottom -= 100;
		float x = guiMan->_guiElements["SINGLEPLAYER"]->GetRect()->y	+ TICKER_TOP_OFFSET;
		if(r->top < x)
			{
				r->top = guiMan->_guiElements["EXIT"]->GetRect()->y+ TICKER_TOP_OFFSET;
				r->bottom = guiMan->_guiElements["EXIT"]->GetRect()->y+TICKER_HEIGHT;
			}
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		tex->SetRect(r);	
	}

	
		if(IPMan::GetIPMan()->GetKey(VK_RETURN))
			{
				switch(menuOptions)
				{
				case MENU_OPTIONS::SINGLEPLAYER: return GameState::Playing;
					break;
				case MENU_OPTIONS::MULTIPLAYER: return GameState::GameLobby;
					break;
				case MENU_OPTIONS::SETTINGS: return GameState::Settings;
					break;
				case MENU_OPTIONS::EXIT: 
					exit(1);
					return GameState::Lost;
					break;
				default: return GameState::GameLobby;
					break;
				}
			}
		return GameState::GameLobby;
}

void LobbyScreen::Render()
{
	guiMan->Render();
}

LobbyScreen::~LobbyScreen(void)
{
	delete b ;	
	delete r;	
	delete guiMan;
}


void LobbyScreen::WindowResize()
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



RECT* LobbyScreen::GetRECTFromRect(Rect* rect)
{
	RECT* r = new RECT();
	r->left		= rect->x	-	TICKER_WIDTH;
	r->top		= rect->y	+	TICKER_TOP_OFFSET;
	r->right	= rect->x;						//End it where the text starts
	r->bottom	= rect->y	+	TICKER_HEIGHT; //End its y where the 
	return r;
}
