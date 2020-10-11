#pragma once
//
#include "Message.h"
#include "Variant.h"
//
namespace Framework
{
	namespace Mid // Message id
	{
		/////////////////////////////////////////////////////
		#define MCR_REGISTER_MESSAGE_NAME(x) x,
		typedef enum
		{
			LMSG_INVALID = 0,
			#include "LuaMsgNames.h"
			LMSG_NUM
		} LMsgId;
		#undef MCR_REGISTER_MESSAGE_NAME
		///////////////////////////////////////////////////
		#define MCR_REGISTER_MESSAGE_NAME(x) #x,
		static const char* LMessageNameText[] =
		{
			"LMSG_Invalid",
			#include "LuaMsgNames.h"
			"LMsg_Invalid_Max_Num"
		};
		#undef MCR_REGISTER_MESSAGE_NAME
		///////////////////////////////////////////////////
	}
	//
	class LuaMessage: public Message
	{
	public:
							LuaMessage( void )
								:Message( Mid::MSG_LUA_MSG ),
								ID( Mid::LMSG_INVALID ),
								Obj( 0 )
							{
								//
							};
							//
							LuaMessage( Mid::LMsgId id, utility::Object obj = 0 )
								:Message( Mid::MSG_LUA_MSG ),
								ID( id ),
								Obj( obj )
							{
								//
							};
							//
							LuaMessage( const LuaMessage &orig )
								:Message( Mid::MSG_LUA_MSG ),
								ID( orig.ID ),
								Obj( orig.Obj )
							{
								//
							}
							//
	LuaMessage&				operator= ( const LuaMessage &rhs )
							{
								if( this != &rhs )
								{
									ID = rhs.ID;
									Obj = rhs.Obj;
								}
								return *this;
							}
							//
							~LuaMessage(){}
							//
	const char*				GetLName()
							{
								return Mid::LMessageNameText[ ID ];
							}
	//
	public:
		utility::Object		Obj;
		Mid::LMsgId			ID;
	};
}