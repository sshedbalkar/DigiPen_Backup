#pragma once
#include "Precompiled.h"

namespace Framework
{
class GameObjectComposition;
class Message;
class ISerializer;

class Script
{
public:
	virtual ~Script() {}
	virtual void Serialize( ISerializer& stream ) {};
	virtual void Initialize( GameObjectComposition* object ) = 0;
	virtual void Update( float timeslice ) = 0;
	virtual void SendMessage( Message* msg ) = 0;
};

}