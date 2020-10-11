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
#include "LoadLevelClass.h"
#include "Level_1.h"
#include "debugGeometry.h"
#include "GSM.h"
#include "ILevel.h"
#include "Level_2.h"
#include "LevelEditor.h"
#include "ComponentCreator.h"
#include "ActivationProperties.h"

namespace Framework
{
	//Our global pointer to the game.
	GameLogic* LOGIC = NULL;
	ILevel* l1;						//#### added temporary line


	void GameLogic::Initialize()
	{
		RegisterComponent(ActivationProperties);
		l1 = new Level_1();					//#### added temporary line
		CORE->gsm()->initialize(l1);








	}

	GameLogic::GameLogic():
		_paused(true)
	{
		ErrorIf(LOGIC!=NULL,"Logic already initialized");
		LOGIC = this;
	}

	GameLogic::~GameLogic()
	{
		delete l1;							//#### added temporary line
	}

	static float volume=0.0f; //temp code remove later
	//static bool playsounds=true; //temp code remove later


	void GameLogic::SendMessage(Message * m )
	{
		if(EDITOR && EDITOR->inEditor)return;
		m_PowerGun.SendMessageA(m);
		switch( m->MessageId )
		{
		// The user has pressed a (letter/number) key, we may respond by creating
		// a specific object based on the key pressed.

		case Mid::CharacterKey:
			{
				MessageCharacterKey * key = (MessageCharacterKey*)m;
				if(key->character==VK_ESCAPE)
				{
					Message m(Mid::Quit);
					CORE->BroadcastMessage(&m);
				}else if(key->character=='`' && EDITOR)
				{
					EDITOR->enterEditMode();
					GRAPHICS->TurnOnDebugCamera();
				}
				break;
			}
		case Mid::CharacterKeyRelease:
			{
				break;
			}
		case Mid::VirtualKey:
			{
				
				break;

				
			}
		case Mid::MouseMove:
			{
				//The mouse has moved, update the mouse's world position
				MouseMove * mouseMove = (MouseMove*)m;
				break;
			}
		case Mid::MouseWheel :
			{
				break;
			}
		case Mid::RMouseButton:
			{
				RMouseButton * rmouse = (RMouseButton*)m;
				if(rmouse->ButtonIsPressed)
				{
					if (objectPointedAt)
					{
						/*Vector3 k,v;
						k.x=(objectPointedAt->has(Transform)->Scale.x /2.0f);
						k.y=(objectPointedAt->has(Transform)->Scale.y /2.0f);
						k.z=0.0f;

						v = objectPointedAt->has(RigidBody)->getTransform().transform(k);
						std::cout<<v.x<<", "<<v.y<<", "<<v.z<<std::endl;
		
						k.x=-(objectPointedAt->has(Transform)->Scale.x /2.0f);
						k.y=(objectPointedAt->has(Transform)->Scale.y /2.0f);
						k.z=0.0f;

						v = objectPointedAt->has(RigidBody)->getTransform().transform(k);
						std::cout<<v.x<<", "<<v.y<<", "<<v.z<<std::endl;

						k.x=0.0f;
						k.y=(objectPointedAt->has(Transform)->Scale.y /2.0f);
						k.z=(objectPointedAt->has(Transform)->Scale.z /2.0f);

						v = objectPointedAt->has(RigidBody)->getTransform().transform(k);
						std::cout<<v.x<<", "<<v.y<<", "<<v.z<<std::endl;

						k.x=0.0f;
						k.y=(objectPointedAt->has(Transform)->Scale.y /2.0f);
						k.z=-(objectPointedAt->has(Transform)->Scale.z /2.0f);

						v = objectPointedAt->has(RigidBody)->getTransform().transform(k);
						std::cout<<v.x<<", "<<v.y<<", "<<v.z<<std::endl;*/
					}
				}
				else
				{
					
				}
				break;
			}
		case Mid::LMouseButton:
			{
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed)
				{
				}
				else
				{
					
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
	

	static bool temp123 = false;
	void GameLogic::Update(float dt)
	{

		if(EDITOR && EDITOR->inEditor)return;
		m_PowerGun.Update();
		/*if(objectPointedAt)
			std::cout<<objectPointedAt->archId.c_str()<<std::endl;*/
		if(temp123)
		{
			/*GOC *g;
			Vec3 tt= Vec3(-89,-16.5,-115);
			float offsetx=0.0f;
			for (int i=0;i<9;i++)
			{
				for(int j=0;j<5;++j)
				{
					g=FACTORY->BuildFromArchetype("GOC_Type_CUBE");
					g->has(Transform)->Position=tt;
					g->has(Transform)->Scale=Vec3(20,20,10);
					static_cast<CollisionBox*>(g->has(CollisionPrimitive))->halfSize=Vector3(10,10,5);;
					tt.y+=20;
					g->has(RigidBody)->setAwake(false);
					g->Initialize();
				}
					tt.x+=20;
					tt.y=-16.5;
			}*/
			temp123=false;
		}
		
	}

	
}
