#pragma once

#include "Composition.h"
#include "VMath.h"

namespace Framework
{

class Transform;
//class Texture;
class Model;

class ModelComponent : public GameComponent
{	
public:
	ModelComponent * Next;
	ModelComponent * Prev;
	ModelComponent();
	virtual ~ModelComponent();

	virtual void Serialize( ISerializer& stream );
	virtual void Initialize();
	virtual void Free();
	void Draw(ID3D11DeviceContext* deviceContext, float dt, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
			      D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower);//Draw the sprite to the screen

	void ChangeTexture( const char* );
///private:
	Transform* transform;
	std::string ModelName;
	Model* pModel;
	std::string TextureName;
	//Texture* pTexture;
};

}