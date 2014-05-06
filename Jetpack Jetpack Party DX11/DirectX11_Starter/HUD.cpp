#include "HUD.h"
#include "Debug.h"

float HUD::fuel = 0;

HUD::HUD(SpriteRenderer* renderer, FontRenderer* fontRenderer)
{
	this->renderer			= renderer;
	this->fontRenderer		= fontRenderer;
	RECT tempRect;
	GetClientRect(GetActiveWindow(), &tempRect);
	screen = Rect::GetRectFromRECT(&tempRect);
	HUDBGRect				= new Rect(0, 0, 1920, 250);
	fuelRect				= new Rect(screen->width / 2 - 125, 0, 250, 100);

	HR (CreateWICTextureFromFile(DXConnection::Instance()->device, DXConnection::Instance()->deviceContext,
		L"../Assets/HUD/HUD_BG.png",
		&this->HUDResource,
		&this->HUDTexture));

	HR (CreateWICTextureFromFile(DXConnection::Instance()->device, DXConnection::Instance()->deviceContext,
	L"../Assets/HUD/fuel.png",
	&this->fuelResource,
	&this->fuelTexture));

	rank = 1;
	maxRacers = 10;
}


HUD::~HUD(void)
{
	delete HUDBGRect;
	delete fuelRect;
	delete screen;
	ReleaseMacro(HUDTexture);
	ReleaseMacro(fuelTexture);
	ReleaseMacro(HUDResource);
	ReleaseMacro(fuelResource);
}


inline void HUD::setFuel(float fuelValue)
{
	this->fuel = fuelValue;
}


inline float HUD::getFuel()
{
	return this->fuel;
}


inline void HUD::setRank(short newRank)
{
	this->rank = newRank;
}


inline short HUD::getRank()
{
	return this->rank;
}


inline void HUD::setMaxRacers(unsigned short maxRacers)
{
	this->maxRacers = maxRacers;
}

void HUD::Render()
{
	renderer->Begin(SpriteSortMode::SpriteSortMode_Texture);
	RECT* temp1;
	RECT* temp2;

	fuelRect->width = fuel * 2.5;

	temp1 = Rect::GetRECTFromRect(fuelRect);
	temp2 = Rect::GetRECTFromRect(HUDBGRect);

	//Draw bg first
	renderer->GetSpriteBatch()->Draw(HUDTexture, *temp2, nullptr, XMLoadFloat4(&XMFLOAT4(1, 1, 1, 1)), 0);
	
	//Draw fuel bar
	renderer->GetSpriteBatch()->Draw(fuelTexture, *temp1);

	//building the string
	wstringstream* ss = new wstringstream();
	*ss << "Position \n";
	*ss << " ";
	*ss << rank;
	*ss << " / " ;
	*ss << maxRacers;
	wstring* t = new wstring(ss->str());

	//draw string
	fontRenderer->GetSpriteFont()->DrawString(renderer->GetSpriteBatch(), t->c_str(), XMFLOAT2(HUDBGRect->width + HUDBGRect->x - 300, 2)); 
	renderer->End();
	
	//delete stuff
	delete t;
	delete ss;
	delete temp1;
	delete temp2;
}