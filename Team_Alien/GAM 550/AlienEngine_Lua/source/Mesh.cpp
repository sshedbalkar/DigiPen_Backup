///////////////////////////////////////////////////////////////////////////////////////
//
//	Mesh.cpp
//	
//	Authors: Chia-Wei Wu
//	Copyright (c) 2012, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Mesh.h"

#include "ChunkReader.h"

namespace Framework
{

Mesh::Mesh()
{
	vertexBufferData = NULL;
	indexBufferData = NULL;
	d3dVertexBuffer = NULL;
	d3dIndexBuffer = NULL;
	//vertexDecl = NULL;
	NumVertices = 0;
	//NumPrimitives = 0;
}

Mesh::~Mesh()
{
	//SafeDelete( vertexDecl );
	SafeRelease( d3dVertexBuffer );
	SafeRelease( d3dIndexBuffer );
}

void Mesh::ReadFrom( ChunkReader& file )
{
	GChunk meshChunk = file.ReadChunkHeader();

	file.Read( VertexTypeIndex );
	if(VertexTypeIndex == 0 ){//static
		VertexSize = sizeof(ModelVertex);//@@
		//VertexSize = VertexDescription::Desc[ VertexType ].SizeOfVertex;

		file.Read( NumIndices );
		indexBufferData = new unsigned int[ NumIndices ];
		file.ReadArray( indexBufferData , NumIndices );

		file.Read( NumVertices );
		//vertexBufferData = new ModelVertex[ NumVertices ];
		vertexBufferData = new unsigned int[ NumVertices * VertexSize ];
		file.ReadArraySize( vertexBufferData , NumVertices * VertexSize );
		//NumPrimitives = NumIndices/3;
	}else{//Animated
		VertexSize = sizeof(InputSkinnedModelVertex);

		file.Read( NumIndices );
		indexBufferData = new unsigned int[ NumIndices ];
		file.ReadArray( indexBufferData , NumIndices );

		file.Read( NumVertices );
		//vertexBufferData = new ModelVertex[ NumVertices ];
		vertexBufferData = new unsigned int[ NumVertices * VertexSize ];
		file.ReadArraySize( vertexBufferData , NumVertices * VertexSize );
		/*unsigned int InputVertexSize = sizeof(InputSkinnedModelVertex);
		VertexSize = sizeof(SkinnedModelVertex);
		file.Read( NumIndices );
		indexBufferData = new unsigned int[ NumIndices ];
		file.ReadArray( indexBufferData , NumIndices );

		file.Read( NumVertices );
		InputSkinnedModelVertex* InputVertexBufferData = new InputSkinnedModelVertex[ NumVertices ];
		file.ReadArraySize( InputVertexBufferData , NumVertices * InputVertexSize );

		SkinnedModelVertex* NewBufferData = new SkinnedModelVertex[NumVertices];
		for(unsigned int i = 0 ; i < NumVertices ; ++i){
			NewBufferData[i].pos = InputVertexBufferData[i].pos;
			NewBufferData[i].norm = InputVertexBufferData[i].norm;
			NewBufferData[i].tex = InputVertexBufferData[i].tex;
			for( unsigned int j = 0 ; j<4 ; ++j){
				NewBufferData[i].w[j] = InputVertexBufferData[i].w[j];
				NewBufferData[i].i[j] = static_cast<unsigned int>( InputVertexBufferData[i].i[j] );
			}
		}
		vertexBufferData = NewBufferData;*/
	}
}

void Mesh::Initialize(ID3D11Device * device)
{
	HRESULT result;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    //vertexBufferDesc.ByteWidth = sizeof(VertexType) * NumVertices;
	vertexBufferDesc.ByteWidth = VertexSize * NumVertices;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertexBufferData;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	result = device->CreateBuffer( &vertexBufferDesc, &vertexData, &d3dVertexBuffer);
	if(FAILED(result))
	{
		__debugbreak();
		return;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * NumIndices;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indexBufferData;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &d3dIndexBuffer);
	if(FAILED(result))
	{
		__debugbreak();
		return ;
	}

	SafeDelete( vertexBufferData );
	SafeDelete( indexBufferData );
	//Load into vertex Buffer
	//device->CreateVertexBuffer(NumVertices*VertexSize,D3DUSAGE_WRITEONLY, 0,D3DPOOL_MANAGED, &d3dVertexBuffer , NULL );
	/*void *pVertices = NULL;
	d3dVertexBuffer->Lock( 0, NumVertices*VertexSize, (void**)&pVertices, 0 );
	memcpy( pVertices, vertexBufferData , NumVertices * VertexSize );
	d3dVertexBuffer->Unlock();

	device->CreateVertexDeclaration( VertexDescription::Desc[VertexType].Declartion , &vertexDecl  );

	device->CreateIndexBuffer( NumIndices * sizeof(unsigned int) , D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED , &d3dIndexBuffer  , NULL );
	d3dIndexBuffer->Lock( 0 , 0 , (void**)&pVertices , 0 );
	memcpy( pVertices, indexBufferData , NumIndices * sizeof(unsigned int) );
	d3dIndexBuffer->Unlock();*/
}

void Mesh::SetAndDraw(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;
	
	// Set vertex buffer stride and offset.
	stride = VertexSize;
	//stride = sizeof(unsigned int); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &d3dVertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*device->SetVertexDeclaration( vertexDecl );
	device->SetStreamSource( 0, d3dVertexBuffer, 0, VertexSize );
	device->SetIndices( d3dIndexBuffer );
	device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0 , 0 , NumVertices , 0 , NumPrimitives );*/
}

}
