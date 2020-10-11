#include "Rays.h"
#include "GameLogic.h"
#include <ctime>
#include <iostream>
#include "HeroProperties.h"
#include "particleEmitter.h"
#include "AI_Type2.h"
#include "Audio.h"

namespace Framework
{
	Rays *RAYS=NULL;

	Rays::Rays()
	{ 
		numberOfRays=0;
		line_list=new LineListType();
		line_list_boss=new LineListType();
		GRAPHICS->RegisterLineList(line_list);
		GRAPHICS->RegisterBeamColor(&m_BeamColor);
		GRAPHICS->RegisterLineList2(line_list_boss);
		lastbody=NULL;
		BossLastBody=NULL;
		m_IsRayTouchingTeleportable=false;
		m_currentTimer=0.0f;
		m_BossTimer=0.0;
		m_TimeToTeleport= 0.6f;
		m_AllowTeleport=false;
		m_AllowDrawRays=false;
		m_BeamColor= Vec4( 0.9f, 0.3f, 0.05f, 1.0f );	
		RAYS=this;
	}

	void Rays::SetNewBeamColor(Colors c)
	{
		if(c==COLORS_Normal)		   {m_BeamColor.x=0.95f; m_BeamColor.y=0.7f; m_BeamColor.z=0.6f; m_BeamColor.w=1.0f;}
		else if(c==COLORS_Connecting)  {m_BeamColor.x=0.4f; m_BeamColor.y=0.5f; m_BeamColor.z=0.95f; m_BeamColor.w=1.0f;}
		else if(c==COLORS_Teleporting) {m_BeamColor.x=0.9f; m_BeamColor.y=0.9f; m_BeamColor.z=0.9f; m_BeamColor.w=1.0f;}
	}

	void Rays::Update(Vec2 heroPos,Vec2 mousePos,float dt)
	{
		line_list->clear();
		line_list_boss->clear();
		//boss first ray
		if(LOGIC->FINALBOSS)
		{
			if((LOGIC->FINALBOSS->has(AI_Type2)->GetState()==TELEPORTING || heroPos.x>466) && (LOGIC->HERO->has(HeroProperties)->AllowMovement) && (LOGIC->FINALBOSS->has(Body)->isCollidingDown))
			{
				Line line;
				line.start.x=LOGIC->FINALBOSS->has(Body)->Position.x;
				line.start.y=LOGIC->FINALBOSS->has(Body)->Position.y+1.5f;
				if(LOGIC->FINALBOSS->has(AI_Type2)->GetState()==TELEPORTING)
					line.end=Vec2(489,7.5);
				else if(heroPos.x>466)
					line.end=heroPos;
				line.visible = true;
				line.colliding_side = 0;
				line_list_boss->push_back(line);
				BossLastBody=NULL;
				BossRayCollisionCheck();
				LineListType::iterator it;
				it=line_list_boss->begin();
				
				if(BossLastBody)
				{
					AUDIO->Stop("beep");
					if(BossLastBody->CompositionTypeId==GOC_Type_MovingBox1)
					{
						if(m_BossTimer<0.3)
							m_BossTimer+=dt;
						else
						{
							BossLastBody->has(Body)->Position=Vec2(493.5,16);
							AUDIO->Play("boss_teleporting_hero");
							m_BossTimer=0.0f;
						}
					}else
						m_BossTimer=0.0f;
				}else
				{									
					if(it->end==heroPos)
					{
						AUDIO->Play("beep");
						if(m_BossTimer<0.5)
							m_BossTimer+=dt;
						else
						{
							LOGIC->HERO->has(Body)->Position=Vec2(493.5,16);
							LOGIC->HERO->has(HeroProperties)->DisableHeroMovement();
							AUDIO->Stop("beep");
							m_BossTimer=0.0f;
							AUDIO->Play("boss_teleporting_hero");
						}
					}else
						m_BossTimer=0.0f;
				}
			}else
				m_BossTimer=0.0f;
		}

		//normal hero rays
		//if(!m_AllowDrawRays)
		//	return;
		lastbody=NULL;
		Line line;
		line.start.x=heroPos.x;
		line.start.y=heroPos.y+1.1f;
		line.end=mousePos;
		line.visible = m_AllowDrawRays;
		line.colliding_side = 0;
		line_list->push_back(line);
		numberOfRays=line_list->size();
		unsigned int i=0;
		m_IsRayTouchingTeleportable=false;
		
		while(i<line_list->size())
		{
			CollisionCheck();
			numberOfRays=line_list->size();
			++i;
			if(numberOfRays>5) break;
		}

		if(lastbody && (lastbody->CompositionTypeId==GOC_Type_Enemy1 || lastbody->CompositionTypeId==GOC_Type_Boss1  ||lastbody->CompositionTypeId==GOC_Type_MovingBox1 || lastbody->CompositionTypeId==GOC_Type_Pillar))
		{
			SetNewBeamColor(COLORS_Connecting);
			m_IsRayTouchingTeleportable=true;
		}
		else
		{
			SetNewBeamColor(COLORS_Normal);
		}

		if (m_AllowTeleport && m_IsRayTouchingTeleportable && !LOGIC->HERO->has(HeroProperties)->GetHeroIsMoving() && 
			LOGIC->HERO->has(HeroProperties)->collidingDownWith!=lastbody)
		{
			//std::cout<<m_currentTimer<<std::endl;
			if(m_TimeToTeleport<m_currentTimer )
			{
				Teleport();
				m_AllowTeleport=false;
				m_currentTimer=0.0f;//reset timer
			}
			else
			{
				SetNewBeamColor(COLORS_Teleporting);
				m_currentTimer+=dt;
			}
			LOGIC->HERO->has(ParticleEmitter)->GetEmitter("teleporting_effect")->Activate();
		}
		else{
			LOGIC->HERO->has(ParticleEmitter)->GetEmitter("teleporting_effect")->Activate(false);
			m_currentTimer=0.0f;
			m_AllowTeleport=false;
			AUDIO->Stop("teleport");
		}
		
	}
	
