
#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "VMath.h"
#include "Body.h"
#include "WindowsSystem.h"
#include "MeshInstance.h"

namespace Framework
{
	enum EnemyStateMachine{
		IDLE=0,
		FOLLOWING_HERO,
		GOTO_ORIGINALPOS,
		TELEPORTING
	};

	class AI_Type1 : public GameComponent
	{
	public:
		AI_Type1():m_State(0),m_isFollowingHero(false),m_EnemySpeedX(0.0f),m_followWhenOnDifferentY(false),RangeToFollowHero(0.0f),m_timer(0.0f){};
		~AI_Type1();

		
		virtual void Initialize();
		virtual void SendMessage(Message *);
		
		float CaclulateDistanceFromHeroX();
		float CaclulateDistanceFromHeroY();
		float CaclulateDistanceXFromOtherObj(float & x);
		void DecideWhatToDo();
		void DoAction();
		bool CheckIfHeroWithinRange();
		bool CheckIfEnemySeesHero();
		bool CheckIfHeroHasSameY();
		void Update(float dt);
		void Jump();

		float GetEnemySpeedX(){return m_EnemySpeedX;}
		float SetEnemySpeedX(float &s){m_EnemySpeedX= s;}

	private:
		float m_distXfromHero;
		float RangeToFollowHero;
		int m_State;
		bool m_isFollowingHero;
		float m_EnemySpeedX;
		bool m_followWhenOnDifferentY;
		Body  * m_Body;
		float yUp;
		float yDown;
		int m_movingDirection;
		float m_EnemyJumpSpeedY;
		bool m_foundObstacle;
		GOC * previousObstacle;
		int previousCollideSide;
		int currentCollideSide;
		bool m_stuck;
		float m_timer;
	};
}