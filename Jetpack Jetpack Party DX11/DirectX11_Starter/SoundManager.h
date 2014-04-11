#pragma once
#ifndef _SOUNDMANAGER_H
#define _SOUNDMANAGER_H
#include "Toolkit\Audio\SoundCommon.h"
#include <string>
#include "Toolkit\Audio\WAVFileReader.h"
#include "Toolkit\Audio\SoundCommon.h"
#include "SoundInstance.h"

using namespace DirectX;

class SoundManager
{
public:
	SoundManager(void)
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED );
		waveformatex = new WAVEFORMATEX();
		flags = AUDIO_ENGINE_FLAGS::AudioEngine_Default;
		category = AUDIO_STREAM_CATEGORY::AudioCategory_GameEffects;
		engineSfx = new AudioEngine(flags, nullptr, nullptr, category);
		category = AUDIO_STREAM_CATEGORY::AudioCategory_GameMedia;
		BG = new SoundInstance(L"../Assets/SampleBG.wav", engineSfx);
		//BG->Enable3D();
		BG->SafePlay();
	}

	~SoundManager(void)
	{
		delete engineSfx;
		delete engineBGMusic;
		delete waveformatex;
	}

	void SoundManager::Update()
	{
		engineSfx->Update();
		engineBGMusic->Update();
	}


private:
	AudioEngine* engineSfx;
	AudioEngine* engineBGMusic;
	AUDIO_ENGINE_FLAGS flags;
	WAVEFORMATEX* waveformatex;
	AUDIO_STREAM_CATEGORY category;
	SoundInstance* BG;
};

#endif