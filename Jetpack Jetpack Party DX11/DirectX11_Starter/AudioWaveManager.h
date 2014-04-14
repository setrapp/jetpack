#pragma once

#include "AudioWaveManager.h"
#include "Toolkit\Audio\SoundCommon.h"
#include <string>
#include "Toolkit\Audio\WAVFileReader.h"
#include "Toolkit\Audio\SoundCommon.h"
#include "SoundInstance.h"
#include <vector>
#include "Common.h"
#include <map>

using namespace std;

//Obselete. Use SoundInstance
class AudioWaveManger
{
public:

	AudioWaveManger(wchar_t* path, AudioEngine* engine)
	{
		waveBank = new WaveBank(engine, path);
	}

	void AudioWaveManger::Play(bool loop)
	{
		if(waveBank)
		{						
			auto p = waveBank->CreateInstance(0);
			//p->Play(loop);
			int id = 0;
			if (instances.find(id) == instances.end())
				{
					instances[id] = p.get();
					p.get()->Play();
				}
			else
			{
				//Item is present
				if(instances[id]->GetState() != SoundState::PLAYING)
					instances[id]->Play();
			}		
		}		
	}

	void AudioWaveManger::Play(int id, bool loop)
	{
		if(waveBank)
		{			
			auto p = waveBank->CreateInstance(0);
			//p->Play(loop);
			if (instances.find(id) == instances.end())
				{
					instances[id] = p.get();
					p.get()->Play();
				}
			else
			{
				//Item is present
				if(instances[id]->GetState() != SoundState::PLAYING)
					instances[id]->Play();
			}		
		}		
	}

	void AudioWaveManger::Play(char* track, bool loop)
	{
		if(waveBank)
		{			
			auto p = waveBank->CreateInstance(0);
			const int id = waveBank->Find(track);
			
			if (instances.find(id) == instances.end())
				{
					instances[id] = p.get();
					instances[id]->Play(loop);
					p.get()->SetVolume(10000);
				}
			else
			{
				//Item is present
				if(instances[id]->GetState() != SoundState::PLAYING)
					instances[id]->Play();
			}		
		}		
	}

	void AudioWaveManger::Pause()
	{
		if(waveBank)
		{
			int id = 0;
			if (instances.find(id) != instances.end())
				{
					instances[id]->Pause();
				}
		}
	}

	void AudioWaveManger::Pause(int id)
	{
		if(waveBank)
		{
			if (instances.find(id) != instances.end())
				{
					instances[id]->Pause();
				}
		}
	}

	void AudioWaveManger::Pause(char* track)
	{
		if(waveBank)
		{
			int id = waveBank->Find(track);
			if (instances.find(id) != instances.end())
				{
					instances[id]->Pause();
				}
		}
	}

	void AudioWaveManger::Update()
	{
		if(waveBank)
		{
			
		}
	}

	~AudioWaveManger()
	{
		delete waveBank;
	}

	WaveBank* GetWaveManager()
	{
		return waveBank;
	}

	void AudioWaveManger::Mute(bool mute)
	{
		/*for(int i = 0 ; i< instances.size(); i++)
			if(!mute)
				instances.at(i)->SetVolume(0);			
			else
				instances.at(i)->SetVolume(1);*/
	}
private:
	DirectX::WaveBank* waveBank;	
	std::map<int, SoundEffectInstance*> instances;
	AUDIO_ENGINE_FLAGS flags;
	
};