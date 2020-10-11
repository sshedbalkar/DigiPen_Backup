#include "Precompiled.h"
#include "ActivationProperties.h"
#include <algorithm>
#include <vector>
#include "GameLogic.h"
#include "Audio.h"

namespace Framework
{

	ActivationProperties::ActivationProperties()
	{
		m_MarkToOpen=false;
		m_ResetOnLoad=0;
		previousOpen=false;
	}

	ActivationProperties::~ActivationProperties()
	{
	}


	void ActivationProperties::Initialize()
	{ 
		m_owner=this->GetOwner();

	}



	void ActivationProperties::Activate()
	{
	}


	void ActivationProperties::Update(const float & dt)
	{

	}

	void ActivationProperties::SendMessageA(Message *m)
	{
		if(m->MessageId== Mid::Collide)
		{
		}
	}


	void ActivationProperties::Serialize(ISerializer& stream)
	{	
		ISerializer* str = stream.clone();
		if(str->isGood())
		{
			str->readProperty("ObjectActivationId", *str);
			str->readValue("value", m_ActivationId);
			//
			str->goToParent();
			str->readProperty("ResetOnLoad", *str);
			str->readValue("value", m_ResetOnLoad);
			//
			str->goToParent();
			str->readProperty("ActivatedBy", *str);
			str->readValue("value", m_ActivatedByVec,4);
			
		}
		delete str;
	}

}
