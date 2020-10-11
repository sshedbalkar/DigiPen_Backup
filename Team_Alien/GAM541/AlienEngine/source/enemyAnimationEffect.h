#pragma once
#include "Script.h"

namespace Framework
{
class Transform;
class Body;
class MeshInstance;

class EnemyAnimationEffect : public Script
{
private:
	int state;
	int inner_state;
	Transform* transform;
	Body* body;
	MeshInstance* mesh;
public:
	virtual ~EnemyAnimationEffect();
	virtual void Initialize( GameObjectComposition* object );
	virtual void Update( float timeslice );
	virtual void SendMessage( Message* msg );
private:
};

}