	void Rays::CollisionCheck()
	{
		ObjectLinkList<Body>::iterator first= PHYSICS->Bodies.begin();
		ObjectLinkList<Body>::iterator last= PHYSICS->Bodies.end();
		LineListType::iterator it;
		it=line_list->begin();
		Vec2 test,reflect;
		float x=0.0f,y=0.0f;
		test=(it+(numberOfRays-1))->end;
		GOC* temp=0;
		
		int colliding_side = 0;
		for(;first!=last;++first)
		{
			if(!(first->GetOwner()->CompositionTypeId==GOC_Type_Hero || first->GetOwner()->CompositionTypeId==GOC_Type_InvisibleBoxOfDeath || !first->GetOwner()->has(Body)->isCollidable ))
			{
				if(GRAPHICS->FrustumTest(first->GetOwner()->has(Transform)))
				{
					float halfWidth,halfHeight;
					halfWidth=static_cast<ShapeAAB*>(first->BodyShape)->Extents.x;
					halfHeight=static_cast<ShapeAAB*>(first->BodyShape)->Extents.y;

					//lower side of the box
					y=first->Position.y-halfHeight;
					x=FindX(y);
					if(x<=(first->Position.x+halfWidth) && x>=(first->Position.x-halfWidth))
					{
						if(SquaredDistance((it+(numberOfRays-1))->start,Vec2(x,y))<=SquaredDistance((it+(numberOfRays-1))->start,test)
							&& SquaredDistance((it+(numberOfRays-1))->start,(it+(numberOfRays-1))->end)>=SquaredDistance((it+(numberOfRays-1))->end,Vec2(x,y))
							&& SquaredDistance((it+(numberOfRays-1))->start,Vec2(x,y))>=0.01)
						{
							test=Vec2(x,y);
							reflect.x=x+(x-(it+(numberOfRays-1))->start.x);
							temp=first->GetOwner();
							reflect.y=(it+(numberOfRays-1))->start.y;
							colliding_side = 1;
						}
					
					}

					//top of the box
					y=first->Position.y+halfHeight;
					x=FindX(y);
					if(x<=first->Position.x+halfWidth && x>=first->Position.x-halfWidth)
					{
						if(SquaredDistance((it+(numberOfRays-1))->start,Vec2(x,y))<=SquaredDistance((it+(numberOfRays-1))->start,test)
							&& SquaredDistance((it+(numberOfRays-1))->start,(it+(numberOfRays-1))->end)>=SquaredDistance((it+(numberOfRays-1))->end,Vec2(x,y))
							&& SquaredDistance((it+(numberOfRays-1))->start,Vec2(x,y))>=0.01)
						{
							test=Vec2(x,y);
							reflect.x=x+(x-(it+(numberOfRays-1))->start.x);
							reflect.y=(it+(numberOfRays-1))->start.y;
							temp=first->GetOwner();
							colliding_side = 2;
						}
					}

					//left side of the box
					x=first->Position.x-halfWidth;
					y=FindY(x);
					if(y<=first->Position.y+halfHeight && y>=first->Position.y-halfHeight)
					{
						if(SquaredDistance((it+(numberOfRays-1))->start,Vec2(x,y))<=SquaredDistance((it+(numberOfRays-1))->start,test)
							&& SquaredDistance((it+(numberOfRays-1))->start,(it+(numberOfRays-1))->end)>=SquaredDistance((it+(numberOfRays-1))->end,Vec2(x,y))
							&& SquaredDistance((it+(numberOfRays-1))->start,Vec2(x,y))>=0.01)
						{
							test=Vec2(x,y);
							reflect.x=(it+(numberOfRays-1))->start.x;
							reflect.y=y+(y-(it+(numberOfRays-1))->start.y);
							temp=first->GetOwner();
							colliding_side = 3;
						}
					}

					//right side of the box
					x=first->Position.x+halfWidth;
					y=FindY(x);
					if(y<=first->Position.y+halfHeight && y>=first->Position.y-halfHeight)
					{
						if(SquaredDistance((it+(numberOfRays-1))->start,Vec2(x,y))<=SquaredDistance((it+(numberOfRays-1))->start,test)
							&& SquaredDistance((it+(numberOfRays-1))->start,(it+(numberOfRays-1))->end)>=SquaredDistance((it+(numberOfRays-1))->end,Vec2(x,y))
							&& SquaredDistance((it+(numberOfRays-1))->start,Vec2(x,y))>=0.01)
						{
							test=Vec2(x,y);
							reflect.x=(it+(numberOfRays-1))->start.x;		
							reflect.y=y+(y-(it+(numberOfRays-1))->start.y);
							temp=first->GetOwner();
							colliding_side = 4;
						}
					}
				}
			}
		}
		if((it+(numberOfRays-1))->end!=test && (it+(numberOfRays-1))->start!=test)
		{
			(it+(numberOfRays-1))->end=test;
			(it+(numberOfRays-1))->colliding_side = colliding_side;
			lastbody=temp;
			if(temp->CompositionTypeId==GOC_Type_Mirror)
			{
				Line line;
				line.start=test;
				Vec2 temp1=reflect-test;
				normalize(temp1);
				line.end=test+temp1*40;
				line.visible = m_AllowDrawRays;
				line.colliding_side = colliding_side;
				line_list->push_back(line);
			}
			temp=NULL;
		}
	}

