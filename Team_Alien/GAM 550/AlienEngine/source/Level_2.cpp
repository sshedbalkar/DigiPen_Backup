#include "Precompiled.h"
#include "Level_2.h"
#include "Core.h"
#include "DebugTools.h"
#include "GSM.h"
#include "Factory.h"
#include "GameLogic.h"
#include "cameraclass.h"
#include "Hero.h"
#include "ActivationProperties.h"
#include "TimeMechanicManager.h"
#include "ModelComponent.h"

//
namespace Framework
{
	//LoadLevelClass* ll;					//#### added temporary line
	Level_2::Level_2()
	{
		//
	}
	Level_2::~Level_2()
	{
		delete m_loader;						//#### added temporary line
	}
	int Level_2::load(void)
	{
		std::cout<<"Level_2: Load\n";
		//CORE->gsm()->deletePrevLevel();
		m_loader = new LoadLevelClass();		//#### added temporary line
		m_loader->Initialize();
		return 1;
	}
	int Level_2::initialize(void)
	{
		
		std::cout<<"Level_2: init\n";
		m_loadingEffectTimer=0.0f;
		m_loadingEffectDone=false;
		//
		m_loader->LoadLevelFile( LEVEL_2_FILE );

		GRAPHICS->GetCamera()->Setlookat( rotateVectorQuaternion( GRAPHICS->GetCameraLookAt(), HERO->GetHero()->has(Transform)->QUATERNION ) );
		TIMEMANAGER->BackDisabled=true;
		return 1;
	}
	int Level_2::update(float dt)
	{
		if(!m_loadingEffectDone){
			m_loadingEffectTimer+=dt;
			if (m_loadingEffectTimer>1.0f){
				m_loadingEffectDone=true;
			}
		}

		

		CORE->updateSystems(dt);

		
		Transform * m_transform= HERO->GetHero()->has(Transform);
		float posX,posZ,posY;
		posX=m_transform->Position.x;
		posZ=m_transform->Position.z;
		posY=m_transform->Position.y;
		int tutorialBoxId=-1;
		for (unsigned int i=0;i<TurorialBoxes.size();i++)
		{
			GOC *goc=TurorialBoxes[i];
			float minX,maxX,minZ,maxZ,minY,maxY;
			minX=goc->has(Transform)->Position.x-goc->has(Transform)->Scale.x*0.5f;
			maxX=goc->has(Transform)->Position.x+goc->has(Transform)->Scale.x*0.5f;
			minZ=goc->has(Transform)->Position.z-goc->has(Transform)->Scale.z*0.5f;
			maxZ=goc->has(Transform)->Position.z+goc->has(Transform)->Scale.z*0.5f;
			minY=goc->has(Transform)->Position.y-goc->has(Transform)->Scale.y*0.5f;
			maxY=goc->has(Transform)->Position.y+goc->has(Transform)->Scale.y*0.5f;
			if(posX>minX && posX<maxX && posZ>minZ && posZ<maxZ && posY>minY && posY<maxY)
			{
				tutorialBoxId = goc->LevelBoxId;
				break;
			}
		}

		

		
		for(int i=1;i<9;++i)
		{
			LOGIC->ActivatableObjectsList[i]->has(ActivationProperties)->MarkAsActivated(false);
			
		}
		if (tutorialBoxId==1 && TIMEMANAGER->GetTimeState()==TIME_NORMAL)
		{
			LOGIC->ActivatableObjectsList[1]->has(ActivationProperties)->MarkAsActivated(true);
		}
		if (tutorialBoxId==1 && TIMEMANAGER->GetTimeState()==TIME_PAUSED)
		{
			LOGIC->ActivatableObjectsList[2]->has(ActivationProperties)->MarkAsActivated(true);
		}
		if (tutorialBoxId==2 && LOGIC->ActivatableObjectsList[15]->has(ActivationProperties)->IsActivated())
		{
			LOGIC->ActivatableObjectsList[3]->has(ActivationProperties)->MarkAsActivated(true);
		}

		if (tutorialBoxId==3 || tutorialBoxId==4)
		{
			LOGIC->ActivatableObjectsList[4]->has(ActivationProperties)->MarkAsActivated(true);
		}
		if (tutorialBoxId>4)
		{
			TIMEMANAGER->BackDisabled=false;
		}
		if (tutorialBoxId==6 )
		{
			//LOGIC->ActivatableObjectsList[6]->has(ActivationProperties)->MarkAsActivated(true);
			for (unsigned int i=0;i<specialCylinders.size();++i)
			{
				specialCylinders[i]->has(ModelComponent)->ChangeToTexture(specialCylinders[i]->has(ModelComponent)->InitialSwapTextureName);
			}

		}
		else
		{
			for (unsigned int i=0;i<specialCylinders.size();++i)
			{
				specialCylinders[i]->has(ModelComponent)->ChangeToTexture(specialCylinders[i]->has(ModelComponent)->InitialTextureName);
			}
		}




		return 1;
	}
	int Level_2::draw(void)
	{
		return 1;
	}
	int Level_2::free(void)
	{
		std::cout<<"Level 2: Free\n";
		m_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		GRAPHICS->Unload();
		return 1;
	}
	int Level_2::unload(void)
	{
		std::cout<<"Level 2: Unload\n";
		return 1;
	}
	int Level_2::id(void)
	{
		return 2;
	}
	int Level_2::restart(void)
	{
		std::cout<<"Level 2: Restart\n";
		CORE->gsm()->next(GSM::LEVEL_RESTART);
		return 1;
	}
}