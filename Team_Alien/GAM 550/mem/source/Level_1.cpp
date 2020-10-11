#include "Precompiled.h"
#include "Level_1.h"
#include "Core.h"
#include "DebugTools.h"
#include "GSM.h"
#include "Factory.h"
#include "GameLogic.h"
#include "TextDisplay.h"
#include "Global.h"
#include "GameLogic.h"
#include "ModelComponent.h"
#include "ActivationProperties.h"
#include "WindowsSystem.h"
//#include "Graphics.h"
#include "cameraclass.h"

namespace Framework
{
	//LoadLevelClass* ll;					//#### added temporary line
	Level_1::Level_1()
	{
		::strcpy_s( m_file, LEVEL_1_FILE );
	}
	Level_1::~Level_1()
	{
		delete m_loader;						//#### added temporary line
	}
	int Level_1::load(void)
	{
		std::cout<<"Load Level: "<<m_file<<"\n";
		m_loader = new LoadLevelClass();		//#### added temporary line
		
		m_loader->Initialize();
		return 1;
	}
	int Level_1::initialize(void)
	{
		
		GUN=0;
		std::cout<<"Level_1: init\n";
		m_loadingEffectTimer=0.0f;
		m_loadingEffectDone=false;
		//
		m_loader->LoadLevelFile( LEVEL_1_FILE );
		HERO->GetHero()->has(ModelComponent)->SwapTexture();
		POWERGUN->IsActivated=false;
		TIMEMANAGER->Disabled=true;
		LOGIC->ActivatableObjectsList[1]->has(ActivationProperties)->MarkAsActivated(true);
		pickedUp=false;
		
		GRAPHICS->GetCamera()->Setlookat( rotateVectorQuaternion( GRAPHICS->GetCameraLookAt(), HERO->GetHero()->has(Transform)->QUATERNION ) );

		return 1;
	}
	int Level_1::update(float dt)
	{
		CORE->updateSystems(dt);


		if(EDITOR && EDITOR->inEditor) return 0;
		if(!m_loadingEffectDone){
			m_loadingEffectTimer+=dt;
			if (m_loadingEffectTimer>1.0f){
				m_loadingEffectDone=true;
			}
		}

		if(POWERGUN->gravityGun.GetPickedItem())
			pickedUp=true;
		Transform * m_transform= HERO->GetHero()->has(Transform);
		float posX,posZ,posY;
		int tutorialBoxId=-1;
		posX=m_transform->Position.x;
		posZ=m_transform->Position.z;
		posY=m_transform->Position.y;

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


		//print(tutorialBoxId);
		for(int i=1;i<9;++i)
		{
			LOGIC->ActivatableObjectsList[i]->has(ActivationProperties)->MarkAsActivated(false);
			if (i==tutorialBoxId)
			{
				if(tutorialBoxId>3)
				{
					if(POWERGUN->IsActivated)
					{
						if(tutorialBoxId==4 && !pickedUp)
						{
							//if(LOGIC->objectPointedAt && LOGIC->objectPointedAt->CompositionTypeId!=GOC_Type_Cube)
							//{
								LOGIC->ActivatableObjectsList[i]->has(ActivationProperties)->MarkAsActivated(true);
							//}
						}
						if(tutorialBoxId==7 && pickedUp && POWERGUN->gravityGun.GetPickedItem())
						{
							LOGIC->ActivatableObjectsList[i]->has(ActivationProperties)->MarkAsActivated(true);
						}

					}
				}
				else
				{
					LOGIC->ActivatableObjectsList[i]->has(ActivationProperties)->MarkAsActivated(true);
				}
				
			}
		}

		if(tutorialBoxId==4 && POWERGUN->IsActivated && POWERGUN->gravityGun.GetPickedItem() && POWERGUN->IsActivated && pickedUp)
		{
			LOGIC->ActivatableObjectsList[5]->has(ActivationProperties)->MarkAsActivated(true);
		}
		/*if( POWERGUN->IsActivated && POWERGUN->gravityGun.GetPickedItem())
		{
			LOGIC->ActivatableObjectsList[6]->has(ActivationProperties)->MarkAsActivated(true);
		}*/



		if(GUN)
		{
			if(POWERGUN->IsActivated)
			{
				GUN->Destroy();
				GUN=0;
			}
		}

		/*if(POWERGUN->gravityGun.GetPickedItem())
		{
			LOGIC->ActivatableObjectsList[1]->has(ActivationProperties)->MarkAsActivated(false);
		}*/
		
		
		return 1;
	}
	int Level_1::draw(void)
	{
		return 1;
	}
	int Level_1::free(void)
	{
		std::cout<<"Free Level: "<<m_file<<"\n";
		m_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		GRAPHICS->Unload();
		return 1;
	}
	int Level_1::unload(void)
	{
		std::cout<<"Unload Level: "<<m_file<<"\n";
		return 1;
	}
	int Level_1::id(void)
	{
		return 1;
	}
	int Level_1::restart(void)
	{
		std::cout<<"Restart Level: "<<m_file<<"\n";
		CORE->gsm()->next(GSM::LEVEL_RESTART);
		return 1;
	}
	int val1 = 1;
	char str[32];
	const char* Level_1::UpdText( DispTextData* d )
	{
		::sprintf_s( str, 32, "Hello: %d", val1 );
		++val1;
		return str;
	}
	const char* Level_1::UpdText2( DispTextData* d )
	{
		::sprintf_s( str, 32, "Hey: %d", val1 );
		++val1;
		return str;
	}
}