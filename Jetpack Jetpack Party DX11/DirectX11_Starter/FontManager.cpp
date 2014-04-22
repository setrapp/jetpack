#include "FontManager.h"

FontManager* FontManager::instance = NULL;

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
	delete instance;
	delete device;
	for(auto f : fonts)
	{
		f.second->~FontRenderer();
		delete f.first;
	}
}

FontRenderer* FontManager::AddFont(char* fontName, ID3D11Device* device, DirectX::SpriteBatch* spBatch, wchar_t* path) 
{
	if(fonts.find(fontName) != fonts.end())
	{
		return fonts.find(fontName)->second;
	}

	FontRenderer* f = new FontRenderer(spBatch, device, path);
	fonts.insert(std::make_pair(fontName, f)); // [fontNamef;
	
	return f;
}

void FontManager::DeleteFont(char* fontName)
{
	if(fonts.find(fontName) != fonts.end())
	{
		fonts.erase(fontName);
	}

}

FontRenderer* FontManager::GetFont(char* fontName) const
{
	if(fonts.find(fontName) != fonts.end())
		return fonts.find(fontName)->second;
}

FontManager* FontManager::GetInstance() 
{	
	if(!instance)
		instance = new FontManager();

	return instance;
}