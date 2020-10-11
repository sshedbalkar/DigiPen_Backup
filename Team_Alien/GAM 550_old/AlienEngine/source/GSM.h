#ifndef GSM_H
#define GSM_H
//
//#include "Core.h"
//
namespace Framework
{
	class CoreEngine;
	class ILevel;
	class LoadLevelClass;

	class GSM
	{
		typedef std::list<ILevel*> LevelListType;
	private:
		GSM();
		~GSM();
		//
		int levelLoad(void);
		int levelInitialize(void);
		int levelUpdate(float dt);
		int levelDraw(void);
		int levelFree(void);
		int levelUnload(void);
		//
	public:
		int initialize( );
		int update();
		//
		ILevel* next(void);
		ILevel* next(ILevel* level);
		ILevel* next( const char* filepath );
		ILevel* prev(void);
		ILevel* prev(ILevel* level);
		ILevel* current(void);
		ILevel* current(ILevel* level);
		//
		void deletePrevLevel();
		//
		friend class CoreEngine;
		//
	public:
		static ILevel* LEVEL_RESTART;
		static ILevel* LEVEL_QUIT;
		//
	private:
		ILevel* _next;
		ILevel* _prev;
		ILevel* _current;
		LoadLevelClass* level_loader;
		LevelListType levels;
	};
}
//
#endif GSM_H