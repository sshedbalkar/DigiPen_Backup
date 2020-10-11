#pragma once

#include "Component.h"
#include <deque>
#include <map>

namespace Framework
{

class Script;
class LogicCreator;

class ScriptManager : public GameComponent
{
	typedef std::deque<Script*> ScriptListType;

private:
	ScriptListType scripts;
public:
	ScriptManager* Next;
	ScriptManager* Prev;

public:
	ScriptManager();
	virtual ~ScriptManager();

	virtual void Serialize( ISerializer& stream );
	virtual void Initialize();
	virtual void Free();

	virtual void SendMessage( Message* msg );
	void Update( float timeslice );

private:
	void SerializeAndAddLogic( std::string& id, ISerializer& stream );

};

}
