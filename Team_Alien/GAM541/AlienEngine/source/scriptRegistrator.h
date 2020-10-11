#pragma once

#include <map>

namespace Framework
{

class ScriptCreator;

class ScriptRegistrator : public std::map<std::string, ScriptCreator*>
{
public:
	void Initialize();
	void Free();
};

}
