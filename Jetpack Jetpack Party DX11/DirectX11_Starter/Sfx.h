#ifndef _SFX_H
#define _SFX_H
#include <string>
#include "Toolkit\Audio\WAVFileReader.h"
#include "Toolkit\Audio\SoundCommon.h"

using namespace DirectX;
class Sfx
{
public:
	std::string												EffectName;
	AudioEngine*											p;
	WaveBank*							                    g_waveBank;
	SoundEffect*							                g_soundEffect;
	SoundEffectInstance*							        g_effect1;	
	uint32_t                                                g_audioEvent;
	float                                                   g_audioTimerAcc;

	HDEVNOTIFY                                              g_hNewAudio;
	Sfx()
	{
		CoInitializeEx( nullptr, COINIT_MULTITHREADED );
	/*	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
		p = new AudioEngine(eflags);
		
		g_audioEvent = 0;
		g_audioTimerAcc = 10.f;

		g_waveBank = new WaveBank( p, L"wavebank.xwb" );
		g_soundEffect =  new SoundEffect( p, L"sunk.wav");
		g_effect1 = (g_soundEffect->CreateInstance()).get();
		
		g_waveBank->Play(1);
		g_effect1->Play(true);*/
	}

	void Sfx::Update(float dt)
	{		
	/*	g_waveBank->Play(1);
		p->Update();*/
	}
};

#endif