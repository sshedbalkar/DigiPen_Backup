#pragma once

#include "VMath.h"
#include "Component.h"

namespace Framework
{

class Surface : public GameComponent
{
public:
	float coefficient[4];
	Vec2 span_x, span_z;
	float density;

public:
	Surface() {};
	~Surface() {};
	
	virtual void Serialize(ISerializer& stream);

	void TestingGrassSurface( Vec2& _span_x, Vec2& _span_z, float _density );
	void TestingTileSurface( Vec2& _span_x, Vec2& _span_z, float _density );
};

}
