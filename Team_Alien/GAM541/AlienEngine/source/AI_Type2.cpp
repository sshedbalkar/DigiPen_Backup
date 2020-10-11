
#include "Precompiled.h"
#include "AI_Type2.h"
#include "Message.h"
#include "GameLogic.h"
#include "particleEmitter.h"
#include "Audio.h"
#include "Core.h"

namespace Framework
{

	enum BossStateMachine{
		IDLE=0,
		FOLLOWING_HERO,
		GOTO_ORIGINALPOS,
		TELEPORTING
	};

	AI_Type2::~AI_Type2()
	{
	}


	void AI_Type2::Initialize()
	{ 
		m_EnemySpeedX=2.5f;
		m_Body = this->GetOwner()->has(Body);
		m_movingDirection=0;
		m_EnemyJumpSpeedY=9.0f;
		m_foundObstacle=false;
		previousCollideSide=0;
		m_foundGap=false;
	}

	void AI_Type2::Jump()
	{
		m_Body->Velocity.y=m_EnemyJumpSpeedY;
	}

	void AI_Type2::Teleport()
	{
		//m_Body->Velocity.y=m_EnemyJumpSpeedY;
	}



	void AI_Type2::DecideWhatToDo()
	{
		if (m_Body->Position.x>495.0f && m_Body->Position.x<497.5f && m_Body->Position.y<10.0f)
		{
			if (LOGIC->ActivateObj->m_buttons[20]==0)
				m_foundGap=true;
		}
		

		if (m_Body->Position!=m_Body->InitialPosition && !m_foundGap)
		{
			m_State=GOTO_ORIGINALPOS;
		}
		else if (m_foundGap)
		{
			m_State=TELEPORTING;
			this->GetOwner()->has(ParticleEmitter)->GetEmitter("teleporting_effect")->Activate(true);
		}
		else
		{
			m_State=IDLE;
		}

	}

	void AI_Type2::DoAction(float & dt)
	{
		if (m_State==IDLE)
		{
			//do nothing..
		}
		else if (m_State==GOTO_ORIGINALPOS)
		{
			if (m_Body->Position.y>m_Body->InitialPosition.y+0.2 && m_Body->Position.y>m_Body->InitialPosition.y-0.2 && m_Body->Position.x>493)//boss is up move right
			{
				if (!m_Body->isCollidingDown && !m_Body->isCollidingLeft &&  !m_Body->isCollidingRight)
				{
					m_movingDirection=0;
				}
				else
					m_movingDirection=1;
			}
			else if(m_Body->Position.y>m_Body->InitialPosition.y+0.2 && m_Body->Position.y>m_Body->InitialPosition.y-0.2 && m_Body->Position.x<493 && m_Body->Position.x>483)//boss is up move left
			{
			if (!m_Body->isCollidingDown && !m_Body->isCollidingLeft &&  !m_Body->isCollidingRight)
				{
					m_movingDirection=0;
				}
				else
					m_movingDirection=-1;
			}
			else if(m_Body->Position.y>m_Body->InitialPosition.y+0.2 && m_Body->Position.y>m_Body->InitialPosition.y-0.2 && m_Body->Position.x<483)//boss is up move left
			{
				if (!m_Body->isCollidingDown && !m_Body->isCollidingLeft &&  !m_Body->isCollidingRight)
				{
					m_movingDirection=0;
				}
				else
					m_movingDirection=1;
			}
			else if(m_Body->Position.x>m_Body->InitialPosition.x+0.1 && m_Body->Position.x>m_Body->InitialPosition.x-0.1)//boss is to the right and same y move left
			{
				m_movingDirection=-1;
			}
			else if(m_Body->Position.x<m_Body->InitialPosition.x+0.1 && m_Body->Position.x<m_Body->InitialPosition.x-0.1)//boss is to the left and same y move right
			{
				m_movingDirection=1;
			}
			else if(m_Body->Position.x<m_Body->InitialPosition.x+0.1 && m_Body->Position.x>m_Body->InitialPosition.x-0.1)
			{
				m_Body->Position.x=m_Body->InitialPosition .x;
				m_movingDirection=0;
			}


			if (m_foundObstacle)
			{
				Jump();
				m_foundObstacle=false;
			}

			
			
			m_Body->Velocity.x=m_EnemySpeedX*(float)m_movingDirection;

			if (!m_Body->isCollidingLeft &&  !m_Body->isCollidingRight)
			{
				previousObstacle=0;
			}
		}
		else if (m_State==TELEPORTING)
		{
			m_Body->Velocity.x=0.0f;
			if (m_timer<1.5f)
				m_timer+=dt;
			else{
				m_Body->Position.x=489.6f;
				m_timer=0.0f;
				this->GetOwner()->has(ParticleEmitter)->GetEmitter("teleporting_effect")->Activate(false);
				m_foundGap=false;
			}
		}

		if(abs(m_Body->Velocity.x-0.0)>0.001)
			AUDIO->Play("bosswalk");
		else
			AUDIO->Stop("bosswalk");
		
	}
		

	void AI_Type2::Update(float dt)
	{
		DecideWhatToDo();
		DoAction(dt);
	}

	void AI_Type2::SendMessageA(Message *m)
	{
		if(m->MessageId== Mid::Collide)
		{

			MessageCollide * msg = (MessageCollide*)m;

			if (msg->CollideObject->CompositionTypeId ==GOC_Type_Boss1   )
			{
				if (msg->CollidedWith ->CompositionTypeId ==GOC_Type_InvisibleBoxOfDeath  )
				{
					if(LOGIC->FINALBOSS)
					{
						MessageDying *m= new MessageDying(msg->CollideObject ->CompositionTypeId);
						//CORE->BroadcastMessage(m);
						LOGIC->SendMessage(m);
						LOGIC->FINALBOSS=0;
						AUDIO->Stop("bosswalk");
						GetOwner()->Destroy(); 
					}
				}

				else if (msg->CollidedWith ->CompositionTypeId !=GOC_Type_Hero && msg->CollidedWith ->CompositionTypeId !=GOC_Type_ButtonPlatform1 && msg->CollidedWith ->CompositionTypeId !=GOC_Type_ButtonPlatform2)
				{
					if (m_Body->isCollidingLeft  || m_Body->isCollidingRight   )
					{
						if (m_Body->isCollidingLeft )
							currentCollideSide=-1;
						else
							currentCollideSide=1;
						//std::cout<<"CURRENT SIDE: "<<currentCollideSide<<std::endl;
						if(previousObstacle!=msg->CollidedWith || previousCollideSide!=currentCollideSide)
						{
							m_foundObstacle=true;
							previousObstacle=msg->CollidedWith ;
							previousCollideSide=currentCollideSide;
						}
					}
				}

			}
		}
	}
}
