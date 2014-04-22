#pragma once
#include "Toolkit\Inc\SpriteFont.h"
#include <d3d11.h>
#include "Renderer.h"
#include "Toolkit\Inc\SpriteBatch.h"


class FontRenderer	: Renderer
{
public:
	FontRenderer::FontRenderer(DirectX::SpriteBatch* spBatch, ID3D11Device* device, wchar_t* spriteFontFile)
	{
		font = new DirectX::SpriteFont(device, spriteFontFile);
		this->spriteBatch = spBatch;
	}


	DirectX::SpriteFont* FontRenderer::GetSpriteFont()
	{
		return font;
	}

	DirectX::SpriteBatch* FontRenderer::GetSpriteBatch()
	{
		return this->spriteBatch;
	}

	~FontRenderer(void)
	{
		
	}

private: 	
	DirectX::SpriteFont* font;
	DirectX::SpriteBatch* spriteBatch;
};
