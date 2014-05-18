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


LobbyScreen::LobbyScreen(FontRenderer* renderer, SpriteRenderer* spRenderer, const short ScreenWidth, const short ScreenHeight) 
{
	guiMan = new GUIManager();

	GUITextBox* rdy = new GUITextBox(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 0 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"READY", 1, AnimationType::RIGHTTOLEFT, renderer, 4000,
		L"../Assets/Textures/textboxbackground.png", spRenderer, Colors::Black);
	guiMan->Add("READY", rdy);
	GUIText* exit = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 200 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"EXIT", 5, AnimationType::RIGHTTOLEFT, renderer, 4000, Colors::Black);
	guiMan->Add("EXIT", exit);	

	if(!r)
		r = new RECT();

	r->left		= rdy->GetRect().x	-	TICKER_WIDTH;
	r->top		= rdy->GetRect().y	+	TICKER_TOP_OFFSET;
	r->right	= rdy->GetRect().x;						//End it where the text starts
	r->bottom	= rdy->GetRect().y	+	TICKER_HEIGHT; //End its y where the 

	auto p = Rect::GetRECTFromRect(new Rect(0, 0, ScreenWidth, ScreenHeight));
	guiMan->Add("Background", new GUITexture(p, L"../Assets/Textures/Menu_bg.png", spRenderer, true));
	delete p;
	
	guiMan->_guiElements["READY"]->SetDepth(0.5f);
	guiMan->_guiElements["EXIT"]->SetDepth(0.5f);

	this->fontRenderer = renderer;	

	currstate = GameState::GameLobby;
}


LobbyScreen::~LobbyScreen(void)
{
	delete b ;	
	delete r;	
	delete guiMan;
}

GameState LobbyScreen::Update(const float dt){
	guiMan->Update(dt);

	if(guiMan->_guiElements["READY"]->Clicked())
		return GameState::Playing;	

	if(guiMan->_guiElements["READY"]->IsMouseHovering() || menuOptions == MENU_OPTIONS::READY)
	{
		guiMan->_guiElements["READY"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["READY"]->SetScale(XMFLOAT2(1.105f, 1.105f));
		menuOptions = MENU_OPTIONS::READY;
		GUITexture* tex = (GUITexture*)(guiMan->_guiElements["Texture"]);
		RECT* rectBig = GetRECTFromRect(guiMan->_guiElements["READY"]->GetRect());
		tex->SetRect(rectBig);
		delete rectBig;
	}
	else
	{
		guiMan->_guiElements["READY"]->SetColor(XMFLOAT4(0, 0, 0, 1));
		guiMan->_guiElements["READY"]->SetScale(XMFLOAT2(1, 1));

	}
}

void LobbyScreen::Render()
{
	guiMan->Render();
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
