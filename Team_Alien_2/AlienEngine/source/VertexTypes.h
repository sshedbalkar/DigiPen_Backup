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
////Our custom FVF, which describes our custom vertex structure.
//#define VERTEX2D_FVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
//#define LINE_FVF (D3DFVF_XYZ)
//
/////A structure for our custom vertex type.
//struct Vertex2D{
//	Vec3 Position; //The position of the vertex relative to other vertices.
//	Vec3 Normal; // Vertex normal
//	float tu, tv;   //The texture coordinates used to map the texture to the vertex.
//};


struct ModelVertex
{
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
	//const static D3D11_INPUT_ELEMENT_DESC Decl[6];
};

struct SkinnedModelVertex
{
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
	float w[4];
	char i[4];
	//const static D3D11_INPUT_ELEMENT_DESC Decl[6];
};

//struct VertexDescription
//{
//	unsigned int VertexType;
//	unsigned int SizeOfVertex;
//	const D3D11_INPUT_ELEMENT_DESC * Declartion;
//	const static VertexDescription Desc[2];
//};

}
