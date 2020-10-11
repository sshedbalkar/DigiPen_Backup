///////////////////////////////////////////////////////////////////////////////////////
//
//	Core.cpp
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
//
#include "Precompiled.h"

#include "Precompiled.h"
#include "Core.h"
#include "WindowsSystem.h"
#include "GSM.h"
#include "ComponentRegistration.h"
//
namespace Framework
{

	//A global pointer to the core
	CoreEngine* CORE;

	CoreEngine::CoreEngine():
		_lastTime(0),
		_gameActive(true),
		_dt(0.0f),
		_gsm(new GSM())
	{
		CORE = this; //Set the global pointer
	}

	CoreEngine::~CoreEngine()
	{
		//Nothing for the destructor to do
		delete _gsm;
	}

	void CoreEngine::Initialize()
	{
		std::vector<ISystem*>::iterator it1 = Systems.begin(), it2 = Systems.end();
		while (it1 != it2)
		{
			(*it1) -> Initialize();
			++it1;
		}
		ComponentRegistration();
		_gsm->initialize( );
//		_gsm->next( "Objects\\level_0.xml" );
	}

	void CoreEngine::GameLoop()
	{
		//Initialize the last time variable so our first frame
		//is "zero" seconds (and not some huge unknown number)
		_lastTime = timeGetTime();
		GetSystemTime( &launch_time );
		//
		while(_gsm->current() != GSM::LEVEL_QUIT)
		{
			if(_gsm->current() == GSM::LEVEL_RESTART)
			{
				_gsm->current(_gsm->next(_gsm->prev()));
			}
			else
			{
				_gsm->update();
				_gsm->levelLoad();
			}
			_gsm->levelInitialize();
		
			while(_gsm->current() == _gsm->next())
			{
				updateSystems(_dt > 0.031f? 0.031f : _dt);
				unsigned currenttime = timeGetTime();
				_dt = (currenttime - _lastTime) / 1000.0f;
				_lastTime = currenttime;
				//
				//LONGLONG det;
				//LARGE_INTEGER current;
				//QueryPerformanceCounter( &current );
				//static LONGLONG last = current.QuadPart;
				//det = current.QuadPart;
				//det -= last;
				//printf( "%I64d\n", det );
				//last = current.QuadPart;
			}
			_gsm->levelFree();
			unloadSystems();
			_gsm->levelUnload();
			
			_gsm->prev(_gsm->current());
			_gsm->current(_gsm->next());
		}
	}
	void CoreEngine::updateSystems(float dt)
	{
		std::vector<ISystem*>::iterator it1 = Systems.begin(), it2 = Systems.end();
		while (it1 != it2)
		{
			(*it1) -> Update(dt);
			++it1;
		}
	}
	float CoreEngine::frameTime()
	{
		return _dt;
	}
	float CoreEngine::fps()
	{
		return 1.0f/_dt;
	}
	void CoreEngine::LaunchLevel( const char* filepath )
	{
		_gsm->next( filepath );
	}
	void CoreEngine::ResumePrevLevel()
	{
		_gsm->next( _gsm->prev() );
	}
	//GSM* CoreEngine::gsm()
	//{
	//	return _gsm;
	//}
	void CoreEngine::BroadcastMessage(Message* message)
	{
		//The message that tells the game to quit
		//if ( message->MessageId == Mid::CharacterKey ) _gsm->next("Objects\\level_test.xml");
		if (message->MessageId == Mid::Quit)
		{
			//_gameActive = false;
			_gsm->next(GSM::LEVEL_QUIT);
		}
		//Send the message to every system--each
		//system can figure out whether it cares
		//about a given message or not
		std::vector<ISystem*>::iterator it1 = Systems.begin(), it2 = Systems.end();
		while (it1 != it2)
		{
			(*it1) -> SendMessage(message);
			++it1;
		}
	}

	void CoreEngine::AddSystem(ISystem* system)
	{
		//Add a system to the core to be updated
		//every frame
		Systems.push_back(system);
		//
		WindowsSystem* sys;
		if(sys = dynamic_cast<WindowsSystem*>(system))
		{
			_winSys = sys;
		}
	}
	
	void CoreEngine::unloadSystems()
	{
		for (unsigned i = 0; i < Systems.size(); ++i)
		{
			Systems[Systems.size()-i-1]->Unload();
		}

	}
	void CoreEngine::DestroySystems()
	{
		//Delete all the systems in reverse order
		//that they were added in (to minimize any
		//dependency problems between systems)
		for (unsigned i = 0; i < Systems.size(); ++i)
		{
//			std::cout <<Systems[Systems.size() - i - 1]->GetName() <<std::endl;
			delete Systems[Systems.size()-i-1];
		}

	}
}
