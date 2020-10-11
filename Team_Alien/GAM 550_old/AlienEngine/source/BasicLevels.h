#ifndef BASICLEVELS_H
#define BASICLEVELS_H
//
#include "ILevel.h"
#include "GSM.h"
//
namespace Framework
{
	class Level_Restart: public ILevel
	{
	private:
		Level_Restart();
		~Level_Restart();
	public:
		//
		friend class GSM;
	};
	//
	class Level_Quit: public ILevel
	{
	private:
		Level_Quit();
		~Level_Quit();
	public:
		//
		friend class GSM;
	};
}
//
#endif BASICLEVELS_H