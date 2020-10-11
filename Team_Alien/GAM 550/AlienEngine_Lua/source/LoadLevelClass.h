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
#include "Graphics.h"
//
namespace Framework
{

	struct ObjectParameters
	{
		std::string Arch_id;
		Vec3 Position,Scale;
		D3DXQUATERNION Quaternion;
		int ObjectActivationId;
		Vec4 ActivatedBy;

		ObjectParameters()
		{
			Position = Vec3(0,0,0);
			Scale = Vec3(1,1,1);
			ObjectActivationId = 0;
			Quaternion = D3DXQUATERNION(0,0,0,1);
			ActivatedBy=Vec4(0,0,0,0);
		}
	};


	
	class LoadLevelClass
	{
	public:	
		LoadLevelClass();
		~LoadLevelClass();
		void Initialize();
		void loadGameSettings(const std::string& file);
		void LoadLevelFile(const std::string& file);
		GOC* CreateObjectAt(ObjectParameters parameters);
		void freeLogicData();
		void AddGOCtoLogicContainer(GOC*);
	private:
		GameData* _gameData;

	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern LoadLevelClass* LevelClass;
}


