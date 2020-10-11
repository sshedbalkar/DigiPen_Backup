#pragma once

namespace Framework
{
class Script;

class ScriptCreator
{
public:
	virtual Script* Create() = 0;

};

template< class Type >
class ScriptCreatorType : public ScriptCreator
{
public:
	virtual Script* Create() { return new Type; }
};

}
