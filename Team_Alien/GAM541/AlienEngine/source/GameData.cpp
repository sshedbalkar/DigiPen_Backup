#include "GameData.h"
//#include "Archetype.h"
//#include "Precompiled.h"
#include "DebugTools.h"
#include "Utility.h"
//
namespace Framework
{
	GameData* GAMEDATA = NULL;
	//
	GameData::GameData()
		:_reader(NULL),
		_archetypes(NULL),
		_stream(NULL)
	{
		//ErrorIf(GAMEDATA!=NULL,"Gamedata already initialized\n");
		GAMEDATA = this;
	}
	GameData::~GameData()
	{
		safeDelete(_archetypes);
		safeDelete(_gameSettings);
		safeDelete(_stream);
		safeDelete(_reader);
	}
	void GameData::initialize()
	{
		_reader = new XMLReader();
		_archetypes = new ArchetypeCollection();
	}
	void GameData::loadLevelData(const char* file)
	{
		std::cout<<"Opening Game Settings data file: "<<file<<"\n";
		if(_reader->open(file))
		{
			_stream = _reader->data().clone();
			//
			_gameSettings = _stream->clone();
			_gameSettings->readProperty("Settings", *_gameSettings);
			//
			_archetypes->createCollectionFrom(*_stream);
			//_archetypes->testFn();
		}
	}
}