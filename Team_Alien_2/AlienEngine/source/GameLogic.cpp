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


namespace Framework
{
	//Our global pointer to the game.
	GameLogic* LOGIC = NULL;
	ILevel* l1;						//#### added temporary line



	class Entity
{
    public:

        Entity(){}
        ~Entity(){}

        void setSpeed(double adSpeed){m_dSpeed = adSpeed;}
        void setPosition(double adPosition){m_dPosition = adPosition;}

        double getSpeed(){return m_dSpeed;}
        double getPosition(){return m_dPosition;}
		 double m_dSpeed;
		  double m_dPosition;
    private:

       
       

};
	
	void GameLogic::Initialize()
	{
		l1 = new Level_1();					//#### added temporary line
		CORE->gsm()->initialize(l1);

		//========
		int x=777;
		GOC * newObject = FACTORY->CreateEmptyComposition();
		Transform *debug=new Transform();
		newObject->AddComponent(CT_Transform,debug);
		newObject->has(Transform)->Position.x=0.0;
		////Get the transform and adjust the position
		//Transform * transform = newObject->has(Transform);
		//
		//transform->Position = Vec3(position.x, position.y, 0.0f);
		//transform->RotationX = rotation;
		//transform->Scale = scale;

		//Body * body = newObject->has(Body);
		//if(body)
		//{
		//	body->Position=Vec2(position.x, position.y);
		//	if (body->BodyShape->Id==Shape::SidBox)
		//	{
		//		body->BodyScale=bodyScale;
		//		static_cast<ShapeAAB*>(body->BodyShape)->Extents=body->BodyScale;
		//		if(DrawDebug)
		//		{
		//			DebugGeometry *debug=new DebugGeometry();
		//			debug->AddDebugShape(DEBUG_SHAPE_RECTANGLE, Vec3(bodyScale.x*2,bodyScale.y*2,0), Vec4(1,0,0,1));
		//			newObject->AddComponent(CT_DebugGeometry,debug);
		//		}
		//	}
		//}


		//=======
		newObject->CompositionTypeId=666;
		lua_State *myLuaState = luaL_newstate();
		luaL_openlibs(myLuaState);


		
		Entity test =   Entity();
		test.m_dSpeed=3.14;
		
	
	   // lua_State *myLuaState = lua_open();
		// Connect LuaBind to this lua state
		luabind::open(myLuaState);
		// Export our class with LuaBind	
		luabind::module(myLuaState) [
			luabind::class_<Entity>("GOC")
				.def(luabind::constructor<>())
				 .property("Speed", &Entity::m_dSpeed , &Entity::m_dSpeed)
            //.property("Position", &Entity::m_dPosition, &Entity::m_dPosition)
		];
		{
		luabind::globals(myLuaState)["test"] = &test;
		luabind::object table = luabind::newtable(myLuaState);
		table[ "Instance1" ] = &test;
		//table[ "Instance2" ] = x;
		//table[ "Instance3" ] = test.m_dSpeed;
		//table[ "Instance3" ] = new MyClass( 2 );
		luabind::globals(myLuaState)["TableOfInstances"] = table;
		}
	
		//luabind::globals(myLuaState)["hoho"] = &test;

		cout<<test.m_dSpeed<<endl ;
		//

	   // table["Entity1"] = entity1;

		//How to pass entity object to lua

		luaL_loadfile(myLuaState, "LuaScripts\\test.lua");
		int i = lua_pcall(myLuaState, 0, 0, 0);
		cout<<test.m_dSpeed<<endl ;
		lua_close(myLuaState);
		

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
		switch( m->MessageId )
		{
		// The user has pressed a (letter/number) key, we may respond by creating
		// a specific object based on the key pressed.

		case Mid::CharacterKey:
			{
				
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
				//GRAPHICS->ScreenToWorldSpace(&WorldMousePosition,&mouseMove->MousePosition);
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
	
	void GameLogic::Update(float dt)
	{
		//printf("Hello World!\n");
	}

	
}
