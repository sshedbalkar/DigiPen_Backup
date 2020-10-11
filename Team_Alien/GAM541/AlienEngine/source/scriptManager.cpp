// other stuff
#include "script.h"
#include "ScriptManager.h"
#include "GameLogic.h"
#include "XMLSerialization.h"
#include "scriptRegistrator.h"
#include "scriptCreator.h"

namespace Framework
{
//========================================================
ScriptManager::ScriptManager()
{
}
//========================================================
ScriptManager::~ScriptManager()
{
	Free();
}
//========================================================
void ScriptManager::Serialize( ISerializer& stream )
{
	std::vector<ISerializer*> streams;
	stream.readProperty( "logic", streams );
	std::string str;

	for ( unsigned int i = 0; i < streams.size(); ++i )
	{
		streams[i]->readProperty( "id", str );
		SerializeAndAddLogic( str, stream );
		delete streams[i];
	}
	LOGIC->AddScriptManager( this );
}
//========================================================
void ScriptManager::Initialize()
{
	ScriptListType::iterator it;
	for ( it = scripts.begin(); it != scripts.end(); ++it )
	{
		(*it)->Initialize( GetOwner() );
	}
}
//========================================================
void ScriptManager::Free()
{
	LOGIC->RemoveScriptManager( this );
	ScriptListType::iterator it;
	for ( it = scripts.begin(); it != scripts.end(); ++it )
	{
		delete (*it);
	}
}
//========================================================
void ScriptManager::Update( float timeslice )
{
	ScriptListType::iterator it;
	for ( it = scripts.begin(); it != scripts.end(); ++it )
	{
		(*it)->Update( timeslice );
	}
}
//========================================================
void ScriptManager::SendMessage( Message* msg )
{
	ScriptListType::iterator it;
	for ( it = scripts.begin(); it != scripts.end(); ++it )
	{
		(*it)->SendMessage( msg );
	}
}
//========================================================
void ScriptManager::SerializeAndAddLogic( std::string& id, ISerializer& stream  )
{
	Script* new_logic = NULL;
	ScriptRegistrator* registrator = LOGIC->GetScriptRegistrator();
	new_logic = (*registrator)[id]->Create();

	if ( new_logic ) 
	{
		new_logic->Serialize( stream );
		scripts.push_back( new_logic );
	}
}

}
