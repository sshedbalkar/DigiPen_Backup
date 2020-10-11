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
#include "Graphics.h"
#include "LevelEditor.h"
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

	GOC* Physics::GetGOCFromIntersection(Vector3 & position,Vector3 & direction)
	{
		GOC * Object=NULL;
		Vector3 tempposition;
		Ray ray(position,direction);
		CollisionDetector detect;
		real distance=REAL_MAX;
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			if(it->GetOwner()->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Sphere)
			{
				if(detect.rayCastSphere(*(static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))),ray,tempposition))
				{
					real a=squaredDistance(position,tempposition);
					if(a<distance)
					{
						distance=a;
						Object=it->GetOwner();
					}
				}
			}else if(it->GetOwner()->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Box)
			{
				if(detect.rayCastBox(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),ray,tempposition))
				{
					real a=squaredDistance(position,tempposition);
					if(a<distance)
					{
						distance=a;
						Object=it->GetOwner();
					}
				}
			}
		}

		for(RigidBodyIterator it1=LevelData.begin();it1!=LevelData.end();++it1)
		{
			if(it1->GetOwner()->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Sphere)
			{
				if(detect.rayCastSphere(*(static_cast<CollisionSphere*>(it1->GetOwner()->has(CollisionPrimitive))),ray,tempposition))
				{
					real a=squaredDistance(position,tempposition);
					if(a<distance)
					{
						distance=a;
						Object=it1->GetOwner();
					}
				}
			}else if(it1->GetOwner()->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Box)
			{
				if(detect.rayCastBox(*(static_cast<CollisionBox*>(it1->GetOwner()->has(CollisionPrimitive))),ray,tempposition))
				{
					real a=squaredDistance(position,tempposition);
					if(a<distance)
					{
						distance=a;
						Object=it1->GetOwner();
					}
				}
			}
		}
		return Object;
	}

	void Physics::GenerateContacts(float dt)
	{
		CollisionPlane plane;
		plane.direction=Vector3(0,1,0);
		plane.offset=-2;

		Vector3 position(GRAPHICS->screen_width/2.0,GRAPHICS->screen_height/2.0,0),direction;
		GRAPHICS->ScreenToWorldSpaceRay(position,direction);
		LOGIC->objectPointedAt=GetGOCFromIntersection(position,direction);

		//rigid bodies against other rigid bodies and the plane at the origin
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			if(it->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
			{
				//detector.boxAndHalfSpace(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),plane,&cData);
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
				//detector.sphereAndHalfSpace(*(static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))),plane,&cData);
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
			it->setAcceleration(it->initialAcceleration);
			(it)->PublishResults();
		}
		for(RigidBodyIterator it=LevelData.begin();it!=LevelData.end();++it)
		{
			it->setAcceleration(it->initialAcceleration);
			(it)->PublishResults();
		}
		//Broadcast physics collision messages AFTER physics
		//has update the bodies
		Contact* lastContact = cData.contactArray+cData.contactCount;
		//std::cout<<cData.contactCount<<std::endl;
		for (Contact* contact= cData.contactArray; contact < lastContact; contact++)
		{
			MessageCollide messageCollide;
			messageCollide.ContactNormal = contact->contactNormal;
			if(contact->body[1] != NULL)
				messageCollide.CollidedWith = contact->body[1]->GetOwner();
			else
				messageCollide.CollidedWith = NULL;
			messageCollide.CollideObject = contact->body[0]->GetOwner();
			if(!contact->body[0]->canRotate && (messageCollide.ContactNormal*Vector3(0,1,0))>=(0.5*messageCollide.ContactNormal.magnitude()))
			{
				contact->body[0]->setAcceleration(0,0,0);
				contact->body[0]->setVelocity(0,0,0);
			}
			contact->body[0]->GetOwner()->SendMessage( &messageCollide );		
			if( contact->body[1] != NULL )
			{
				messageCollide.ContactNormal = contact->contactNormal*-1;
				messageCollide.CollidedWith = contact->body[0]->GetOwner();
				messageCollide.CollideObject = contact->body[1]->GetOwner();
				if(!contact->body[1]->canRotate && (messageCollide.ContactNormal*Vector3(0,1,0))>=(0.5*messageCollide.ContactNormal.magnitude()))
				{
					contact->body[1]->setAcceleration(0,0,0);
					contact->body[1]->setVelocity(0,0,0);
				}
				contact->body[1]->GetOwner()->SendMessage( &messageCollide );				
			}
		}
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
		if(EDITOR && EDITOR->inEditor)return;
		Step(dt);
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
