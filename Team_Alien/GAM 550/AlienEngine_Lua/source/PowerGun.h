#pragma once //Makes sure this header is only included once
#include <vector>
#include "Message.h"
#include "BoomBoom.h"
#include "Magnet.h"
#include "Connector.h"

namespace Framework
{
	class PowerGun
	{
		//friend class GameLogic;
	public:
		PowerGun(){}
		~PowerGun(){}
		void Update();	
	    void SendMessage(Message *);
		BoomBoom bombs;	
		Magnet magnet;
		Connector connector;
	private:
		enum powerState{powerConnect,powerBomb,powerMagnet,noOfPower} m_state;
		
	};
}