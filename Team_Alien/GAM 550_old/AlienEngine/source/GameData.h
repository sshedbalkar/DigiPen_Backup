////////////////////////////////////////////////////////////////
//
//	GameData.h
//	Parses an external game settings xml file and holds the data for later usage by other parts of the game.
//
//	Author: Santosh Shedbalkar
//	Copyright 2011, Digipen Institute of Technology
//
////////////////////////////////////////////////////////////////
#ifndef GAMEDATA_H
#define GAMEDATA_H
//#pragma message("Compiling: "__FILE__)
//
//#include <vector>
//#include "System.h"
#include "XMLSerialization.h"
#include "Archetype.h"
//#include "VMath.h"
//
namespace Framework
{
	class GameData
	{
		struct ArchFileInfo
		{
			unsigned int reader_number;
			unsigned int stream_number;
		};
	public:
		GameData();
		~GameData();
		void initialize();
		void LoadArchetypeData(const char* file);
		void ReloadArchetypeData( void* info, const char* file );
		inline ArchetypeCollection* archetypes()
		{
			return _archetypes;
		}
		//
	private:
		std::vector<ArchFileInfo*> archfiles;
		std::vector<XMLReader*> readers;
		std::vector<ISerializer*> streams;
		ArchetypeCollection* _archetypes;
	};
	//
	extern GameData* GAMEDATA;
}
//
#endif