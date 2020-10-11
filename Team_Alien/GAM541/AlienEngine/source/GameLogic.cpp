///////////////////////////////////////////////////////////////////////////////////////
//
//	Game Logic
//	Simple game logic for demo. A good way to approach understand the game sample
//	is to walk through all the code in game logic and follow the execution through
//	the rest of the engine.
//	
//	Authors: , 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "GameLogic.h"
#include "Physics.h"
#include "Graphics.h"
#include "Audio.h"
#include "WindowsSystem.h"
#include "Core.h"
#include "Utility.h"
#include <ctime>
#include "particleEmitter.h"
#include "HeroProperties.h"
#include "AI_Type1.h"
#include "AI_Type2.h"
#include "LoadLevelClass.h"
#include "Level_1.h"
#include "debugGeometry.h"
#include "GSM.h"
#include "ComponentRegistration.h"
#include "ILevel.h"
#include "Level_2.h"

#include "scriptManager.h"
#include "scriptRegistrator.h"

namespace Framework
{
	//Our global pointer to the game.
	GameLogic* LOGIC = NULL;
	ILevel* l1;						//#### added temporary line


	void GameLogic::Initialize()
	{
		ComponentRegistration();
		//LoadLevelClass * ll= new LoadLevelClass();
		//ll->Initialize ();
		//Level_1* l1 = new Level_1();
		l1 = new Level_1();					//#### added temporary line
		Ray= new Rays();
		CamControl= new CameraControl();
		CheckPointsControl = new CheckPoints();
		ActivateObj= new ActivateObject ();
		CORE->gsm()->initialize(l1);
		CamControl->Initialize();
		CheckPointsControl->Initialize ();
		InitializeScriptEngine();
	}

	GameLogic::GameLogic():
		_paused(true)
	{
		ErrorIf(LOGIC!=NULL,"Logic already initialized");
		HERO = 0;
		CAMERA= 0;
		FINALBOSS= 0;
		LOGIC = this;
		GameOverPlatform=0;
		m_GodMode=false;
		mute=false;
	}

	GameLogic::~GameLogic()
	{
		FreeScriptEngine();
		delete ActivateObj;				//#### added temporary line
		delete Ray;
		delete CamControl;
		delete CheckPointsControl;
		delete l1;							//#### added temporary line
	}

	static float volume=0.0f; //temp code remove later
	//static bool playsounds=true; //temp code remove later


