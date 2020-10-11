///////////////////////////////////////////////////////////////////////////////
///
///	 \file Model.h 
///  Defines the Model Component.
///
///	 Authors: Chia-Wei Wu
///  Copyright (c) 2012, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "VertexTypes.h"
#include "Mesh.h"
#include "Animation.h"
#include "texturearrayclass.h"

namespace Framework{

class ModelInfo;

enum ModelType
{
	Static,
	Skinned
};

enum MultiTextureType
{
	SingleTexture = 0,
	TextureBlend,
	LightMap,
	AlphaBlend,
	Fog,
	ClipPlane,
	Transparent
};

class Model{
public:
	Mesh* mesh;
	ModelType modelType;
	Model();
	~Model();
	AnimationController * Controller;
	virtual void Initialize();
	void Draw(ID3D11DeviceContext* deviceContext);
	unsigned int GetIndexCount();
	void SwapTexture();
	bool LoadTexture(ID3D11Device*, char*, char*, char*);
	void ReleaseTexture();

	ID3D11ShaderResourceView** GetTextureArray();

	TextureArrayClass* m_TextureArray;

	MultiTextureType mtt;

	ModelInfo * modelInfo1;

};

Model * LoadModel(std::string filename, ID3D11Device * device, char* textureFilename1, char* textureFilename2, char* textureFilename3);
}
