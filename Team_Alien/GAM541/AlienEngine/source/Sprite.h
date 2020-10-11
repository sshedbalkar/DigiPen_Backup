///////////////////////////////////////////////////////////////////////////////
///
///	 \file Sprite.h 
///  Defines the Sprite Component.
///
///	 Authors: 
///  Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Composition.h"
#include "VMath.h"

namespace Framework
{

class Transform;
class Texture;

/// A two-dimensional hardware accelerated sprite class using textures.
/// Has color, size, and a sprite texture name.
/// Depends on Transform.
class Sprite : public GameComponent
{
public:
	Transform * transform;
	Texture* texture;
	int flag;
	bool masked;
	bool is_hud_sprite;
public:
	//Linked list Nodes
	Sprite();
	~Sprite();
	virtual void Serialize(ISerializer& stream);	
	virtual void Initialize();

	void SetTexture( Texture* _texture ) { texture = _texture; }

	Sprite* Next;
	Sprite* Prev;
};

}
