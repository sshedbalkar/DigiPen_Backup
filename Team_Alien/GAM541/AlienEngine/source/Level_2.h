#ifndef LEVEL_2_H
#define LEVEL_2_H
//
#include "ILevel.h"
#include "LoadLevelClass.h"
//
namespace Framework
{
	class Level_2: public ILevel
	{
	public:
		Level_2();
		~Level_2();
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