	void Rays::BossRayCollisionCheck()
	{
		ObjectLinkList<Body>::iterator first= PHYSICS->Bodies.begin();
		ObjectLinkList<Body>::iterator last= PHYSICS->Bodies.end();
		LineListType::iterator it;
		it=line_list_boss->begin();
		Vec2 test;
		float x=0.0f,y=0.0f;
		test=it->end;
		int colliding_side = 0;
		
		for(;first!=last;++first)
		{
			if(!(first->GetOwner()->CompositionTypeId==GOC_Type_Hero || first->GetOwner()->CompositionTypeId==GOC_Type_Boss1 || first->GetOwner()->CompositionTypeId==GOC_Type_InvisibleBoxOfDeath || !first->GetOwner()->has(Body)->isCollidable ))
			{
				if(GRAPHICS->FrustumTest(first->GetOwner()->has(Transform)))
				{
					float halfWidth,halfHeight;
					halfWidth=static_cast<ShapeAAB*>(first->BodyShape)->Extents.x;
					halfHeight=static_cast<ShapeAAB*>(first->BodyShape)->Extents.y;

					//lower side of the box
					y=first->Position.y-halfHeight;
					x=FindX1(y);
					if(x<=(first->Position.x+halfWidth) && x>=(first->Position.x-halfWidth))
					{
						if(SquaredDistance(it->start,Vec2(x,y))<=SquaredDistance(it->start,test)
							&& SquaredDistance(it->start,it->end)>=SquaredDistance(it->end,Vec2(x,y))
							&& SquaredDistance(it->start,Vec2(x,y))>=0.01)
						{
							colliding_side = 1;
							test=Vec2(x,y);
							BossLastBody=first->GetOwner();
						}
					}

					//top of the box
					y=first->Position.y+halfHeight;
					x=FindX1(y);
					if(x<=first->Position.x+halfWidth && x>=first->Position.x-halfWidth)
					{
						if(SquaredDistance(it->start,Vec2(x,y))<=SquaredDistance(it->start,test)
							&& SquaredDistance(it->start,it->end)>=SquaredDistance(it->end,Vec2(x,y))
							&& SquaredDistance(it->start,Vec2(x,y))>=0.01)
						{
							test=Vec2(x,y);
							BossLastBody=first->GetOwner();
							colliding_side = 2;
						}
					}

					//left side of the box
					x=first->Position.x-halfWidth;
					y=FindY1(x);
					if(y<=first->Position.y+halfHeight && y>=first->Position.y-halfHeight)
					{
						if(SquaredDistance(it->start,Vec2(x,y))<=SquaredDistance(it->start,test)
							&& SquaredDistance(it->start,it->end)>=SquaredDistance(it->end,Vec2(x,y))
							&& SquaredDistance(it->start,Vec2(x,y))>=0.01)
						{
							test=Vec2(x,y);
							BossLastBody=first->GetOwner();
							colliding_side = 3;
						}
					}

					//right side of the box
					x=first->Position.x+halfWidth;
					y=FindY1(x);
					if(y<=first->Position.y+halfHeight && y>=first->Position.y-halfHeight)
					{
						if(SquaredDistance(it->start,Vec2(x,y))<=SquaredDistance(it->start,test)
							&& SquaredDistance(it->start,it->end)>=SquaredDistance(it->end,Vec2(x,y))
							&& SquaredDistance(it->start,Vec2(x,y))>=0.01)
						{
							test=Vec2(x,y);
							BossLastBody=first->GetOwner();
							colliding_side = 4;
						}
					}
				}
			}
		}
		if(it->end!=test && it->start!=test)
		{
			it->end=test;
			it->colliding_side = colliding_side;
		}
	}

