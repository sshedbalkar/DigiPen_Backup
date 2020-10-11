#pragma once
#include "Script.h"

namespace Framework
{
class Transform;
class Body;
class MeshInstance;

class HeroFacialEffect : public Script
{
private:
	int state;
	int target_state;
	int inner_state;
	float timer;
	Transform* transform;
	Body* body;
	MeshInstance* mesh;

public:
	virtual ~HeroFacialEffect();
	virtual void Initialize( GameObjectComposition* object );
	virtual void Update( float timeslice );
	virtual void SendMessage( Message* msg );
private:
};

}