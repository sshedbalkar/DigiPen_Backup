
#include "Precompiled.h"
#include "AI_Type1.h"
#include "Message.h"
#include "GameLogic.h"
#include "Audio.h"

namespace Framework
{



	AI_Type1::~AI_Type1()
	{
	}


	void AI_Type1::Initialize()
	{ 
		RangeToFollowHero=14.0f;
		m_EnemySpeedX=2.5f;
		m_followWhenOnDifferentY=false;
		m_Body = this->GetOwner()->has(Body);
		m_movingDirection=0;
		m_EnemyJumpSpeedY=7.5f;
		m_foundObstacle=false;
		previousCollideSide=0;
		m_distXfromHero=0.0f;
		yUp=m_Body->Position.y+static_cast<ShapeAAB*>(m_Body->BodyShape)->Extents.y;
		yDown=m_Body->Position.y-static_cast<ShapeAAB*>(m_Body->BodyShape)->Extents.y;
	}

	void AI_Type1::Jump()
	{
		m_Body->Velocity.y=m_EnemyJumpSpeedY;
	}


	
	float AI_Type1::CaclulateDistanceFromHeroX()
	{
		m_distXfromHero= abs(m_Body->Position.x-LOGIC->HERO->has(Body)->Position.x);
		return m_distXfromHero;
	}

	float AI_Type1::CaclulateDistanceFromHeroY()
	{
		return abs(m_Body->Position.y-LOGIC->HERO->has(Body)->Position.y);
	}

	float AI_Type1::CaclulateDistanceXFromOtherObj(float & x)
	{
		return abs(m_Body->Position.x - x);
	}


	void AI_Type1::DecideWhatToDo()
	{
		if(CheckIfHeroWithinRange()){
			if (CheckIfHeroHasSameY())
			{
				if(CheckIfEnemySeesHero())
				{
					m_State=FOLLOWING_HERO ;
				}
				else
				{
					m_State=GOTO_ORIGINALPOS ;
				}
			}
			else 
			{
				m_State=GOTO_ORIGINALPOS ;
			}
		}
		else
		{
			m_State=GOTO_ORIGINALPOS ;
		}

		if (m_State==GOTO_ORIGINALPOS)
		{
			if(m_Body->Position.x==m_Body->InitialPosition .x)
			{
				m_State=IDLE;
			}
		}
	}

	void AI_Type1::DoAction()
	{
		if (m_State==IDLE)
		{
			//do nothing..
		}
		else if (m_State==GOTO_ORIGINALPOS)
		{
			
			if(m_Body->Position.x<m_Body->InitialPosition.x+0.05 && m_Body->Position.x>m_Body->InitialPosition.x-0.05)
			{
				m_Body->Position.x=m_Body->InitialPosition .x;
				m_movingDirection=0;
				m_State=IDLE;
				//std::cout<<"AT ORIGINAL POS"<<std::endl;
			}
			else if(m_Body->Position.x<m_Body->InitialPosition .x)
				m_movingDirection=1;
			else if(m_Body->Position.x>m_Body->InitialPosition .x)
				m_movingDirection=-1;

			if(m_stuck && m_movingDirection==previousCollideSide)
			{
				if (m_Body->isCollidingLeft || m_Body->isCollidingRight)
				{
				//	m_movingDirection=0;
				}
			}

			if (m_foundObstacle)
			{
				Jump();
				m_foundObstacle=false;
			}

			m_Body->Velocity.x=m_EnemySpeedX*(float)m_movingDirection;
		}
		else if (m_State==FOLLOWING_HERO)
		{
			
			yUp=m_Body->Position.y+static_cast<ShapeAAB*>(m_Body->BodyShape)->Extents.y;
			yDown=m_Body->Position.y-static_cast<ShapeAAB*>(m_Body->BodyShape)->Extents.y;
			if(m_Body->Position.x<LOGIC->HERO->has(Body)->Position.x)
				m_movingDirection=1;
			else if(m_Body->Position.x>LOGIC->HERO->has(Body)->Position .x)
				m_movingDirection=-1;
			
			if(m_stuck && m_movingDirection==previousCollideSide)
			{
				if (m_Body->isCollidingLeft || m_Body->isCollidingRight){}
					//m_movingDirection=0;
			}
			
			m_Body->Velocity.x=m_EnemySpeedX*(float)m_movingDirection;
			
			if (m_foundObstacle)
			{
				Jump();
				m_foundObstacle=false;
			}
			if(!m_stuck)
				AUDIO->Play("danger");

		}

		if (!m_Body->isCollidingLeft &&  !m_Body->isCollidingRight)
		{
			previousObstacle=0;
			m_stuck=false;
		}
	}

