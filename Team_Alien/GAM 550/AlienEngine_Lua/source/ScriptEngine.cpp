#include "Precompiled.h"
#include "LuaHeaders.h"
#include "ScriptEngine.h"
#include "ComponentCreator.h"
#include "Factory.h"
#include "ScriptComponent.h"
#include "LuaLibrary.h"
#include "WindowsSystem.h"
#include "Core.h"
//#include "resources.h"
//#include "PathHelper.h"
#include "CTestClass1.h"

#include "LuaLibrary.h"
#include <sstream>
#include <exception>
#include "LuaMessage.h"
//
namespace Framework
{
	ScriptEngine* SCRIPTENGINE = NULL;
	static int count = 0;
	//
	ScriptEngine::ScriptEngine()
		:L( NULL ),
		ObjTest( new CTestClass1() )
	{
		SCRIPTENGINE = this;
	}
	//
	ScriptEngine::~ScriptEngine()
	{
		this->Free();
	}
	//
	void print1(int number)
	{
		std::cout << "hello world " << number << "\n";
	}
	//
	void ScriptEngine::Initialize()
	{
		printf("Loading lua scripts...\n");
		RegisterComponent( ScriptComponent );
		//
		L = luaL_newstate();
		luaL_openlibs( L );
		luabind::open( L );
		//luabind::set_pcall_callback( &ScriptEngine::PCallError );
		//
		/*
		luabind::module( L )
		[
			luabind::def("print1", &print1)
		];
		*/
		//
		//float f = 1.234f;
		//LuaLibrary::Add( &ScriptEngine::TestFn, this, &f );
		//LuaLibrary::Open( L );
		//int i = 123;
		//LuaLibrary::Add( &ToLuaCTestClass1::Open, &i );
		LuaLibrary::Open( L );
		//
		if( OpenFile( "LuaScripts\\ScriptList.lua" ) == 0 )
		{
			CallWithErrorHandling( 0, 0 );
		}
		//
		if( OpenFile( "LuaScripts\\Global.lua" ) == 0 )
		{
			CallWithErrorHandling( 0, 0 );
		}
		//
		luabind::globals( L )[ "ObjTest" ] = ObjTest;
		luabind::call_function<void>( L, "Dummy1", 0.1f );
	}
	//
	int ScriptEngine::TestFn( lua_State* L, float* f )
	{
		std::cout<<"Float: "<<*f<<"\n";
		return 0;
	}
	//
	void ScriptEngine::Update( float timeslice )
	{
		++count;
		//return;
		try
		{
			/*
			if( count == 100 )
			{
				std::cout<<"Sending C2\n";
				Class2 *c2 = new Class2();
				c2->Var2 = 123;
				c2->Var3 = 246;
				//luabind::call_function<void>( L, "AddNewMessage", c2 );
			}
			*/
			//luabind::call_function<void>( L, "Dummy1", timeslice );
		}
		catch( const std::exception &error )
		{
			std::cout<<"[ERR: SCRIPT_ENG]: "<<error.what()<<"\n";
		}
	}
	//
	void ScriptEngine::Free()
	{
		if( L )
		{
			lua_close( L );
			L = NULL;
		}
	}
	//
	void ScriptEngine::Unload()
	{
		//
	}
	//
	void ScriptEngine::SendMessage( Message* msg )
	{
		return;
		if( msg->MessageId == Mid::MSG_LUA_MSG )
		{
			luabind::call_function<void>( L, "AddNewMessage", static_cast<LuaMessage*>(msg) );
		}
		else
		{
			//std::cout<<"M: "<<msg->GetName()<<"\n";
			switch( msg->MessageId )
			{
			case Mid::CharacterKey:
				{
					luabind::call_function<void>( L, "AddNewMessage", msg );
				}
				break;
			case Mid::CharacterKeyRelease:
				{
					luabind::call_function<void>( L, "AddNewMessage", msg );
				}
				break;
			case Mid::VirtualKey:
				{
					luabind::call_function<void>( L, "AddNewMessage", msg );
				}
				break;
			case Mid::LMouseButton:
				{
					luabind::call_function<void>( L, "AddNewMessage", msg );
				}
				break;
			case Mid::RMouseButton:
				{
					luabind::call_function<void>( L, "AddNewMessage", msg );
				}
				break;
			case Mid::MouseMove:
				{
					//std::cout<<"adding mouse move event: "<<msg->MessageId<<"\n";
					luabind::call_function<void>( L, "AddNewMessage", msg );
				}
				break;
			case Mid::MouseWheel:
				{
					luabind::call_function<void>( L, "AddNewMessage", msg );
				}
				break;
			}
		}
	}
	//
	void ScriptEngine::RunScript( GameObjectComposition* object, const char* script_id )
	{
		std::cout<<"Running script: "<<script_id<<"\n";
		//char str[100];
		//sprintf_s(str, "xyz: %d", 1);
		//fwrite(str, sizeof(char), 7, stdout);
		//OpenFile( script_id );
		try
		{
			CTestClass1 *o1 = new CTestClass1();
			luabind::call_function<void>( L, "ApplyScriptToObject", script_id, o1 );
		}
		catch( const std::exception &error )
		{
			std::cout<<"[ERR: SCRIPT_ENG]: "<<script_id<<": "<<error.what()<<"\n";
		}
		/*
		lua_getglobal( L, "ApplyScriptToObject" );
		lua_pushstring( L, script_id );
		//lua_pushinteger( L, static_cast<lua_Integer>(223344) );
		CTestClass1** lua_object = ( CTestClass1** )( lua_newuserdata(L, sizeof(void*)) );
		luaL_getmetatable( L, "CTestClass1" );
		lua_setmetatable( L, -2 );
		(*lua_object) = o1;
		//
		this->CallWithErrorHandling( 2, 0 );
		//
		lua_settop( L, 0 );
		*/
		
	}
	//
	void ScriptEngine::RegisterScript( const char* path )
	{
		//RESOURCES->AddResource( path, NULL, this, &ScriptEngine::ReloadScript );
	}
	//
	void ScriptEngine::ReloadScript( void* , const char* path )
	{
		/*
		lua_getglobal( L, "ReloadScript" );
		std::string name;
		PathHelper::PathGetFilename( path, name );
		lua_pushstring( L, name.c_str() );
		this->CallWithErrorHandling( 1, 0 );
		*/
	}
	//
	int ScriptEngine::OpenFile( const char* path )
	{
		
		int error = luaL_loadfile( L, path );
		//int error = luaL_dofile( L, path );
		if(error)
		{
			std::cout<<"loadinggggggggg";
			char err_msg[256];
			sprintf_s(err_msg, "%s", lua_tostring( L, -1));
			lua_pop(L, 1);
			printf( "[LUA]:%s\n", err_msg );
		}
		return error;
	}
	//
	int ScriptEngine::CallWithErrorHandling( int nargs, int nresults )
	{
		int error = lua_pcall( L, nargs, nresults, 0 );
		if (error) 
		{
			char err_msg[256];
			sprintf_s(err_msg, "%s", lua_tostring( L, -1));
			lua_pop(L, 1);
			printf( "[ERR:LUA]: %s\n", err_msg );
		}
		return error;
	}
	//
	int ScriptEngine::PCallError( lua_State* L )
	{
		lua_Debug d;
		lua_getstack( L, 1, &d );
		lua_getinfo( L, "Sln", &d );
		std::string err = lua_tostring( L, -1 );
		lua_pop( L, 1 );
		//
		std::stringstream msg;
		msg<<"["<<d.short_src << ":" << d.currentline<<"]: ";
		if(d.name != 0)
		{
			msg << "(" << d.namewhat << " " << d.name << ")";
		}
		msg << " " << err;
		lua_pushstring(L, msg.str().c_str());
		return 1;
	}
}
