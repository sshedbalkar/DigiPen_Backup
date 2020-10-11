#pragma once
//
#include "LuaMessage.h"
#include "LuaLibrary.h"
#include <iostream>
#include "LuaHeaders.h"
//
static int counter = -1;
namespace Framework
{
	class ToLuaLuaMessage
	{
	public:
		static int Open( lua_State* L )
		{
			#define MCR_REGISTER_MESSAGE_NAME(x) CreateVal(#x),
			luabind::module( L )
			[
				luabind::class_< Message >( "Message" )
					.def_readonly( "MessageId", &Message::MessageId )
					.enum_( "constants" )
						[
							luabind::value( Mid::MessageNameText[ 0 ], 0 ),
							#include "MsgNames.h"
							luabind::value( Mid::MessageNameText[ ++counter ], counter )
						]
			];
			counter = -1;
			luabind::module( L )
			[
				luabind::class_< LuaMessage, luabind::bases<Message> >( "LuaMessage" )
					.def_readonly( "Obj", &LuaMessage::Obj )
					.def_readonly( "ID", &LuaMessage::ID )
					.def( "GetLName", (const char*(LuaMessage::*)(void)) &LuaMessage::GetLName )
					.enum_( "constants" )
						[
							luabind::value( Mid::LMessageNameText[ 0 ], 0 ),
							#include "LuaMsgNames.h"
							luabind::value( Mid::LMessageNameText[ ++counter ], counter )
						]
			];
			#undef MCR_REGISTER_MESSAGE_NAME
			//
			return 0;
		}
		//
	private:
		static luabind::value CreateVal( const char* eval )
		{
			std::cout<<"Adding: "<<eval<<": "<<counter+1<<"\n";
			return luabind::value( eval, ++counter);
		}
	};
}