#include "Precompiled.h"
#include "ModelComponent.h"
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
#include "fogshaderclass.h"
#include "clipplaneshaderclass.h"
#include "transparentshaderclass.h"
#include "d3dclass.h"
#include <iostream>

namespace Framework
{

//===================================================
ModelComponent::ModelComponent() : 
transform (NULL), 
pModel( NULL ), 
pTextureArray(NULL)
{
	SwapModelName="";
	InitialSwapModelName="";
}
//===================================================
ModelComponent::~ModelComponent()
{
	GRAPHICS->ModelComponentList.erase( this );
}
//===================================================
void ModelComponent::Serialize( ISerializer& stream )
{
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		str->readValue("ModelName", ModelName);
		str->readValue("SwapModelName", SwapModelName);
		InitialSwapModelName=SwapModelName;
		InitialModelName=ModelName;
	}
	delete str;
}
//===================================================
void ModelComponent::SwapModel()
{
	if (SwapModelName.size()>0)
	{
		ModelName.swap(SwapModelName);
		pModel = GRAPHICS->GetModel( ModelName );
		pTextureArray = pModel->m_TextureArray;
	}
}
void ModelComponent::DefaultModel ()
{
	ModelName=InitialModelName;
	SwapModelName=InitialSwapModelName;
	pModel = GRAPHICS->GetModel( ModelName );
}

void ModelComponent::Initialize()
{
	transform = GetOwner()->has( Transform );
	if ( !transform ) __debugbreak();
	GRAPHICS->ModelComponentList.push_back( this );
	//pTexture = GRAPHICS->GetTexture( TextureName );
	pModel = GRAPHICS->GetModel( ModelName );
	if(!pModel) __debugbreak();
	pTextureArray = pModel->m_TextureArray;

//	TexturedBox* box_material = (TexturedBox*)GRAPHICS->GetMaterial( MAT_TEXTUREDBOX );
//	if ( box_material ){
//		if ( texture ){
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

void ModelComponent::Draw(ID3D11DeviceContext* deviceContext, float dt, const D3DXMATRIX& worldMatrix, const D3DXMATRIX& WorldviewMatrix, const
						D3DXMATRIX& WorldprojectionMatrix, ID3D11ShaderResourceView** textureArray, const D3DXVECTOR3& lightDirection, const D3DXVECTOR4& ambientColor, const
			      D3DXVECTOR4& diffuseColor, const D3DXVECTOR3& cameraPosition, const D3DXVECTOR4& specularColor, float specularPower)
{
	Mat4 matTrans, matQuadRotation, matScale, meshMatrix, matModelWorldView;
	D3DXMatrixTranslation(&matTrans, transform->Position.x, transform->Position.y, transform->Position.z);
	D3DXMatrixRotationQuaternion(&matQuadRotation, &transform->QUATERNION);
	D3DXMatrixScaling(&matScale, transform->Scale.x, transform->Scale.y, transform->Scale.z);

	meshMatrix = matScale * matQuadRotation * matTrans;
	matModelWorldView = meshMatrix * worldMatrix;
							
	bool result;

	pModel->Draw(deviceContext );

	if(pModel->Controller){
		pModel->Controller->Update(dt);
		//if( GRAPHICS->BindPose )
		if(0)
			pModel->Controller->ProcessBindPose();
		else
			pModel->Controller->Process();

		D3DXMATRIX boneMatrices[80];
		//boneMatrices.resize( pModel->Controller->skel->bones.size() );
		for(unsigned int i=0;i < pModel->Controller->skel->bones.size() ; ++i) {
			//The matrices passed to the shader transform the vertex into bone space and then apply the bones animation
			boneMatrices[i] = pModel->Controller->skel->GetModelToBoneSpaceTransform( i ) * pModel->Controller->BoneMatrixBuffer[i];
			D3DXMatrixTranspose( &boneMatrices[i], &boneMatrices[i] );
		}
		//Render the model using the texture shader.
		result = GRAPHICS->m_AnimateShader->Render( deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray(), lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower, boneMatrices);
		if(!result){__debugbreak();return;}

		for( unsigned int b=0 ; b < pModel->Controller->skel->bones.size() ; ++b)
		{
			Bone& bone = pModel->Controller->skel->bones[b];
			for( unsigned int c=0 ; c<bone.Children.size() ; ++c)
			{
				Vec3 points[2];
				Vec3 p1, p2;
				D3DXMATRIX tempMat1, tempMat2;
				points[0] = GetPosition( pModel->Controller->BoneMatrixBuffer[ bone.BoneIndex ] );
				points[1] = GetPosition( pModel->Controller->BoneMatrixBuffer[ bone.Children[c]->BoneIndex ] );
				D3DXVec3TransformCoord(&p1, &points[0], &matModelWorldView);
				D3DXVec3TransformCoord(&p2, &points[1], &matModelWorldView);
				//GRAPHICS->m_D3D->TurnZBufferOff();
				//GRAPHICS->lineDrawer.DrawLine(points[0], points[1], Vec4(0,1,0,1), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix );
				//GRAPHICS->m_D3D->TurnZBufferOn();
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( p1, Vec4(0,1,0,1)) );
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( p2, Vec4(0,1,0,1)) );
				//GRAPHICS->pLineDrawer->DrawTransform(points,2,&matModelWorldViewProj,D3DCOLOR_RGBA(0,255,0,255) );
			}
		}
	}else{
		if(pModel->mtt == SingleTexture){
			result = GRAPHICS->m_LightShader->Render( deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray(), lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);
			if(!result){__debugbreak();return;}
		}

		if(pModel->mtt == TextureBlend){
			result = GRAPHICS->m_MultiTextureShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray());
			if(!result){__debugbreak();return;}
		}

		if(pModel->mtt == LightMap){
			result = GRAPHICS->m_LightMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray());
			if(!result){__debugbreak();return;}
		}

		if(pModel->mtt == AlphaBlend){
			result = GRAPHICS->m_AlphaMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray());
			if(!result){__debugbreak();return;}
		}

		if(pModel->mtt == Fog){
			result = GRAPHICS->m_FogShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, 
				     pModel->GetTextureArray(), GRAPHICS->fogStart, GRAPHICS->fogEnd);
			if(!result){return;}

		}
		if(pModel->mtt == ClipPlane){
			result = GRAPHICS->m_ClipPlaneShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, 
				     pModel->GetTextureArray(), GRAPHICS->clipPlane);
			if(!result){return;}
		}
		if(pModel->mtt == Transparent)
		{
			GRAPHICS->m_D3D->TurnOnAlphaBlending();
			result =GRAPHICS-> m_TransparentShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix,
					     WorldprojectionMatrix, pModel->GetTextureArray(), GRAPHICS->blendAmount);
			if(!result){return;}
			GRAPHICS->m_D3D->TurnOffAlphaBlending();
		}
	}

}

/*void ModelComponent::DrawMultiTexture(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
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

	if(pModel->mtt == TextureBlend){
		result = GRAPHICS->m_MultiTextureShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pModel->GetTextureArray());
		if(!result){__debugbreak();	return;}
	}

	if(pModel->mtt == LightMap){
		result = GRAPHICS->m_LightMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pModel->GetTextureArray());
		if(!result){__debugbreak();return;}
	}
}*/

//===================================================
void ModelComponent::Free()
{
}
//===================================================
void ModelComponent::ChangeTextureDebug( )
{
	pTextureArray = GRAPHICS->GetGlobalTexture( "cube1" );
	if(!pTextureArray) __debugbreak();
}
void ModelComponent::ChangeTextureBack( ){
	pTextureArray = pModel->m_TextureArray;
}

}//end namespace