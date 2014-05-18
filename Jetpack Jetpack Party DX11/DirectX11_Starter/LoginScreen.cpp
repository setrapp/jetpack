#include "LoginScreen.h"
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

LoginScreen::LoginScreen(FontRenderer* renderer, SpriteRenderer* spRenderer, const short ScreenWidth, const short ScreenHeight) 
{
	guiMan = new GUIManager();	

	GUITextBox* mp = new GUITextBox(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 0 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"MULTIPLAYER", 1, AnimationType::RIGHTTOLEFT, renderer, 4000,
		L"../Assets/Textures/textboxbackground.png", spRenderer, Colors::Black);
	guiMan->Add("MULTIPLAYER", mp);
	GUIText* exit = new GUIText(&Rect(80 + MENUBUTTONS_LEFT_OFFSET, 200 + MENUBUTTONS_TOP_OFFSET, 0 ,0), L"EXIT", 5, AnimationType::RIGHTTOLEFT, renderer, 4000, Colors::Black);
	guiMan->Add("EXIT", exit);	

	if(!r)
		r = new RECT();

	r->left		= mp->GetRect().x	-	TICKER_WIDTH;
	r->top		= mp->GetRect().y	+	TICKER_TOP_OFFSET;
	r->right	= mp->GetRect().x;						//End it where the text starts
	r->bottom	= mp->GetRect().y	+	TICKER_HEIGHT; //End its y where the 


	auto p = Rect::GetRECTFromRect(new Rect(0, 0, ScreenWidth, ScreenHeight));
	guiMan->Add("Background", new GUITexture(p, L"../Assets/Textures/Menu_bg.png", spRenderer, true));
	delete p;

	guiMan->_guiElements["MULTIPLAYER"]->SetDepth(0.5f);
	guiMan->_guiElements["EXIT"]->SetDepth(0.5f);
	guiMan->_guiElements["Background"]->SetDepth(1);

	this->fontRenderer = renderer;	
	
	currstate = GameState::Login;
}


GameState LoginScreen::Update(const float dt)
{	
	guiMan->Update(dt);

	if(guiMan->_guiElements["MULTIPLAYER"]->Clicked())
		return GameState::GameLobby;	

	if(guiMan->_guiElements["EXIT"]->Clicked())
		PostQuitMessage(0);

	if(guiMan->_guiElements["MULTIPLAYER"]->IsMouseHovering())
	{
		guiMan->_guiElements["MULTIPLAYER"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["MULTIPLAYER"]->SetScale(XMFLOAT2(1, 1));
	}
	else
	{
		guiMan->_guiElements["MULTIPLAYER"]->SetColor(XMFLOAT4(1, 1, 1, 1));
		guiMan->_guiElements["MULTIPLAYER"]->SetScale(XMFLOAT2(1, 1));

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
	currstate = KeyboardInputProcess();
	return currstate;
}

void LoginScreen::WindowResize()
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


GameState LoginScreen::KeyboardInputProcess()
{
	if(IPMan::GetIPMan()->GetKeyDown(KeyType::BACKWARD))
	{
		/*//Because the height of the fonts is 100
		r->top += 100;
		r->bottom += 100;
		if(r->top > 200 + TICKER_TOP_OFFSET)
			{
				r->top = TICKER_TOP_OFFSET;
				r->bottom = guiMan->_guiElements["SINGLEPLAYER"]->GetRect()->y + TICKER_HEIGHT; 
			}*/
	}
	else 
		if(IPMan::GetIPMan()->GetKeyDown(KeyType::FORWARD))
	{
		/*//Because the height of the fonts is 100
		r->top -= 100;
		r->bottom -= 100;
		float x = guiMan->_guiElements["SINGLEPLAYER"]->GetRect()->y	+ TICKER_TOP_OFFSET;
		if(r->top < x)
			{
				r->top = guiMan->_guiElements["EXIT"]->GetRect()->y+ TICKER_TOP_OFFSET;
				r->bottom = guiMan->_guiElements["EXIT"]->GetRect()->y+TICKER_HEIGHT;
			}*/
	}
		return GameState::Login;
}

void LoginScreen::Render()
{
	guiMan->Render();
}

LoginScreen::~LoginScreen(void)
{
}
