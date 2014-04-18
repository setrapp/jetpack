#include "Menu.h"
#include <DirectXMath.h>

Menu::Menu(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
	newGameArea = new Rect();
	newGameArea->x = 0;
	newGameArea->y = 0;
	newGameFlag = false;
}

void Menu::ProcessMouseInput(WPARAM btnState, int x, int y)
{	
	if(newGameArea->Contains(x, y))
	{
		newGameFlag = true;
	}
}

void Menu::setRenderers(FontRenderer* f)
{
	this->fontRenderer = f;
	auto p = fontRenderer->GetSpriteFont()->MeasureString(L"NEW GAME");
	newGameArea->width = p.m128_f32[0];
	newGameArea->height = p.m128_f32[1];
}

GameState Menu::Update(float dt)
{
	GameState currstate;
	if(newGameFlag)
	{
		currstate = GameState::Playing;
	}
	else
	{
		currstate = GameState::Started;
	}
	return currstate;	
}

void Menu::Render(ID3D11DeviceContext* deviceContext)
{
	fontRenderer->GetSpriteFont()->DrawString(fontRenderer->GetSpriteBatch(), L"NEW GAME", XMFLOAT2(newGameArea->x, newGameArea->y), Colors::Green, 0, XMFLOAT2(0,0), 1, SpriteEffects_None, 0);	
	fontRenderer->GetSpriteFont()->DrawString(fontRenderer->GetSpriteBatch(), L"Exit", XMFLOAT2(0, 100), Colors::Red, 0, XMFLOAT2(0,0), 1, DirectX::SpriteEffects_None,0 );
}

Menu::~Menu(void)
{
}
