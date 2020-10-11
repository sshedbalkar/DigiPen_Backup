///////////////////////////////////////////////////////////////////////////////
///
///	\file Message.h
///	Object Messaging System used by engine for cross game object and system 
///	communication. Messages are passed as base class pointers and downcast 
///	to their derieved types by the receiver usually inside of a switch
///	block.
///
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

namespace Framework
{
	/*
	namespace Mid // Message id
	{
		enum MessageIdType
		{
			None,
			Quit,
			Collide,
			ToggleDebugInfo,
			CharacterKey,
			CharacterKeyRelease,
			VirtualKey,
			LMouseButton,
			RMouseButton,
			MouseMove,
			FileDrop,
			FrameByFrame,
			AdvanceFrame,
			MouseWheel,
			ActivateObj,
			Dying,
			GameWon,
			Menu,
			LeaveMenu,
			SystemPause
		};
	}
	*/
	//
	namespace Mid // Message id
	{
		/////////////////////////////////////////////////////
		#define MCR_REGISTER_MESSAGE_NAME(x) x,
		typedef enum
		{
			MSG_INVALID = 0,
			#include "MsgNames.h"
			MSG_NUM
		} MessageIdType;
		#undef MCR_REGISTER_MESSAGE_NAME
		///////////////////////////////////////////////////
		#define MCR_REGISTER_MESSAGE_NAME(x) #x,
		static const char* MessageNameText[] =
		{
			"MSG_Invalid",
			#include "MsgNames.h"
			"Invalid_Max_Num"
		};
		#undef MCR_REGISTER_MESSAGE_NAME
		///////////////////////////////////////////////////
	}
	//
	///Base message class. New message types are defined by deriving from this
	///class and mapping defined an element of the MessageIdType enumeration.
	class Message
	{
	public:
							Message(Mid::MessageIdType id): MessageId(id){};
							Message(const Message& rhs): MessageId(rhs.MessageId){};
		virtual				~Message(){};
		virtual Message*	clone(){return new Message(*this);};
		//
		const char*			GetName()
		{
			return Mid::MessageNameText[ MessageId ];
		}
		//
		Mid::MessageIdType	MessageId;
	};

}
