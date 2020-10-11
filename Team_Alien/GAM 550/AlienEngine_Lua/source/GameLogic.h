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
#include "PowerGun.h"
#include <iostream>
#include <vector>
//
namespace Framework
{
	class GameLogic : public ISystem
	{
		friend class ObjectActivator;
		friend class LoadLevelClass;
	public:	
		GameLogic();
		~GameLogic();
		void Initialize();
		void Update(float timeslice);
		virtual std::string GetName(){return "GameLogic";}
		virtual void SendMessage(Message *);
		PowerGun m_PowerGun;

		GOC* objectPointedAt;
	private:
		enum m_powerState{power1,power2,power3,noOfPower};
		void loadGameSettings(const std::string& file);
		bool _paused;
		std::vector<GOC*> m_ActivatableObjectsList;
	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern GameLogic* LOGIC;
}


