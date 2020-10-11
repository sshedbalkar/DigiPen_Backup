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
namespace Framework{

Model::Model() : 
mesh(NULL), 
Controller(NULL)
{
}

Model::~Model(){
	SafeDelete( mesh );
	SafeDelete( Controller );
}

void Model::Initialize(){
}

Model* LoadModel(std::string filename, ID3D11Device * device, char* textureFilename)
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
	result = newModel->LoadTexture(device, textureFilename);
	if(!result)
	{
		return NULL;
	}

	return newModel;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_Texture->GetTexture();
}

void Model::Draw(ID3D11DeviceContext* deviceContext)
{
	if( modelType == Skinned ) {
		/*std::vector< D3DXMATRIX > boneMatrices;
		boneMatrices.resize( Controller->skel->bones.size() );
		for(unsigned int i=0;i<boneMatrices.size();++i) {
			//The matrices passed to the shader transform the vertex into bone space and then apply the bones animation
			boneMatrices[i] = Controller->skel->GetModelToBoneSpaceTransform( i ) * Controller->BoneMatrixBuffer[i];
		}
		pEffect->SetMatrixArray( "WorldMatrixArray" , &boneMatrices[0] ,  boneMatrices.size() );}*/
		mesh->SetAndDraw( deviceContext );
	}else{
		mesh->SetAndDraw( deviceContext );
	}

	if(this->Controller){
		for( unsigned int b=0 ; b < this->Controller->skel->bones.size() ; ++b)
		{
			Bone& bone = this->Controller->skel->bones[b];
			for( unsigned int c=0 ; c<bone.Children.size() ; ++c)
			{
				Vec3 points[2];
				points[0] = GetPosition( this->Controller->BoneMatrixBuffer[ bone.BoneIndex ] );
				points[1] = GetPosition( this->Controller->BoneMatrixBuffer[ bone.Children[c]->BoneIndex ] );
				//GRAPHICS->pLineDrawer->DrawTransform(points,2,&matModelWorldViewProj,D3DCOLOR_RGBA(0,255,0,255) );
			}
		}
	}
}

bool Model::LoadTexture(ID3D11Device* device, char* filename)
{
	bool result;

	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void Model::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

unsigned int Model::GetIndexCount(){
	return mesh->NumIndices;
}

}
