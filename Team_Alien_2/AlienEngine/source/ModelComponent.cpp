#include "Precompiled.h"
#include "ModelComponent.h"
#include "Composition.h"
#include "Transform.h"
#include "Graphics.h"
#include "Model.h"
#include "lightshaderclass.h"
#include "AnimateShaderClass.h"
#include "d3dclass.h"
#include <iostream>

namespace Framework
{

//===================================================
ModelComponent::ModelComponent() : 
transform (NULL), 
pModel( NULL )
//pTexture ( NULL )
{
}
//===================================================
ModelComponent::~ModelComponent()
{
	GRAPHICS->ModelComponentList.erase( this );
}
//===================================================
void ModelComponent::Serialize( ISerializer& stream )
{
//	ISerializer* stream_platform = stream.clone();
//	std::string str;
//	stream_platform->readValue( "Box", str );
//	if ( str != "none" ) texture = GRAPHICS->GetTexture( str.c_str() );
//	stream_platform->readValue( "Top", str );
//	top = Material::GetMaterialTypeFromString( str );
//	stream_platform->readValue( "Bottom", str );
//	bottom = Material::GetMaterialTypeFromString( str );
//	if ( !stream_platform->readValue( "Shininess", shininess ) ) shininess = 20;
//	if ( !stream_platform->readValue( "BloomIntensity", bloom_intensity ) ) bloom_intensity = 0.0f;
//	if ( !stream_platform->readValue( "TextureMethod", tex_method ) ) tex_method = 1.0f;
//	int i;
//	if ( !stream_platform->readValue( "Flag", i ) ) i = 37;
//	flag = (unsigned int)i;
//
//	delete stream_platform;
}
//===================================================
void ModelComponent::Initialize()
{
	transform = GetOwner()->has( Transform );
	if ( !transform ) __debugbreak();
	GRAPHICS->ModelComponentList.push_back( this );
	//pTexture = GRAPHICS->GetTexture( TextureName );
	pModel = GRAPHICS->GetModel( ModelName );
	if(!pModel) __debugbreak();

//	TexturedBox* box_material = (TexturedBox*)GRAPHICS->GetMaterial( MAT_TEXTUREDBOX );
//	if ( box_material )
//	{
//		if ( texture )
//		{
//			box_material->SetTexture( &texture, shininess, bloom_intensity, tex_method, flag );
//			box_material->RegisterVolume( transform, 0 );
//		}
//	}
//
//	Material* top_material = GRAPHICS->GetMaterial( top );
//	if ( top_material )
//		top_material->RegisterVolume( transform, 1 );
//	
//	Material* bottom_material = GRAPHICS->GetMaterial( bottom );
//	if ( bottom_material )
//		bottom_material->RegisterVolume( transform, -1 );
}

void ModelComponent::Draw(ID3D11DeviceContext* deviceContext, float dt, D3DXMATRIX worldMatrix, D3DXMATRIX WorldviewMatrix, 
						D3DXMATRIX WorldprojectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
			      D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	Mat4 matTrans, matQuadRotation, matScale, meshMatrix, matModelWorldView;
	D3DXMatrixTranslation(&matTrans, transform->Position.x, transform->Position.y, transform->Position.z);
	D3DXMatrixRotationQuaternion(&matQuadRotation, &transform->QUATERNION);
	D3DXMatrixScaling(&matScale, transform->Scale.x, transform->Scale.y, transform->Scale.z);

	meshMatrix = matScale * matQuadRotation * matTrans;
	matModelWorldView = meshMatrix * WorldviewMatrix;
							
	bool result;

	pModel->Draw(deviceContext );

	if(pModel->Controller){
		D3DXMATRIX boneMatrices[80];
		//boneMatrices.resize( pModel->Controller->skel->bones.size() );
		for(unsigned int i=0;i < pModel->Controller->skel->bones.size() ; ++i) {
			//The matrices passed to the shader transform the vertex into bone space and then apply the bones animation
			boneMatrices[i] = pModel->Controller->skel->GetModelToBoneSpaceTransform( i ) * pModel->Controller->BoneMatrixBuffer[i];
		}
		//Render the model using the texture shader.
		result = GRAPHICS->m_AnimateShader->Render( deviceContext, pModel->GetIndexCount(), worldMatrix, matModelWorldView, WorldprojectionMatrix, pModel->GetTexture(), lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower, boneMatrices);
		if(!result)
		{
			__debugbreak();
			return;
		}
	}else{
		result = GRAPHICS->m_LightShader->Render( deviceContext, pModel->GetIndexCount(), worldMatrix, matModelWorldView, WorldprojectionMatrix, pModel->GetTexture(), lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);
		if(!result)
		{
			__debugbreak();
			return;
		}
	}

}

//===================================================
void ModelComponent::Free()
{
}
//===================================================
void ModelComponent::ChangeTexture( const char* id )
{
	/*Texture* t = GRAPHICS->GetTexture( id );
	if ( t ) texture = t;*/
}

}//end namespace