#include "Precompiled.h"
#include "Connector.h"
#include "Physics.h"
#include "ModelComponent.h"
#include "PhysicsMath.h"
#include "VMath.h"

namespace Framework
{

	Connector::Connector()
	{
		ObjectA=NULL;ObjectB=NULL;Bridge=NULL;
	}


	void Connector::AddBody1(GOC* a)
	{
		if (ObjectA==a)
			return;

		if (ObjectA)//not null
		{
			ObjectA->has(ModelComponent)->SwapModel();//restore previous model
		}
		
		if (a!=NULL && a->CompositionTypeId==GOC_Type_Wall_Selectable )
		{
			ObjectA=a;
			ObjectA->has(ModelComponent)->SwapModel();//the new selected component should swap Model
			if(ObjectB && ObjectB==ObjectA)//if A=B
			{
				//replace ObjectB
				ObjectB->has(ModelComponent)->SwapModel();
				ObjectB=0;
			}
			else if(ObjectB)//is A selected
			{
				if(CheckIfCanConnect())
					ConnectObjects();
			}
		}
		else
		{
			ObjectA=0;
		}
		
	}	


	bool Connector::CheckIfCanConnect()
	{
		if (ObjectA->has(Transform)->Position.x==ObjectB->has(Transform)->Position.x && ObjectA->has(Transform)->Position.z!=ObjectB->has(Transform)->Position.z)
		{
			return true;
		}
		else if (ObjectA->has(Transform)->Position.z==ObjectB->has(Transform)->Position.z && ObjectA->has(Transform)->Position.x!=ObjectB->has(Transform)->Position.x)
		{
			return true;
		}
		if(Bridge)
			Bridge->Destroy();
		return false;
	}

	void HelperFunc(Vector3 & v, Vector3 & w,Vector3 & closestA, float & distance)
	{
		float tempdist;
		tempdist=lengthSquared(Vector3ToVec3(v-w));
		if (tempdist<distance)
		{
			distance=tempdist;
			closestA=v;
		}

	}

