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
#include "ComponentCreator.h"
#include "Core.h"
#include "GameLogic.h"
#include "Message.h"
//
namespace Framework
{
	Physics * PHYSICS = NULL;

	Physics::Physics():
		_paused(true),resolver(maxContacts*8)
	{
		ErrorIf(PHYSICS!=NULL,"Physics already initialized");
		PHYSICS = this;

		cData.contactArray = contacts;


		//old code
		//TimeAccumulation = 0.0f;
		Gravity = Vec2(0,-20);
		MaxVelocity = 1000;
		MaxVelocitySq = MaxVelocity*MaxVelocity;
		PenetrationEpsilon = 0.00001f;
		PenetrationResolvePercentage = 0.8f;
	}

	void Physics::Initialize()
	{
		RegisterComponent(RigidBody);
		RegisterComponent(Body);
		RegisterComponent(CollisionPrimitive);		
		RegisterComponent(CollisionSphere);		
		RegisterComponent(CollisionBox);		
	}

	void Physics::IntegrateBodies(float dt)
	{
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			it->integrate(dt);
			it->GetOwner()->has(CollisionPrimitive)->calculateInternals();
		}
		for(RigidBodyIterator it= LevelData.begin();it!= LevelData.end();++it)
		{
			it->GetOwner()->has(CollisionPrimitive)->calculateInternals();
		}
	}

	void Physics::GenerateContacts(float dt)
	{
		CollisionPlane plane;
		plane.direction=Vector3(0,1,0);
		plane.offset=0;

		//rigid bodies against other rigid bodies and the plane at the origin
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			if(it->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
			{
				detector.boxAndHalfSpace(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),plane,&cData);
				RigidBodyIterator it1 = it;
				++it1;
				for(;it1!=RigidBodies.end();++it1)
				{
					if(it->getAwake() || it1->getAwake())
					{
						if(it1->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
							detector.boxAndBox(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),*(static_cast<CollisionBox*>(it1->GetOwner()->has(CollisionPrimitive))),&cData);
						else if(it1->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
							detector.boxAndSphere(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),*(static_cast<CollisionSphere*>(it1->GetOwner()->has(CollisionPrimitive))),&cData);
					}
				}
			}else if(it->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
			{
				detector.sphereAndHalfSpace(*(static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))),plane,&cData);
				RigidBodyIterator it1 = it;
				++it1;
				for(;it1!=RigidBodies.end();++it1)
				{
					if(it->getAwake() || it1->getAwake())
					{
						if(it1->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
							detector.boxAndSphere(*(static_cast<CollisionBox*>(it1->GetOwner()->has(CollisionPrimitive))),*(static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))),&cData);
						else if(it1->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
							detector.sphereAndSphere(*(static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))),*(static_cast<CollisionSphere*>(it1->GetOwner()->has(CollisionPrimitive))),&cData);
					}
				}
			}
		}

		//rigid bdies against the level data i.e mass=0
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			if(it->getAwake())
			{
				if(it->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
				{
					for(RigidBodyIterator it1 = LevelData.begin();it1!=LevelData.end();++it1)
					{
						if(it1->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
							detector.boxAndBox(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),*(static_cast<CollisionBox*>(it1->GetOwner()->has(CollisionPrimitive))),&cData);
						else if(it1->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
							detector.boxAndSphere(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),*(static_cast<CollisionSphere*>(it1->GetOwner()->has(CollisionPrimitive))),&cData);
					}
				}else if(it->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
				{
					for(RigidBodyIterator it1 = LevelData.begin();it1!=LevelData.end();++it1)
					{
						if(it1->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
							detector.boxAndSphere(*(static_cast<CollisionBox*>(it1->GetOwner()->has(CollisionPrimitive))),*(static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))),&cData);
						else if(it1->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
							detector.sphereAndSphere(*(static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))),*(static_cast<CollisionSphere*>(it1->GetOwner()->has(CollisionPrimitive))),&cData);
					}
				}
			}
		}


	}


	void Physics::PublishResults()
	{
		//Commit all physics updates
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			(it)->PublishResults();
		}
		for(RigidBodyIterator it=LevelData.begin();it!=LevelData.end();++it)
		{
			(it)->PublishResults();
		}
		//Broadcast physics collision messages AFTER physics
		//has update the bodies
		/*for(unsigned i=0;i<Contacts.NumberOfContacts;++i)
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
		}*/
	}

	void Physics::ResetCollisionData()
	{
		cData.reset(maxContacts);
		cData.friction=(real)0.9;
		cData.restitution=(real)0.1;
		cData.tolerance=(real)0.1;
	}

	void Physics::Step(float dt)
	{
		
			IntegrateBodies(dt);

			ResetCollisionData();

			GenerateContacts(dt);

			resolver.setIterations(cData.contactCount*3);
			resolver.resolveContacts(cData.contactArray,cData.contactCount,dt);

			PublishResults();
	}

	void Physics::Update(float dt)
	{
		//if (dt > 0.05f) 
		//	dt = 0.05f;
		Step(dt);
	}

	GOC * Physics::TestPoint(Vec2 testPosition)
	{
		/*for(BodyIterator it=Bodies.begin();it!=Bodies.end();++it)
		{
			if( it->BodyShape->TestPoint(testPosition) )
				return it->GetOwner();
		}*/

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
