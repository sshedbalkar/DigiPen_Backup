///////////////////////////////////////////////////////////////////////////////////////
//
//	Core.cpp
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
//
#include "Precompiled.h"
#include "Core.h"
#include "DebugTools.h"
#include "WindowsSystem.h"
#include "GSM.h"
//
namespace Framework
{

	//A global pointer to the core
	CoreEngine* CORE;

	CoreEngine::CoreEngine():
		_lastTime(0),
		_gameActive(true),
		_fps(3000),
		_dt(0.0f),
		_frameMode(false),
		_advanceFrame(0),
		_frame(0),
		_gsm(new GSM()),
		_paused(false)
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
	}

	void CoreEngine::GameLoop()
	{
		//Initialize the last time variable so our first frame
		//is "zero" seconds (and not some huge unknown number)
		_lastTime = timeGetTime();
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
				if(_frameMode)
				{
					_lastTime =  timeGetTime();
					if(_advanceFrame > 0)
					{
						std::cout<<"Advancing to next frame\n";
						--_advanceFrame;
						//updateSystems(1.0f/_fps);
						_gsm->levelUpdate(1.0f/_fps);
						_gsm->levelDraw();
						++_frame;
					}
					else
					{
						_winSys->Update(1.0f/_fps);
					}
				}
				else
				{
					//std::cout<<"CURRENT FRAME: "<<_frame<<"\n";

					//updateSystems(_dt > 0.031f? 0.031f : _dt);
					_gsm->levelUpdate(_dt > 0.034f ? 0.034f : _dt );
					_gsm->levelDraw();
					//
					unsigned currenttime;
					float dt;
					do
					{
						currenttime = timeGetTime();
						dt = (currenttime - _lastTime) / 1000.0f;
					} while(dt < (1.0f/_fps));
					//
					_dt = dt;
					_lastTime = currenttime;
					++_frame;
				}
			}

			_gsm->levelFree();

			if(_gsm->next() != GSM::LEVEL_RESTART)
			{
				_gsm->levelUnload();
			}

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
	void CoreEngine::fps(int f)
	{
		_fps = f;
	}
	GSM* CoreEngine::gsm()
	{
		return _gsm;
	}
	void CoreEngine::BroadcastMessage(Message* message)
	{
		//The message that tells the game to quit
		if (message->MessageId == Mid::Quit)
		{
			//_gameActive = false;
			_gsm->next(GSM::LEVEL_QUIT);
		}
		if(message->MessageId == Mid::FrameByFrame)
		{
			_frameMode = !_frameMode;
			if(_frameMode)
			{
				std::cout<<"Entering Frame by Frame mode\n";
			}
			else
			{
				std::cout<<"Exiting Frame by Frame mode\n";
			}
		}
		if(message->MessageId == Mid::AdvanceFrame)
		{
			const AdvanceFrame *m = static_cast<const AdvanceFrame*>(message);
			_advanceFrame = m->count;
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
	bool CoreEngine::pauseSystems(bool pause)
	{
		_paused = pause;
		//
		SystemPause m(_paused);
		std::vector<ISystem*>::iterator it1 = Systems.begin(), it2 = Systems.end();
		while (it1 != it2)
		{
			(*it1) -> SendMessage(&m);
			++it1;
		}
		//
		return _paused;
	}
	bool CoreEngine::togglePauseSystems()
	{
		return pauseSystems(!_paused);
	}
	bool CoreEngine::isPaused()
	{
		return _paused;
	}
}
