#include "Precompiled.h"
#include "groupSelection.h"
#include "Factory.h"
#include "camera.h"
#include "Transform.h"

namespace Framework
{

//=================================================================================================
GroupSelection::GroupSelection()
{
}
//=================================================================================================
GroupSelection::~GroupSelection()
{
}
//=================================================================================================
void GroupSelection::Select( float x, float y, Camera* camera )
{
	const GameObjectFactory::GameObjectIdMapType& gocmap = FACTORY->GameObjectIdMap;
	GameObjectFactory::GameObjectIdMapType::const_iterator it = gocmap.begin();

	for ( it = gocmap.begin(); it != gocmap.end(); ++it )
	{
		Transform* transform = it->second->has( Transform );
		if ( !transform ) continue;
		if ( transform->Position.x == x && transform->Position.y == y ) object_list.push_back( it->second );
	}
}
//=================================================================================================
void GroupSelection::Select( float x0, float y0, float x1, float y1, Camera* camera )
{
	const GameObjectFactory::GameObjectIdMapType& gocmap = FACTORY->GameObjectIdMap;
	GameObjectFactory::GameObjectIdMapType::const_iterator it = gocmap.begin();
	D3DXMATRIX matrix;
	camera->GetViewProjMatrix( &matrix );
	D3DXMatrixTranspose( &matrix, &matrix );
	
//	printf( "%0.3f %0.3f, %0.3f %0.3f\n", x0, y0, x1, y1 );

	for ( it = gocmap.begin(); it != gocmap.end(); ++it )
	{
		Transform* transform = it->second->has( Transform );
		if ( !transform ) continue;
		D3DXVECTOR4 positionWS( transform->Position[0], transform->Position[1], transform->Position[2], 1.0f );
		D3DXVECTOR4 positionPS;
		D3DXVec4Transform( &positionPS, &positionWS, &matrix );
		float x = positionPS.x / positionPS.w, y = positionPS.y / positionPS.w;
		x += 1.0f; y += 1.0f;
		x *= 0.5f; y *= 0.5f;
		if (  x > x0 && y > y0 && x < x1 && y < y1 ) object_list.push_back( it->second );
	}
}
//=================================================================================================
GroupSelection::GOCListType::iterator GroupSelection::Begin()
{
	return object_list.begin();
}
//=================================================================================================
GroupSelection::GOCListType::iterator GroupSelection::End()
{
	return object_list.end();
}
//=================================================================================================
void GroupSelection::Clear()
{
//	printf( "%d\n", object_list.size() );
	object_list.clear();
}

}
