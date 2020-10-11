#include "Precompiled.h"
#include "lua.h"
#include "scriptEngine.h"
#include "ComponentCreator.h"
#include "Factory.h"
#include "scriptComponent.h"
#include "BasicGlues.h"
#include "WindowsSystem.h"
#include "Core.h"
#include "resources.h"
#include "PathHelper.h"

#include "luaLibrary.h"

namespace Framework
{

ScriptEngine* SCRIPTENGINE = NULL;

//==================================
ScriptEngine::ScriptEngine()
{
	SCRIPTENGINE = this;
}
//==================================
ScriptEngine::~ScriptEngine()
{
	this->Free();
}
//==================================
void ScriptEngine::Initialize()
{
	printf("Loading lua scripts...\n");
	RegisterComponent(ScriptComponent);

	L = lua_open();
	luaL_openlibs( L );
	
	LuaLibrary::Add( BasicGlues::Open, this );

	LuaLibrary::Open( L );
	
	if ( this->OpenFile( "scripts\\global.lua" ) == 0 )
	{
		this->CallWithErrorHandling( 0, 0 );
	}
	if ( this->OpenFile( "scripts\\registration.lua" ) == 0 )
	{
		this->CallWithErrorHandling( 0, 0 );
	}
	if ( this->OpenFile( "scripts\\constants.lua" ) == 0 )
	{
		this->CallWithErrorHandling( 0, 0 );
	}
	
	lua_getglobal( L, "GameEntrance" );
	this->CallWithErrorHandling( 0, 0 );
}
//==================================
void ScriptEngine::Update( float timeslice )
{
	lua_getglobal( L, "Update" );
	lua_pushnumber( L, timeslice );
	this->CallWithErrorHandling( 1, 0 );

//	printf("%d\n", lua_gettop(L) );
}
//==================================
void ScriptEngine::Free()
{
	if ( L )
	{
		lua_close( L );
		L = NULL;
	}
}
//==================================
void ScriptEngine::Unload()
{
	lua_getglobal( L, "Unload" );
	this->CallWithErrorHandling( 0, 0 );
}
//==================================
void ScriptEngine::SendMessage( Message* msg )
{
	lua_getglobal( L, "InputMessage" );

	switch ( msg->MessageId )
	{
	case Mid::Resolution:
		{
			Resolution* m = (Resolution*)msg;
			lua_pushstring( L, "resolution" );
			lua_pushnumber( L, m->w );
			lua_pushnumber( L, m->h );
			CallWithErrorHandling( 3, 0 );
			break;
		}
	case Mid::CharacterKey:
		{
			MessageVirtualKey* m = (MessageVirtualKey*)msg;
			lua_pushstring( L, "ckdown" );
			lua_pushnumber( L, m->character );
			lua_pushnumber( L, 0 );
			CallWithErrorHandling( 3, 0 );
			break;
		}
	case Mid::VirtualKey:
		{
			MessageVirtualKey* m = (MessageVirtualKey*)msg;
			lua_pushstring( L, "vkdown" );
			lua_pushnumber( L, m->character );
			lua_pushnumber( L, 0 );
			CallWithErrorHandling( 3, 0 );
			break;
		}
	case Mid::VirtualKeyRelease:
		{
			MessageVirtualKeyRelease* m = (MessageVirtualKeyRelease*)msg;
			lua_pushstring( L, "vkup" );
			lua_pushnumber( L, m->character );
			lua_pushnumber( L, 0 );
			CallWithErrorHandling( 3, 0 );
			break;
		}
	case Mid::MouseMove:
		{
			lua_pop( L, 1 );
			MouseMove* m = (MouseMove*)msg;
			lua_getglobal( L, "blackboard" );
			lua_pushstring( L, "mx" );
			lua_pushnumber( L, m->MousePosition.x );
			lua_settable( L, -3 );
			lua_pushstring( L, "my" );
			lua_pushnumber( L, m->MousePosition.y );
			lua_settable( L, -3 );
			lua_pop( L, 1 );
			break;
		}
	case Mid::LMouseButton:
		{
			LMouseButton* m = static_cast<LMouseButton*>( msg );
			lua_pushstring( L, m->ButtonIsPressed ? "ldown" : "lup" );
			CallWithErrorHandling( 1, 0 );
		}
		break;
	case Mid::RMouseButton:
		{
			RMouseButton* m = static_cast<RMouseButton*>( msg );
			lua_pushstring( L, m->ButtonIsPressed ? "rdown" : "rup" );
			CallWithErrorHandling( 1, 0 );
		}
		break;
	case Mid::MMouseButton:
		{
			MMouseButton* m = static_cast<MMouseButton*>( msg );
			lua_pushstring( L, m->ButtonIsPressed ? "mdown" : "mup" );
			CallWithErrorHandling( 1, 0 );
		}
		break;
	case Mid::MouseWheel:
		{
			MouseWheel* m = (MouseWheel*)msg;
			lua_pushstring( L, "mwheel" );
			lua_pushnumber( L, m->Delta );
			CallWithErrorHandling( 2, 0 );
			break;
		}
#ifdef ALLOW_DRAGNDROP
	case Mid::FileDrop:
		{
			FileDrop* m = (FileDrop*)msg;
			std::string ext;
			std::string path = m->FileName;
			PathHelper::PathGetExtension( path, ext );
			PathHelper::PathChangeExtension( path, "" );
			path.pop_back();

			lua_pushstring( L, "filedrop" );
			lua_pushstring( L, path.c_str() );
			lua_pushstring( L, ext.c_str() );
			this->CallWithErrorHandling( 3, 0 );
		}
		break;
#endif
	case Mid::FocusGain:
		lua_pop( L, 1 );
		lua_getglobal( L, "blackboard" );
		lua_pushstring( L, "focused" );
		lua_pushnumber( L, 1 );
		lua_settable( L, -3 );
		lua_pop( L, 1 );
		break;
	case Mid::FocusLose:
		lua_pop( L, 1 );
		lua_getglobal( L, "blackboard" );
		lua_pushstring( L, "focused" );
		lua_pushnumber( L, 0 );
		lua_settable( L, -3 );
		lua_pop( L, 1 );
		break;
	default:
		lua_pop( L, 1 );
		break;
	}
}
//=========================
void ScriptEngine::RunScript( GameObjectComposition* object, const char* script_id )
{
	lua_getglobal( L, "ApplyScriptToObject" );
	lua_pushstring( L, script_id );
	GOC** lua_object = (GOC**)(lua_newuserdata( L, sizeof(void*) ) );
	luaL_getmetatable( L, "LuaBook.goc" );
	lua_setmetatable( L, -2 );
	(*lua_object) = object;

	this->CallWithErrorHandling( 2, 0 );

	lua_settop( L, 0 );
}
//=========================
void ScriptEngine::RegisterScript( const char* path )
{
	RESOURCES->AddResource( path, NULL, this, &ScriptEngine::ReloadScript );
}
//=========================
void ScriptEngine::ReloadScript( void* , const char* path )
{
	lua_getglobal( L, "ReloadScript" );
	std::string name;
	PathHelper::PathGetFilename( path, name );
	lua_pushstring( L, name.c_str() );
	this->CallWithErrorHandling( 1, 0 );
}
//=========================
int ScriptEngine::OpenFile( const char* path )
{
	int error = luaL_loadfile( L, path );
	if (error) 
	{
		char err_msg[256];
		sprintf_s(err_msg, "%s", lua_tostring( L, -1));
		lua_pop(L, 1);
		printf( "%s\n", err_msg );
	}
	return error;
}
//=========================
int ScriptEngine::CallWithErrorHandling( int nargs, int nresults )
{
	int error = lua_pcall( L, nargs, nresults, 0 );
	if (error) 
	{
		char err_msg[256];
		sprintf_s(err_msg, "%s", lua_tostring( L, -1));
		lua_pop(L, 1);
		printf( "%s\n", err_msg );
	}
	return error;
}

}
