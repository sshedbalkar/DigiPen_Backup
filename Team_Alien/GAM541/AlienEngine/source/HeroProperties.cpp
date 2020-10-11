
#include "Precompiled.h"
#include "HeroProperties.h"
#include "Message.h"
#include "GameLogic.h"
#include "Audio.h"
#include "Core.h"

namespace Framework
{


	HeroProperties::~HeroProperties()
	{
	}


	void HeroProperties::Initialize()
	{ 
		m_HeroSpeedX=5.0f;
		m_HeroSpeedY=7.6f;
		m_TurnRight=true;
		AllowMovement=true;
	}

	void HeroProperties::Jump()
	{
		if(AllowMovement)
			this->GetOwner()->has(Body)->Velocity.y=m_HeroSpeedY;
	}


	void HeroProperties::Update(float dt)
	{
		static_cast<void>(dt);
		m_previousIsMoving=m_IsMoving; // CAUTION!! m_IsMoving must be only be changed inside this function!!!
		//jump
		if( IsSpaceHeld() || IsWHeld()  )
		{
			if (LOGIC->m_GodMode )
				Jump();
			if (LOGIC->HERO->has(Body)->isCollidingDown )
			{
				Jump();
			}
		}
		
		//movement
		if(AllowMovement)
		{
			if(IsAHeld())
			{
				m_IsMoving=true;
				m_TurnRight=false;
				this->GetOwner()->has(Body)->Velocity.x= -m_HeroSpeedX;
			}else if(IsDHeld())
			{
				m_IsMoving=true;
				m_TurnRight=true;
				this->GetOwner()->has(Body)->Velocity.x= m_HeroSpeedX;
			}else
			{
				m_IsMoving=false;
				this->GetOwner()->has(Body)->Velocity.x=0.0;
			}
		}else
		{
			m_IsMoving=false;
			
		}

		if (!this->GetOwner()->has(Body)->isCollidingDown && !this->GetOwner()->has(Body)->isCollidingUp )//means is in the air
		{
			m_IsMoving=true;
		}
		else if(!m_IsMoving)//if it was not changed by X and Y
		{
			m_IsMoving=false;
		}
	}

	void HeroProperties::DisableHeroMovement()
	{
		this->GetOwner()->has(Body)->Velocity.x=0.0;
		AllowMovement=false;
	}

	void HeroProperties::SendMessageA(Message *m)
	{
		if(m->MessageId== Mid::Collide)
		{
			MessageCollide * msg = (MessageCollide*)m;
			if (msg->CollidedWith->CompositionTypeId ==GOC_Type_InvisibleBoxOfDeath || msg->CollidedWith->CompositionTypeId ==GOC_Type_Door1  ||  msg->CollidedWith->CompositionTypeId ==GOC_Type_Door2)
			{
				AllowMovement=true;
			}

			if (msg->CollidedWith->CompositionTypeId ==GOC_Type_InvisibleBoxOfDeath || msg->CollidedWith->CompositionTypeId ==GOC_Type_Enemy1 || msg->CollidedWith->CompositionTypeId ==GOC_Type_Platform_Death)
			{
				MessageDying *m= new MessageDying(msg->CollideObject ->CompositionTypeId);
				CORE->BroadcastMessage(m);
				if (msg->CollidedWith->CompositionTypeId ==GOC_Type_InvisibleBoxOfDeath || msg->CollidedWith->CompositionTypeId ==GOC_Type_Enemy1)
				{
					if(!AUDIO->isPlaying("water"))
						AUDIO->Play("death1");
				}
				else
					AUDIO->Play("death2");
				if (msg->CollidedWith->CompositionTypeId ==GOC_Type_Enemy1)
				{
					msg->CollidedWith->has(AI_Type1)->Initialize();
				}
				LOGIC->CheckPointsControl->Load ();
			}
		}
	}
}
