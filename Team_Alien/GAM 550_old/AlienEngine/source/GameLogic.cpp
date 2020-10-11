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
#include "Audio.h"
#include "WindowsSystem.h"
#include "Core.h"
#include "Utility.h"
#include <ctime>
#include "LoadLevelClass.h"
#include "GSM.h"
#include "ComponentRegistration.h"
#include "ILevel.h"
#include "RigidBody.h"


namespace Framework
{
	//Our global pointer to the game.
	GameLogic* LOGIC = NULL;



	void GameLogic::Initialize()
	{
		
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

	}

	static float volume=0.0f; //temp code remove later


	void GameLogic::SendMessage(Message * m )
	{
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
		case Mid::VirtualKeyRelease:
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
				}else
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
	
	static bool test123=true;
	static bool test12=false;

	void GameLogic::Update(float dt)
	{
		
		if(test123)
		{
			{
				GOC * gameObject1 = FACTORY->BuildFromArchetype("DebugCube");
				Transform * transform2 = gameObject1->has(Transform);
				transform2->Position = Vec3(0,10,0);
				//transform2->rotation = D3DXQUATERNION(2,4,2,8);

				RigidBody *body2=gameObject1->has(RigidBody);
				
				//body2->setVelocity(0,0,0.1);
				body2->setRotation(Vector3(0,0,0));

				gameObject1->Initialize();
			}

			
			test123=false;
		}

		if(test12)
		{
				Ray ray(Vector3(10,0,0),Vector3(-1,0,0));
				Vector3 point;
				CollisionDetector detect;
				for(ObjectLinkList<RigidBody>::iterator it=PHYSICS->RigidBodies.begin();it!=PHYSICS->RigidBodies.end();++it)
				{
					if(it->GetOwner()->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Sphere)
					{
						if(detect.rayCastSphere(*(static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))),ray,point))
						{
							std::cout<<point;
						}
					}else if(it->GetOwner()->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Box)
					{
						if(detect.rayCastBox(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),ray,point))
						{
							std::cout<<point;
						}
					}


				}


		}
		test12=true;

		if(!_paused)
		{	
			Vec2 temp=Vec2(WINDOWSSYSTEM->MousePosition.x,WINDOWSSYSTEM->MousePosition.y);
		}
	}


	
}
