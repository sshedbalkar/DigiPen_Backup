#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"

namespace Framework
{
	class CheckPointComponent: public GameComponent
	{
		friend class LoadLevelClass;
		friend class SaveLevelClass;
	public:
		CheckPointComponent();
		~CheckPointComponent();
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream){};
		virtual void RemoveComponentFromGame();
		virtual void RestoreComponentToGame();
		Vec3 & GetHeroResetPos(){return m_resetPos;}

	private:
		Vec3 m_resetPos;
		
	};
}