#pragma once
//
#include "LuaLibrary.h"
#include <iostream>
#include "LuaHeaders.h"
#include "Utility.h"
#include "VMath.h"
//
namespace Framework
{
	class LuaMessage;
	//
	class ToLuaUtility
	{
	public:
		static int Open( lua_State* L )
		{
			luabind::module( L )
			[
				luabind::class_< D3DXVECTOR2 >( "Vec2" )
					.def_readwrite( "x", &D3DXVECTOR2::x )
					.def_readwrite( "y", &D3DXVECTOR2::y )
			];
			//
			return 0;
		}
	};
}