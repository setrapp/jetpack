#pragma once
#include "Toolkit\Audio\WAVFileReader.h"
#include "Toolkit\Audio\SoundCommon.h"

using namespace DirectX;

class SoundInstance
{
public :
	SoundInstance(wchar_t* path, AudioEngine* engine)
	{
		if(engine)
			{
				temp = new SoundEffect(engine, path);				
				auto p = (temp->CreateInstance());
				effect = p.get();
				if(!effect)
				{
					throw 1;
				}
			}
		else
			throw 1;
	}

	~SoundInstance(void)
	{
		delete effect;
		delete temp;
	}

	void SoundInstance::Play()
	{
		if(effect)
			effect->Play();
	}

	void SoundInstance::SafePlay()
	{
		if(effect)
		{

			temp->Play();
		}
	}

	void SoundInstance::Stop()
	{
		if(effect)
		{
			effect->Stop();
		}
	}

	void SoundInstance::Pan(float pan)
	{
		if(effect)
		{
			effect->SetPan(pan);
		}
	}

	//Wont work right now
	void SoundInstance::Enable3D()
	{
		if(effect)
		{
			AudioListener* listner = new AudioListener();
			AudioEmitter* emitter = new AudioEmitter();
			effect->Apply3D(*listner, *emitter);
		}
	}

	void SoundInstance::ChangeVolume(float vol)
	{
		if(effect)
		{
			effect->SetVolume(vol);
		}
	}

	//Wont work right now
	bool SoundInstance::Loop(bool loop)
	{
		if(effect)
		{
			auto p = effect->pImpl.get();			
			return effect->IsLooped();
		}
		return false;
	}
private:
	SoundEffectInstance* effect;
	SoundEffect* temp;
};