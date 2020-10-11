#include "BasicMovementEffect.h"
#include "Composition.h"
#include "Transform.h"
#include "Body.h"
//#include "XMLSerialization.h"
#include "Graphics.h"

namespace Framework
{

#define ROTATING_SPEED			10.0f
//======================================
BasicMovementEffect::~BasicMovementEffect()
{
}
//======================================
void BasicMovementEffect::Serialize( ISerializer& stream )
{
}
//======================================
void BasicMovementEffect::Initialize( GameObjectComposition* object )
{
	transform = object->has(Transform);
	body = object->has(Body);

	facing_right = true;
	target_rotation = 0.0f;
}
//======================================
void BasicMovementEffect::Update( float timeslice )
{
	if ( body->Velocity.x > 0.1f ) facing_right = true;
	else if ( body->Velocity.x < -0.1f ) facing_right = false;
	if ( abs(body->Velocity.y > 8.0f) )
	{
		body->Velocity.y /= abs(body->Velocity.y );
		body->Velocity.y = 8.0f;
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
void BasicMovementEffect::SendMessage( Message* msg ){}
//======================================
void BasicMovementEffect::FixRotation()
{
	float x;
	GRAPHICS->ScreenPosition( &transform->Position, &x, NULL );
	if ( x < 0.0f || x > 1.0f ) return;
	
	x *= 0.45f;
	if ( facing_right )
		x += -1.1f;
	else 
		x += 0.65f;

	target_rotation = x;
}


};
