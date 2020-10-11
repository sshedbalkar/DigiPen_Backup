#include "scriptRegistrator.h"
#include "scriptCreator.h"
// include every script here
	// graphics scripts
	#include "basicMovementEffect.h"
	#include "heroAnimationEffect.h"
	#include "heroFacialEffect.h"
	#include "enemyAnimationEffect.h"
	#include "enemyFacialEffect.h"
	#include "bossMovementEffect.h"
	#include "bossAnimationEffect.h"
	#include "bossFacialEffect.h"
	#include "rippleEffect.h"
#include "instructionLogic.h"
#include "endGameActions.h"


#define REGISTER_SCRIPT(type) insert( std::pair<std::string,ScriptCreator*>(#type, new ScriptCreatorType<type>) )

namespace Framework
{

//======================
void ScriptRegistrator::Initialize()
{
	//register every script here
	REGISTER_SCRIPT(BasicMovementEffect);
	REGISTER_SCRIPT(HeroAnimationEffect);
	REGISTER_SCRIPT(HeroFacialEffect);
	REGISTER_SCRIPT(EnemyAnimationEffect);
	REGISTER_SCRIPT(EnemyFacialEffect);
	REGISTER_SCRIPT(BossMovementEffect);
	REGISTER_SCRIPT(BossAnimationEffect);
	REGISTER_SCRIPT(BossFacialEffect);

	REGISTER_SCRIPT(RippleEffect);

	REGISTER_SCRIPT(InstructionLogic);
	REGISTER_SCRIPT(EndGameActions);
}
//======================
void ScriptRegistrator::Free()
{
	iterator it;
	for ( it = begin(); it != end(); ++it )
	{
		delete it->second;
	}
	clear();
}

}