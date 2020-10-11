#pragma once //Makes sure this header is only included once
#include "Engine.h"
#include "VMath.h"


namespace Framework
{
	
	class LevelEditor : public ISystem
	{
	public:
		LevelEditor():GrabbedObject(0),m_enabled(false),m_screenMove(false),m_bodymove(false),m_EditScale(false){}
		~LevelEditor(){};
		virtual std::string GetName(){return "LevelEditor";}
		virtual void SendMessage(Message *);
		void Update(float t);

	private:
		GOC* GrabbedObject;
		bool m_enabled;
	    bool m_screenMove;
	    bool m_bodymove;
	    bool m_EditScale;
		Vec2 BodyMouseDiff,ScaleVec;
	};
}