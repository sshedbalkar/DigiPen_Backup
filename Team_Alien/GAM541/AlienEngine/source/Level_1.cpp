#include "Level_1.h"
#include "Core.h"
#include "LoadLevelClass.h"
#include "DebugTools.h"
#include "GSM.h"
#include "Factory.h"
//
namespace Framework
{
	//LoadLevelClass* ll;					//#### added temporary line
	Level_1::Level_1()
	{
		//
	}
	Level_1::~Level_1()
	{
		delete m_loader;						//#### added temporary line
	}
	int Level_1::load(void)
	{
		std::cout<<"Level 1: Load\n";
		m_loader = new LoadLevelClass();		//#### added temporary line
		m_loader->Initialize();
		return 1;
	}
	int Level_1::initialize(void)
	{
		std::cout<<"Level_1: init\n";
		m_loadingEffectTimer=0.0f;
		m_loadingEffectDone=false;
		//
		m_loader->LoadLevelFile("Objects\\Level_0.xml");
		return 1;
	}
	int Level_1::update(float dt)
	{
		if(!m_loadingEffectDone){
			m_loadingEffectTimer+=dt;
			if (m_loadingEffectTimer>1.0f){
				m_loadingEffectDone=true;
			}
		}
		//std::cout<<"Level_1: update: "<<dt<<"\n";
		CORE->updateSystems(dt);
		return 1;
	}
	int Level_1::draw(void)
	{
		return 1;
	}
	int Level_1::free(void)
	{
		std::cout<<"Level 1: Free\n";
		m_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		GRAPHICS->Unload();
		return 1;
	}
	int Level_1::unload(void)
	{
		std::cout<<"Level 1: Unload\n";
		return 1;
	}
	int Level_1::id(void)
	{
		return 1;
	}
	int Level_1::restart(void)
	{
		std::cout<<"Level 1: Restart\n";
		CORE->gsm()->next(GSM::LEVEL_RESTART);
		return 1;
	}
}