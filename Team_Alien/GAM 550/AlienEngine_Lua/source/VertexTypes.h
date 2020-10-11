///////////////////////////////////////////////////////////////////////////////
///
///	\file VertexTypes.h
///	Text based concrete serializer class.
///
///	Authors: Chia-Wei Wu
///	Copyright (c) 2012, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "VMath.h"
#include "FileElements.hpp"
namespace Framework{

struct ModelVertex
{
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
	/*Vec3 tangent;
	Vec3 binormal;*/
	//const static D3D11_INPUT_ELEMENT_DESC Decl[6];
};

struct InputSkinnedModelVertex
{
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
	float w[4];
	byte i[4];
};

struct SkinnedModelVertex
{
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
	float w[4];
	unsigned int i[4];
};

}