	void GameLogic::SendMessage(Message * m )
	{
		SendMessageScriptEngine( m );
		switch( m->MessageId )
		{
		// The user has pressed a (letter/number) key, we may respond by creating
		// a specific object based on the key pressed.

		case Mid::CharacterKey:
			{
				// Cast to the derived message type
				MessageCharacterKey * key = (MessageCharacterKey*)m;
				if(key->character == 'l')	// DIE
				{
					//CheckPointsControl->Load ();
				}
				else if(key->character == 'y')	// audio
				{
					/*playsounds=!playsounds;
					if(playsounds)
						AUDIO->resumeAll();
					else
						AUDIO->pauseAll();*/
				}
				else if(key->character == 'm')	// level restart
				{
					mute=(!mute);
					if(mute)
						AUDIO->changeVolume(0.0);
					else
						AUDIO->changeVolume(1.0);
				}
				break;
			}
		case Mid::CharacterKeyRelease:
			{
				break;
			}
		case Mid::VirtualKey:
			{
				// Cast to the derived message type
				//MessageVirtualKey * key = (MessageVirtualKey*)m;
				//if(key->character == VK_F1)	// Tobggle Frame pause or enter frame by frame mode
				//{
				//	ToggleFrameByFrameMode fbf;
				//	CORE->BroadcastMessage(&fbf);
				//}
				//else if(key->character == VK_F3)	// Advance the frame in the frame by frame mode
				//{
				//	AdvanceFrame af(1);
				//	CORE->BroadcastMessage(&af);
				//}
				//else if(key->character == VK_F5)	// cheat mode
				//{
				//	if(m_GodMode){
				//		m_GodMode=false;
				//		HERO->has(HeroProperties)->SetHeroSpeed (5.0f);
				//		GRAPHICS->MAX_CAMERA_MOVING_SPEED=10.0f;
				//	}
				//	else{
				//		m_GodMode=true;
				//		HERO->has(HeroProperties)->SetHeroSpeed (20.0f);
				//		GRAPHICS->MAX_CAMERA_MOVING_SPEED=1000.0f;
				//	}
				//}
				//else if(key->character == VK_F6)	
				//{
				//	HERO->has(Body)->Position.x=267.0f;
				//	HERO->has(Body)->Position.y=-6.0f;
				//
				//}
				//else if(key->character == VK_F7)	
				//{
				//	HERO->has(Body)->Position.x=364.0f;
				//	HERO->has(Body)->Position.y=-2.0f;
				//
				//}
				//else if(key->character == VK_F8)	
				//{
				//	HERO->has(Body)->Position.x=403.0f;
				//	HERO->has(Body)->Position.y=-21.0f;
				//
				//}
				//else if(key->character == VK_F9)	
				//{
				//	HERO->has(Body)->Position.x=458.0f;
				//	HERO->has(Body)->Position.y=2.0f;
				//
				//}
				break;

				
			}
		case Mid::MouseMove:
			{
				//The mouse has moved, update the mouse's world position
				MouseMove * mouseMove = (MouseMove*)m;
				GRAPHICS->ScreenToWorldSpace(&WorldMousePosition,&mouseMove->MousePosition);
				break;
			}
		case Mid::MouseWheel :
			{
				//zoom in/out
				/*MouseWheel * mouseWheel = (MouseWheel*)m;
				if (mouseWheel->Delta>0)
					volume+=0.05f;
				else if (mouseWheel->Delta<0)
					volume-=0.05f;
				if(volume>1.0) volume=1.0;
				if(volume<0.0) volume=0.0;
				AUDIO->changeVolume("background",volume);*/
				break;
			}
		case Mid::RMouseButton:
			{
				RMouseButton * rmouse = (RMouseButton*)m;
				if(rmouse->ButtonIsPressed)
				{
					GRAPHICS->ScreenToWorldSpace(&WorldMousePosition,&rmouse->MousePosition);
					Ray->SetAllowDrawRays(true);
					if(!_paused) AUDIO->Play("laser");
				}
				else
				{
					Ray->SetAllowDrawRays(false);
					Ray->DestroyRays();
					AUDIO->Stop("laser");
				}
				break;
			}
		case Mid::LMouseButton:
			{
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed && !HERO->has(HeroProperties)->GetHeroIsMoving () && Ray->IsAllowedDrawRays() && Ray->IsRayTouchingTeleportable())
				{
					Ray->StartTeleportation(true);	
					AUDIO->Play("teleport");
				}
				else
				{
					Ray->StartTeleportation(false);	
					HERO->has(ParticleEmitter)->GetEmitter("teleporting_effect")->Activate(false);
					AUDIO->Stop("teleport");
				}
				break;
			}
		case Mid::SystemPause:
			{
				const SystemPause *msg = static_cast<const SystemPause*>(m);
				_paused = msg->Pause;
				break;
			}
		}
	}
	
	void GameLogic::Update(float dt)
	{
		
		if(!_paused)
		{	
			Vec2 temp=Vec2(WINDOWSSYSTEM->MousePosition.x,WINDOWSSYSTEM->MousePosition.y);
			GRAPHICS->ScreenToWorldSpace(&WorldMousePosition,&temp);
			//std::cout<<WorldMousePosition.x << " " <<WorldMousePosition.y<<endl;
			UpdateScriptEngine( dt );

			std::list<GOC*>::iterator  it;
			
			if (m_GodMode){
				if(HERO->has(Body)->Velocity.y<0)
					HERO->has(Body)->Velocity.y=0;
			}
	
			ActivateObj->Update(dt);
	
			for(it=ListOfEnemies.begin();it!=ListOfEnemies.end();++it)
			{
				(*it)->has(AI_Type1)->Update(dt);
			}
			if (FINALBOSS)
			{
				FINALBOSS->has(AI_Type2)->Update(dt);
			}
			
			CamControl->Update(dt);
			HERO->has(HeroProperties)->Update(dt);	
			Ray->Update(HERO->has(Body)->Position,WorldMousePosition,dt);
			
		}
	}

//------------------------------------------------
// ScriptEngine
//------------------------------------------------
	void GameLogic::InitializeScriptEngine()
	{
		script_registrator = new ScriptRegistrator;
		script_registrator->Initialize();
	}
	void GameLogic::FreeScriptEngine()
	{
		script_registrator->Free();
		delete script_registrator;
	}
	void GameLogic::UpdateScriptEngine( float timeslice )
	{
		ScriptManagerListType::iterator it;
		for ( it = script_managers.begin(); it != script_managers.end(); ++it )
		{
			it->Update( timeslice );
		}
	}
	void GameLogic::SendMessageScriptEngine( Message* msg )
	{
		ScriptManagerListType::iterator it;
		for ( it = script_managers.begin(); it != script_managers.end(); ++it )
		{
			it->SendMessage( msg );
		}
	}
	void GameLogic::AddScriptManager( ScriptManager* manager )
	{ script_managers.push_back( manager ); }
	void GameLogic::RemoveScriptManager( ScriptManager* manager )
	{ script_managers.erase( manager ); }



	
}
