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
#include "Level_2.h"
#include "Level_3.h"
#include "Level_4.h"
#include "Level_5.h"
#include "Level_6.h"
#include "LevelEditor.h"
#include "LightComponent.h"
#include "ReturnCodes.h"
#include "RopeComponent.h"
#include "MsgRecvrComponent.h"
#include "MainMenu.h"
#include "particlesystemclass.h"
#include "TestClass1.h"
#include "Checkbox.h"
#include "ActivationProperties.h"
#include "RadioButtonGroup.h"
//
std::vector<Framework::MsgRecvrComponent*> l_rcvrsToDelete;
std::vector<Framework::MsgRecvrComponent*> l_rcvrsToAdd;
//
namespace Framework
{
	//Our global pointer to the game.
	GameLogic* LOGIC = NULL;
						//#### added temporary line


	void GameLogic::Initialize()
	{
		m_componentManager.RegisterComponents();
		//
		level = new Level_1();					//#### added temporary line
		CORE->gsm()->initialize(level);
		Pause(true);
		
		objectPointedAt=0;
		objectPointedAtMagnet=0;
		testObj=0;
		testObj1=0;
		temp123=true;
		//g_menu->CreateMenuFrom("Objects\\Menu.xml" );
		//g_menu->ShowFrame( &GameLogic::MenuCallback, this, "F1" );
		//
		CORE->AddFirstFrameCallback( &GameLogic::OnFirstFrame, this, 0 );
		//
		//Memory::TestClass1::AcquireAllocator();
		//Memory::TestClass1* tc1 = new Memory::TestClass1();
		//tc1 = new Memory::TestClass1();
		//delete tc1;
	}
	/*
	int choice = 3;
	void GameLogic::MenuCallback( const char* fid, const std::vector<std::string>* option )
	{
		--choice;
		if( choice == 2 )
		{
			g_menu->ShowFrame( &GameLogic::MenuCallback, this, "F2" );
		}
		else if( choice == 1 )
		{
			g_menu->ShowFrame( &GameLogic::MenuCallback, this, "F3" );
		}
	}
	*/
	GameLogic::GameLogic():
		_paused(true),
		m_levelId( 0 ),
		m_msgLock( 0 )
	{
		ErrorIf(LOGIC!=NULL,"Logic already initialized");
		LOGIC = this;
		m_saver=new SaveLevelClass ;
	}

	GameLogic::~GameLogic()
	{
		//delete level;							//#### added temporary line
		delete m_saver;
	}

