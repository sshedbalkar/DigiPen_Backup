#include "Containers.h"
#include "DXObject.h"

namespace Framework
{
	
//==============================================================
const char* DXObject::TypeName()
{
	return type.name();
}
//==============================================================
bool DXObject::IsType( const std::type_info& other_type )
{
	return ( type == other_type );
}
//==============================================================
bool DXObject::IsUnderScope( unsigned int against )
{
	return ( scope >= against );
}
//==============================================================
//==============================================================
DXObjectList::DXObjectList()
{
}
//==============================================================
DXObjectList::~DXObjectList()
{
	DXObjectListType::iterator it = objects.begin();
	while ( it != objects.end() )
	{
		if ( *it) { delete( *it ); *it = NULL; }
		++it;
	}
}
//==============================================================
void DXObjectList::Sweep( unsigned int scope )
{
	DXObjectListType::iterator it = objects.begin();
	while ( it != objects.end() )
	{
		if ( (*it)->IsUnderScope( scope ) )
		{
			if (*it) { delete(*it); *it = NULL; }
			it = objects.erase( it );
		}
		else ++it;
	}
}


}