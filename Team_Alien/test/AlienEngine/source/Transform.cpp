///////////////////////////////////////////////////////////////////////////////////////
//
//	Transform.cpp - The Transform Component
//	Authors: Chris Peters
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Transform.h"

namespace Framework
{
	Transform::Transform()
	{
		Position = Vec3(0,0,0);
		Rotation = 0.0f;
	}

	void Transform::Serialize(ISerializer& stream)
	{
		StreamRead(stream,Position);
		StreamRead(stream,Rotation);
	}
	
}



