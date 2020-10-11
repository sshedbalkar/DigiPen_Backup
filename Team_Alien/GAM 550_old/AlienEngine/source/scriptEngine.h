#pragma once

#include "System.h"

struct lua_State;

namespace Framework
{

class GameObjectComposition;

class ScriptEngine : public ISystem
{
private:
	lua_State* L;
public:
	ScriptEngine();
	~ScriptEngine();
	virtual void Initialize();
	virtual void Update( float timeslice );
	virtual void Free();
	virtual void Unload();
	virtual std::string GetName() { return "script engine"; };
	virtual void SendMessage( Message* msg );
	
	lua_State* GetLuaState() { return L; };
	void RunScript( GameObjectComposition* object, const char* script_id );
	
	void RegisterScript( const char* name );
	void ReloadScript( void* , const char* name );

private:
	int OpenFile( const char* path );
	int CallWithErrorHandling( int nargs, int nresults );
};


extern ScriptEngine* SCRIPTENGINE;


}
