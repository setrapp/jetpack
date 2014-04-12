#pragma once
#ifndef _SOUNDMANAGER_H
#define _SOUNDMANAGER_H
#include "Toolkit\Audio\SoundCommon.h"
#include <string>
#include "Toolkit\Audio\WAVFileReader.h"
#include "Toolkit\Audio\SoundCommon.h"
#include "SoundInstance.h"
#include "AudioWaveManager.h"

using namespace DirectX;

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
		BG = new SoundInstance(L"../Assets/SampleBG.wav", engineSfx);
		//BG->Enable3D();
		//BG->SafePlay();
		man = new AudioWaveManger(L"../Assets/tempwavebank.xwb", engineSfx);
		man->Play("party", true);
		mute = false;
	}

	~SoundManager(void)
	{
		delete engineSfx;
		delete waveformatex;
	}

	void SoundManager::Update()
	{
		if(GetAsyncKeyState('M'))
		{
			mute != mute;
			Mute(mute);
		}
		engineSfx->Update();
	}

	AudioEngine* SoundManager::GetSoundEngine()
	{
		return engineSfx;
	}

	void SoundManager::Mute(bool mute)
	{
		man->Mute(mute);
		BG->Mute(mute);
	}



private:
	AudioEngine* engineSfx;
	AUDIO_ENGINE_FLAGS flags;
	WAVEFORMATEX* waveformatex;
	AUDIO_STREAM_CATEGORY category;
	SoundInstance* BG;
	AudioWaveManger* man;
	bool mute;
};

#endif