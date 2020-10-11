#include "Precompiled.h"
#include <algorithm>
#include <fstream>
#include "Graphics.h"
#include "Message.h"
#include "Core.h"
#include "d3dclass.h"
#include "FilePath.h"
#include "cameraclass.h"
#include "WindowsSystem.h"
//component registration
#include "Factory.h"
#include "ComponentCreator.h"
#include "ModelComponent.h"
#include "lightclass.h"
#include "bitmapclass.h"
#include "FrustumClass.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "fogshaderclass.h"
#include "clipplaneshaderclass.h"
#include "rendertexturebitmapclass.h"
#include "fadeshaderclass.h"
#include "textclass.h"
#include "LevelEditor.h"
#include "ModelClass.h"
#include "textureshaderclass.h"
#include "VMath.h"
#include "texturearrayclass.h"
#include "depthshaderclass.h"
#include "particlesystemclass.h"
#include "Global.h"
#include "TextDisplay.h"
#include "TextureDraw.h"
#include "GameLogic.h"
#include "TimeMechanicManager.h"
#include "PostProcessor.h"
#include "ShaderCompilation.h"
#include "Deferred.h"
#include "Blur.h"
#include "orthowindowclass.h"
#include "glowmapshaderclass.h"
#include "glowshaderclass.h"
#include "lightmapshaderclass.h"
#include "texturearrayclass.h"
#include "FXAA.h"
#include "MainMenu.h"
#include "Instancing.h"

namespace Framework
{
//======================================
const float FrameRateCacPeriod=2.0f;
void Graphics::Update( float dt )
{
	Vector3 v=GRAPHICS->TimePauseColor;
	//print(v);

	// FPS DISPLAY
	//LOGIC->textOnScreen.Display();
	////Calculate Frame Rate
	//FrameRateCacAccuTimer+=dt;
	//++FrameRateCacAccuCounter;
	//if(FrameRateCacAccuTimer>=FrameRateCacPeriod){
	//	FrameRate= FrameRateCacAccuCounter/FrameRateCacAccuTimer;
	//	FrameRateCacAccuCounter=0;FrameRateCacAccuTimer=0;
	//	m_Text->SetFps( FrameRate, m_D3D->GetDeviceContext() );
	//}

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(dt);

	if(m_StartDigipenLogo)
	{
		// Update the accumulated time with the extra frame time addition.
		m_accumulatedTime += dt;
		if(m_accumulatedTime >= 3.0f)
		{
			m_StartDigipenLogo = false;
			m_StartTeamLogo = true;
			m_accumulatedTime = 0.0f;
		}
	}

	if(m_StartTeamLogo)
	{
		// Update the accumulated time with the extra frame time addition.
		m_accumulatedTime += dt;
		if(m_accumulatedTime >= 3.0f)
		{
			m_StartTeamLogo = false;
			m_StartGameLogo = true;
			m_accumulatedTime = 0.0f;
		}
	}

	if(m_StartGameLogo)
	{
		// Update the accumulated time with the extra frame time addition.
		m_accumulatedTime += dt;
		if(m_accumulatedTime >= 3.0f)
		{
			m_StartGameLogo = false;
			m_StartMenu = true;
			m_accumulatedTime = 0.0f;
		}
	}

	if(m_StartMenu)
	{
		g_mainMenu->Show( (int)GRAPHICS->screen_width*0.5f, (int)GRAPHICS->screen_height*0.5f );
		m_StartMenu = false;
		m_DoMenu = true;
	}

	if(m_RollCredits)
	{
		// Update the accumulated time with the extra frame time addition.
		m_accumulatedTime += dt;
		if(m_accumulatedTime >= 10.0f)
		{
			m_RollCredits = false;
			m_StartMenu = true;
			m_accumulatedTime = 0.0f;
		}
	}

	if(m_StartGame)
	{
		// If fading is complete then render the scene normally using the back buffer.		
		// Render the scene to back buffer
		RenderScene(dt);
	}
	else
	{
		// Clear the buffers to begin the scene.
		m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

		D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
		m_Camera->GetViewMatrix(viewMatrix);
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetProjectionMatrix(projectionMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);

		bool result;

		m_D3D->TurnZBufferOff();

		if(m_StartDigipenLogo)
		{
			result = m_DigipenLogo->Render(m_D3D->GetDeviceContext(), 0.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DigipenLogo->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_DigipenLogo->GetTextureArray());
			if(!result){return;}
		}

		if(m_StartTeamLogo)
		{
			result = m_TeamLogo->Render(m_D3D->GetDeviceContext(), 0.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_TeamLogo->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_TeamLogo->GetTextureArray());
			if(!result){return;}
		}		

		if(m_StartGameLogo)
		{
			result = m_GameLogo->Render(m_D3D->GetDeviceContext(), 0.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_GameLogo->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_GameLogo->GetTextureArray());
			if(!result){return;}
		}		

		if(m_DoMenu)
		{
			m_D3D->TurnOnAlphaBlending();
			g_textureDraw->Render( m_TextureShader, &IdentityMatrix, &IdentityMatrix, &orthoMatrix );
			m_D3D->TurnOffAlphaBlending();
		}

		if(m_RollCredits)
		{
			result = m_Credits->Render(m_D3D->GetDeviceContext(), 0.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Credits->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_Credits->GetTextureArray());
			if(!result){return;}
		}

		// TEXT
		// Generate the view matrix based on the Text camera's position.
		m_D3D->GetOrthoMatrix(orthoMatrix);
		// Turn on the alpha blending before rendering the text.
		m_D3D->TurnOnAlphaBlending();
		// Render the text strings.
		result = m_Text->Render(m_D3D->GetDeviceContext(), IdentityMatrix, orthoMatrix);
		if(!result){return;}
		g_textDisplay->Update( false );
		// Turn off alpha blending after rendering the text.
		m_D3D->TurnOffAlphaBlending();
		// Turn the Z buffer back on now that all 2D rendering has completed.
		m_D3D->TurnZBufferOn();

		// Turn the Z buffer back on now that all 2D rendering has completed.
		m_D3D->TurnZBufferOn();

		// Present the rendered scene to the screen.
		m_D3D->EndScene();
	}	
}

void Graphics::RollCredits()
{
	m_StartGame = false;
	m_RollCredits = true;
}

void Graphics::AddTwoPointsToLineDrawer(Vector3 start, Vector3 end, Vec4 color)
{
	lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(start),color));
	lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(end),color));

}

