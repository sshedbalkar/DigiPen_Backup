///////////////////////////////////////////////////////////////////////////////////////
//
//	Physics.cpp
//	
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Physics.h"
#include "DebugDraw.h"
#include "Body.h"
#include "ComponentCreator.h"
#include "Core.h"
#include "GameLogic.h"
#include "Message.h"
//
namespace Framework
{
	Physics * PHYSICS = NULL;

	Physics::Physics():
		_paused(true)
	{
		ErrorIf(PHYSICS!=NULL,"Physics already initialized");
		PHYSICS = this;
		TimeAccumulation = 0.0f;
		Gravity = Vec2(0,-20);
		MaxVelocity = 1000;
		MaxVelocitySq = MaxVelocity*MaxVelocity;
		PenetrationEpsilon = 0.00001f;
		PenetrationResolvePercentage = 0.8f;
		StepModeActive = false;
		AdvanceStep = false;
	}

	void Physics::Initialize()
	{
		RegisterComponent(Body);
	}

	void Physics::IntegrateBodies(float dt)
	{
		for(BodyIterator it=Bodies.begin();it!=Bodies.end();++it)
		{
			it->Integrate(dt);
		}
	}

	void Physics::DetectContacts(float dt)
	{
		static_cast<void>(dt);
		BodyIterator bodyA = Bodies.begin();
		BodyIterator lastBody = Bodies.last(); //end - 1
		
		//Broad phase should be added this is N^2
		for(;bodyA!=Bodies.end();++bodyA)
		{
			if ( bodyA->IsStatic ) continue;
			BodyIterator bodyB = Bodies.begin();//bodyA;
			//++bodyB;
			for(;bodyB!=Bodies.end();++bodyB)
			{
				if ( bodyA == bodyB ) continue;
				//Do not collide static bodies with other static bodies
				if( bodyA->isCollidable & bodyB->isCollidable  ){
					if( !bodyA->IsStatic || !bodyB->IsStatic )
					{
						if( bodyA->GetOwner()->CompositionTypeId == GOC_Type_Pillar  || bodyB->GetOwner()->CompositionTypeId == GOC_Type_Pillar )
						{
						}
						else if( (bodyA->GetOwner()->CompositionTypeId == GOC_Type_Hero   &&  bodyB->GetOwner()->CompositionTypeId == GOC_Type_WaterPlatform)  ||
							(bodyB->GetOwner()->CompositionTypeId == GOC_Type_Hero    &&  bodyA->GetOwner()->CompositionTypeId == GOC_Type_WaterPlatform))
						{
						}
						else if ((bodyA->GetOwner()->CompositionTypeId == GOC_Type_Hero && bodyB->GetOwner()->CompositionTypeId == GOC_Type_InvisibleBox1)  ||
							(  bodyA->GetOwner()->CompositionTypeId == GOC_Type_InvisibleBox1 && bodyB->GetOwner()->CompositionTypeId == GOC_Type_Hero))
						{
							//do nothing}
						}
						else if ((bodyA->GetOwner()->CompositionTypeId == GOC_Type_Hero && bodyB->GetOwner()->CompositionTypeId == GOC_Type_Mirror)  ||
							(  bodyA->GetOwner()->CompositionTypeId == GOC_Type_Mirror && bodyB->GetOwner()->CompositionTypeId == GOC_Type_Hero))
						{
							//do nothing}
						}else
						{
							Collsion.GenerateContacts( (bodyA)->BodyShape , (bodyA)->Position , (bodyB)->BodyShape , (bodyB)->Position , &Contacts );
						}
					}
				}
			}
		}
	}


	void Physics::PublishResults()
	{
		//Commit all physics updates
		for(BodyIterator it=Bodies.begin();it!=Bodies.end();++it)
		{
			(it)->PublishResults();
		}

		//Broadcast physics collision messages AFTER physics
		//has update the bodies
		for(unsigned i=0;i<Contacts.NumberOfContacts;++i)
		{
			BodyContact* contact = &Contacts.contactArray[i];
			MessageCollide messageCollide;
			messageCollide.ContactNormal = contact->ContactNormal;
			messageCollide.Impulse = contact->ContactImpulse;
			messageCollide.CollidedWith = contact->Bodies[1]->GetOwner();
			messageCollide.CollideObject = contact->Bodies[0]->GetOwner();
			contact->Bodies[0]->GetOwner()->SendMessage( &messageCollide );
			
			if( contact->Bodies[1] != NULL )
			{
				messageCollide.ContactNormal = -contact->ContactNormal;
				messageCollide.Impulse = contact->ContactImpulse;
				messageCollide.CollidedWith = contact->Bodies[0]->GetOwner();
				messageCollide.CollideObject = contact->Bodies[1]->GetOwner();
				contact->Bodies[1]->GetOwner()->SendMessage( &messageCollide );
			}
		}
	}

	void Physics::Step(float dt)
	{
		if(!_paused)
		{
			IntegrateBodies(dt);

			Contacts.Reset();

			DetectContacts(dt);

			Contacts.ResolveContacts(dt);

			PublishResults();
		}
	}

	void Physics::Update(float dt)
	{
		if(!_paused)
		{
			const float TimeStep = dt;
			//
			if( !StepModeActive )
			{
				TimeAccumulation += dt;
				TimeAccumulation = std::min( TimeAccumulation , TimeStep *5 );
				if( TimeAccumulation > TimeStep )
				{
					TimeAccumulation-= TimeStep;
					Step( TimeStep );
				}
			}
			else
			{
				TimeAccumulation = 0.0f;
				if( AdvanceStep )
				{
					Step( TimeStep );
					AdvanceStep = false;
				}
			}
		}
	}

	GOC * Physics::TestPoint(Vec2 testPosition)
	{
		for(BodyIterator it=Bodies.begin();it!=Bodies.end();++it)
		{
			if( it->BodyShape->TestPoint(testPosition) )
				return it->GetOwner();
		}

		return NULL;
	}

	void Physics::SendMessage(Message *msg)
	{
		if (msg->MessageId == Mid::SystemPause)
		{
			const SystemPause *m = static_cast<const SystemPause*>(msg);
			_paused = m->Pause;
		}
	};
}
