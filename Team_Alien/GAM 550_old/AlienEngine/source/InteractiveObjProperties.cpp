
#include "Precompiled.h"
#include "InteractiveObjProperties.h"
#include <algorithm>
#include <vector>
#include "GameLogic.h"
#include "Audio.h"

namespace Framework
{

	InteractiveObjProperties::InteractiveObjProperties()
	{
		m_MarkToOpen=false;
		m_minDoorScale=0.3f;
		ResetOnLoad=0;
		previousOpen=false;
	}

	InteractiveObjProperties::~InteractiveObjProperties()
	{
	}


	void InteractiveObjProperties::Initialize()
	{ 
		m_owner=this->GetOwner();
		m_Transform = m_owner->has(Transform);
		m_Body = m_owner->has(Body);
		m_BodyShape= static_cast<ShapeAAB*>(m_Body->BodyShape);
		yScaleOriginal=m_Transform->Scale.y;
		yBodyScaleOriginal=m_BodyShape->Extents.y;


	}

	bool InteractiveObjProperties::Dying()
	{
		if(this->GetOwner()->CompositionTypeId==GOC_Type_Door1)
		{
			m_Transform->Scale.y*=0.98f;
			if(m_Transform->Scale.y<=m_minDoorScale)
				return true;
		}
		return false;
	}

	void InteractiveObjProperties::DoorAction()
	{
		int id = GetOwner()->CompositionTypeId;
		if(id == GOC_Type_Door1 ){
			if (this->m_MarkToOpen)
			{
				OpenDoor();
				if(previousOpen!=m_MarkToOpen)
				{
					AUDIO->Stop("door_activate");
					AUDIO->Play("door_deactivate");
				}
			}
			else
			{
				CloseDoor();
				if(previousOpen!=m_MarkToOpen)
				{
					AUDIO->Stop("door_deactivate");
					AUDIO->Play("door_activate");
				}
			}
		}
		else if(id == GOC_Type_Door2 ){
			if (this->m_MarkToOpen)
			{	
				CloseDoor();
				if(previousOpen!=m_MarkToOpen)
				{
					AUDIO->Stop("door_deactivate");
					AUDIO->Play("door_activate");
				}
			}
			else
			{
				OpenDoor();
				if(previousOpen!=m_MarkToOpen)
				{
					AUDIO->Stop("door_activate");
					AUDIO->Play("door_deactivate");
				}
			}
		}
			

		previousOpen=m_MarkToOpen;
	}

	void InteractiveObjProperties::CloseDoor()
	{
		m_Body->isCollidable=true;
	}

	void InteractiveObjProperties::OpenDoor()
	{
		m_Body->isCollidable=false;
	}

	void InteractiveObjProperties::MoveButtonDown(Message *m)
	{
		//move button down
		MessageCollide * msg = (MessageCollide*)m;
		m_Body->Position.y=m_Body->InitialPosition.y-0.18f;
		
		if(msg->CollidedWith->has(Body)->Velocity.y>0.000000001 || msg->CollidedWith->has(Body)->Velocity.y<-0.000000001)
		{
			
			msg->CollidedWith->has(Body)->Position.y-=0.18f;
			msg->CollidedWith->has(Body)->Velocity.y=0.0f;
		}
	}

	void InteractiveObjProperties::ButtonAction()
	{
		if(!(m_Body->isCollidingDown || m_Body->isCollidingUp || m_Body->isCollidingLeft || m_Body->isCollidingRight))
			{
				if(abs(m_Body->Position.y-m_Body->InitialPosition.y)>EPSILON)
				{
					m_Body->Position.y=m_Body->InitialPosition.y-(abs(m_Body->Position.y-m_Body->InitialPosition.y)*0.8f);
					
				}
		}
	}


	void InteractiveObjProperties::Update(const float & dt)
	{
		static_cast<void>(dt);
		if (m_owner->CompositionTypeId ==GOC_Type_ButtonPlatform1||m_owner->CompositionTypeId ==GOC_Type_ButtonPlatform2)
		{
			ButtonAction();
		}
		else if (m_owner->CompositionTypeId ==GOC_Type_Door1 || m_owner->CompositionTypeId ==GOC_Type_Door2)
		{
			DoorAction();
		}

	}

	void InteractiveObjProperties::SendMessageA(Message *m)
	{
		if(m->MessageId== Mid::Collide)
		{
			MessageCollide * msg = (MessageCollide*)m;

			if (msg->CollideObject->CompositionTypeId ==GOC_Type_ButtonPlatform1 || msg->CollideObject->CompositionTypeId ==GOC_Type_ButtonPlatform2 )
			{
				MessageActivateObj  msgActivate(msg->CollideObject);
				MoveButtonDown(m);
				
			}
			else if (msg->CollideObject->CompositionTypeId ==GOC_Type_MovingBox1 && msg->CollidedWith->CompositionTypeId ==GOC_Type_InvisibleBoxOfDeath   )
			{
				m_Body->Position=m_Body->InitialPosition ;
			}
		}
	}


	void InteractiveObjProperties::Serialize(ISerializer& stream)
	{	
		ISerializer* str = stream.clone();
		if(str->isGood())
		{
			str->readProperty("SecondaryObjectId", *str);
			str->readValue("value", m_ObjectSecondaryId);
			//
			str->goToParent();
			str->readProperty("ActivatesObjectWithSecondaryId", *str);
			str->readValue("value", m_ObjectIdToActivate);
			//
			str->goToParent();
			str->readProperty("ResetOnLoad", *str);
			str->readValue("value", ResetOnLoad);
			
			str->goToParent();
			str->readProperty("ActivatedBySecondaryObjectIds", *str);
			str->readValue("value", m_ObjectActivatedByVec);
			
		}
		delete str;
	}

}
