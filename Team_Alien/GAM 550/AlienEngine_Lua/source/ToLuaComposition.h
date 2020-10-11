#pragma once
//
#include "Composition.h"
#include "LuaLibrary.h"
#include <iostream>
#include "LuaHeaders.h"
#include "ScriptComponent.h"
//
namespace Framework
{
	class ToLuaComposition
	{
	public:
		static int Open( lua_State* L, int* num )
		{
			/*
			luabind::module( L)
			[
				luabind::class_< GameObjectComposition >( "GOC" )
					.def( luabind::constructor<>() )
					.def( "GetComponetType", (ScriptComponent*(GameObjectComposition::*)(ComponentTypeId)) &GameObjectComposition::GetComponetType<ScriptComponent> )
			];
			*/
			//
			return 0;
		}
	};
}