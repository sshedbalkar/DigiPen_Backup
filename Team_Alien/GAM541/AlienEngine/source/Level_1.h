#ifndef LEVEL_1_H
#define LEVEL_1_H
//
#include "ILevel.h"
#include "LoadLevelClass.h"
//
namespace Framework
{
	class Level_1: public ILevel
	{
	public:
		Level_1();
		~Level_1();
		//
		int load(void);
		int initialize(void);
		int update(float dt);
		int draw(void);
		int free(void);
		int unload(void);
		int id(void);
		int restart(void);
	private:
		float m_loadingEffectTimer;
		bool m_loadingEffectDone;
		LoadLevelClass *m_loader;
	};
}
//
#endif