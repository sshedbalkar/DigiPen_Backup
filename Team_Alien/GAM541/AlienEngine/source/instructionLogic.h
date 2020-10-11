#pragma once
#include "Script.h"

namespace Framework
{
class Transform;
class VisualPlatform;

class InstructionLogic : public Script
{
private:
	int state;
	int inner_state;
	float timer;
	bool rdown;
	bool on_target;

	VisualPlatform* platform;
	Transform* transform;
	
public:
	virtual ~InstructionLogic();
	virtual void Initialize( GameObjectComposition* object );
	virtual void Update( float timeslice );
	virtual void SendMessage( Message* msg );
private:
	void CursorCheck();
};

}