	float Rays::FindX(float y)
	{
		LineListType::iterator it;
		it=line_list->begin();
		float slope=(((it+(numberOfRays-1))->end.y - (it+(numberOfRays-1))->start.y)/((it+(numberOfRays-1))->end.x - (it+(numberOfRays-1))->start.x));
		return ( (y-(it+(numberOfRays-1))->end.y) * (1/slope) + (it+(numberOfRays-1))->end.x  ); 
	}

	float Rays::FindY(float x)
	{
		LineListType::iterator it;
		it=line_list->begin();
		float slope=(((it+(numberOfRays-1))->end.y - (it+(numberOfRays-1))->start.y)/((it+(numberOfRays-1))->end.x - (it+(numberOfRays-1))->start.x));
		return ( (x-(it+(numberOfRays-1))->end.x) * (slope) + (it+(numberOfRays-1))->end.y  ); 
	}

	float Rays::FindX1(float y)
	{
		LineListType::iterator it;
		it=line_list_boss->begin();
		float slope=((it->end.y - it->start.y)/(it->end.x - it->start.x));
		return ( (y-it->end.y) * (1/slope) + it->end.x  ); 
	}

	float Rays::FindY1(float x)
	{
		LineListType::iterator it;
		it=line_list_boss->begin();
		float slope=((it->end.y - it->start.y)/(it->end.x - it->start.x));
		return ( (x-it->end.x) * (slope) + it->end.y  ); 
	}

	float Rays::SquaredDistance(Vec2 a,Vec2 b)
	{
		return((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
	}

	void Rays::DestroyRays()
	{
		if(line_list!=NULL)
		{
			line_list->clear();
			lastbody=NULL;
		}
		return;
	}

	void Rays::StartTeleportation (bool t)
	{
		m_AllowTeleport=t;
		if (!m_AllowTeleport)
			m_currentTimer=0.0f;//reset timer

	}


	void Rays::Teleport()
	{
		if(lastbody)
		{
			if(lastbody->CompositionTypeId==GOC_Type_Enemy1 || lastbody->CompositionTypeId==GOC_Type_MovingBox1 || lastbody->CompositionTypeId==GOC_Type_Boss1 )
			{
				Vec2 temp=LOGIC->HERO->has(Body)->Position;
				LOGIC->HERO->has(Body)->Position=lastbody->has(Body)->Position;
				lastbody->has(Body)->Position=temp;
				//LOGIC->HERO->has(Body)->Position.y-=1;
				LOGIC->HERO->has(ParticleEmitter)->GetEmitter("teleporting_effect")->Activate(false);
			}
			else if(lastbody->CompositionTypeId==GOC_Type_Pillar )
			{
				LOGIC->HERO->has(Body)->Position=lastbody->has(Body)->Position;
				LOGIC->HERO->has(ParticleEmitter)->GetEmitter("teleporting_effect")->Activate(false);
			}
			
		}
	}

	bool Rays::IsLastBodyABlock()
	{
		if(lastbody)
		{
			if(lastbody->CompositionTypeId==GOC_Type_MovingBox1)
				return true;
			else return false;
		}
		else 
			return false;
	}

}