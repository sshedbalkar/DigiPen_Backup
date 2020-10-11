#pragma once
//
#include "LuaLibrary.h"
#include <iostream>
#include "LuaHeaders.h"
#include "WindowsSystem.h"
//
namespace Framework
{
	class LuaMessage;
	//
	class ToLuaKBMouseMsgs
	{
	public:
		static int Open( lua_State* L )
		{
			luabind::module( L )
			[
				luabind::class_< MessageCharacterKey, luabind::bases<Message> >( "MessageCharacterKey" )
					.def_readonly( "character", &MessageCharacterKey::character ),
				//
				luabind::class_< MessageVirtualKey, luabind::bases<Message> >( "MessageVirtualKey" )
					.def_readonly( "character", &MessageVirtualKey::character ),
				//
				luabind::class_< MessageCharacterKeyRelease, luabind::bases<Message> >( "MessageCharacterKeyRelease" )
					.def_readonly( "character", &MessageCharacterKeyRelease::character ),
				//
				luabind::class_< LMouseButton, luabind::bases<Message> >( "LMouseButton" )
					.enum_( "constants" )
						[
							luabind::value( "LeftMouse", 0 ),
							luabind::value( "RightMouse", 1 )
						]
					.def_readonly( "MouseButtonIndexId", &LMouseButton::MouseButtonIndex )
					.def_readonly( "ButtonIsPressed", &LMouseButton::ButtonIsPressed )
					.def_readonly( "MousePosition", &LMouseButton::MousePosition ),
				//
				luabind::class_< RMouseButton, luabind::bases<Message> >( "RMouseButton" )
					.enum_( "constants" )
						[
							luabind::value( "LeftMouse", 0 ),
							luabind::value( "RightMouse", 1 )
						]
					.def_readonly( "MouseButtonIndexId", &RMouseButton::MouseButtonIndex )
					.def_readonly( "ButtonIsPressed", &RMouseButton::ButtonIsPressed )
					.def_readonly( "MousePosition", &RMouseButton::MousePosition ),
				//
				luabind::class_< MouseMove, luabind::bases<Message> >( "MouseMove" )
					.def_readonly( "MousePosition", &MouseMove::MousePosition ),
				//
				luabind::class_< MouseWheel, luabind::bases<Message> >( "MouseWheel" )
					.def_readonly( "Key", &MouseWheel::Key )
					.def_readonly( "Delta", &MouseWheel::Delta )
				//
			];
			//
			return 0;
		}
	};
}