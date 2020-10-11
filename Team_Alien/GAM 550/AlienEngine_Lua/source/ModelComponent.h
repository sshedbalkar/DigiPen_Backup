#pragma once
#include "Composition.h"
#include "VMath.h"

namespace Framework
{

class Transform;
//class Texture;
class Model;
class TextureArrayClass;

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
	void Draw(ID3D11DeviceContext* deviceContext, float dt, const D3DXMATRIX&, const D3DXMATRIX&, const D3DXMATRIX&, ID3D11ShaderResourceView** textureArray, const D3DXVECTOR3& lightDirection, const D3DXVECTOR4& ambientColor, const
			      D3DXVECTOR4& diffuseColor, const D3DXVECTOR3& cameraPosition, const D3DXVECTOR4& specularColor, float specularPower);//Draw the sprite to the screen

	//void DrawMultiTexture(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);
	void SwapModel();
	void DefaultModel();
	void ChangeTextureDebug( );
	void ChangeTextureBack( );
///private:
	Transform* transform;
	std::string ModelName;
	Model* pModel;
	std::string TextureName;
	TextureArrayClass* pTextureArray;
	
	std::string SwapModelName;
	std::string InitialSwapModelName;
	std::string InitialModelName;
	
};

}