#pragma once //Makes sure this header is only included once
#include <vector>
#include "

namespace Framework
{
	class PowerGun
	{
		//friend class GameLogic;
	public:
		PowerGun(){}
		~PowerGun(){}
		void update();		
		void SendMessage(Message *);
	private:
		enum m_powerState{power1,power2,power3,noOfPower};

	};
}