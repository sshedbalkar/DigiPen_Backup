#pragma once
#include "Script.h"

namespace Framework
{
class Transform;
class Body;

class BasicMovementEffect : public Script
{
private:
	Transform* transform;
	Body* body;
	float target_rotation;
	bool facing_right;
public:
	virtual ~BasicMovementEffect();
	virtual void Serialize( ISerializer& stream );
	virtual void Initialize( GameObjectComposition* object );
	virtual void Update( float timeslice );
	virtual void SendMessage( Message* msg );
private:
	void FixRotation();
};

}