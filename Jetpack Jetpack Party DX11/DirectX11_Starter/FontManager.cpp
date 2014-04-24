#include "FontManager.h"

FontManager* FontManager::instance = NULL;

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
	if (this == instance)
	{
		instance = NULL;
	}

	delete device;
	for(auto f : fonts)
	{
		delete f.second;
	}
}

FontRenderer* FontManager::AddFont(char* fontName, ID3D11Device* device, DirectX::SpriteBatch* spBatch, wchar_t* path) 
{
	if(fonts.find(fontName) != fonts.end())
	{
		return fonts.find(fontName)->second;
	}

	FontRenderer* f = new FontRenderer(spBatch, device, path);
	fonts.insert(std::make_pair(fontName, f));
	
	return f;
}

void FontManager::DeleteFont(char* fontName)
{
	std::map<char *const, FontRenderer*>::iterator it = fonts.find(fontName);
	if(it != fonts.end())
	{
		delete it->second;
		fonts.erase(fontName);
	}

}

FontRenderer* FontManager::GetFont(char* fontName) const
{
	if(fonts.find(fontName) != fonts.end())
		return fonts.find(fontName)->second;
}

FontManager* FontManager::Instance() 
{	
	if(!instance)
		instance = new FontManager();

	return instance;
}