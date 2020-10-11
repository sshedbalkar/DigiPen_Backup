#pragma once
//
#include "LuaLibrary.h"
#include <iostream>
#include "LuaHeaders.h"
#include "Factory.h"
//
namespace Framework
{
	class ToLuaFactory
	{
	public:
		static int Open( lua_State* L )
		{
			luabind::module( L )
			[
				luabind::class_< GameObjectFactory >( "GameObjectFactory" )
					.def( "SendMessage", (void(GameObjectFactory::*)(Message*)) &GameObjectFactory::SendMessage )
					.def( "CreateEmptyComposition", (GOC*(GameObjectFactory::*)(void)) &GameObjectFactory::CreateEmptyComposition )
			];
			//
			return 0;
		}
	};
}