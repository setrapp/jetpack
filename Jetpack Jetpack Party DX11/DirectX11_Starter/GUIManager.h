#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <d3d11.h>
#include "Common.h"
#include "DXConnection.h"
#include "Rect.h"
#include <vector>
#include "GUIBase.h"
#include <map>

using namespace std;
using namespace DirectX;

class GUIManager {
public:
	GUIManager()
	{
		
	}

	~GUIManager()
	{
			map<char*, GUIBase*>::iterator i = _guiElements.begin();
			while(i != _guiElements.end())
			{
				i->second->~GUIBase();
				i++;
			}
	}

	void Add(char* name, GUIBase* _guiElement)
	{
		if(_guiElement)
			_guiElements[name] = (_guiElement);
	}
	
	bool Remove(char* _guiElementName) 
	{
		{
			auto f = _guiElements.find(_guiElementName);
			if(f != _guiElements.end())
			{
				_guiElements.erase(f);
				return true;
			}
		}
		return false;
	}

		 

	void Update(float dt)
	{
		ShowCursor(true);
		POINT point;
		if(GetCursorPos(&point)) 
		{
			auto i = _guiElements.begin();
			while(i != _guiElements.end())
			{
				i->second->Update(&point, dt);
				i++;
			}
		}
	}

	void Render()
	{
		auto i = _guiElements.begin();
		while(i != _guiElements.end())
		{
			i->second->Render();
			i++;
		}
	}

public:
	map<char*, GUIBase*> _guiElements;
};