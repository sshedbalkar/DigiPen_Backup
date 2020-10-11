#include "Precompiled.h"
#include "ModelComponent2.h"
#include "Composition.h"
#include "Transform.h"
#include "Graphics.h"
#include "Model.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "AnimateShaderClass.h"
#include "multitextureshaderclass.h"
#include "lightmapshaderclass.h"
#include "alphamapshaderclass.h"
#include "specmapshaderclass.h"
#include "d3dclass.h"
#include <iostream>

namespace Framework
{

//===================================================
ModelComponent2::ModelComponent2() : 
transform (NULL), 
pModel( NULL )
{
}
//===================================================
ModelComponent2::~ModelComponent2()
{
	GRAPHICS->ModelComponent2List.erase( this );
}
//===================================================
void ModelComponent2::Serialize( ISerializer& stream )
{
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		str->readValue("ModelName", ModelName);
	}
	delete str;
}
//===================================================
void ModelComponent2::Initialize()
{
	transform = GetOwner()->has( Transform );
	if ( !transform ) __debugbreak();
	GRAPHICS->ModelComponent2List.push_back( this );
	//pTexture = GRAPHICS->GetTexture( TextureName );
	pModel = GRAPHICS->GetModel2( ModelName );
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

void ModelComponent2::Draw(ID3D11DeviceContext* deviceContext, float dt, D3DXMATRIX worldMatrix, D3DXMATRIX WorldviewMatrix, 
						D3DXMATRIX WorldprojectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor,
			      D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	Mat4 matTrans, matQuadRotation, matScale, meshMatrix, matModelWorldView;
	D3DXMatrixTranslation(&matTrans, transform->Position.x, transform->Position.y, transform->Position.z);
	D3DXMatrixRotationQuaternion(&matQuadRotation, &transform->QUATERNION);
	D3DXMatrixScaling(&matScale, transform->Scale.x, transform->Scale.y, transform->Scale.z);

	meshMatrix = matScale * matQuadRotation * matTrans;
	matModelWorldView = meshMatrix * worldMatrix;
							
	bool result;

	//pModel->Draw(deviceContext );

	//if(pModel->Controller){
	//	D3DXMATRIX boneMatrices[80];
	//	//boneMatrices.resize( pModel->Controller->skel->bones.size() );
	//	for(unsigned int i=0;i < pModel->Controller->skel->bones.size() ; ++i) {
	//		//The matrices passed to the shader transform the vertex into bone space and then apply the bones animation
	//		boneMatrices[i] = pModel->Controller->skel->GetModelToBoneSpaceTransform( i ) * pModel->Controller->BoneMatrixBuffer[i];
	//		D3DXMatrixTranspose( &boneMatrices[i], &boneMatrices[i] );
	//	}
	//	//Render the model using the texture shader.
	//	result = GRAPHICS->m_AnimateShader->Render( deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pModel->GetTextureArray(), lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower, boneMatrices);
	//	if(!result)
	//	{
	//		__debugbreak();
	//		return;
	//	}

	//	for( unsigned int b=0 ; b < pModel->Controller->skel->bones.size() ; ++b)
	//	{
	//		Bone& bone = pModel->Controller->skel->bones[b];
	//		for( unsigned int c=0 ; c<bone.Children.size() ; ++c)
	//		{
	//			Vec3 points[2];
	//			Vec3 p1, p2;
	//			D3DXMATRIX tempMat1, tempMat2;
	//			points[0] = GetPosition( pModel->Controller->BoneMatrixBuffer[ bone.BoneIndex ] );
	//			points[1] = GetPosition( pModel->Controller->BoneMatrixBuffer[ bone.Children[c]->BoneIndex ] );
	//			D3DXVec3TransformCoord(&p1, &points[0], &matModelWorldView);
	//			D3DXVec3TransformCoord(&p2, &points[1], &matModelWorldView);
	//			//GRAPHICS->m_D3D->TurnZBufferOff();
	//			//GRAPHICS->lineDrawer.DrawLine(points[0], points[1], Vec4(0,1,0,1), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix );
	//			//GRAPHICS->m_D3D->TurnZBufferOn();
	//			GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( p1, Vec4(0,1,0,1)) );
	//			GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( p2, Vec4(0,1,0,1)) );
	//			
	//			//GRAPHICS->pLineDrawer->DrawTransform(points,2,&matModelWorldViewProj,D3DCOLOR_RGBA(0,255,0,255) );
	//		}
	//	}

	//}
	//
	//else
	//{
		if(pModel->bt == Spec)
		{
			pModel->Render(GRAPHICS->m_D3D->GetDeviceContext());

			result = GRAPHICS->m_SpecMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pModel->GetTextureArray(), lightDirection, diffuseColor, ambientColor, cameraPosition, specularColor, specularPower);
			if(!result)
			{
				__debugbreak();
				return;
			}
		}
	//}

}

/*void ModelComponent2::DrawMultiTexture(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
				     D3DXMATRIX WorldviewMatrix, D3DXMATRIX WorldprojectionMatrix, ID3D11ShaderResourceView** textureArray)
{
	Mat4 matTrans, matQuadRotation, matScale, meshMatrix, matModelWorldView;
	D3DXMatrixTranslation(&matTrans, transform->Position.x, transform->Position.y, transform->Position.z);
	D3DXMatrixRotationQuaternion(&matQuadRotation, &transform->QUATERNION);
	D3DXMatrixScaling(&matScale, transform->Scale.x, transform->Scale.y, transform->Scale.z);

	meshMatrix = matScale * matQuadRotation * matTrans;
	matModelWorldView = meshMatrix * worldMatrix;
							
	bool result;

	pModel->Draw(deviceContext );

	if(pModel->mtt == TextureBlend)
	{
		result = GRAPHICS->m_MultiTextureShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pModel->GetTextureArray());
		if(!result)
		{
			__debugbreak();
			return;
		}
	}

	if(pModel->mtt == LightMap)
	{
		result = GRAPHICS->m_LightMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pModel->GetTextureArray());
		if(!result)
		{
			__debugbreak();
			return;
		}
	}
}*/

//===================================================
void ModelComponent2::Free()
{
}
//===================================================
void ModelComponent2::ChangeTexture( const char* id )
{
	/*Texture* t = GRAPHICS->GetTexture( id );
	if ( t ) texture = t;*/
}

}//end namespace