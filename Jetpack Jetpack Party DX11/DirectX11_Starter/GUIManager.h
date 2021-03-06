#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <d3d11.h>
#include "Common.h"
#include "DXConnection.h"
#include "Rect.h"
#include <vector>
#include "GUIText.h"
#include <map>
#include "AnimationBase.h"
#include <unordered_map>
using namespace std;
using namespace DirectX;

class GUIManager {
public:
	GUIManager()
	{
		
	}

	~GUIManager()
	{		
			unordered_map<char*, GUIBase*>::iterator i = _guiElements.begin();
			while(i != _guiElements.end())
			{
				//delete i->first;
				delete i->second;
				i++;
			}
	}

	void Add(char* name, GUIBase* _guiElement)
	{
		if(_guiElement)
			_guiElements[name] = static_cast<GUIBase*>(_guiElement);
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

		 

	void Update(const float dt)
	{
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
	unordered_map<char*, GUIBase*> _guiElements;
};