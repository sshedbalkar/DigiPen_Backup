#include "EnemyAnimationEffect.h"
#include "Composition.h"
#include "Transform.h"
#include "Body.h"
#include "MeshInstance.h"
#include "Graphics.h"

namespace Framework
{

#define STATE_STANDING			0
#define STATE_WALKING			1
#define STATE_AIR				2
#define ON_ENTER				0
#define ON_UPDATE				1

//======================================
EnemyAnimationEffect::~EnemyAnimationEffect()
{
}
//======================================
void EnemyAnimationEffect::Initialize( GameObjectComposition* object )
{
	transform = object->has(Transform);
	body = object->has(Body);
	mesh = object->has(MeshInstance);

	state = STATE_STANDING;
	inner_state = ON_ENTER;
}
//======================================
void EnemyAnimationEffect::Update( float timeslice )
{
	static_cast<void>(timeslice);
	if ( !body->isCollidingDown ) {state = STATE_AIR;inner_state = ON_ENTER; }
	switch( state )
	{
	case STATE_STANDING:
		switch( inner_state )
		{
		case ON_ENTER:
			mesh->SetAnimation( "enemy_standing", true );
			inner_state = ON_UPDATE;
			break;
		case ON_UPDATE:
			if ( abs( body->Velocity.x ) > 0.2f )
			{ state = STATE_WALKING; inner_state = ON_ENTER; }
			break;
		}
		break;
	case STATE_WALKING:
		switch( inner_state )
		{
		case ON_ENTER:
			mesh->SetAnimation( "enemy_walking", true );
			inner_state = ON_UPDATE;
			break;
		case ON_UPDATE:
			if ( abs( body->Velocity.x ) < 0.2f  )
			{ state = STATE_STANDING; inner_state = ON_ENTER; }
			break;
		}
		break;

	case STATE_AIR:
		switch( inner_state )
		{
		case ON_ENTER:
			mesh->is_paused = true;
			inner_state = ON_UPDATE;
			break;
		case ON_UPDATE:
			if ( body->isCollidingDown )
			{
				if ( abs( body->Velocity.x ) > 0.2f ) state = STATE_WALKING;
				else state = STATE_STANDING;
				inner_state = ON_ENTER;
			}
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
void EnemyAnimationEffect::SendMessage( Message* msg ){}



};
