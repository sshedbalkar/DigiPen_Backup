#include "BossMovementEffect.h"
#include "Composition.h"
#include "Transform.h"
#include "Body.h"
//#include "XMLSerialization.h"
#include "GameLogic.h"
#include "Graphics.h"

namespace Framework
{

#define ROTATING_SPEED			10.0f
//======================================
BossMovementEffect::~BossMovementEffect()
{
}
//======================================
void BossMovementEffect::Serialize( ISerializer& stream )
{
}
//======================================
void BossMovementEffect::Initialize( GameObjectComposition* object )
{
	transform = object->has(Transform);
	hero_trans = LOGIC->HERO->has(Transform);
	body = object->has(Body);

	facing_right = true;
	target_rotation = 0.0f;
}
//======================================
void BossMovementEffect::Update( float timeslice )
{
	if ( body->Velocity.x > 0.1f ) facing_right = true;
	else if ( body->Velocity.x < -0.1f ) facing_right = false;
	else
	{
		float dx = hero_trans->Position.x - transform->Position.x;
		if ( dx > 0.1f ) facing_right = true;
		else if ( dx < -0.1 ) facing_right = false;
	}

	FixRotation();
	float delta_rotation = target_rotation - transform->Rotation;
	float angle_to_rotate = abs(delta_rotation);
	float angle_per_frame = ROTATING_SPEED * timeslice;

	if ( angle_to_rotate > 0.2f )
	{
		delta_rotation /= angle_to_rotate;
		delta_rotation *= angle_per_frame;
		transform->Rotation += delta_rotation;
	}
	else transform->Rotation = target_rotation;
}
//======================================
void BossMovementEffect::SendMessage( Message* msg ){}
//======================================
void BossMovementEffect::FixRotation()
{
	float x;
	GRAPHICS->ScreenPosition( &transform->Position, &x, NULL );
	if ( x < 0.0f || x > 1.0f ) return;
	
	x *= 0.45f;
	if ( facing_right )
		x += -0.9f;
	else 
		x += 0.45f;

	target_rotation = x;
}


};
