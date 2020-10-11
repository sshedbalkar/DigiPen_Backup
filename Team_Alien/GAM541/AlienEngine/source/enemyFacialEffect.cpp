#include "EnemyFacialEffect.h"
#include "Composition.h"
#include "Transform.h"
#include "Body.h"
#include "MeshInstance.h"
#include "GameLogic.h"
#include "Graphics.h"

namespace Framework
{

#define STATE_NO_EXPRESSION		0
#define STATE_CONCENTRATING		1
#define STATE_DELAY				2
#define ON_ENTER				0
#define ON_UPDATE				1

//======================================
EnemyFacialEffect::~EnemyFacialEffect()
{
}
//======================================
void EnemyFacialEffect::Initialize( GameObjectComposition* object )
{
	transform = object->has(Transform);
	body = object->has(Body);
	mesh = object->has(MeshInstance);
	hero_trans = LOGIC->HERO->has(Transform);

	state = STATE_NO_EXPRESSION;
	inner_state = ON_ENTER;
}
//======================================
void EnemyFacialEffect::Update( float timeslice )
{
	Vec3 hero_direction = hero_trans->Position;
	hero_direction -= transform->Position;

	switch( state )
	{
	case STATE_NO_EXPRESSION:
		switch( inner_state )
		{
		case ON_ENTER:
			mesh->SetTexture( "EnemyFace0" );
			inner_state = ON_UPDATE;
			break;
		case ON_UPDATE:
			if ( body->Velocity.x * hero_direction.x > 0.1f )
			{
				state = STATE_DELAY;
				timer = 0.3f;
				target_state = STATE_CONCENTRATING;
				inner_state = ON_ENTER;
			}
			break;
		}
		break;
		
	case STATE_CONCENTRATING:
		switch( inner_state )
		{
		case ON_ENTER:
			mesh->SetTexture( "EnemyFace1" );
			inner_state = ON_UPDATE;
			break;
		case ON_UPDATE:
			if ( body->Velocity.x * hero_direction.x < -0.1f  )
			{
				state = STATE_DELAY;
				timer = 0.7f;
				target_state = STATE_NO_EXPRESSION;
				inner_state = ON_ENTER;
			}
			break;
		}
		break;

	case STATE_DELAY:
		switch( inner_state )
		{
		case ON_ENTER:
			inner_state = ON_UPDATE;
			break;
		case ON_UPDATE:
			timer -= timeslice;
			if ( timer < 0.0f )
			{ state = target_state; inner_state = ON_ENTER; }
			break;
		}
		break;

	case -1:
		switch( inner_state )
		{
		case ON_ENTER:
			break;
		case ON_UPDATE:
			break;
		}
		break;
	}
}
//======================================
void EnemyFacialEffect::SendMessage( Message* msg ){}



};
