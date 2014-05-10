#include "HUD.h"
#include "Debug.h"

float HUD::fuel = 0;

HUD::HUD(SpriteRenderer* renderer, FontRenderer* fontRenderer)
{	
	this->renderer			= renderer;
	this->fontRenderer		= fontRenderer;

	Reset();


	HR (CreateWICTextureFromFile(DXConnection::Instance()->device, 
		DXConnection::Instance()->deviceContext,
		L"../Assets/HUD/HUD_BG.png",
		&this->HUDResource,
		&this->HUDTexture));

	HR (CreateWICTextureFromFile(DXConnection::Instance()->device, 
		DXConnection::Instance()->deviceContext,
	L"../Assets/HUD/fuel.png",
	&this->fuelResource,
	&this->fuelTexture));
	fontScale = 1;
}

inline void HUD::Reset()
{
	RECT tempRect;
	screen = new Rect();

	GetClientRect(GetActiveWindow(), &tempRect);

	auto temp	= Rect::GetRectFromRECT(&tempRect);
	*screen		= *temp;

	delete temp;

	HUDBGRect				= new Rect(0, 0, screen->width, 250);
	fuelRect				= new Rect(screen->width / 2 - 125, 0, 250, 100);
	rank					= 1;
	maxRacers				= 10;
	fontScale				= (float)screen->width / (float)screen->height;
	//Reverse current aspect ratio, so we can scale to other ratios too. 
	fontScale			   *= (float)9 / (float)16;

	xTextOffset = fontRenderer->GetSpriteFont()->MeasureString(L"Position \n 10 / 10").m128_f32[1] * fontScale;
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


/*inline void HUD::setFuel(float fuelValue)
{
	this->fuel = fuelValue;
}


inline float HUD::getFuel()
{
	return this->fuel;
}*/


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

	//since the width is 250. 
	fuelRect->width = fuel * 2.5f;

	temp1 = Rect::GetRECTFromRect(fuelRect);
	temp2 = Rect::GetRECTFromRect(HUDBGRect);

	//Draw bg first
	renderer->GetSpriteBatch()->Draw(
		HUDTexture, 
		*temp2, 
		nullptr, 
		XMLoadFloat4(&XMFLOAT4(1, 1, 1, 1)), 
		0);
	
	//Draw fuel bar
	renderer->GetSpriteBatch()->Draw(
		fuelTexture, 
		*temp1);

	//building the string
	wstringstream* ss = new wstringstream();
	*ss << "Position \n";
	*ss << " ";
	*ss << rank;
	*ss << " / " ;
	*ss << maxRacers;
	wstring* t = new wstring(ss->str());

	//draw string
	fontRenderer->GetSpriteFont()->DrawString(
		renderer->GetSpriteBatch(), 
		t->c_str(), 
		XMLoadFloat2(&XMFLOAT2(HUDBGRect->width + HUDBGRect->x - xTextOffset, 20 * fontScale)), 
		Colors::Green, 
		0, 
		g_XMZero, 
		fontScale); 


	renderer->End();
	
	//delete stuff
	delete t;
	delete ss;
	delete temp1;
	delete temp2;
}

void HUD::Resize()
{
	Reset();
}