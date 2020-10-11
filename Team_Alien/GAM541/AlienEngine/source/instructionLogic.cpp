#include "InstructionLogic.h"
#include "Composition.h"
#include "Transform.h"
#include "visualPlatform.h"
#include "Graphics.h"
#include "rays.h"
#include "GameLogic.h"

#define STATE0		0
#define STATE1		1
#define STATE2		2
#define STATE3		3

#define ISTATE_ENTER	0
#define ISTATE_UPDATE	1
#define ISTATE_EXIT		2

namespace Framework
{


//======================================
InstructionLogic::~InstructionLogic()
{
}
//======================================
void InstructionLogic::Initialize( GameObjectComposition* object )
{
	platform = object->has(VisualPlatform);
	transform = object->has(Transform);

	rdown = false;
	on_target = false;
	state = STATE0;
	inner_state = ISTATE_ENTER;
}
//======================================
void InstructionLogic::Update( float timeslice )
{
	switch( state )
	{
	case STATE0:
		switch( inner_state )
		{
		case ISTATE_ENTER:
			platform->ChangeTexture( "InstructionPlatform20" );
			inner_state = ISTATE_UPDATE;
			break;
		case ISTATE_UPDATE:
			if(platform->GetOwner()->has(Transform)->Position.x < LOGIC->HERO->has(Transform)->Position.x+1.0f)
			{
				state = STATE3;
				inner_state = ISTATE_ENTER; 
				break;
			}
			if ( rdown ) { state = STATE1; inner_state = ISTATE_ENTER; }
			break;
		}
		break;
		
	case STATE1:
		switch( inner_state )
		{
		case ISTATE_ENTER:
			platform->ChangeTexture( "InstructionPlatform21" );
			inner_state = ISTATE_UPDATE;
			break;
		case ISTATE_UPDATE:
			if(platform->GetOwner()->has(Transform)->Position.x < LOGIC->HERO->has(Transform)->Position.x+1.0f)
			{
				state = STATE3;
				inner_state = ISTATE_ENTER; 
				break;
			}
			if ( !rdown ) { state = STATE0; inner_state = ISTATE_ENTER; }
			else 
			{
				CursorCheck();
				if ( on_target ) { state = STATE2; inner_state = ISTATE_ENTER; }
			}
			break;
		case ISTATE_EXIT:
			break;
		}
		break;
		
	case STATE2:
		switch( inner_state )
		{
		case ISTATE_ENTER:
			platform->ChangeTexture( "InstructionPlatform22" );
			inner_state = ISTATE_UPDATE;
			break;
		case ISTATE_UPDATE:
			if(platform->GetOwner()->has(Transform)->Position.x < LOGIC->HERO->has(Transform)->Position.x+1.0f)
			{
				state = STATE3;
				inner_state = ISTATE_ENTER; 
				break;
			}
			if ( !rdown ) { state = STATE0; inner_state = ISTATE_ENTER; }
			else 
			{
				CursorCheck();
				if ( !on_target ) { state = STATE1; inner_state = ISTATE_ENTER; }
			}
			break;
		}
		break;

	case STATE3:
		switch( inner_state )
		{
		case ISTATE_ENTER:
			platform->ChangeTexture( "InstructionPlatform19" );
			inner_state = ISTATE_UPDATE;
			break;
		case ISTATE_UPDATE:
			{}
			/*if ( !rdown ) { state = STATE0; inner_state = ISTATE_ENTER; }
			else 
			{
				CursorCheck();
				if ( !on_target ) { state = STATE1; inner_state = ISTATE_ENTER; }
			}
			break;*/
		}
		break;

	}
}
//======================================
void InstructionLogic::SendMessage( Message* msg )
{
	if ( msg->MessageId == Mid::RMouseButton )
	{
		rdown = !rdown;
	}
	else if ( msg->MessageId == Mid::Dying )
	{
		MessageDying * m = (MessageDying*)msg;
		if(m->compositionType==GOC_Type_Hero )
		{
			state = STATE0;
			inner_state = ISTATE_ENTER;
		}
		
	}
}
//======================================
void InstructionLogic::CursorCheck()
{
	if ( RAYS->IsRayTouchingTeleportable() )
		on_target = true;
	else 
		on_target = false;
}



};