void SetCSOutputs(ID3D11DeviceContext* context, ID3D11UnorderedAccessView* uav0, ID3D11UnorderedAccessView* uav1 = NULL,
                    ID3D11UnorderedAccessView* uav2 = NULL, ID3D11UnorderedAccessView* uav3 = NULL,
                    ID3D11UnorderedAccessView* uav4 = NULL, ID3D11UnorderedAccessView* uav5 = NULL)
{
    ID3D11UnorderedAccessView* uavs[6] = { uav0, uav1, uav2, uav3, uav4, uav5 };
    context->CSSetUnorderedAccessViews(0, 6, uavs, NULL);
}

void SetCSShader(ID3D11DeviceContext* context, ID3D11ComputeShader* shader)
{
    context->CSSetShader(shader, NULL, 0);
}

void ClearCSOutputs(ID3D11DeviceContext* context)
{
    SetCSOutputs(context, NULL, NULL, NULL, NULL);
}

void Graphics::ComputeLightTiles()
{
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	UINT32 numTilesX = DispatchSize(LightTileSize, depthBuffer->Width);
    UINT32 numTilesY = DispatchSize(LightTileSize, depthBuffer->Height);
	
	XMMATRIX W, V, P, WVP, VP;
	D3DXMATRIX World, View, Projection, WorldViewProjection, ViewProjection;

	m_D3D->GetProjectionMatrix(Projection);
	m_Camera->GetViewMatrix(View);

	P = Def->DmToXm(Projection);
	V = Def->DmToXm(View);

	VP = XMMatrixMultiply(V,P);
	
	Def->csConstants.Data.View = XMMatrixTranspose(V);
    Def->csConstants.Data.Projection = XMMatrixTranspose(P);

	XMVECTOR det;
	Def->csConstants.Data.InvViewProjection = XMMatrixTranspose(XMMatrixInverse(&det, VP));

	XMStoreFloat3(&Def->csConstants.Data.CameraPosWS, Def->DvToXv(m_Camera->GetPosition()));
    
	Def->csConstants.Data.CameraClipPlanes.x = SCREEN_NEAR;
    Def->csConstants.Data.CameraClipPlanes.y = SCREEN_DEPTH;
	Def->csConstants.Data.NumTilesX = numTilesX;
    Def->csConstants.Data.NumTilesY = numTilesY;
    Def->csConstants.Data.DisplaySizeX = depthBuffer->Width;
    Def->csConstants.Data.DisplaySizeY = depthBuffer->Height;
	if(TIMEMANAGER->GetTimeState() != TIME_NORMAL)
		 Def->csConstants.Data.TimePaused = true;
	else
		 Def->csConstants.Data.TimePaused = false;
	Def->csConstants.Data.TimePauseColor = TimePauseColor;
	Def->csConstants.ApplyChanges(context);
    Def->csConstants.SetCS(context, 0);

	int x = sizeof(CSConstants);

    ID3D11ShaderResourceView* srvs[6] = { NULL };
	
	if( lightsBuffer->NumElements<1)
		srvs[0] = NULL;
	else
		srvs[0] = lightsBuffer->SRView;

    srvs[1] = depthBuffer->SRView;
    srvs[2] = normalsTarget->SRView;
    srvs[3] = diffuseAlbedoTarget->SRView;
    srvs[4] = specularAlbedoTarget->SRView;
    srvs[5] = lightingTarget->SRView;
    SetCSOutputs(context, deferredOutputTarget->UAView);    

    context->CSSetShaderResources(0, 6, srvs);
	SetCSShader(context, Def->lightTilesCS);

    context->Dispatch(numTilesX, numTilesY, 1);

    ClearCSOutputs(context);

    for(UINT32 i = 0; i < 6; ++i)
        srvs[i] = NULL;
    context->CSSetShaderResources(0, 6, srvs);
}

