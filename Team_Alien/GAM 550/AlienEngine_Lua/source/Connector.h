#pragma once //Makes sure this header is only included once
#include <vector>
#include "Composition.h"

namespace Framework
{
	class Connector
	{
		//friend class GameLogic;
	public:
		Connector();
		~Connector(){}
		void Update();		
		void AddBody1(GOC* );
		void AddBody2(GOC* );
		void ConnectObjects();
		bool CheckIfCanConnect();
	private:
		GOC *ObjectA,*ObjectB, *Bridge;
	};
}