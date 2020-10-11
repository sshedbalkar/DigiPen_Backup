#pragma once //Makes sure this header is only included once

#include "GameData.h"
//
namespace Framework
{

		
	class SaveLevelClass
	{
	public:	
		SaveLevelClass();
		~SaveLevelClass();
		void Initialize();
		void SaveLevel(const std::string& file);
	private:
		GameData* _gameData;

	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern SaveLevelClass* SaveClass;
}