bool Graphics::DetermingIfInsideAdjacentToHeroLevelBox( int obj)
{
	int hero=HERO->GetLevelBoxId();
	if(obj<0) return true;
	if(obj==hero ) return true;
	if(obj==hero-1 ) return true;
	if(obj==hero+1 ) return true;
	return false;
}

bool Graphics::DetermingIfInsideSameLevelBox(int obj1, int obj2)
{
	if(obj1<0 || obj2<0) return true;
	if(obj1!=obj2) return false;
	return true;

}

void Graphics::CreateInstancingArray(){
	const unsigned int maxObjectCreatedInLevel = 1024;
	pCubeModelThisFrameArray = (ModelComponent**) malloc( (maxObjectCount_thisLevel + maxObjectCreatedInLevel)*sizeof(ModelComponent*) );
}

void Graphics::DeferredRender(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	//instancing
	unsigned int CubeRenderCount_thisLevel = 0;

	//frustum culling
	//Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);	

	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	ID3D11RenderTargetView* renderTargets[4] = { NULL };
    ID3D11DepthStencilView* ds = depthBuffer->DSView;

	renderTargets[0] = normalsTarget->RTView;
    renderTargets[1] = diffuseAlbedoTarget->RTView;
    renderTargets[2] = specularAlbedoTarget->RTView;
    renderTargets[3] = lightingTarget->RTView;
	float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    for(UINT i = 0; i < 4; ++i)
		context->ClearRenderTargetView(renderTargets[i], color);

	context->OMSetRenderTargets(4, renderTargets, ds);

    context->ClearDepthStencilView(ds, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	if(!DebugDraw)
	{
		// Render deferred here
		for(  ; it!=ModelComponentList.end(); ++it)
		{
			if(it->pTextureArray->mtt != DeferredShading  && it->pTextureArray->mtt != LightBulbColor && it->pTextureArray->mtt != TextureColor && it->pTextureArray->mtt != lava)
				continue;

			if(EDITOR && EDITOR->inEditor && EDITOR->ShowWhatIsExcludedFromBoxes())
			{
				//do nothing here
			}
			else
			{
				if(!DetermingIfInsideAdjacentToHeroLevelBox(it->GetOwner()->LevelBoxId ))
					continue;
			}			

			renderModel=true;
			if(EDITOR && !EDITOR->inEditor)//skip in editor mode
			{
				if (it->GetOwner()->CompositionTypeId==GOC_Type_LightWithoutBulb)//skip for lights without bulb
					continue;
				if (it->GetOwner()->has(RigidBody))
				{
					CollisionPrimitive * colp=it->GetOwner()->has(CollisionPrimitive);
					Vec3 pos=Vector3ToVec3( colp->aabb->center);
					Vec3 scale=Vector3ToVec3(colp->aabb->halfSize);
					renderModel=m_Frustum->CheckRectangle(pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
				}
				else
				{
					float radius = sqrt(lengthSquared(it->transform->Scale));
					renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
				}
			}

			if(renderModel)	
			{				
				//instancing
				//if( it->IsSphere ) continue;
				if( it->IsCube ){
					//store it in the c array, draw it later( instancing )
					pCubeModelThisFrameArray[ CubeRenderCount_thisLevel ] = it;
					++CubeRenderCount_thisLevel;
					
				}else{
					it->Draw( context, dt, worldMatrix, viewMatrix, projectionMatrix, 
						it->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);
				}
				renderCount++;
			}
		}
	}

	//draw instancing cube
	m_pInstancing->drawInstancing( pCubeModelThisFrameArray, CubeRenderCount_thisLevel, dt, context, worldMatrix, viewMatrix, projectionMatrix );
	/*for( unsigned int i=0 ; i < CubeRenderCount_thisLevel ; ++i ){
		pCubeModelThisFrameArray[ i ]->Draw( context, dt, worldMatrix, viewMatrix, projectionMatrix, 
						pCubeModelThisFrameArray[ i ]->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);
	}*/

	for(UINT i = 0; i < 4; ++i)
        renderTargets[i] = NULL;
    context->OMSetRenderTargets(4, renderTargets, NULL);

    ComputeLightTiles();
}

void Graphics::Bloom(float dt)
{
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

    context->CopyResource(colorTarget->Texture, deferredOutputTarget->Texture);

    PostProcessor::Constants constants;
    constants.BloomThreshold = BloomThreshold;
    constants.BloomMagnitude = BloomMagnitude;
    constants.BloomBlurSigma = BloomBlurSigma;
    constants.Tau = AdaptationRate;
    constants.KeyValue = ExposureKeyValue;
    constants.TimeDelta = dt;

	pp->SetConstants(constants);
	pp->Render(context, colorTarget->SRView,  BackBufferRT->GetRTV());
}

void Graphics::GlowRender(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();
	ID3D11DepthStencilView* ds = depthBuffer->DSView;

	ID3D11RenderTargetView* rts[2] = { NULL, NULL };
	
	rts[0] = GlowMapRT->GetRTV();
	rts[1] = PreBlurRT->GetRTV();

	float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	
	context->OMSetRenderTargets(2, rts, ds);

	for(UINT i = 0; i < 2; ++i)
		context->ClearRenderTargetView(rts[i], color);

	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();

	if(!DebugDraw)
	{		
		for(  ; it!=ModelComponentList.end(); ++it)
		{
			if(it->pTextureArray->mtt != Glow)
				continue;

			if(EDITOR && EDITOR->inEditor && EDITOR->ShowWhatIsExcludedFromBoxes())
			{
				//do nothing here
			}
			else
			{
				if(!DetermingIfInsideAdjacentToHeroLevelBox(it->GetOwner()->LevelBoxId ))
					continue;
			}
			
			renderModel=true;
			if(EDITOR && !EDITOR->inEditor)//skip in editor mode
			{
				if (it->GetOwner()->has(RigidBody))
				{
					CollisionPrimitive * colp=it->GetOwner()->has(CollisionPrimitive);
					Vec3 pos=Vector3ToVec3( colp->aabb->center);
					Vec3 scale=Vector3ToVec3(colp->aabb->halfSize);
					renderModel=m_Frustum->CheckRectangle(pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
				}
				else
				{
					float radius = sqrt(lengthSquared(it->transform->Scale));
					renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
				}
			}

			if(renderModel)	
			{				
				Mat4 matTrans, matQuadRotation, matScale, meshMatrix, matModelWorldView;
				D3DXMatrixTranslation(&matTrans, it->transform->Position.x, it->transform->Position.y, it->transform->Position.z);
				D3DXMatrixRotationQuaternion(&matQuadRotation, &it->transform->QUATERNION);
				D3DXMatrixScaling(&matScale, it->transform->Scale.x, it->transform->Scale.y, it->transform->Scale.z);
				meshMatrix = matScale * matQuadRotation * matTrans;
				matModelWorldView = meshMatrix * worldMatrix;
				it->pModel->SetAndDraw( context);								
				m_GlowMapShader->Render(context, it->pModel->GetIndexCount(), matModelWorldView, viewMatrix, projectionMatrix, it->pTextureArray->GetTextureArray()[0], it->pTextureArray->GetTextureArray()[1]);

				renderCount++;
			}
		}
	}

	// Calculate glow (5 pass)
	BlurShader->Render(context, ds);	
}

void Graphics::ForwardRender(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();

	if(!DebugDraw)
	{
		// Render deferred here
		for(  ; it!=ModelComponentList.end(); ++it)
		{
			if(it->pTextureArray->mtt != SingleTexture && it->pTextureArray->mtt != TextureColorFR && it->pTextureArray->mtt != LightMap)
				continue;

			if(EDITOR && EDITOR->inEditor && EDITOR->ShowWhatIsExcludedFromBoxes())
			{
				//do nothing here
			}
			else
			{
				if(!DetermingIfInsideAdjacentToHeroLevelBox(it->GetOwner()->LevelBoxId ))
					continue;
			}			

			renderModel=true;
			if(EDITOR && !EDITOR->inEditor)//skip in editor mode
			{
				if (it->GetOwner()->CompositionTypeId==GOC_Type_LightWithoutBulb)//skip for lights without bulb
					continue;
				if (it->GetOwner()->has(RigidBody))
				{
					CollisionPrimitive * colp=it->GetOwner()->has(CollisionPrimitive);
					Vec3 pos=Vector3ToVec3( colp->aabb->center);
					Vec3 scale=Vector3ToVec3(colp->aabb->halfSize);
					renderModel=m_Frustum->CheckRectangle(pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
				}
				else
				{
					float radius = sqrt(lengthSquared(it->transform->Scale));
					renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
				}
			}

			if(renderModel)	
			{				
				it->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
						it->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);

				renderCount++;
			}
		}
	}
}

void Graphics::TransparentRender(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	if(!DebugDraw)
	{
		//Drawing transparent objects in 2nd pass
		ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();

		for(  ; it!=ModelComponentList.end(); ++it )
		{

			if(it->pTextureArray->mtt != Transparent)
				continue;

			if(EDITOR && EDITOR->inEditor && EDITOR->ShowWhatIsExcludedFromBoxes())
			{
				//do nothing here
			}
			else
			{
				if(!DetermingIfInsideAdjacentToHeroLevelBox(it->GetOwner()->LevelBoxId ))
					continue;
			}

			renderModel=true;
			if(EDITOR && !EDITOR->inEditor)//skip in editor mode
			{
				if (it->GetOwner()->has(RigidBody))
				{
					CollisionPrimitive * colp=it->GetOwner()->has(CollisionPrimitive);
					Vec3 pos=Vector3ToVec3(colp->aabb->center);
					Vec3 scale=Vector3ToVec3( colp->aabb->halfSize);
					renderModel=m_Frustum->CheckRectangle(pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
				}
				else
				{
					float radius = sqrt(lengthSquared(it->transform->Scale));
					renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
				}
			}

			if(renderModel)	
			{
				if((it)->GetOwner()->CompositionTypeId==GOC_Type_LevelBox || (it)->GetOwner()->CompositionTypeId==GOC_Type_TutorialBox )//REMOVE FROM FINAL VERSION
				{
					if(EDITOR && EDITOR->inEditor && EDITOR->GetShowLevelBoxes())
					{
						//do nothing here
					}
					else//dont draw the level data boxes
						continue;
				}
				if((it)->GetOwner()->CompositionTypeId==GOC_Type_CheckPointBox  )//REMOVE FROM FINAL VERSION
				{
					if(EDITOR && EDITOR->inEditor && EDITOR->GetShowCheckPointBoxes())
					{
						//do nothing here
					}
					else//dont draw the level data boxes
						continue;
				}



				// Cubes should be masked while transparent so let them die using original depth stencil state
				if((it)->GetOwner()->CompositionTypeId==GOC_Type_Cube)
				{
					(it)->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
						(it)->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);
				}
				else
				{
					// All other objects should have zero mask while transparent so draw using modified depth stencil state
					m_D3D->TurnOnTransparentDSS();
					(it)->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
						(it)->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);
					m_D3D->TurnOffTransparentDSS();
				}

				renderCount++;
			}
		}
	}
}

void Graphics::Render2D(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	lineDrawer.DrawLineFromList(worldMatrix, viewMatrix, projectionMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	if (EDITOR){
		if(EDITOR->inEditor)
		{
			m_D3D->TurnZBufferOff();
			EDITOR->Draw();
			lineDrawer.DrawLineFromList(worldMatrix, viewMatrix, projectionMatrix );
			m_D3D->TurnZBufferOn();
		}
	}

	RenderUI(dt);
}

void Graphics::RenderUI(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	bool result;

	m_D3D->TurnZBufferOff();

	// CROSSHAIR
	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	m_D3D->TurnOnAlphaBlending();
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), screen_width/2.0f-30.0f, screen_height/2.0f-30.0f );
	if(!result){return;}
	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_Bitmap->GetTextureArray());
	if(!result){return;}
	g_textureDraw->Render( m_TextureShader, &IdentityMatrix, &IdentityMatrix, &orthoMatrix );
	m_D3D->TurnOffAlphaBlending();

	// Time Speed texture
	if(TIMEMANAGER->GetTimeState() == TIME_BACK)
	{
		m_D3D->TurnOnAlphaBlending();

		if(TIMEMANAGER->GetTimeSpeed() == 1)
		{
			result = m_1x->Render(m_D3D->GetDeviceContext(), screen_width-150.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_1x->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_1x->GetTextureArray());
			if(!result){return;}
		}

		else if(TIMEMANAGER->GetTimeSpeed() == 2)
		{
			result = m_2x->Render(m_D3D->GetDeviceContext(), screen_width-150.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_2x->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_2x->GetTextureArray());
			if(!result){return;}
		}

		else if(TIMEMANAGER->GetTimeSpeed() == 4)
		{
			result = m_4x->Render(m_D3D->GetDeviceContext(), screen_width-150.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_4x->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_4x->GetTextureArray());
			if(!result){return;}
		}

		else if(TIMEMANAGER->GetTimeSpeed() == 8)
		{
			result = m_8x->Render(m_D3D->GetDeviceContext(), screen_width-150.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_8x->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_8x->GetTextureArray());
			if(!result){return;}
		}

		else if(TIMEMANAGER->GetTimeSpeed() == 16)
		{
			result = m_16x->Render(m_D3D->GetDeviceContext(), screen_width-150.0f, 0.0f );
			if(!result){return;}
			// Render the bitmap with the texture shader.
			result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_16x->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_16x->GetTextureArray());
			if(!result){return;}
		}
		
		m_D3D->TurnOffAlphaBlending();
	}


	// RENDER TO TEXTURE WINDOW
	if(DW1){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow1->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow1->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, PreBlurRT->GetShaderResourceView());
		if(!result){return;}
		
	}
	if(DW2){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow2->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow2->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, BlurShader->m_DownSampleTexure->GetShaderResourceView());
		if(!result){return;}
	}
	if(DW3){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow3->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow3->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, BlurShader->m_HorizontalBlurTexture->GetShaderResourceView());
		if(!result){return;}
	}
	if(DW4){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow4->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow4->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, BlurShader->m_VerticalBlurTexture->GetShaderResourceView());
		if(!result){return;}
	}
	if(DW5){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow5->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow5->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, PlaceholderRT->GetShaderResourceView());
		if(!result){return;}
	}
	if(DW6){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow6->Render(m_D3D->GetDeviceContext(), 640, 0);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow6->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, BackBufferRT->GetShaderResourceView());
		if(!result){return;}
	}

	// TEXT
	// Generate the view matrix based on the Text camera's position.
	m_D3D->GetOrthoMatrix(orthoMatrix);
	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();
	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), IdentityMatrix, orthoMatrix);
	if(!result){return;}
	g_textDisplay->Update( false );
	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();
}

