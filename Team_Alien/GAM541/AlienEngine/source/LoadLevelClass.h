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

#include "Physics.h"
#include "Engine.h"
#include "GameData.h"
#include "VMath.h"
#include "Rays.h"
//
namespace Framework
{

	
	class LoadLevelClass
	{
	public:	
		LoadLevelClass();
		~LoadLevelClass();
		void Initialize();
		void loadGameSettings(const std::string& file);
		void LoadLevelFile(const std::string& file);
		GOC* CreateObjectAt(const char* archId, Vec2& position, float rotation, Vec3& scale,Vec2& bodyScale);
		GOC* CreateObjectAt(const char* archId, Vec2& position, float rotation, Vec3& scale);
		GOC* CreateObjectAt(const char* archId, Vec2& position, float rotation);
		GOC* CreateObjectAt(const char* archId, Vec2& position);
		void freeLogicData();
	private:
		GameData* _gameData;

	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern LoadLevelClass* LevelClass;
}


