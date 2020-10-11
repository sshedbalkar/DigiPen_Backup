#pragma once

#include "Component.h"

namespace Framework
{

class ScriptComponent : public GameComponent
{
	std::deque<std::string> scripts;
public:
	ScriptComponent();
	virtual ~ScriptComponent();

	virtual void Serialize( ISerializer& _stream );
	virtual void Initialize();
	virtual void Free();

private:
};

}
