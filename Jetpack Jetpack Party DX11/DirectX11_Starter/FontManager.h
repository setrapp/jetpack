#pragma once
#include "Toolkit\Inc\SpriteBatch.h"
#include "Toolkit\Inc\SpriteFont.h"
#include "FontRenderer.h"
#include "DXConnection.h"
#include <map>
#include <unordered_map>
#include <d3d11.h>
#include <vector>

class FontManager
{
private:
	std::unordered_map<char*, FontRenderer*> fonts;
	static FontManager* instance;
	FontManager();
	ID3D11Device* device;
	~FontManager();

public:
	FontRenderer* AddFont(char* fontName, ID3D11Device* device, DirectX::SpriteBatch* spBatch, wchar_t* path);
	void DeleteFont(char* fontName);
	FontRenderer* GetFont(char* fontName)const;
	static FontManager* GetInstance();
};


