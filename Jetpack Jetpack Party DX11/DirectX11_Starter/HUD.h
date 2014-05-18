#pragma once
#include "Entity.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include "Common.h"
#include "DXConnection.h"
#include "FontRenderer.h"
#include "SpriteRenderer.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Rect.h"


using namespace std;
using namespace DirectX;

class HUD 
{
public:
	HUD(SpriteRenderer* renderer, FontRenderer* fontRenderer);
	~HUD(void);
	//inline void setFuel(float fuelValue);
	//inline float getFuel();
	inline void setRank(short newRank);
	inline short getRank();
	inline void setMaxRacers(unsigned short maxRacers);
	void Render();
	inline void Resize();
	inline void Reset();

private:
	SpriteRenderer*				renderer;
	FontRenderer*				fontRenderer;
	Rect*						HUDBGRect;
	Rect*						fuelRect;
	Rect*						screen;
	Rect*						fuelText;
	short						rank;
	unsigned short				maxRacers;
	ID3D11ShaderResourceView*	HUDTexture;
	ID3D11ShaderResourceView*	fuelTexture;
	ID3D11Resource*				HUDResource;
	ID3D11Resource*				fuelResource;
	float						fontScale;
	float						xTextOffset;

public:
	//Pass it as a percentage. Min 0% and max 100%
	static float fuel;
};

