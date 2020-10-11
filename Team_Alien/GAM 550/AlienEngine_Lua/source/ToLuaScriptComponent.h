#pragma once
//
#include "LuaLibrary.h"
#include <iostream>
#include "LuaHeaders.h"
#include "ScriptComponent.h"
//
namespace Framework
{
	class ToLuaScriptComponent
	{
	public:
		static int Open( lua_State* L )
		{
			luabind::module( L )
			[
				luabind::class_< ScriptComponent >( "ScriptComponent" )
					.def( luabind::constructor<>() )
					.def( "TestFn1", (int(ScriptComponent::*)(int)) &ScriptComponent::TestFn1 )
			];
			//
			return 0;
		}
	};
}