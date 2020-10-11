///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Physics.h
///	Basic 2D iterative impulse physics engine.
///	
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

#include "VMath.h"
#include "Engine.h"
#include "Collision.h"
#include "Collisions.h"
#include "Body.h"
#include "Resolution.h"
#include "RigidBody.h"
#include <iostream>

namespace Framework
{

	///Message sent when there is a Collsion between two
	///Body Components.


	class MessageCollide : public Message
	{
	public:
		MessageCollide() : Message(Mid::Collide) {};
		MessageCollide(const MessageCollide& rhs)
			:Message(rhs),
			ContactNormal(rhs.ContactNormal),
			CollidedWith(rhs.CollidedWith),
			CollideObject(rhs.CollideObject)
		{
			//
		}
		MessageCollide* clone()
		{
			return new MessageCollide(*this);
		}
		Vector3 ContactNormal;
		GOC * CollidedWith;
		GOC * CollideObject;


	};

	class Physics: public ISystem
	{
	public:
		Physics();
		virtual void Update(float dt);
		virtual std::string GetName(){return "Physics";}
		void SendMessage(Message * m );
		void Initialize();
	private:
		void IntegrateBodies(float dt);
		void GenerateContacts(float dt);
		void PublishResults();
		void ResetCollisionData();
		void Step(float dt);
		GOC* GetGOCFromIntersection(Vector3 & position,Vector3 & direction);
		bool _paused;
	public:

		typedef ObjectLinkList<Body>::iterator BodyIterator;
		ObjectLinkList<Body> Bodies;
		
		typedef ObjectLinkList<RigidBody>::iterator RigidBodyIterator;
		ObjectLinkList<RigidBody> RigidBodies;
		ObjectLinkList<RigidBody> LevelData;

		const static unsigned maxContacts = 1024;
		Contact contacts[maxContacts];

		CollisionData cData;
		ContactResolver resolver;
		CollisionDetector detector;


		//old code
		GOC * TestPoint(Vec2 testPosition);
		//Gravity of the world
		Vec2 Gravity;
		//Max velocity for a physics body
		float MaxVelocity;
		float MaxVelocitySq;
		//See Resolution.cpp for use
		//Position correction tolerance
		float PenetrationEpsilon;
		//Position correction resolve percentage
		float PenetrationResolvePercentage;
		void ChangeCollisionProperties();

	};

	//A global pointer to the Physics system, used to access it globally.
	extern Physics* PHYSICS;
}


//TODO
//blocks stack destruction
//add ragdoll
//put level data into another list for easy access and put them as body1 during contact generation
//see calculate desired velocity function if objects bouncing  page no.394