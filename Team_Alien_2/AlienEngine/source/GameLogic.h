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
#include <iostream>
//
namespace Framework
{
	class GameLogic : public ISystem
	{
	public:	
		GameLogic();
		~GameLogic();
		void Initialize();
		void Update(float timeslice);
		virtual std::string GetName(){return "GameLogic";}
		virtual void SendMessage(Message *);
		Vec2 WorldMousePosition;
	private:
		void loadGameSettings(const std::string& file);
		bool _paused;
	
	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern GameLogic* LOGIC;
}


