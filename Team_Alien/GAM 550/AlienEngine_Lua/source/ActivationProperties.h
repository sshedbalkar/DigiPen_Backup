#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "VMath.h"
#include "Collision.h"
#include <iostream>
#include <map>
#include "TweakBarManager.h"

namespace Framework
{
	///Body Component provides basic point physics dynamics including mass, 
	///velocity, forces, acceleration, and collision resolution.
	///Component will modify transform component attributes every frame.

	class MessageActivateObj : public Message
	{
	public:
		MessageActivateObj(GOC* c) : Message(Mid::ActivateObj), WhoSentIt(c),wasActivatedAfterMsg(false) {};
		GOC * WhoSentIt;
		bool wasActivatedAfterMsg;
	};


	
	class ActivationProperties : public GameComponent
	{
		friend class TweakBarManager;
		friend class LoadLevelClass;
	public:
		ActivationProperties();
		~ActivationProperties();

		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		void Update(const float &);
		void Activate();
		int GetActivationId(){return m_ActivationId;};
		Vec4 GetObjectActivatedByVec(){return m_ActivatedByVec;}
	private:
		int m_ResetOnLoad;
		bool previousOpen;
		Vec4 m_ActivatedByVec;
		int m_ActivationId;
		bool m_MarkToOpen;
		GOC * m_owner;
	};
}