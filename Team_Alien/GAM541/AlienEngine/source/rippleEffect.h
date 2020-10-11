#pragma once
#include "Script.h"

namespace Framework
{
class Transform;
class Body;
class MeshInstance;
class Water;

class RippleEffect : public Script
{
private:
	int state;
	int inner_state;
	float timer;

	Transform* water_trans;
	Transform* hero_trans;
	Transform* boss_trans;
	Water* mat_water;

public:
	virtual ~RippleEffect();
	virtual void Initialize( GameObjectComposition* object );
	virtual void Update( float timeslice );
	virtual void SendMessage( Message* msg );
private:
};

}