
#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "VMath.h"
#include "Body.h"
#include "WindowsSystem.h"
#include "MeshInstance.h"

namespace Framework
{

	class AI_Type2 : public GameComponent
	{
	public:
		AI_Type2():m_State(0),m_foundObstacle(false),m_timer(0.0f){};
		~AI_Type2();

		
		virtual void Initialize();
		virtual void SendMessage(Message *);
		
		void DecideWhatToDo();
		void DoAction(float & dt);
		
		void Update(float dt);
		void Teleport();
		void Jump();
		float GetEnemySpeedX(){return m_EnemySpeedX;}
		float SetEnemySpeedX(float &s){m_EnemySpeedX= s;}
		int GetState(){return m_State;}

	private:
		int m_State;
		float m_EnemySpeedX;
		Body *m_Body;
		int m_movingDirection;
		bool m_foundObstacle;
		float m_timer;
		float m_EnemyJumpSpeedY;
		GOC * previousObstacle;
		int previousCollideSide;
		int currentCollideSide;
		bool m_foundGap;
	};
}
