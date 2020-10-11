#pragma once //Makes sure this header is only included once
#include "PhysicsMath.h"
#include <vector>
#include "Composition.h"

namespace Framework
{
	class Magnet
	{
		//friend class GameLogic;
	public:
		Magnet(){a=NULL;b=NULL;}
		~Magnet(){}
		void Update();		
		void AddBody1(GOC* i){a=i;}		
		void AddBody2(GOC* i){b=i;}		
	private:
		GOC *a,*b;
	};
}