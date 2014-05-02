#pragma once
#include "Toolkit\Inc\SpriteBatch.h"
#include "Toolkit\Inc\SpriteFont.h"
#include "FontRenderer.h"
#include "DXConnection.h"
#include <map>
#include <unordered_map>
#include <d3d11.h>
#include <vector>
#include "SpriteRenderer.h"
class FontManager
{
private:
	std::map<char*, FontRenderer*> fonts;
	static FontManager* instance;
	FontManager();
	ID3D11Device* device;
	SpriteRenderer* spriteRenderer;

public:
	FontRenderer* AddFont(char* fontName, ID3D11Device* device, SpriteRenderer* spBatch, wchar_t* path);
	void DeleteFont(char* fontName);
	FontRenderer* GetFont(char* fontName)const;
	static FontManager* Instance();
	~FontManager();
};


