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
#include "Rays.h"
#include "ActivateObject.h"
#include "CameraControl.h"
#include "CheckPoints.h"
#include "InteractiveObjProperties.h"


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
		CheckPoints * CheckPointsControl;
		ActivateObject *ActivateObj;
	private:
		Rays *Ray;
		CameraControl *CamControl;
		void loadGameSettings(const std::string& file);
		bool _paused;
		bool mute;
	
	//**script engine**//
	private:
		typedef ObjectLinkList<ScriptManager> ScriptManagerListType;
	private:
		ScriptRegistrator* script_registrator;
		ScriptManagerListType script_managers;
	private:
		void InitializeScriptEngine();
		void UpdateScriptEngine( float timeslice );
		void SendMessageScriptEngine( Message* msg );
		void FreeScriptEngine();
	public:
		ScriptRegistrator* GetScriptRegistrator() { return script_registrator;}
		void AddScriptManager( ScriptManager* manager );
		void RemoveScriptManager( ScriptManager* manager );

	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern GameLogic* LOGIC;
}


