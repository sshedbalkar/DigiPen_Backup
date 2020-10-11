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
//#include "ActivateObject.h"
#include "InteractiveObjProperties.h"
#include "graphics.h"

//
namespace Framework
{
	class ScriptRegistrator;
	class ScriptManager;

	class MessageGameWon : public Message
	{
	public:
		MessageGameWon() : Message(Mid::GameWon){};
	};

	class MessageDying : public Message
	{
	public:
		MessageDying(int c) : Message(Mid::Dying),compositionType(c) {};
		int compositionType;
	};

	
	class GameLogic : public ISystem
	{
	public:	
		GameLogic();
		~GameLogic();
		GOC* HERO;
		GOC* CAMERA;
		GOC* FINALBOSS;
		GOC* GameOverPlatform;
		void Initialize();
		void Update(float timeslice);
		virtual std::string GetName(){return "GameLogic";}
		virtual void SendMessage(Message *);
		Vec2 WorldMousePosition;
		std::list<GOC*> ListOfButtons;
		std::list<GOC*> ListOfDoorsToBeActivated;
		std::list<GOC*> ListOfGOCsDying;
		std::list<GOC*> ListOfEnemies;
		std::list<GOC*> ListOfBoxes;
		bool m_GodMode;
//		CheckPoints * CheckPointsControl;
//		ActivateObject *ActivateObj;
		TwBar *myBar;
		int level;
	private:
//		Rays *Ray;
//		CameraControl *CamControl;
		bool _paused;
		bool mute;
	
	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern GameLogic* LOGIC;
}


