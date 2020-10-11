#include "Precompiled.h"
#include "Instancing.h"
#include "ModelComponent.h"
#include "ModelClass.h"//set and draw
#include "Transform.h"//transform this
#include "texturearrayclass.h"//thisTextureArray
#include "Graphics.h"//call shader
#include "Deferred.h" //call shader
#include "ParticleComponent.h" //lava

//#include "Graphics.h"
namespace Framework
{

struct InstanceType
{
	Mat4 World;
};

Instancing::~Instancing(){ SafeRelease(m_instanceBuffer); }

bool Instancing::InitializeInstancing( ID3D11Device* md3dDevice ){
	
	InstanceType mInstancedData;
	
	D3D11_BUFFER_DESC instanceBufferDesc;
    instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * 1;//the dynamic buffer is empty at the beginning
    instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;
	
    HRESULT result = md3dDevice->CreateBuffer(&instanceBufferDesc, 0, &m_instanceBuffer);
	if(FAILED(result))
	{
		return false;
	}
	return true;
}

//parameters: 
//dt: for lava
void Instancing::drawInstancing( ModelComponent** pCubeModelThisFrameArray, unsigned int CubeRenderCount_thisLevel, 
	float dt, ID3D11DeviceContext* deviceContext, const D3DXMATRIX& worldMatrix, const D3DXMATRIX& WorldviewMatrix, const D3DXMATRIX& WorldprojectionMatrix ){

	const D3DXVECTOR4& ambientColor = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 0.2f), diffuseColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), specularColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	const float& specularPower = 250;
	
	pCubeModelThisFrameArray[ 0 ]->pModel->SetAndDraw(deviceContext);
	unsigned int vertexCount = pCubeModelThisFrameArray[ 0 ]->pModel->GetVertexCount();
	//@@ delete
	unsigned int indexCount = pCubeModelThisFrameArray[ 0 ]->pModel->GetIndexCount();

	for( unsigned int i=0 ; i < CubeRenderCount_thisLevel ; ++i ){
		ModelComponent* const thisModelComponent = pCubeModelThisFrameArray[ i ];
		GOC* const thisGOC = thisModelComponent->GetOwner();
		//get light color for light sources
		D3DXVECTOR3 LightColor = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		if( thisGOC->has(LightComponent))
			LightColor = thisGOC->has(LightComponent)->Color;
		//transform this
		Transform const * const transform = thisModelComponent->transform;
		
		Mat4 matTrans, matQuadRotation, matScale, meshMatrix, matModelWorldView, matRoTrans;
		D3DXMatrixTranslation(&matTrans, transform->Position.x, transform->Position.y, transform->Position.z);
		D3DXMatrixRotationQuaternion(&matQuadRotation, &transform->QUATERNION);
		D3DXMatrixScaling(&matScale, transform->Scale.x, transform->Scale.y, transform->Scale.z);

		//meshMatrix = matScale * matQuadRotation * matTrans;
		//matModelWorldView = meshMatrix * worldMatrix;
		D3DXMatrixMultiply( &matRoTrans, &matQuadRotation, &matTrans );
		D3DXMatrixMultiply( &meshMatrix, &matScale, &matRoTrans );
		D3DXMatrixMultiply( &matModelWorldView, &meshMatrix, &worldMatrix );

		//bool result;
		TextureArrayClass* const thisTextureArray = thisModelComponent->pTextureArray;
		switch( thisTextureArray->mtt ){
		case lava:
			thisGOC->has(ParticleComponent)->CreateParticlesLava(dt);
			thisModelComponent->timer += dt;
		case LightBulbColor:
			if( thisGOC->has(LightComponent)) LightColor = thisGOC->has(LightComponent)->Color;
		case TextureColor:
		case DeferredShading:
			GRAPHICS->GetDeferred()->RenderGBuffer(deviceContext, matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, thisTextureArray->GetTextureArray(), indexCount, transform->Scale, thisModelComponent->TextMultiplier, thisTextureArray->Extendable, static_cast<unsigned>(thisTextureArray->mtt), LightColor, thisModelComponent->timer, 
				true, vertexCount, CubeRenderCount_thisLevel);
			break;
		}


	}
}

}