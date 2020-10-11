#pragma once //Makes sure this header is only included once

#include "fmod/fmod.hpp"
#include "System.h"
//
namespace Framework
{
	class Message;
	//
	class Sound
	{
	private:
		std::string name;
		float volume;
	public:
		Sound();
		~Sound();

		std::string getname() {return name;}
		void setName( std::string Name ){ name=Name; }
		void setVolume(float Volume){ volume=Volume; };
		float getVolume(){ return volume; }

		FMOD_SOUND *sound;
		FMOD_CHANNEL *channel;
	};

	class Audio : public ISystem
	{
	private:
		FMOD_SYSTEM *system;
		typedef std::vector<Sound*> Sounds;
		typedef std::vector<Sound*>::iterator Iterator;
		Sounds sounds;
		bool _paused;
		//
	public:
		Audio();
		~Audio();
		FMOD_SYSTEM* getSystem() { return system; }

		void Initialize();
		void Update(float timeslice);
		virtual std::string GetName(){return "Audio";}
		
		bool serialize();

		bool Load(std::string filename,std::string name,bool loop=false,float volume=1.0);
		Sound* Load(std::string filename);

		bool isPlaying(std::string name);
		bool soundExists(std::string name);

		Sound* findSound(std::string name);
	
		//loopcount=-1 for not loloping
		bool Play(Sound *sound, int loopCount=-1);
		bool Play(std::string name, int loopCount=-1);

		void Stop(std::string name);
		void stopAll();
		void stopAllExcept(std::string name);

		void Pause(std::string name);
		void pauseAll();

		void resume(std::string name);
		void resumeAll();

		//volume 0 to 1
		void changeVolume(float volume);
		void changeVolume(std::string name,float volume);
		//
		virtual void SendMessage(Message *message);
	};

	extern Audio* AUDIO;
}

