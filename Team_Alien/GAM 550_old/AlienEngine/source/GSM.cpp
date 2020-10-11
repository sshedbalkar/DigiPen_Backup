#include "Precompiled.h"

#include "ILevel.h"
#include "GSM.h"
#include "BasicLevels.h"
#include "Core.h"
#include "LoadLevelClass.h"
//
namespace Framework
{
	ILevel* GSM::LEVEL_QUIT = new Level_Quit();
	ILevel* GSM::LEVEL_RESTART = new Level_Restart();
	//
	GSM::GSM()
		:_next(0),
		_prev(0),
		_current(0),
		level_loader(NULL)
	{
		//
	}
	//
	GSM::~GSM()
	{
		delete LEVEL_QUIT;
		delete LEVEL_RESTART;
		S_DELETE( level_loader );
		LevelListType::iterator itb = levels.begin(), ite = levels.end();
		while( itb != ite )
		{
			S_DELETE( *itb );
			itb = levels.erase( itb );
		}
	}
	//
	int GSM::initialize()
	{
		level_loader = new LoadLevelClass;	
		level_loader->Initialize();

		return 1;
	}
	//
	int GSM::update()
	{
		LevelListType::iterator itb = levels.begin(), ite = levels.end();
		while( itb != ite )
		{
			if ( (*itb) != _current && (*itb) != _next && (*itb) != _prev )
			{
				S_DELETE( *itb );
				itb = levels.erase( itb );
			}
			else ++itb;
		}
		return 1;
	}
	int GSM::levelLoad(void)
	{
		return 1;
	}
	int GSM::levelInitialize(void)
	{
		if ( level_loader->LoadLevelFile( _next->filepath ) ) 
		{
			_current = _next;
			return 1;
		}
		return 0;
	}
	int GSM::levelFree(void)
	{
		level_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		return 1;
	}
	int GSM::levelUnload(void)
	{
		return 1;
	}
	//
	ILevel* GSM::next(void)
	{
		return _next;
	}
	ILevel* GSM::next(ILevel* level)
	{
		return _next = level;
	}
	ILevel* GSM::next( const char* filepath )
	{
		ILevel* level = new ILevel;
		level->filepath = filepath;
		levels.push_back( level );
		_next = level;
		return _next;
	}
	ILevel* GSM::prev(void)
	{
		return _prev;
	}
	ILevel* GSM::prev(ILevel* level)
	{
		return _prev = level;
	}
	ILevel* GSM::current(void)
	{
		return _current;
	}
	ILevel* GSM::current(ILevel* level)
	{
		return _current = level;
	}
	void GSM::deletePrevLevel()
	{
		delete _prev;
		_prev = 0;
	}
}