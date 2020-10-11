
#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "VMath.h"
#include "Body.h"
#include "WindowsSystem.h"
#include "MeshInstance.h"
#include "AI_Type1.h"

namespace Framework
{


	
	class HeroProperties : public GameComponent
	{
	public:
		HeroProperties():collidingDownWith(0),m_HeroSpeedX(0),m_HeroSpeedY(0),m_IsMoving(false),m_previousIsMoving(false){};
		~HeroProperties();

		void Jump();
		void SetHeroSpeed(float s){m_HeroSpeedX=s;}
		virtual void Initialize();
		virtual void SendMessage(Message *);
		float GetHeroSpeedX(){return m_HeroSpeedX;}
		float GetHeroSpeedY(){return m_HeroSpeedY;}
		bool GetHeroIsMoving(){return m_IsMoving;}
		//void SetHeroIsMoving(bool n ){m_IsMoving=n;}
		bool IsHeroAllowedToJump(){return m_AllowJump;}
		bool SetHeroAllowedToJump(bool n){m_AllowJump= n;}
		void Update(float dt);
		GOC* collidingDownWith;
		bool AllowMovement;
		void DisableHeroMovement();
	private:
		bool m_TurnRight;
		float m_HeroSpeedY;
		bool m_IsMoving;
		bool m_previousIsMoving;
		float m_HeroSpeedX;
		bool m_AllowJump;
		
	};
}