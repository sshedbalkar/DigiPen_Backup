///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Body.h  Define Body GameComponent
///	
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "VMath.h"
#include "Collision.h"
#include <iostream>
#include <map>

namespace Framework
{
	///Body Component provides basic point physics dynamics including mass, 
	///velocity, forces, acceleration, and collision resolution.
	///Component will modify transform component attributes every frame.
	class Emitter;

	class MessageActivateObj : public Message
	{
	public:
		MessageActivateObj(GOC* c) : Message(Mid::ActivateObj), WhoSentIt(c),wasActivatedAfterMsg(false) {};
		
		GOC * WhoSentIt;
		bool wasActivatedAfterMsg;
	};

	class  ActivatedProperties{
		
	public:
		//int id;
							
	};
	
	
	class InteractiveObjProperties : public GameComponent
	{
	public:
		InteractiveObjProperties();
		~InteractiveObjProperties();

		virtual bool Dying();
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		void OpenDoor();
		void CloseDoor();
		void DoorAction();
		void MarkDoorOpen(bool b){m_MarkToOpen=b;}
		void MoveButtonDown(Message *m);
		void Update(const float &);
		void ButtonAction();
		int ResetOnLoad;
		int GetSecondaryObjID(){return m_ObjectSecondaryId;};
		Vec4 GetObjectActivatedByVec(){return m_ObjectActivatedByVec;}
	private:
		bool previousOpen;
		Vec4 m_ObjectActivatedByVec;
		int m_ObjectIdToActivate;
		int m_ObjectSecondaryId;
		float yScaleOriginal;
		float yBodyScaleOriginal;
		bool m_MarkToOpen;
		float m_minDoorScale;
		Body  * m_Body;
		Transform * m_Transform;
		ShapeAAB * m_BodyShape;
		Emitter* m_Emitter;
		GOC * m_owner;
	};
}