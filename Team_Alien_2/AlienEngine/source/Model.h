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
#include "textureclass.h"

namespace Framework{

enum ModelType
{
	Static,
	Skinned
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

	bool LoadTexture(ID3D11Device*, char*);
	void ReleaseTexture();
	ID3D11ShaderResourceView* GetTexture();

	TextureClass* m_Texture;
};

Model * LoadModel(std::string filename, ID3D11Device * device, char* textureFilename);
}