void Graphics::FullScreenFilter()
{
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();
	ID3D11DepthStencilView* ds = depthBuffer->DSView;

	D3DXMATRIX LightMapOrthoMatrix;

	// Set the render target to be the render to texture.
	LightMappedScene->SetRenderTarget(context);
	// Clear the render to texture.
	LightMappedScene->ClearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	LightMappedScene->GetOrthoMatrix(LightMapOrthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	BlurShader->m_FullScreenWindow->Render(context);

	LightMapTAC->SetTexture1(BackBufferRT->GetShaderResourceView()[0]);

	m_LightMapShader->Render(context,  BlurShader->m_FullScreenWindow->GetIndexCount(), IdentityMatrix, IdentityMatrix, LightMapOrthoMatrix, LightMapTAC->GetTextureArray());
	
	// Turn the Z buffer back on now that all 2D rendering has completed.
	GRAPHICS->GetD3D()->TurnZBufferOn();

	context->CopyResource(BackBufferRT->GetTextureRT(), LightMappedScene->GetTextureRT());
}

void Graphics::CalcFXAA()
{
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	D3DXMATRIX FXAAOrtho;

	// Set the render target to be the render to texture.
	FxaaRT->SetRenderTarget(context);
	// Clear the render to texture.
	FxaaRT->ClearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	FxaaRT->GetOrthoMatrix(FXAAOrtho);

	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	BlurShader->m_FullScreenWindow->Render(context);

	FxaaShader->Render(context, BlurShader->m_FullScreenWindow->GetIndexCount(), IdentityMatrix, IdentityMatrix, FXAAOrtho, BackBufferRT->GetShaderResourceView());
	
	// Turn the Z buffer back on now that all 2D rendering has completed.
	GRAPHICS->GetD3D()->TurnZBufferOn();

	context->CopyResource(BackBufferRT->GetTextureRT(), FxaaRT->GetTextureRT());
}

void Graphics::ParticleFX(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Turn on alpha blending.
	m_D3D->TurnOnAlphaBlending();
	//Update Particle System
	bool result = m_ParticleSystem->UpdateParticleSystem(dt, m_D3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Camera->GetPosition(), GetTexture("particle_circle_mask")->GetTextureArray() );
	if(!result){return;}
	// Turn off alpha blending.
	m_D3D->TurnOffAlphaBlending();
}

void Graphics::RenderScene(float dt)
{
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();
	ID3D11DepthStencilView* ds = depthBuffer->DSView;

	BackBufferRT->Reset(context, ds);
	BackBufferRT->ClearRenderTarget(context, 0.0f, 0.0f, 0.0f, 1.0f);

	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	renderCount = 0;
	renderModel = false;

	// -----------------------------------------------------------------------------------------------------
	// Begin 3D Rendering
	// -----------------------------------------------------------------------------------------------------	

	// -----------------------------------------------------------------------------------------------------
	// Deferred Rendering
	// -----------------------------------------------------------------------------------------------------	
	DeferredRender(dt);

	// -----------------------------------------------------------------------------------------------------
	// Bloom
	// -----------------------------------------------------------------------------------------------------	
	Bloom(dt);

	// -----------------------------------------------------------------------------------------------------
	// Glow
	// -----------------------------------------------------------------------------------------------------
	BackBufferRT->Reset(context, ds);
	GlowRender(dt);

	// -----------------------------------------------------------------------------------------------------
	// Forward Rendering
	// -----------------------------------------------------------------------------------------------------
	BackBufferRT->Reset(context, ds);
	ForwardRender(dt);

	// -----------------------------------------------------------------------------------------------------
	// Transparent
	// -----------------------------------------------------------------------------------------------------
	BackBufferRT->Reset(context, ds);
	TransparentRender(dt);


	// -----------------------------------------------------------------------------------------------------
	// Update Particle Systems
	// -----------------------------------------------------------------------------------------------------
	BackBufferRT->Reset(context, ds);
	ParticleFX(dt);
	
	BackBufferRT->Reset(context, ds);

	// -----------------------------------------------------------------------------------------------------
	// FXAA
	// -----------------------------------------------------------------------------------------------------
	if(AA)
	{
		CalcFXAA();
	}

	// -----------------------------------------------------------------------------------------------------
	// Full Screen Filter
	// -----------------------------------------------------------------------------------------------------
	if(TIMEMANAGER->GetTimeState() != TIME_NORMAL)
	{		
		FullScreenFilter();
	}

	// -----------------------------------------------------------------------------------------------------
	// Draw BackBufferRT to the real back buffer
	// -----------------------------------------------------------------------------------------------------	
	m_D3D->Reset(ds);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	bool res;
	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();
	BlurShader->m_FullScreenWindow->Render(m_D3D->GetDeviceContext());
	// Render the bitmap with the texture shader.
	res = m_TextureShader->Render(context, BlurShader->m_FullScreenWindow->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, BackBufferRT->GetShaderResourceView());
	if(!res){return;}
	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOn();


	// -----------------------------------------------------------------------------------------------------
	// End 3D Rendering
	// -----------------------------------------------------------------------------------------------------	
	//print("renderCount: ", renderCount);		
	m_D3D->Reset(ds);

	// -----------------------------------------------------------------------------------------------------
	// Begin 2D Rendering
	// -----------------------------------------------------------------------------------------------------

	Render2D(dt);	

	// -----------------------------------------------------------------------------------------------------
	// End 2D Rendering
	// -----------------------------------------------------------------------------------------------------

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
}
}

