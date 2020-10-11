#include "Precompiled.h"
#include "scriptComponent.h"
#include "XMLSerialization.h"
#include "scriptEngine.h"


namespace Framework
{
//========================================================
ScriptComponent::ScriptComponent()
{
}
//========================================================
ScriptComponent::~ScriptComponent()
{
	Free();
}
//========================================================
void ScriptComponent::Serialize( ISerializer& stream )
{
	std::vector<ISerializer*> streams;
	stream.readProperty( "script", streams );
	std::string str;

	for ( unsigned int i = 0; i < streams.size(); ++i )
	{
		streams[i]->readProperty( "id", str );

		//if ( LUA ) printf("x");
		//this->GetOwner();
		scripts.push_back( str );

		delete streams[i];
	}
}
//========================================================
void ScriptComponent::Initialize()
{
	for ( unsigned int i = 0; i < scripts.size(); ++i )
	{
		SCRIPTENGINE->RunScript( this->GetOwner(), scripts[i].c_str() );
	}
}
//========================================================
void ScriptComponent::Free()
{
}

}
