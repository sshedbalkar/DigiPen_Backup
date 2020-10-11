///////////////////////////////////////////////////////////////////////////////////////
///
///	\file GameLogic.h
///	Sample game logic for demo.
///	
///	Authors: , 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once //Makes sure this header is only included once

#include "Engine.h"
#include "GameData.h"
#include "VMath.h"
//
namespace Framework
{

	
	class LoadLevelClass
	{
	public:	
		LoadLevelClass();
		~LoadLevelClass();
		void Initialize();
		void loadGameSettings();
		bool LoadLevelFile(const std::string& file);
		GOC* CreateObjectAt(const char* archId, float* position );
		void freeLogicData();
		bool SaveLevelLocal( const char *fileName );
	private:
		GameData* _gameData;

	};

}


