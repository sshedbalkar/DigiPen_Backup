///////////////////////////////////////////////////////////////////////////////////////
//
//	Mesh.cpp
//	
//	Authors: Vaidya Rajagopalan, Ralph D., Chia-Wei Wu
//	Copyright (c) 2012, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Model.h"
#include "Graphics.h"
#include "ChunkReader.h"
#include "FileElements.hpp"
#include "ModelInfo.h"
#include "d3dclass.h"

namespace Framework{

Model::Model() : 
mesh(NULL), 
Controller(NULL),
m_TextureArray(NULL)
{
	//modelInfo1 = new ModelInfo ;
}

Model::~Model(){
	SafeDelete( mesh );
	SafeDelete( Controller );
	ReleaseTexture();
}

void Model::Initialize(){
}

void Model::SwapTexture()
{
	if (modelInfo1->swaptexture1.length()>0)
	{
		modelInfo1->swaptexture1.swap (modelInfo1->texture1 );
	}
	if (modelInfo1->swaptexture2.length()>0)
	{
		modelInfo1->swaptexture2.swap (modelInfo1->texture2 );
	}
	if (modelInfo1->swaptexture3.length()>0)
	{
		modelInfo1->swaptexture3.swap (modelInfo1->texture3 );
	}
	this->LoadTexture(GRAPHICS->m_D3D->GetDevice(),const_cast<char *>(GetStringBody(modelInfo1->texture1)),  const_cast<char *>(GetStringBody(modelInfo1->texture2)),  const_cast<char *>(GetStringBody(modelInfo1->texture3)));

}


Model* LoadModel(std::string filename, ID3D11Device * device, char* textureFilename1, char* textureFilename2, char* textureFilename3)
{
	ChunkReader file;
	file.Open( filename );

	GChunk fileChunk = file.ReadChunkHeader();
	if( fileChunk.Type != FileMark )
		return NULL;
	Model * newModel = new Model();
	newModel->modelType = Static;
	fileChunk = file.PeekChunk();

	while( fileChunk.Type != 0 )
	{
		switch( fileChunk.Type )
		{
			case MarkMesh:
			{
				newModel->mesh = new Mesh();
				newModel->mesh->ReadFrom( file );
				newModel->mesh->Initialize( device );
			}
			break;
			case MarkSkel:
			{			
				newModel->modelType = Skinned;
				newModel->Controller = new AnimationController();

				Skel * newSkel = new Skel();
				newSkel->ReadFrom( file );
				newSkel->Initialize();
				newModel->Controller->SetSkel( newSkel );
				newSkel->Release();
			}
			break;
			case MarkAnimation:
			{
				Animation * newAnimation = new Animation();
				newAnimation->ReadFrom( file );
				newModel->Controller->AddAnimation( newAnimation );
				newAnimation->Release();
			}

			default:
				file.SkipChunk(fileChunk);
			break;
		}
		fileChunk = file.PeekChunk();
	}

	bool result; 

	// Load the texture for this model.
	result = newModel->LoadTexture(device, textureFilename1, textureFilename2, textureFilename3);
	if(!result)
	{
		return NULL;
	}

	//newModel->isMultiTextured = true;

	return newModel;
}

ID3D11ShaderResourceView** Model::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}

void Model::Draw(ID3D11DeviceContext* deviceContext)
{
	mesh->SetAndDraw( deviceContext );
}

bool Model::LoadTexture(ID3D11Device* device, char* filename1, char* filename2, char* filename3)
{
	bool result;
	// Create the texture array object.
	m_TextureArray = new TextureArrayClass;
	if(!m_TextureArray)
	{
		return false;
	}

	// Initialize the texture array object.
	result = m_TextureArray->Initialize(device, filename1, filename2, filename3);
	if(!result)
	{
		return false;
	}

	return true;

}


void Model::ReleaseTexture()
{
	// Release the texture array object.
	SafeDelete(m_TextureArray);
	/*if(m_TextureArray){
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = 0;
	}*/

	return;
}

unsigned int Model::GetIndexCount(){
	return mesh->NumIndices;
}

}
