#include "endGameActions.h"
#include "Composition.h"
#include "Transform.h"
#include "Body.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "HeroProperties.h"
#include "Core.h"

namespace Framework
{


//======================================
EndGameActions::~EndGameActions()
{
}
//======================================
void EndGameActions::Serialize( ISerializer& stream )
{
}
//======================================
void EndGameActions::Initialize( GameObjectComposition* object )
{
	m_platformCreated=false;
	transform = object->has(Transform);
	hero_trans=LOGIC->HERO->has(Transform);
	hero_body=LOGIC->HERO->has(Body);
	m_stopUpdating=false;
}
//======================================
void EndGameActions::Update( float timeslice )
{
	if(!m_stopUpdating)
	{
		if(m_platformCreated)
		{
			if(LOGIC->GameOverPlatform->has(Body)->Position.x<493.0f)
				LOGIC->GameOverPlatform->has(Body)->Position.x+=0.1f;

			if (hero_trans->Position.x>502.5f)
			{
				hero_body->Velocity.x=5.0f;
				LOGIC->HERO->has(HeroProperties)->AllowMovement=false;
				if(	hero_trans->Position.x>511.5f)
				{
					MessageGameWon *m= new MessageGameWon();
					CORE->BroadcastMessage(m);
					m_stopUpdating=true;
				}
			}


		}
		else
		{
			if (hero_trans->Position.x>transform->Position.x)
			{
				MessageGameWon *m= new MessageGameWon();
				CORE->BroadcastMessage(m);
				m_stopUpdating=true;
			}
		}
	}
}
//======================================
void EndGameActions::SendMessage( Message* m )
{
		switch( m->MessageId )
		{
		case Mid::Dying:
			{
				MessageDying * msg = (MessageDying*)m;
				if(msg->compositionType==GOC_Type_Boss1 && LOGIC->GameOverPlatform==0)
				{
					GOC * newObject = FACTORY->BuildFromArchetype("platform_2");
					newObject->archId="platform_2";
					newObject->has(Transform)->Position = Vec3(486.0f, 6.06f-2.0f, 0.0f);
					newObject->has(Transform)->Scale = Vec3(4.0f*2.0f,0.6f*2.0f,1.0f);
					Body * body = newObject->has(Body);
					body->Position=Vec2(486.0f, 6.06f);
					if (body->BodyShape->Id==Shape::SidBox)
					{
						body->BodyScale=Vec2(4,0.6);
						static_cast<ShapeAAB*>(body->BodyShape)->Extents=body->BodyScale;
					}
					newObject->CompositionTypeId=GOC_Type_Platform1;
					newObject->Initialize();
					LOGIC->GameOverPlatform = newObject;
					m_platformCreated=true;
				}
				break;
			}
		}
		
}
//======================================



};
