#pragma once
#include "Toolkit\Audio\WAVFileReader.h"
#include "Toolkit\Audio\SoundCommon.h"

using namespace DirectX;

namespace JetpackAudio
{
class SoundInstance
{
public :
	SoundInstance(wchar_t* path, AudioEngine* engine)
	{
		if(engine)
			{
				effectEffect.reset(new SoundEffect(engine, path));	
				effect = effectEffect.get()->CreateInstance(SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_Use3D);			
				if(!effect)
				{
					throw 1;
				}
			}
		else
			throw 1;

		currentVolume = 1;
	}

	~SoundInstance(void)
	{
		delete effect.release();
		delete effectEffect.release();
	}

	inline void SoundInstance::Play(bool loop)
	{
		
		if(effect)
			effect->Play(loop);
	}

	inline void SoundInstance::SafePlay(bool loop)
	{
		if(effect)
		{
			if(effect->GetState() != SoundState::PLAYING)
				effect->Play(loop);
		}
	}

	inline void SoundInstance::Stop()
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

	void SoundInstance::Pause()
	{
		if(effect)
			effect->Pause();
	}

	//Wont work right now
	inline void SoundInstance::Enable3D(XMFLOAT4 ListnerPosition, XMFLOAT4 EmitterPosition)
	{
		if(effect)
		{
			AudioListener* listner = new AudioListener();
			listner->SetPosition(XMLoadFloat4(&ListnerPosition));
			AudioEmitter* emitter = new AudioEmitter();
			emitter->SetPosition(XMLoadFloat4(&EmitterPosition));
			effect->Apply3D(*listner, *emitter);
		}
	}

	void SoundInstance::ChangeVolume(float vol)
	{
		if(effect)
		{
			effect->SetVolume(vol);
			currentVolume = vol;
		}
	}

	inline void SoundInstance::Mute(bool mute)
	{
		if(effect)
		{
			if(mute)
			{
				effect->SetVolume(0);
			}
			else
			{
				effect->SetVolume(1);
			}
		}
	}

private:
	std::unique_ptr<SoundEffect> effectEffect;
	std::unique_ptr<SoundEffectInstance> effect;
	float currentVolume;
};
}