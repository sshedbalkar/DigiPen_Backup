///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Core.h
///	Defines the CoreEngine.
///	Authors:  
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once
//
#include "System.h"
#include "Message.h"
//
namespace Framework
{
	///The core manages all the systems in the game. Updating them, routing messages, and
	///destroying them when the game ends.
	class WindowsSystem;
	class GSM;
	//
	class CoreEngine
	{
	public:
		CoreEngine();
		~CoreEngine();
		///Update all the systems until the game is no longer active.
		void GameLoop();
		///Destroy all systems in reverse order that they were added.
		void DestroySystems();
		///Broadcasts a message to all systems.
		void BroadcastMessage(Message* m);
		///Adds a new system to the game.
		void AddSystem(ISystem* system);
		///Initializes all systems in the game.
		void Initialize();
		///
		float fps();
		void updateSystems(float dt);
		float frameTime();
		//GSM* gsm();
		bool togglePauseSystems();
		bool isPaused();
		void LaunchLevel( const char* filepath );
		void ResumePrevLevel();

		SYSTEMTIME launch_time;

	private:
		void unloadSystems();
	private:
		//Tracks all the systems the game uses
		std::vector<ISystem*> Systems;
		//
		unsigned _lastTime;
		bool _gameActive;
		float _dt;
		WindowsSystem* _winSys;
		GSM* _gsm;
	};

	///Message to tell the game to quit
	class MessageQuit : public Message
	{
	public:
		MessageQuit() : Message(Mid::Quit) {};
	};
	/////////////
	class Resolution : public Message
	{
	public:
		Resolution() : Message(Mid::Resolution), w(800), h(600) {};
		int w;
		int h;
	};
	//
	class SystemPause: public Message
	{
	public:
		bool Pause;
		friend class CoreEngine;
	//private:
		SystemPause(bool pause):
			Message(Mid::SystemPause),
			Pause(pause)
		{
			//
		};
		~SystemPause(){};
	};
	//A global pointer to the instance of the core
	extern CoreEngine* CORE;
}