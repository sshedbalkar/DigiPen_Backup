#pragma once
#include "Script.h"

namespace Framework
{
class Transform;
class Body;


class EndGameActions : public Script
{
private:
	Transform* transform;
	Transform* hero_trans;
	Body* hero_body;
	bool m_platformCreated;
	bool m_stopUpdating;
public:
	virtual ~EndGameActions();
	virtual void Serialize( ISerializer& stream );
	virtual void Initialize( GameObjectComposition* object );
	virtual void Update( float timeslice );
	virtual void SendMessage( Message* msg );

	
};

}