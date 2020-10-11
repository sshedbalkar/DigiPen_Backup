#include "Precompiled.h"
#include "LightComponent.h"
#include "Transform.h"
#include "GameLogic.h"

namespace Framework
{

	LightComponent::LightComponent()
	{
		Color = Vec3(1.0,1.0,1.0);
		fallout = 5.0f;
		CanMove=false;
	}

	void LightComponent::Initialize()
	{//when we change or restart level
		prevPos=this->GetOwner()->has(Transform)->Position;
	}

	void LightComponent::SendMessage(Message *m)
	{

		
	}
	bool LightComponent::UpdatePrevPos()
	{
		if (this->GetOwner()->has(Transform)->Position!=prevPos && CanMove)
		{
			prevPos=this->GetOwner()->has(Transform)->Position;
			return true;
		}
		else
			return false;
	}


	void LightComponent::Serialize(ISerializer& stream)
	{

	}

	void LightComponent::RemoveComponentFromGame()
	{
		RemoveLightFromGame();
	}

	void LightComponent::Update(float & dt)
	{
		
		
	}

	void LightComponent::RemoveLightFromGame()
	{
		GOC *g=this->GetOwner();
		GOC * temp1=LOGIC->BulbList[m_index];
		GOC * temp2=LOGIC->BulbList.back();
		LOGIC->BulbList[m_index]=temp2;
		temp2->has(LightComponent)->SetIndex(m_index);
		LOGIC->BulbList.back()=temp1;
		LOGIC->BulbList.pop_back();
		GRAPHICS->SetNumOfLights(GRAPHICS->GetNoOfLights()-1);
				
	}
}