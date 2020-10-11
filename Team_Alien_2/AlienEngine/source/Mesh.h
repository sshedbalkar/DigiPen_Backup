///////////////////////////////////////////////////////////////////////////////
///
///	 \file Mesh.h 
///  Defines the Sprite Component.
///
///	 Authors: Chia-Wei Wu
///  Copyright (c) 2012, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "VertexTypes.h"

#include "Asset.hpp"
namespace Framework
{
class ChunkReader;

class Mesh{
public:
	Mesh();
	~Mesh();
	void* vertexBufferData;
	unsigned int * indexBufferData;
	unsigned int VertexTypeIndex;
	unsigned int VertexSize;
	unsigned int NumVertices;
	unsigned int NumIndices;
	//unsigned int NumPrimitives;
	ID3D11Buffer * d3dVertexBuffer;
	ID3D11Buffer * d3dIndexBuffer;
	//D3D11_BUFFER_DESC* vertexDecl;
	
	void Initialize(ID3D11Device * device);
	void ReadFrom( ChunkReader& reader );
	void SetAndDraw(ID3D11DeviceContext* deviceContext);

};
}
