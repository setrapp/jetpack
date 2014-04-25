#pragma once
#ifndef _SOUNDMANAGER_H
#define _SOUNDMANAGER_H
#include "Toolkit\Audio\SoundCommon.h"
#include <string>
#include "Toolkit\Audio\WAVFileReader.h"
#include "Toolkit\Audio\SoundCommon.h"
#include "SoundInstance.h"
#include "InputManager.h"
#include "AudioWaveManager.h"

using namespace DirectX;
using namespace JetpackAudio;

//To depend on the names of the audio tracks we have
typedef enum SoundId
{
	//NAME = id
	NONE = 0,
	SAMPLEBG = 1,
	SINK = 2
};

class SoundManager
{
public:
	SoundManager(void)
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		waveformatex = new WAVEFORMATEX();
		flags = AUDIO_ENGINE_FLAGS::AudioEngine_Default;
		category = AUDIO_STREAM_CATEGORY::AudioCategory_GameEffects;
		engineSfx = new AudioEngine(flags, nullptr, nullptr, category);
		category = AUDIO_STREAM_CATEGORY::AudioCategory_GameEffects;		
		mute = false;
		oldkeystate= -1;
		SoundMap = new map<SoundId, SoundInstance*>();
	}

	~SoundManager(void)
	{
		delete SoundMap;
		delete engineSfx;
		delete waveformatex;
	}

	void SoundManager::LoadSound(SoundId name, wchar_t* path)
	{
			if(Contains(name) == SoundMap->end())
			{
				SoundInstance* instance = new SoundInstance(path, engineSfx);
				(*SoundMap)[name] = instance;
				return;
			}
	}

	//Plays a particular sound effect
	void SoundManager::PlaySoundInstance(SoundId name, bool loop = false, bool safePlay = false)
	{
		auto w = Contains(name);
		if(w != SoundMap->end())
		{
			if(safePlay)
				w->second->SafePlay(loop);
			else
				w->second->Play(loop);
		}
	}
	
	//Pans a Sound Instace
	void SoundManager::Pan(SoundId name, float pan)
	{
		auto w = Contains(name);
		if(w != SoundMap->end())
		{
			w->second->Pan(pan);
		}
	}

	//Pauses a Sound Instace
	void SoundManager::PauseSoundInstance(SoundId name)
	{
		auto w = Contains(name);
		if(w != SoundMap->end())
		{
			w->second->Pause();
		}
	}

	//3D sound mode
	void SoundManager::Go3DMode(XMFLOAT4 ListnerPosition, XMFLOAT4 EmitterPosition)
	{
		map<SoundId, SoundInstance*>::iterator  iterator= SoundMap->begin();
		while(iterator != SoundMap->end())
		{
			iterator->second->Enable3D(ListnerPosition, EmitterPosition);
			iterator++;
		}
	}

	//always call this
	void SoundManager::Update()
	{
		if(IPMan::GetIPMan()->GetSpecialKeyboardState('M') && IPMan::GetIPMan()->GetSpecialKeyboardState('M') != oldkeystate)
		{
			mute = !mute;
			Mute(mute);
		}
		oldkeystate = IPMan::GetIPMan()->GetSpecialKeyboardState('M');
		engineSfx->Update();
	}

	//This is an expensive operation and it is inclusive of the default update()
	void SoundManager::Update3D(XMFLOAT4 ListnerPosition, XMFLOAT4 EmitterPosition)
	{
		map<SoundId, SoundInstance*>::iterator  iterator= SoundMap->begin();
		while(iterator != SoundMap->end())
		{
			iterator->second->Enable3D(ListnerPosition, EmitterPosition);
			iterator++;
		}

		if(IPMan::GetIPMan()->GetSpecialKeyboardState('M') && IPMan::GetIPMan()->GetSpecialKeyboardState('M') != oldkeystate)
		{
			mute = !mute;
			Mute(mute);
		}
		oldkeystate = IPMan::GetIPMan()->GetSpecialKeyboardState('M');
		engineSfx->Update();
	}

	//Give the current instance of SoundEngine
	AudioEngine* SoundManager::GetSoundEngine()
	{
		return engineSfx;
	}

	//Mutes/Unmutes all sounds
	void SoundManager::Mute(bool mute)
	{
		map<SoundId, SoundInstance*>::iterator  iterator= SoundMap->begin();
		while(iterator != SoundMap->end())
		{
			iterator->second->Mute(mute);
			iterator++;
		}
	}

	//Sets the volume of all the soundtracks loaded
	void SoundManager::SetVolumeGeneral(float volume)
	{
		map<SoundId, SoundInstance*>::iterator  iterator= SoundMap->begin();
		while(iterator != SoundMap->end())
		{
			iterator->second->ChangeVolume(volume);
			iterator++;
		}
	}

	//Sets the volume of a single soundtrack
	void SoundManager::SetVolume(SoundId name, float volume)
	{
		auto w = Contains(name);
		if(w != SoundMap->end())
		{
			w->second->ChangeVolume(volume);
		}
	}

private:
	AudioEngine* engineSfx;
	AUDIO_ENGINE_FLAGS flags;
	WAVEFORMATEX* waveformatex;
	AUDIO_STREAM_CATEGORY category;
	bool mute;
	std::map<SoundId, SoundInstance*>* SoundMap;
	int oldkeystate;

	inline map<SoundId, SoundInstance*>::iterator Contains(SoundId name) const
	{
		auto instance = SoundMap->find(name);
		if(instance == SoundMap->end())
			return instance;
		else
			return instance;
	}
};

#endif