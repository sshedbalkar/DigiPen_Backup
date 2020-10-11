#include "RippleEffect.h"
#include "Composition.h"
#include "Transform.h"
#include "Graphics.h"
#include "GameLogic.h"
#include "water.h"
#include "Audio.h"
#include "GameLogic.h"

#define STATE_IDLE			0
#define STATE_RIPPLING		1

namespace Framework
{


//======================================
RippleEffect::~RippleEffect()
{
}
//======================================
void RippleEffect::Initialize( GameObjectComposition* object )
{
	water_trans = object->has( Transform );
	hero_trans = LOGIC->HERO->has( Transform );
	boss_trans = LOGIC->FINALBOSS->has( Transform );
	mat_water = (Water*)GRAPHICS->GetMaterial( MAT_WATER );

	state = STATE_IDLE;
}
//======================================
void RippleEffect::Update( float timeslice )
{
	switch ( state )
	{
	case STATE_IDLE:
		{
			float diffx = abs( hero_trans->Position.x - water_trans->Position.x );
			diffx *= 2.0f;
			float diffy = abs( hero_trans->Position.y - water_trans->Position.y );
			diffy *= 2.0f;
			if ( diffx < water_trans->Scale.x && diffy < water_trans->Scale.y )
			{
				mat_water->ripple_height = 1.5f;
				mat_water->ripple_x = hero_trans->Position.x;
				mat_water->ripple_initial_t = mat_water->t;
				state = STATE_RIPPLING;
				AUDIO->Play("water");
			}
			else if ( LOGIC->FINALBOSS )
			{
				diffx = abs( boss_trans->Position.x - water_trans->Position.x );
				diffx *= 2.0f;
				diffy = abs( boss_trans->Position.y - water_trans->Position.y );
				diffy *= 2.0f;
				if ( diffx < water_trans->Scale.x && diffy < water_trans->Scale.y )
				{
					mat_water->ripple_height = 1.5f;
					mat_water->ripple_x = boss_trans->Position.x;
					mat_water->ripple_initial_t = mat_water->t;
					state = STATE_RIPPLING;
					AUDIO->Play("water");
				}
			}
			if(state==STATE_IDLE)
			{
				Transform *trans=0;
				std::list<GOC*>::iterator it= LOGIC->ListOfBoxes.begin();
				for(;it!=LOGIC->ListOfBoxes.end();++it)
				{
					trans=(*it)->has(Transform);
					diffx = abs( trans->Position.x - water_trans->Position.x );
					diffx *= 2.0f;
					diffy = abs( trans->Position.y - water_trans->Position.y );
					diffy *= 2.0f;
					if ( diffx < water_trans->Scale.x && diffy < water_trans->Scale.y )
					{
						mat_water->ripple_height = 1.5f;
						mat_water->ripple_x = trans->Position.x;
						mat_water->ripple_initial_t = mat_water->t;
						state = STATE_RIPPLING;
						AUDIO->Play("water");
					}
				}
			}
		}
		break;

	case STATE_RIPPLING:
		mat_water->ripple_height *= 0.95f;
		if ( mat_water->ripple_height < 0.02f )
		{
			mat_water->ripple_height = 0.0f;
			state = STATE_IDLE;
		}
		break;
	}
}
//======================================
void RippleEffect::SendMessage( Message* msg )
{

}



};
