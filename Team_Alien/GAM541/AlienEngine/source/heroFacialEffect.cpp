#include "HeroFacialEffect.h"
#include "Composition.h"
#include "Transform.h"
#include "Body.h"
#include "MeshInstance.h"
#include "Graphics.h"

namespace Framework
{

#define STATE_NO_EXPRESSION		0
#define STATE_CONCENTRATING		1
#define STATE_DELAY				2
#define ON_ENTER				0
#define ON_UPDATE				1

//======================================
HeroFacialEffect::~HeroFacialEffect()
{
}
//======================================
void HeroFacialEffect::Initialize( GameObjectComposition* object )
{
	transform = object->has(Transform);
	body = object->has(Body);
	mesh = object->has(MeshInstance);

	state = STATE_NO_EXPRESSION;
	inner_state = ON_ENTER;
}
//======================================
void HeroFacialEffect::Update( float timeslice )
{
//	Texture* texture;
	switch( state )
	{
	case STATE_NO_EXPRESSION:
		switch( inner_state )
		{
		case ON_ENTER:
//			texture = GRAPHICS->GetTexture("HeroFace0");
			mesh->SetTexture( "HeroFace0" );
			inner_state = ON_UPDATE;
			break;
		case ON_UPDATE:
			if ( abs(body->Velocity.x) > 1.0f )
			{
				state = STATE_DELAY;
				timer = 0.5f;
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
			mesh->SetTexture( "HeroFace1" );
			inner_state = ON_UPDATE;
			break;
		case ON_UPDATE:
			if ( abs(body->Velocity.x) < 0.5f )
			{
				state = STATE_DELAY;
				timer = 0.3f;
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
			if ( abs(body->Velocity.x) < 0.5f )	{target_state = STATE_NO_EXPRESSION; }
			else { target_state = STATE_CONCENTRATING; }
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
void HeroFacialEffect::SendMessage( Message* msg ){}



};
