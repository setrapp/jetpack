#pragma once
#ifndef _SOUNDMANAGER_H
#define _SOUNDMANAGER_H


#include <string>
#include "SoundInstance.h"
#include "InputManager.h"
#include "AudioWaveManager.h"
#include "Jukebox.h"

#define FORCE_MUTE

using namespace DirectX;
using namespace JetpackAudio;

//To depend on the names of the audio tracks we have
typedef enum SoundId
{
	//NAME = id
	NONE = 0,
	SAMPLEBG = 1,
	COUNTDOWN = 2,
	THRUSTER = 3,	
};

class SoundManager
{
public:	
	JukeBox* jukebox;
	JukeBox* menuJukeBox;

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


		jukebox = new JukeBox();	
		jukebox->AddTrack(L"../Assets/Sounds/JukeBox/0.wav", engineSfx);
		jukebox->AddTrack(L"../Assets/Sounds/JukeBox/1.wav", engineSfx);
		jukebox->AddTrack(L"../Assets/Sounds/JukeBox/2.wav", engineSfx);
		jukebox->AddTrack(L"../Assets/Sounds/JukeBox/3.wav", engineSfx);
		jukebox->AddTrack(L"../Assets/Sounds/JukeBox/4.wav", engineSfx);

		menuJukeBox = new JukeBox();
		menuJukeBox->AddTrack(L"../Assets/Sounds/JukeBox/1.wav", engineSfx);
		menuJukeBox->AddTrack(L"../Assets/Sounds/JukeBox/2.wav", engineSfx);
		menuJukeBox->AddTrack(L"../Assets/Sounds/JukeBox/3.wav", engineSfx);
		menuJukeBox->AddTrack(L"../Assets/Sounds/JukeBox/4.wav", engineSfx);
	}

	~SoundManager(void)
	{
		for (std::map<SoundId, SoundInstance*>::iterator it = SoundMap->begin(); it != SoundMap->end(); it++)
		{
			delete it->second;
		}
		delete SoundMap;
		delete engineSfx;
		delete waveformatex;
		delete jukebox;
		delete menuJukeBox;
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
	void SoundManager::PlaySoundInstance(SoundId name, bool loop = false, bool safePlay = false, float volume = 1)
	{
		auto w = Contains(name);
		if(w != SoundMap->end())
		{
			if(safePlay)
				w->second->SafePlay(loop);
			else
				w->second->Play(loop);
			
		w->second->ChangeVolume(volume);
		}

	}

	void SoundManager::StopSoundInstance(SoundId name)
	{
		auto w = Contains(name);
		if(w != SoundMap->end())
			w->second->Stop();
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
	void SoundManager::Update(float dt)
	{
		if(IPMan::GetIPMan()->GetKey('M') && IPMan::GetIPMan()->GetKey('M') != oldkeystate)
		{
			mute = !mute;
			Mute(mute);
		}
		oldkeystate = IPMan::GetIPMan()->GetKey('M');
		engineSfx->Update();

		if(jukebox->Playing())
			jukebox->Update(dt);

		if(menuJukeBox->Playing())
			menuJukeBox->Update(dt);
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

		if(IPMan::GetIPMan()->GetKey('M') && IPMan::GetIPMan()->GetKey('M') != oldkeystate)
		{
			mute = !mute;
			Mute(mute);
		}
		oldkeystate = IPMan::GetIPMan()->GetKey('M');
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
			jukebox->MUTE(mute);
			menuJukeBox->MUTE(mute);
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

	void PlayJukeBox()
	{
		jukebox->Play();
	}

	void PauseJukeBox()
	{
		jukebox->Pause();
	}

	void StopJukeBox()
	{
		jukebox->Stop();
	}

	void PlayMenuJukeBox()
	{
		menuJukeBox->Play();
	}

	void PauseMenuJukeBox()
	{
		menuJukeBox->Pause();
	}

	void StopMenuJukeBox()
	{
		menuJukeBox->Stop();
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