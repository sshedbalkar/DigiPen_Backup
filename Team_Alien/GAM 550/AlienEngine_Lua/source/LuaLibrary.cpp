#include "LuaLibrary.h"
#include "LuaHeaders.h"
#include "ToLuaComposition.h"
#include "ToLuaScriptComponent.h"
#include "ToLuaFactory.h"
#include "ToLuaCTestClass1.h"
#include "ToLuaLuaMessage.h"
#include "ToLuaUtility.h"
#include "ToLuaKBMouseMsgs.h"
//
namespace Framework
{
	LuaLibrary::LuaFuncList LuaLibrary::m_function_list;
	//
	LuaLibrary::LuaLibrary()
	{
		//
	}
	//
	LuaLibrary::~LuaLibrary()
	{
		for each( Method<void, void>* m in m_function_list )
		{
			delete m;
		}
	}
	//
	void LuaLibrary::Open( lua_State* L )
	{
		OpenAllLibs();
		//
		for each( Method<void, void>* m in m_function_list )
		{
			utility::ArgumentVector v1;
			v1.push_back( L );
			v1.push_back( m->data );
			//
			utility::Variant &r = m->func->Invoke( m->object, v1 );
			if( r.As<int>() != RET_SUCCESS )
			{
				std::cout<<"[ERR] LuaLibrary::Open: Failed\n";
			}
		}
	}
	void LuaLibrary::OpenAllLibs( void )
	{
		int i = 123;
		LuaLibrary::Add( &ToLuaScriptComponent::Open );
		LuaLibrary::Add( &ToLuaFactory::Open );
		LuaLibrary::Add( &ToLuaCTestClass1::Open, &i );
		//LuaLibrary::Add( &ToLuaLuaMessage::Open );
		//LuaLibrary::Add( &ToLuaUtility::Open );
		//LuaLibrary::Add( &ToLuaKBMouseMsgs::Open );
	}
}