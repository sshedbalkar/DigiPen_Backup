#include "Precompiled.h"
#include "Utility.h"
#include "resources.h"
#include "Message.h"
#include "WindowsSystem.h"

namespace Framework
{

Resources* RESOURCES = NULL;

//===============================================
Resources::Resources()
{
	RESOURCES = this;
}
//===============================================
Resources::~Resources()
{
	Free();
}
//===============================================
void Resources::Initialize()
{
}
//===============================================
void Resources::Update( float timeslice )
{
}
//===============================================
void Resources::SendMessage( Message* msg )
{
	if ( msg->MessageId == Mid::VirtualKeyRelease )
	{
		MessageVirtualKeyRelease* m = (MessageVirtualKeyRelease*)msg;
		if ( m->character == VK_F5 ) 
		{
			RefreshResources();
		}
	}
}
//===============================================
void Resources::Unload()
{
	ResourceListType::iterator it = resource_list.begin();
	while ( it != resource_list.end() )
	{
		if ( (*it)->scope > 0 )
		{
			S_DELETE(*it);
			it = resource_list.erase( it );
		}
		else ++it;
	}
}
//===============================================
void Resources::Free()
{
	ResourceListType::iterator it = resource_list.begin();
	while ( it != resource_list.end() )
	{
		S_DELETE(*it);
		++it;
	}
	resource_list.clear();
}
//===============================================
void Resources::RefreshResources()
{
	WIN32_FILE_ATTRIBUTE_DATA filedata;
	SYSTEMTIME lastmod;

	ResourceListType::iterator it;
	for ( it = resource_list.begin(); it != resource_list.end(); ++it )
	{
		GetFileAttributesEx( (*it)->path.c_str(), GetFileExInfoStandard, &filedata );
		FileTimeToSystemTime( &filedata.ftLastWriteTime, &lastmod );
		if ( !( lastmod == (*it)->lastbuild ) )
		{
			(*it)->Refresh();
			(*it)->lastbuild = lastmod;
		}
	}
}


}