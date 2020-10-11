#include "scene.h"
#include <map>
#include <set>
#include "XMLSerialization.h"
#include "Composition.h"
#include "Transform.h"
#include "Factory.h"

namespace Framework
{

//======================================================
Scene::Scene()
{
}
//======================================================
Scene::~Scene()
{
	Free();
}
//======================================================
void Scene::Initialize( const char* _filename )
{
	filename = _filename;
}
//static GOC* temp;
//======================================================
void Scene::Load()
{
	XMLReader reader;
	ISerializer* stream;
	std::string id;

	reader.open(filename.c_str());
	stream = reader.data().clone();

	std::vector<ISerializer*> entries;
	stream->readProperty("object", entries );

	for ( unsigned int i = 0; i < entries.size(); ++i )
	{
		entries[i]->readProperty("id", id );
		GOC* goc = FACTORY->BuildFromArchetype(id.c_str());
//		if ( id == "tree0" ) temp = goc;
		SceneObject* object = new SceneObject;
		object->transform = goc->has(Transform);
		if ( !entries[i]->readValue("Position", object->transform->Position ) )
			object->transform->Position = Vec3( 0.0f,0.0f,0.0f );
		if ( !entries[i]->readValue("Scaling", object->transform->Scale ) )
			object->transform->Scale = Vec3( 1.0f,1.0f,1.0f );
		if ( !entries[i]->readValue("RotationX", object->transform->RotationX ) )
			object->transform->RotationX = 0.0f;
		if ( !entries[i]->readValue("RotationY", object->transform->Rotation ) )
			object->transform->Rotation = 0.0f;
		if ( !entries[i]->readValue("RotationZ", object->transform->RotationZ ) )
			object->transform->RotationZ = 0.0f;
		if ( !entries[i]->readValue("Velocity", object->velocity ) )
			object->velocity = Vec3( 0.0f,0.0f,0.0f );
		if ( !entries[i]->readValue("RotatingVelocity", object->rotating_velocity ) )
			object->rotating_velocity = Vec3( 0.0f,0.0f,0.0f );
		
		goc->Initialize();
		objects.push_back( object );
		delete entries[i];
	}
	
	delete stream;
	
}
//======================================================
void Scene::Update( float timeslice )
{
	SceneObjectListType::iterator it;
	for ( it = objects.begin(); it != objects.end(); ++it )
	{
//		if ( temp ) {FACTORY->Destroy( temp ); temp = NULL;}
		Vec3 dpos = (*it)->velocity * timeslice;
		Vec3 drot = (*it)->rotating_velocity * timeslice;
		(*it)->transform->Position += dpos;
		(*it)->transform->RotationX += drot.x;
		(*it)->transform->Rotation += drot.y;
		(*it)->transform->RotationZ += drot.z;
	}
}
//======================================================
void Scene::Unload()
{
	SceneObjectListType::iterator it;
	for ( it = objects.begin(); it != objects.end(); ++it )
		S_DELETE( *it );
	objects.clear();
}
//======================================================
void Scene::Free()
{
	Unload();
}

}