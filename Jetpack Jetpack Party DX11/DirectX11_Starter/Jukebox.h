#pragma once
#include "Toolkit\Audio\WAVFileReader.h"
#include "Toolkit\Audio\SoundCommon.h"
#include "SoundInstance.h"
#include <queue>
#include <vector>
#include <string>
#include <vector>
#include <wchar.h>

using namespace std;


namespace JetpackAudio
{
	class JukeBox
	{
	private:
		vector<SoundInstance*> soundCollection;
		queue<SoundInstance*> soundPlaylist;
		SoundInstance* currentSound;
		bool playing;

	public:		
		JukeBox()
		{
			playing = false;
			time = 0;
		}

		~JukeBox()
		{
			delete currentSound;
			
			//Somehow this fails. So, there are mem leaks.. :(
			/*for(auto a : soundCollection)
				if(a->effect)
					delete a;
*/
		}

		void AddTrack(const wchar_t* path, DirectX::AudioEngine* engine)
		{
			SoundInstance* i = new SoundInstance(path, engine);
			soundCollection.push_back(i);
			soundPlaylist.push(i);
			i->ChangeVolume(0.05);
		}

		void Play()
		{
			if(!playing)
			{
				if(time < closeTime)
				{
					if(currentSound)
					{
						currentSound->Play(false);
						playing = true;
						return;
					}
				}

				if(!soundPlaylist.empty())
				{
					currentSound = soundPlaylist.front();
					soundPlaylist.pop();
					currentSound->Play(false);			
					playing = true;
					closeTime = time + currentSound->sizeInMilliseconds() / 1000;
				}
				else
				{
					Shuffle();
					Play();
				}
			}
		}


		void MUTE(const bool vol)
		{
			for(auto a : soundCollection)
				a->Mute(vol);
		}

		void Shuffle()
		{
			for(SoundInstance* i : soundCollection)
			{
				soundPlaylist.push(i);
			}
		}

		void Pause()
		{
			playing = false;
			currentSound->Pause();
		}

		inline void Stop()
		{
			playing = false;
			currentSound->Stop();
		}

		inline bool Playing()
		{			
			return playing;
		}

		void Update(const float dt)
		{
			if(time > closeTime)
				{
					playing = false;

					Play();
				}

			time += dt;
		}

		//In milliseconds
		double time;
		double closeTime;
	};
};