	bool AI_Type1::CheckIfHeroWithinRange()
	{
		if(CaclulateDistanceFromHeroX()<RangeToFollowHero) 
		{
			return true;
		}
		else 
			return false;
	}

	bool AI_Type1::CheckIfHeroHasSameY()
	{
		if(yUp+4.4f > LOGIC->HERO->has(Body)->Position.y-static_cast<ShapeAAB*>(LOGIC->HERO->has(Body)->BodyShape)->Extents.y)//yup enemy > than ydown hero
			return true;
		else 
			return false;
	}

	bool AI_Type1::CheckIfEnemySeesHero() //NOT USED NOW, RETURNS TRUE ALWAYS
	{
		return true;
		//float heroHalfWidth,yHeroUp,yHeroDown,yObjUp,yObjDown;
		//
		//heroHalfWidth=static_cast<ShapeAAB*>(LOGIC->HERO->has(Body)->BodyShape)->Extents.x;
		//yHeroUp=LOGIC->HERO->has(Body)->Position.y+static_cast<ShapeAAB*>(LOGIC->HERO->has(Body)->BodyShape)->Extents.y;
		//yHeroDown=LOGIC->HERO->has(Body)->Position.y-static_cast<ShapeAAB*>(LOGIC->HERO->has(Body)->BodyShape)->Extents.y;

		//ObjectLinkList<Body>::iterator first= PHYSICS->Bodies.begin();
		//ObjectLinkList<Body>::iterator last= PHYSICS->Bodies.end();
		//
		//Body * body;
		//for(;first!=last;++first)
		//{
		//	if(first->GetOwner()->CompositionTypeId!=GOC_Type_Enemy1 )
		//		continue;

		//	body=first->GetOwner()->has(Body);
		//	if(!body)
		//		continue;
		//	
		//	if (CaclulateDistanceXFromOtherObj(body->Position.x) > RangeToFollowHero)
		//		continue;

		//	yObjUp = body->Position.y+static_cast<ShapeAAB*>(first->BodyShape)->Extents.x;
		//	yObjDown  = body->Position.y-static_cast<ShapeAAB*>(first->BodyShape)->Extents.x;

		//	//ONLY one vertical line from top to bottom that goes though the center is enough for the game
		//	if(LineIntersect(LOGIC->HERO->has(Body)->Position.x,LOGIC->HERO->has(Body)->Position.y,m_Body->Position.x,m_Body->Position.y,body->Position.x,yObjUp,body->Position.x,yObjDown))
		//	{
		//	}
		//}
	}
	

	void AI_Type1::Update(float dt)
	{
		static_cast<void>(dt);
		DecideWhatToDo();
		DoAction();
		m_timer=0.0f;
	}

	void AI_Type1::SendMessageA(Message *m)
	{
		if(m->MessageId== Mid::Collide)
		{

			MessageCollide * msg = (MessageCollide*)m;

			if (msg->CollideObject->CompositionTypeId ==GOC_Type_Enemy1  )
			{
				if (msg->CollidedWith ->CompositionTypeId !=GOC_Type_Hero && msg->CollidedWith ->CompositionTypeId !=GOC_Type_ButtonPlatform1 && msg->CollidedWith ->CompositionTypeId !=GOC_Type_ButtonPlatform2)
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
						else //this means u are stuck
						{
							m_stuck=true;
						}
					}
				}

			}
		}
	}
}
