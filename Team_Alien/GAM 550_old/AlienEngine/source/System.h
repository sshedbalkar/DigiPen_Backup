///////////////////////////////////////////////////////////////////////////////////////
///
///	\file System.h Defines the system interface.
///
///	Authors:  
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

namespace Framework
{
	///System is a pure virtual base class (which is to say, an interface) that is
	///the base class for all systems used by the game. 
	
	class Message;

	class ISystem
	{
	public:
		///Systems can receive all message send to the Core. 
		///See Message.h for details.
		virtual void SendMessage(Message* message) {}

		///All systems are updated every game frame.
		virtual void Update(float timeslice) = 0;	

		///All systems provide a string name for debugging.
		virtual std::string GetName() = 0;	

		///Initialize the system.
		virtual void Initialize(){};

		///load level
		virtual void Load(){}

		///unload level
		virtual void Unload(){}

		///free(un-initialize)
		virtual void Free(){}

		///All systems need a virtual destructor to have their destructor called 
		virtual ~ISystem(){}						
	};
}