	void Connector::ConnectObjects()
	{
		Vector3 k,v,w,closestA, closestB;
		float distance=FLT_MAX;
		

		//first take center of point B
		w = ObjectB->has(Transform)->Position;
		
		//check the distance from all top points of A and store the closestPoint============
		k.x=(ObjectA->has(Transform)->Scale.x /2.0f);
		k.y=(ObjectA->has(Transform)->Scale.y /2.0f);
		k.z=0.0f;
		v = ObjectA->has(RigidBody)->getTransform().transform(k);
		std::cout<<v;
		HelperFunc(v,w,closestA,distance);

		k.x=-(ObjectA->has(Transform)->Scale.x /2.0f);
		k.y=(ObjectA->has(Transform)->Scale.y /2.0f);
		k.z=0.0f;
		v = ObjectA->has(RigidBody)->getTransform().transform(k);
		std::cout<<v;
		HelperFunc(v,w,closestA,distance);

		k.x=0.0f;
		k.y=(ObjectA->has(Transform)->Scale.y /2.0f);
		k.z=(ObjectA->has(Transform)->Scale.z /2.0f);
		v = ObjectA->has(RigidBody)->getTransform().transform(k);
		std::cout<<v;
		HelperFunc(v,w,closestA,distance);

		k.x=0.0f;
		k.y=(ObjectA->has(Transform)->Scale.y /2.0f);
		k.z=-(ObjectA->has(Transform)->Scale.z /2.0f);
		v = ObjectA->has(RigidBody)->getTransform().transform(k);
		std::cout<<v;
		HelperFunc(v,w,closestA,distance);
		//====================================================================================
		
		distance=FLT_MAX;
		w=closestA;
		//check the distance from closestA to all top points of B and store the closest point==
		k.x=(ObjectB->has(Transform)->Scale.x /2.0f);
		k.y=(ObjectB->has(Transform)->Scale.y /2.0f);
		k.z=0.0f;
		v = ObjectB->has(RigidBody)->getTransform().transform(k);
		std::cout<<v;
		HelperFunc(v,w,closestB,distance);

		k.x=-(ObjectB->has(Transform)->Scale.x /2.0f);
		k.y=(ObjectB->has(Transform)->Scale.y /2.0f);
		k.z=0.0f;
		v = ObjectB->has(RigidBody)->getTransform().transform(k);
		std::cout<<v;
		HelperFunc(v,w,closestB,distance);

		k.x=0.0f;
		k.y=(ObjectB->has(Transform)->Scale.y /2.0f);
		k.z=(ObjectB->has(Transform)->Scale.z /2.0f);
		v = ObjectB->has(RigidBody)->getTransform().transform(k);
		std::cout<<v;
		HelperFunc(v,w,closestB,distance);

		k.x=0.0f;
		k.y=(ObjectB->has(Transform)->Scale.y /2.0f);
		k.z=-(ObjectB->has(Transform)->Scale.z /2.0f);
		v = ObjectB->has(RigidBody)->getTransform().transform(k);
		std::cout<<v;
		HelperFunc(v,w,closestB,distance);
		//====================================================================================
		
		if (closestA.z==ObjectA->has(Transform)->Position.z)//then z is constant, create object on y
		{
			Bridge= FACTORY->BuildFromArchetype("GOC_Type_Bridge");//HARDCODED CHANGE LATER 
			Vec3 newPos= Vec3((closestA.x + closestB.x)/2.0f,ObjectA->has(Transform)->Position.y,(closestA.z+closestB.z)/2.0f);
			newPos.y+=ObjectA->has(Transform)->Scale.y/2.0f - Bridge->has(Transform)->Scale.y/2.0f;
			Bridge->has(Transform)->Scale=Vec3(sqrt(distance),1.0,40);
			Bridge->has(Transform)->Position=newPos;
			//D3DXQUATERNION q(0,0,0,1);
			//bridge->has(Transform)->QUATERNION=q*calculateQuaterion(R_PI/2.0,Vec3(0,1,0));
			Bridge->Initialize();
		}
		else if (closestA.x==ObjectA->has(Transform)->Position.x)//then z is constant, create object on y
		{
			Bridge= FACTORY->BuildFromArchetype("GOC_Type_Bridge");//HARDCODED CHANGE LATER 
			Vec3 newPos= Vec3((closestA.x + closestB.x)/2.0f,ObjectA->has(Transform)->Position.y,(closestA.z+closestB.z)/2.0f);
			//Vec3 newPos= Vec3((ObjectA->has(Transform)->Position.x + ObjectB->has(Transform)->Position.x)/2.0f,ObjectA->has(Transform)->Position.y,(ObjectA->has(Transform)->Position.z+ObjectB->has(Transform)->Position.z)/2.0f);
			newPos.y+=ObjectA->has(Transform)->Scale.y/2.0f - Bridge->has(Transform)->Scale.y/2.0f;
			Bridge->has(Transform)->Scale=Vec3(40.0,1.0,sqrt(distance));
			Bridge->has(Transform)->Position=newPos;
			//D3DXQUATERNION q(0,0,0,1);
			//bridge->has(Transform)->QUATERNION=q*calculateQuaterion(R_PI/2.0,Vec3(0,1,0));
			Bridge->Initialize();
		}

		std::cout<<distance<<std::endl;
		std::cout<<closestA.x<<", "<<closestA.y<<", "<<closestA.z<<std::endl;
		std::cout<<closestB.x<<", "<<closestB.y<<", "<<closestB.z<<std::endl<<std::endl;
		
	}
	
	void Connector::AddBody2(GOC* b)
	{
		if (ObjectB==b)
			return;

		if (ObjectB)//not null
		{
			ObjectB->has(ModelComponent)->SwapModel();//restore previous model
		}

		if (b!=NULL && b->CompositionTypeId==GOC_Type_Wall_Selectable)
		{
			ObjectB=b;
			ObjectB->has(ModelComponent)->SwapModel();//the new selected component should swap Model
			if(ObjectA && ObjectB==ObjectA)//if A=B
			{
				//replace ObjectA
				ObjectA->has(ModelComponent)->SwapModel();
				ObjectA=0;
			}
			else if(ObjectA)//is A selected
			{
				if(CheckIfCanConnect())
					ConnectObjects();
			}
		}
		else
		{
			ObjectB=0;
		}
	}		

	void Connector::Update()
	{
		if(!ObjectA || 	!ObjectB)
		{
			if(Bridge){
				Bridge->Destroy();
				Bridge=NULL;}
		}
	}
}