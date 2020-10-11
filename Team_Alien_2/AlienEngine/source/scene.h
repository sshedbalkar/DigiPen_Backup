#pragma once

#include <list>
#include "VMath.h"

namespace Framework
{

class Transform;
class ParticleEmitter;
class Sprite;

class Scene
{
	struct SceneObject
	{
		Transform* transform;
		Vec3 velocity;
		Vec3 rotating_velocity;
	};
	typedef std::list<SceneObject*> SceneObjectListType;

private:
	SceneObjectListType objects;
	std::string filename;

public:
	Scene();
	~Scene();
	
	void Initialize( const char* filename );
	void Update( float dt );
	void Load();
	void Unload();
	void Free();

};

}