#pragma once
#include "Toolkit\Inc\SpriteFont.h"
#include <d3d11.h>

class FontRenderer
{
public:
	FontRenderer::FontRenderer( ID3D11Device* device, wchar_t* spriteFontFile)
	{
		font = new DirectX::SpriteFont(device, spriteFontFile);
	}

	void FontRenderer::setSpriteBatch(DirectX::SpriteBatch* sp)
	{
		if(!this->spriteBatch)
			this->spriteBatch = sp;
	}

	~FontRenderer(void);	
	const DirectX::SpriteFont* font;

private: 
	DirectX::SpriteBatch* spriteBatch;
};

