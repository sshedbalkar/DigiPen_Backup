#include "Precompiled.h"
#include "ScriptComponent.h"
#include "XMLSerialization.h"
#include "ScriptEngine.h"
#include "Utility.h"
#include <iostream>
//
namespace Framework
{
	ScriptComponent::ScriptComponent()
	{
		//
	}
	//
	ScriptComponent::~ScriptComponent()
	{
		Free();
	}
	//
	void ScriptComponent::Serialize( ISerializer& stream )
	{
		std::vector<ISerializer*> streams;
		stream.readProperty( "Script", streams );
		std::string str;
		//
		for each( ISerializer* strm in streams )
		{
			strm->readProperty( "id", str );
			std::cout<<"Lua script: "<<str.c_str()<<"\n";
			m_scripts.push_back( str );
		}
		utility::deletePointersVec( streams );
	}
	//
	void ScriptComponent::Initialize()
	{
		for( unsigned int i = 0; i < m_scripts.size(); ++i )
		{
			SCRIPTENGINE->RunScript( this->GetOwner(), m_scripts[i].c_str() );
		}
	}
	//
	void ScriptComponent::Free()
	{
		//
	}
	//
	int ScriptComponent::TestFn1( int n )
	{
		std::cout<<"ScriptComponent: TestFn1: "<<n<<"\n";
		return n*2;
	}
}
