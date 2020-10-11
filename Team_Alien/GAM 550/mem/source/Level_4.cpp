#include "Precompiled.h"
#include "Level_4.h"
#include "Core.h"
#include "DebugTools.h"
#include "GSM.h"
#include "Factory.h"
#include "GameLogic.h"
#include "ActivationProperties.h"
#include "LightComponent.h"
#include "VMath.h"
#include "WindowsSystem.h"

//
namespace Framework
{
	//LoadLevelClass* ll;					//#### added temporary line
	Level_4::Level_4()
	{
		//
	}
	Level_4::~Level_4()
	{
		delete m_loader;						//#### added temporary line
	}
	int Level_4::load(void)
	{
		std::cout<<"Level_4: Load\n";
		//CORE->gsm()->deletePrevLevel();
		m_loader = new LoadLevelClass();		//#### added temporary line
		m_loader->Initialize();
		return 1;
	}
	int Level_4::initialize(void)
	{
		std::cout<<"Level_4: init\n";
		m_loadingEffectTimer=0.0f;
		m_loadingEffectDone=false;
		//
		m_timer=0.0f;
		m_loader->LoadLevelFile( LEVEL_4_FILE );
		return 1;
	}
	int Level_4::update(float dt)
	{
		m_timer+=dt;
		if(!m_loadingEffectDone){
			m_loadingEffectTimer+=dt;
			if (m_loadingEffectTimer>1.0f){
				m_loadingEffectDone=true;
			}
		}
		

		CORE->updateSystems(dt);
		if(EDITOR && EDITOR->inEditor || LOGIC->IsPaused())
			return 1;



		for(unsigned int i=0;i<LOGIC->BulbList.size();++i)//update lights
		{
			GRAPHICS->DeferredLights[i].Color=LOGIC->BulbList[i]->has(LightComponent)->Color;
			GRAPHICS->DeferredLights[i].Falloff=LOGIC->BulbList[i]->has(LightComponent)->fallout;
			GRAPHICS->DeferredLights[i].Position=LOGIC->BulbList[i]->has(Transform)->Position;
		}
		GRAPHICS->CreateLightBuffer();

		//print("SIZE",sizeof(RigidBodyData));
		//print("LOGIC LIGHTS",LOGIC->BulbList.size());
		//print("GRAPHICS NOOF LIGHTS",GRAPHICS->GetNoOfLights());
		if(TIMEMANAGER->GetTimeState() == TIME_NORMAL)
		{
			if (!LOGIC->ActivatableObjectsList[2]->has(ActivationProperties)->IsActivated())
			{
				LOGIC->ActivatableObjectsList[4]->has(ActivationProperties)->MarkAsActivated(true);
				return 1;
			}
			else
			{
				LOGIC->ActivatableObjectsList[4]->has(ActivationProperties)->MarkAsActivated(false);
			}
				
			
			if( m_timer>0.02)
			{
				m_timer=0.0f;
				for(unsigned i=0;i<cubeThrower.size();++i)
				{
					if(  GRAPHICS->GetNoOfLights()<1001 )
					{
						GOC *g;
						g = FACTORY->BuildFromArchetype("GOC_Type_LightBulbWithMass");
						g->has(RigidBody)->setMass(100);
						g->has(Transform)->Scale=Vec3(0.5,0.5,0.5);
						static_cast<CollisionSphere*>(g->has(CollisionPrimitive))->radius=g->has(Transform)->Scale.x*0.5f;
						Transform * m_Transform=cubeThrower[i]->has(Transform);
						g->has(Transform)->Position=Vec3(m_Transform->Position );
						g->has(Transform)->Position.x = Rand(g->has(Transform)->Position.x-1.0f,g->has(Transform)->Position.x+1.0f);
						g->has(Transform)->Position.y = Rand(g->has(Transform)->Position.y-1.0f,g->has(Transform)->Position.y+1.0f);
						g->has(Transform)->Position.z = Rand(g->has(Transform)->Position.z-1.0f,g->has(Transform)->Position.z+1.0f);
				
				

						g->has(Transform)->QUATERNION=D3DXQUATERNION (0,0,0,1);


						Vector3 pos= m_Transform->Position;
						Quaternion q= m_Transform->QUATERNION;
						Matrix4 m;
						m.setOrientationAndPos(q,pos);
						Vector3 dir= m.transformDirection(Vector3(0,-1,0));

						g->has(RigidBody)->setVelocity(dir*35);
						if(g->has(LightComponent)  )
						{
							g->has(LightComponent)->Color.x = Rand(0,1);
							g->has(LightComponent)->Color.y = Rand(0,1);
							g->has(LightComponent)->Color.z = Rand(0,1);
							g->has(LightComponent)->fallout=Rand(2,4);
							LevelClass->CreateLight(g);
							//LOGIC->BulbList.push_back(g);
						}
						g->Initialize();
			
					}
				}
				
			}
		}
		

		return 1;
	}
	int Level_4::draw(void)
	{
		return 1;
	}
	int Level_4::free(void)
	{
		std::cout<<"Level 4: Free\n";
		m_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		GRAPHICS->Unload();
		return 1;
	}
	int Level_4::unload(void)
	{
		std::cout<<"Level 4: Unload\n";
		return 1;
	}
	int Level_4::id(void)
	{
		return 2;
	}
	int Level_4::restart(void)
	{
		std::cout<<"Level 4: Restart\n";
		CORE->gsm()->next(GSM::LEVEL_RESTART);
		return 1;
	}
}