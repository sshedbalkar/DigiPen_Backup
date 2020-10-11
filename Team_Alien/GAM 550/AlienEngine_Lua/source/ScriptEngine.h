#pragma once
//
#include "System.h"
//
struct lua_State;

namespace Framework
{
	class GameObjectComposition;
	class CTestClass1;
	//
	class ScriptEngine: public ISystem
	{
	public:
								ScriptEngine();
								~ScriptEngine();
		virtual void			Initialize();
		virtual void			Update( float timeslice );
		virtual void			Free();
		virtual void			Unload();
		virtual std::string		GetName() { return "Script Engine"; };
		virtual void			SendMessage( Message* msg );
	
		lua_State*				GetLuaState() { return L; };
		void					RunScript( GameObjectComposition* object, const char* script_id );
	
		void					RegisterScript( const char* name );
		void					ReloadScript( void* , const char* name );
		//
	private:
		int						OpenFile( const char* path );
		int						CallWithErrorHandling( int nargs, int nresults );
		int						TestFn( lua_State* L, float* f );
		static int				PCallError( lua_State* L );
		//
	private:
		lua_State*				L;
		CTestClass1*			ObjTest;
	};
	//
	extern ScriptEngine*		SCRIPTENGINE;
}
