#pragma once

#include "Component.h"
#include "material.h"

namespace Framework
{

class Transform;
class Texture;

class VisualPlatform : public GameComponent
{
	Transform* transform;
	MaterialType top;
	MaterialType bottom;
	Texture* texture;
	float bloom_intensity;
	int shininess;
	unsigned int flag;
	float tex_method;//positive - wrap, negative - stretch
	
public:
	VisualPlatform();
	virtual ~VisualPlatform();

	virtual void Serialize( ISerializer& stream );
	virtual void Initialize();
//	virtual void Load();
//	virtual void Unload();
	virtual void Free();

	void ChangeTexture( const char* );



};

}