	static float volume=0.0f; //temp code remove later
	//static bool playsounds=true; //temp code remove later
	int GameLogic::ResetLevel()
	{
		Pause(true);

		return LoadLevel( m_levelId );
	}
	//
	void GameLogic::OnFirstFrame( void* data )
	{
		/*
		LOGIC->Pause( true );
		GRAPHICS->TurnOnDebugCamera();
		Checkbox* cb = new Checkbox( 1 );
		cb->Init( "Assets\\textures\\cb2.png", "Assets\\textures\\cb1.png", "Assets\\textures\\aa_1.png", "Assets\\textures\\aa_2.png", "Assets\\textures\\aa_3.png", 100, 100 );
		//
		const char* imgs[] =
		{
			"Assets\\textures\\aa_1.png", "Assets\\textures\\aa_2.png", "Assets\\textures\\aa_3.png", 
			"Assets\\textures\\aa_1.png", "Assets\\textures\\aa_2.png", "Assets\\textures\\aa_3.png", 
			"Assets\\textures\\aa_1.png", "Assets\\textures\\aa_2.png", "Assets\\textures\\aa_3.png"
		};
		RadioButtonGroup* rb = new RadioButtonGroup( 3 );
		rb->Init( "Assets\\textures\\rb_1.png", "Assets\\textures\\rb_0.png", imgs, 100, 200 );
		*/
		//if(!EDITOR)
			//g_mainMenu->Show( (int)GRAPHICS->screen_width*0.5f, (int)GRAPHICS->screen_height*0.5f );
		
	}
	//
	int GameLogic::LoadLevel( unsigned id )
	{
		m_levelId = id%6;
		switch( m_levelId )
		{
		case 0: { level = new Level_1(); } break;
		case 1: { level = new Level_2(); } break;
		case 2: { level = new Level_3(); } break;
		case 3: { level = new Level_4(); } break;
		case 4: { level = new Level_5(); } break;
		case 5: { level = new Level_6(); } break;
		}
		//
		CORE->gsm()->next( level );
		//
		return RET_SUCCESS;
	}
	//
	int GameLogic::LoadNextLevel()
	{
		return LoadLevel( m_levelId + 1 );
	}
	//
	int GameLogic::LoadPrevLevel()
	{
		return LoadLevel( m_levelId - 1 );
	}
	//
	int GameLogic::AddMsgRecvr( MsgRecvrComponent* recvr )
	{
		l_rcvrsToAdd.push_back( recvr );
		return RET_SUCCESS;
	}
	//
	int GameLogic::RemoveMsgRecvr( MsgRecvrComponent* recvr )
	{
		l_rcvrsToDelete.push_back( recvr );
		return RET_SUCCESS;
	}
	//
	void GameLogic::SendMessage(Message* m )
	{
		if( m_msgLock == 0 )
		{
			for each( MsgRecvrComponent* recvr in l_rcvrsToAdd )
			{
				m_msgRecvrs.push_back( recvr );
			}
			l_rcvrsToAdd.clear();
		}
		//
		for each( MsgRecvrComponent* recvr in m_msgRecvrs )
		{
			if( !recvr->Deleted() )
			{
				++m_msgLock;
				recvr->SendMessageA( m );
				--m_msgLock;
			}
		}
		//
		if( m_msgLock == 0 )
		{
			for each( MsgRecvrComponent* recvr in l_rcvrsToDelete )
			{
				for( std::vector<MsgRecvrComponent*>::iterator it1 = m_msgRecvrs.begin(), it2 = m_msgRecvrs.end(); it1 != it2; ++it1 )
				{
					if( *it1 == recvr )
					{
						m_msgRecvrs.erase( it1 );
						break;
					}
				}
			}
			l_rcvrsToDelete.clear();
		}
		//
		if(EDITOR && EDITOR->inEditor)return;
		m_Hero.SendMessageA(m);
		m_PowerGun.SendMessageA(m);
		m_timeMechanism.SendMessageA(m);
		//
		switch( m->MessageId )
		{
		// The user has pressed a (letter/number) key, we may respond by creating
		// a specific object based on the key pressed.
		case Mid::Collide :
			{
				m_logicCollisions.SendMessageA(m);
				break;
			}
		case Mid::CharacterKey:
			{
				MessageCharacterKey* key = (MessageCharacterKey*)m;
				if(key->character==VK_ESCAPE)
				{
					/*
					if(EDITOR)
					{
						Message m(Mid::Quit);
						CORE->BroadcastMessage(&m);
					}
					else
					{
						g_mainMenu->Show( int(GRAPHICS->screen_width*0.5f), int(GRAPHICS->screen_height*0.5f) );
					}
					*/
					if( GRAPHICS->IsStartGame() )
					{
						GRAPHICS->SetStopGame();
						GRAPHICS->Pause();
						g_mainMenu->Show( int(GRAPHICS->screen_width*0.5f), int(GRAPHICS->screen_height*0.5f) );
					}
					
				}else if(key->character=='`' && EDITOR)
				{
					PHYSICS->SetDebugDraw(false);
					GRAPHICS->SetDebugDraw(false);
					_paused=true;
					m_Hero.ShowPositionInEditor();
					EDITOR->enterEditMode();
					GRAPHICS->TurnOnDebugCamera();
				}
				/*else if(key->character=='n')
				{
					
					LoadNextLevel();
				}*/
				/*else if(key->character=='r')
				{
					GRAPHICS->GetParticleSystemClass()->ClearAllParticles();
					ResetLevel();
				}
			
				
				else if(key->character=='b')
				{
					
					LoadPrevLevel();
				}else if(key->character=='i')
				{
					for(unsigned int i=0;i<GRAPHICS->DeferredLights.size();++i)
					{
						Light  & light=GRAPHICS->DeferredLights[i];
						light.Color.x = Rand(0,1);
						light.Color.y = Rand(0,1);
						light.Color.z = Rand(0,1);
						light.Falloff+=0.1f;
					}
					GRAPHICS->CreateLightBuffer();
				}else if(key->character=='z')
				{
					if(objectPointedAt->has(ComplexObject))
					{
						objectPointedAt->has(ComplexObject)->DestroyPointedObject();
					}
				}*/
				
				else if (key->character==VK_RETURN)
				{
					Pause(false);
				}
				break;
			}
		case Mid::CharacterKeyRelease:
			{
				MessageCharacterKey* key = (MessageCharacterKey*)m;
				//
			/*	if( key->character == VK_F12 )
				{
					ToggleTimeDisplay td;
					CORE->BroadcastMessage( &td );
				}*/
				//if( key->character == VK_F11 )
				//{
				//	bool t=PHYSICS->GetIsDebugMode();
				//	//GRAPHICS->SetDebugDraw(!t);
				//	PHYSICS->SetDebugDraw(!t);
				//}
				//if( key->character == VK_F2 )
				//{
				//	ToggleFrameByFrameMode msg;
				//	CORE->BroadcastMessage( &msg );
				//}
				//if( key->character == VK_F3 )
				//{
				//	AdvanceFrame msg( 1 );
				//	CORE->BroadcastMessage( &msg );
				//}
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
	void GameLogic::Pause( bool val )
	{
		_paused = val;
		PHYSICS->Pause(val);
	}



	void GameLogic::Update(float dt)
	{
		/*std::bitset<16> bitset;
		bitset[4]=1;
		print (sizeof(bitset));
		print (bitset.to_ulong());*/
		if(_paused) return;
		if(EDITOR && EDITOR->inEditor)return;
		
		m_componentManager.Update(dt);


		if(temp123)
		{		
			Vector3 pos1(-7.1,4.02,36.67),pos2(-4.5,3.69,33.83);
			Vector3 velevel=  pos2-pos1;
			velevel.normalise();

			{
				//testObj=FACTORY->BuildFromArchetype("GOC_Type_CylinderTest");
				//testObj->has(Transform)->Position=Vec3(-7.4,2,36.67);
				//testObj->has(Transform)->Scale = Vec3(1.5,4,1.5);
				//static_cast<CollisionCylinder*>(testObj->has(CollisionPrimitive))->radius=0.75;
				//static_cast<CollisionCylinder*>(testObj->has(CollisionPrimitive))->height=4.0;
				//testObj->Initialize();
			}
			temp123=false;
		}

	//	//debug to show constraints remove later
		if(!temp123)
		{
			std::list<Joint>::iterator joint= PHYSICS->joints.begin();
			Vec4 color(1,0,0,1);
			unsigned int c=0;
			for(;joint!=PHYSICS->joints.end();++joint,c++)
			{
				////GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(joint->worldPosition[0]), color) );
				////GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(joint->worldPosition[1]), color) );	
				if(c>RopePartsList.size()-1) continue;
				GOC * temp=RopePartsList[c];
				//	rope connecotr
				Vector3 pos= (joint->worldPosition[0] + joint->worldPosition[1])*0.5;
				temp->has(Transform)->Position=Vector3ToVec3(pos);
				Vector3 size=joint->worldPosition[0] - joint->worldPosition[1];
				float mag=(float)size.magnitude();
				temp->has(Transform)->Scale=Vec3(0.05f,mag,0.05f);

				Vector3 v=size;
				Vector3 axis= Vector3(0,1,0)%size;
				axis.normalise();
				size.normalise();
				real angle=acos(size.y);

				temp->has(Transform)->QUATERNION=calculateQuaterion((float)angle,Vector3ToVec3(axis));

			}
			color=Vec4(0,0,1,1);
			std::list<Constraint>::iterator constraint= PHYSICS->constraints.begin();
			int i=0;
			for(;constraint!=PHYSICS->constraints.end();++constraint)
			{
				//GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(constraint->bodyWorldPosition), color) );
				//GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(constraint->worldPosition), color) );		
				//rope connecotr
				if(constraint->error > 2)
				{
					Vector3 pos= (constraint->body->getPosition()  + constraint->worldPosition)*0.5;
					RolesList[i]->has(Transform)->Position=Vector3ToVec3(pos);
					Vector3 size=constraint->body->getPosition() - constraint->worldPosition;
					float mag = (float)size.magnitude();
					RolesList[i]->has(Transform)->Scale=Vec3(0.1f,mag,0.1f);

					Vector3 v=size;
					Vector3 axis= Vector3(0,1,0)%size;
					axis.normalise();
					size.normalise();
					real angle=acos(size.y);

					RolesList[i]->has(Transform)->QUATERNION=calculateQuaterion((float)angle,Vector3ToVec3(axis));
					i++;
				}
			}

			{//laser sound code
				float minLaserDist = FLT_MAX;
				for(int i=0;i<LaserObjectsList.size();++i)
				{
					Vector3 pos = LaserObjectsList[i]->has(Transform)->Position;
					Vector3 a = HERO->heroPos;
					a-=pos;
					if(minLaserDist > a.magnitude())
						minLaserDist= a.magnitude();
				}
				if(minLaserDist<15)
				{
				minLaserDist = minLaserDist/15.0;
				minLaserDist*=0.07;
				AUDIO->changeVolume("laser",abs(0.07-minLaserDist));
				}
				else
					AUDIO->changeVolume("laser",0.0);
			}


			{//lava sound code
				float minLaserDist = FLT_MAX;
				int mb =0;
				for(int i=0;i<LavaObjectsList.size();++i)
				{
					Vector3 pos = LavaObjectsList[i]->has(Transform)->Position;
					Vector3 a = HERO->heroPos;
					Vector3 dis123 = pos- a;

					if(HERO->GetLevelBoxId() != 3 && HERO->GetLevelBoxId() != 4)
						continue;

					float asd = abs(pos.x - a.x) - LavaObjectsList[i]->has(Transform)->Scale.x/2.0;
					asd = abs(asd);
					if(minLaserDist>asd)
						minLaserDist= asd;
					
					asd = abs(pos.y - a.y) - LavaObjectsList[i]->has(Transform)->Scale.y/2.0;
					asd = abs(asd);
					if(minLaserDist>asd)
						minLaserDist= asd;
					
					asd = abs(pos.z - a.z) - LavaObjectsList[i]->has(Transform)->Scale.z/2.0;
					asd = abs(asd);
					if(minLaserDist>asd)
						minLaserDist= asd;
				}
				if(minLaserDist<8)
				{
					minLaserDist = minLaserDist/8.0;
					minLaserDist*=0.1;
					AUDIO->changeVolume("lava",abs(0.1-minLaserDist));
				}
				else
					AUDIO->changeVolume("lava",0.0);
			}


			{//door sound code
				AUDIO->Pause("door");
				if(TIMEMANAGER->GetTimeState()==TIME_NORMAL)
				{
					for(int i=0;i<DoorList.size();++i)
					{
						if(DoorList[i]->has(ActivationProperties)->doormoving)
						{
							AUDIO->resume("door");
							break;
						}
					}
				}
				
			}
		}

			
		//Dont remove this line
}
	
}
