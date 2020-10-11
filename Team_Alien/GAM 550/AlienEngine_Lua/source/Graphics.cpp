#include "Precompiled.h"
#include <fstream>
#include "Graphics.h"
#include "Message.h"
#include "Core.h"
#include "d3dclass.h"
#include "Model.h"
#include "FilePath.h"
#include "cameraclass.h"
#include "WindowsSystem.h"
//component registration
#include "Factory.h"
#include "ComponentCreator.h"
#include "ModelComponent.h"
#include "ModelComponent2.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "colorshaderclass.h"
#include "bitmapclass.h"
#include "AnimateShaderClass.h"
#include "FrustumClass.h"
#include "multitextureshaderclass.h"
#include "lightmapshaderclass.h"
#include "alphamapshaderclass.h"
#include "bumpmapshaderclass.h"
#include "specmapshaderclass.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "fogshaderclass.h"
#include "clipplaneshaderclass.h"
#include "transparentshaderclass.h"
#include "rendertexturebitmapclass.h"
#include "fadeshaderclass.h"
#include "modelclass.h"
#include "textclass.h"
#include "LevelEditor.h"
#include "GraphicsAssetsLoader.h"
#include "VMath.h"
#include "texturearrayclass.h"

namespace Framework
{
Graphics* GRAPHICS = NULL;
//======================================
Graphics::Graphics() : 
m_D3D(NULL), 
m_Camera(NULL),
UI_Camera(NULL),
Text_Camera(NULL),
m_AnimateShader(NULL),
m_Light(NULL),
m_LightShader(NULL),
m_Bitmap(NULL), 
m_ColorShader(NULL),
m_Frustum(NULL),
m_MultiTextureShader(NULL),
m_LightMapShader(NULL),
m_AlphaMapShader(NULL),
m_BumpMapShader(NULL),
WireFrame(false),
m_SpecMapShader(NULL),
m_RenderTexture(NULL),
m_DebugWindow(NULL),
m_FogShader(NULL),
m_ClipPlaneShader(NULL),
m_Text(NULL),
m_TransparentShader(NULL),
m_FadeShader(NULL),
m_RenderTextureBitmap(NULL)
{
	GRAPHICS = this;
}
//======================================
Graphics::~Graphics()
{
	for( TextureMap::iterator it = GlobalTextures.begin();it!=GlobalTextures.end();++it)
		SafeDelete(it->second);
	for( ModelMap::iterator it = Models.begin();it!=Models.end();++it)
		delete it->second;
	for( ModelMap2::iterator it = Models2.begin();it!=Models2.end();++it)
		delete it->second;
	Free();
}
void Graphics::LoadModelToMap(ModelInfo * mi){
	Model* pNewModel = NULL;
	FilePath Xfile(mi->bin );
	pNewModel = LoadModel(Xfile.FullPath.c_str(), m_D3D->GetDevice(), const_cast<char *>(GetStringBody(mi->texture1)), const_cast<char *>(GetStringBody(mi->texture2)),const_cast<char *>(GetStringBody(mi->texture3)));
	pNewModel->mtt = mi->textureType  ;
	if( pNewModel != NULL ){
		Models[Xfile.FileName.c_str()] = pNewModel;
		pNewModel->modelInfo1=mi;
	}else{
		ErrorIf(false,"Failed to load Model %s in %s", Xfile.FileName.c_str(), Xfile.FullDirectory.c_str());
		__debugbreak();
	}
}

void Graphics::LoadModelToMap2( Model2Info * mi)
{
	ModelClass *pNewModel = new ModelClass;
	if(!pNewModel){
		return;
	}
	FilePath Xfile(mi->bin);

	pNewModel->Initialize(m_D3D->GetDevice(), const_cast<char*>(Xfile.FullPath.c_str()), const_cast<char *>(GetStringBody(mi->texture1)), const_cast<char *>(GetStringBody(mi->texture2)),const_cast<char *>(GetStringBody(mi->texture3)));
	pNewModel->bt = mi->textureType;
	if( pNewModel != NULL ){
		Models2[Xfile.FileName.c_str()] = pNewModel;
	}else{
		ErrorIf(false,"Failed to load Model %s", Xfile.FileName.c_str());
		__debugbreak();
	}
}

//======================================
void Graphics::Initialize()
{
	printf("Initializing graphics..\n");
	//@@
	const bool FULL_SCREEN = false;
	const bool VSYNC_ENABLED = true;
	const float SCREEN_DEPTH = 10000.0f;
	const float SCREEN_NEAR = 0.1f;

	bool result;
	// Create the Direct3D object.
	m_D3D = new D3DClass();
	if(!m_D3D){
		return ;
	}
	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screen_width, screen_height, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, "Could not initialize Direct3D.", "Error", MB_OK);
		return ;
	}
	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera){ return; }
	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, 1000.0f);
	SetCursorPos( static_cast<int>(screen_width/2.0f), static_cast<int>(screen_height/2.0f) );

	// Create the UI camera object.
	UI_Camera = new CameraClass;
	if(!UI_Camera){ return; }
	// Set the initial position of the camera.
	UI_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	Text_Camera = new CameraClass;
	if(!Text_Camera){ return;	}
	// Initialize a base view matrix with the camera for 2D user interface rendering.
	Text_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	Text_Camera->Render(0.0f);
	Text_Camera->GetViewMatrix(viewMatrix);
	// Create the text object.
	m_Text = new TextClass;
	if(!m_Text){ return; }
	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screen_width, screen_height, viewMatrix);
	if(!result){
		MessageBox(hwnd, "Could not initialize the text object.", "Error", MB_OK);
		return;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if(!m_Bitmap){ return; }
	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screen_width, screen_height, "Assets//textures//crosshair.png", 55, 62.5);
	if(!result){
		MessageBox(hwnd, "Could not initialize the bitmap object.", "Error", MB_OK);
		return;
	}

	//Create the animate shader object
	m_AnimateShader = new AnimateShaderClass;
	if(!m_AnimateShader){ return; }
	// Initialize the animate shader object.
	result = m_AnimateShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the animate shader object.", "Error", MB_OK);
		return;
	}
	//Create the color shader object
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader){ return; }
	// Initialize the animate shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the animate shader object.", "Error", MB_OK);
		return;
	}

	// Create the frustum object.
	m_Frustum = new FrustumClass;
	if(!m_Frustum){ return; }

	// Create the multitexture shader object.
	m_MultiTextureShader = new MultiTextureShaderClass;
	if(!m_MultiTextureShader){ return; }
	// Initialize the multitexture shader object.
	result = m_MultiTextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result) {
		MessageBox(hwnd, "Could not initialize the multitexture shader object.", "Error", MB_OK);
		return;
	}

	// Create the light map shader object.
	m_LightMapShader = new LightMapShaderClass;
	if(!m_LightMapShader) { return; }
	// Initialize the light map shader object.
	result = m_LightMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the light map shader object.", "Error", MB_OK);
		return;
	}

	// Create the alpha map shader object.
	m_AlphaMapShader = new AlphaMapShaderClass;
	if(!m_AlphaMapShader){return;}
	// Initialize the alpha map shader object.
	result = m_AlphaMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the alpha map shader object.", "Error", MB_OK);
		return;
	}
	// Create the spec map shader object.
	m_SpecMapShader = new SpecMapShaderClass;
	if(!m_SpecMapShader){return;}
	// Initialize the specular map shader object.
	result = m_SpecMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the specular map shader object.", "Error", MB_OK);
		return;
	}

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture){return;}
	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screen_width, screen_height);
	if(!result){return;}

	// Create the debug window object.
	m_DebugWindow = new DebugWindowClass;
	if(!m_DebugWindow){return;}
	// Initialize the debug window object.
	result = m_DebugWindow->Initialize(m_D3D->GetDevice(), screen_width, screen_height, 100, 100);
	if(!result){
		MessageBox(hwnd, "Could not initialize the debug window object.", "Error", MB_OK);
		return;
	}

	// Create the fog shader object.
	m_FogShader = new FogShaderClass;
	if(!m_FogShader){return;}
	// Initialize the fog shader object.
	result = m_FogShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the fog shader object.", "Error", MB_OK);
		return;
	}

	// Create the clip plane shader object.
	m_ClipPlaneShader = new ClipPlaneShaderClass;
	if(!m_ClipPlaneShader){return;}
	// Initialize the clip plane shader object.
	result = m_ClipPlaneShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the clip plane shader object.", "Error", MB_OK);
		return;
	}

	// Create the transparent shader object.
	m_TransparentShader = new TransparentShaderClass;
	if(!m_TransparentShader)
	{
		return;
	}

	// Initialize the transparent shader object.
	result = m_TransparentShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the transparent shader object.", "Error", MB_OK);
		return;
	}

	// Create the bitmap object.
	m_RenderTextureBitmap = new RenderTextureBitmapClass;
	if(!m_Bitmap)
	{
		return;
	}

	// Initialize the bitmap object.
	result = m_RenderTextureBitmap->Initialize(m_D3D->GetDevice(), screen_width, screen_height, screen_width, screen_height);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the bitmap object.", "Error", MB_OK);
		return;
	}

	// Create the fade shader object.
	m_FadeShader = new FadeShaderClass;
	if(!m_FadeShader)
	{
		return;
	}

	// Initialize the fade shader object.
	result = m_FadeShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the fade shader object.", "Error", MB_OK);
		return;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader){return;}
	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd,"Could not initialize the light shader object.","Error", MB_OK);
		return;
	}


	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light){return;}
	// Initialize the light object.
	m_Light->SetAmbientColor(0.25f, 0.25f, 0.25f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, -1.0f, 0.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(100.0f);

	//Set I to Identity Matrix
	D3DXMatrixIdentity(&I);
	// Setup a clipping plane.
	// vec4(xaxis, yaxis, zaxis, height of cull)
	clipPlane = D3DXVECTOR4(0.0f, 1.0f, 0.0f, -20.0f);

	// Set the color of the fog to grey.
	fogColor = 0.5f;
	// Set the start and end of the fog.
	fogStart = 0.0f;
	fogEnd = 10.0f;

	// Set the blending amount to 50%.
	blendAmount = 0.50f;

	// Set the fade in time to 3000 milliseconds.
	m_fadeInTime = 0.1f;

	// Initialize the accumulated time to zero milliseconds.
	m_accumulatedTime = 0;

	// Initialize the fade percentage to zero at first so the scene is black.
	m_fadePercentage = 0;

	// Set the fading in effect to not done.
	m_fadeDone = false;


	GraphicsAssetsLoader loader;
	loader.Initialize();

	vector<ModelInfo*> myvec= loader.GetModelInfoVector();
	vector<ModelInfo*>::iterator iter;
	for (iter = myvec.begin(); iter != myvec.end(); ++iter){
		LoadModelToMap(*iter ); 
	}

	vector<Model2Info*> myvec2= loader.GetModel2InfoVector();
	vector<Model2Info*>::iterator iter2;
	for (iter2 = myvec2.begin(); iter2 != myvec2.end(); ++iter2){
		LoadModelToMap2(*iter2); 
	}
	LoadGlobalTexture("Assets/textures/cube1.png", "", "");

	RegisterComponent(Transform);
	RegisterComponent(ModelComponent);
	RegisterComponent(ModelComponent2);
}
//======================================
void Graphics::Free(){

	// Release the fade shader object.
	if(m_FadeShader)
	{
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
	}

	// Release the bitmap object.
	if(m_RenderTextureBitmap)
	{
		m_RenderTextureBitmap->Shutdown();
		delete m_RenderTextureBitmap;
		m_RenderTextureBitmap = 0;
	}

	// Release the transparent shader object.
	if(m_TransparentShader)
	{
		m_TransparentShader->Shutdown();
		delete m_TransparentShader;
		m_TransparentShader = 0;
	}

	// Release the text object.
	if(m_Text){
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
	// Release the clip plane shader object.
	if(m_ClipPlaneShader){
		m_ClipPlaneShader->Shutdown();
		delete m_ClipPlaneShader;
		m_ClipPlaneShader = 0;
	}
	// Release the fog shader object.
	if(m_FogShader){
		m_FogShader->Shutdown();
		delete m_FogShader;
		m_FogShader = 0;
	}
	// Release the debug window object.
	if(m_DebugWindow){
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}
	// Release the render to texture object.
	if(m_RenderTexture){
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}
	// Release the specular map shader object.
	if(m_SpecMapShader){
		m_SpecMapShader->Shutdown();
		delete m_SpecMapShader;
		m_SpecMapShader = 0;
	}
	// Release the alpha map shader object.
	if(m_AlphaMapShader){
		m_AlphaMapShader->Shutdown();
		delete m_AlphaMapShader;
		m_AlphaMapShader = 0;
	}
	// Release the light map shader object.
	if(m_LightMapShader){
		m_LightMapShader->Shutdown();
		delete m_LightMapShader;
		m_LightMapShader = 0;
	}
	// Release the multitexture shader object.
	if(m_MultiTextureShader){
		m_MultiTextureShader->Shutdown();
		delete m_MultiTextureShader;
		m_MultiTextureShader = 0;
	}
	// Release the D3D object.@@
	SafeDelete(m_D3D);
	// Release the camera object.RAD
	SafeDelete(m_Camera);
	// Release the UI camera object.RAD
	SafeDelete(UI_Camera);
	// Release the Text camera object.RAD
	SafeDelete(Text_Camera);
	// Release the bitmap object.
	if(m_Bitmap){
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}
	// Release the light object.RAD
	SafeDelete(m_Light);
	// Release the light shader object.RAD
	SafeDelete(m_LightShader);
	SafeDelete(m_AnimateShader);
	SafeDelete(m_ColorShader);
	// Release the frustum object.
	SafeDelete(m_Frustum);
}
//======================================
void Graphics::Unload(){
}
//======================================
void Graphics::Update( float dt )
{	
	if(!m_fadeDone)
	{
		// Update the accumulated time with the extra frame time addition.
		m_accumulatedTime += dt;
		// While the time goes on increase the fade in amount by the time that is passing each frame.
		if(m_accumulatedTime < m_fadeInTime)
		{
			// Calculate the percentage that the screen should be faded in based on the accumulated time.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		}else{
			// If the fade in time is complete then turn off the fade effect and render the scene normally.
			m_fadeDone = true;

			// Set the percentage to 100%.
			m_fadePercentage = 1.0f;
		}
	}
	bool result; 
	//// Render the entire scene to the texture first.
	//result = RenderToTexture(dt);
	//if(!result){
	//	return;
	//}

	if(m_fadeDone){
		// If fading in is complete then render the scene normally using the back buffer.
		RenderScene(dt);
	}else{
		// If fading in is not complete then render the scene to a texture and fade that texture in.
		result = RenderToTexture(dt);
		if(!result){ return;}
		result = RenderFadingScene(dt);
		if(!result)	{return;}
	}	
}

bool Graphics::RenderToTexture(float dt){

	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());
	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	UI_Camera->Render(dt);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	UI_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();
	for(  ; it!=ModelComponentList.end(); ++it)
	{		
			it->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
			it->pModel->GetTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), 
			m_Light->GetDiffuseColor(), UI_Camera->GetPosition(), m_Light->GetSpecularColor(),
			m_Light->GetSpecularPower());
	}

	ObjectLinkList<ModelComponent2>::iterator it2 = ModelComponent2List.begin();
	for(  ; it2!=ModelComponent2List.end(); ++it2)
	{
			it2->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
			it2->pModel->GetTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), 
			m_Light->GetDiffuseColor(), UI_Camera->GetPosition(), m_Light->GetSpecularColor(),
			m_Light->GetSpecularPower());
	}
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
	return true;
}

bool Graphics::RenderFadingScene(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	// Generate the view matrix based on the camera's position.
	UI_Camera->Render(dt);
	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	UI_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_RenderTextureBitmap->Render(m_D3D->GetDeviceContext(), 0, 0);
	if(!result){return false;}
	// Render the bitmap using the fade shader.
	result = m_FadeShader->Render(m_D3D->GetDeviceContext(), m_RenderTextureBitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, 
				      m_RenderTexture->GetShaderResourceView(), m_fadePercentage);
	if(!result){return false;}
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();
	// Present the rendered scene to the screen.
	m_D3D->EndScene();
	return true;
}

void Graphics::RenderScene(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;;
	bool result, renderModel;
	int renderCount;
	int radius;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	//m_D3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(dt);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	//Uncomment lines beginning with FC to enable frustum culling
	// FC Construct the frustum.
	// FC m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	// FC renderCount = 0;

	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();
	for(  ; it!=ModelComponentList.end(); ++it)
	{
		// FC Set the radius of the sphere to 1.0 since this is already known.
		// FC radius = sqrt(lengthSquared(it->transform->Scale));
		
		// FC renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
		
		//FC if(renderModel)	
		//FC {
			
				it->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
				it->pModel->GetTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), 
				m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
				m_Light->GetSpecularPower());
			
		//FC 	renderCount++;
		//FC }
	}

	ObjectLinkList<ModelComponent2>::iterator it2 = ModelComponent2List.begin();
	for(  ; it2!=ModelComponent2List.end(); ++it2)
	{
		it2->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
				it2->pModel->GetTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), 
				m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
				m_Light->GetSpecularPower());
	}
	//m_Model->Render(m_D3D->GetDeviceContext());
	//m_BumpMapShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
	//			m_Model->GetTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor());



	//FC printf("%d", renderCount);

	m_D3D->TurnZBufferOff();
	if (EDITOR)
	{
		EDITOR->Draw(worldMatrix, viewMatrix, projectionMatrix );
		lineDrawer.DrawLineFromList(worldMatrix, viewMatrix, projectionMatrix );
	}
	m_D3D->TurnZBufferOn();




	//// RENDER TO TEXTURE WINDOW
	//// Generate the view matrix based on the camera's position.
	//UI_Camera->Render(dt);
	//// Get the world, view, and ortho matrices from the camera and d3d objects.
	//m_D3D->GetWorldMatrix(worldMatrix);
	//UI_Camera->GetViewMatrix(viewMatrix);
	//m_D3D->GetOrthoMatrix(orthoMatrix);	
	//// Turn off the Z buffer to begin all 2D rendering.
	//m_D3D->TurnZBufferOff();	
	//// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//result = m_DebugWindow->Render(m_D3D->GetDeviceContext(), 50, 50);
	//if(!result){ return; }
	//// Render the debug window using the texture shader.
	//result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), I, I, orthoMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(), m_Light->GetAmbientColor(), 
	//		m_Light->GetDiffuseColor(), UI_Camera->GetPosition(), m_Light->GetSpecularColor(),
	//		m_Light->GetSpecularPower());		
	//if(!result){return;}
	//// Turn the Z buffer back on now that all 2D rendering has completed.
	//m_D3D->TurnZBufferOn();



	// TEXT
	// Generate the view matrix based on the Text camera's position.
	Text_Camera->Render(dt);
	Text_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();
	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();
	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), I, orthoMatrix);
	if(!result)
	{
		return;
	}
	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();
	


	// CROSSHAIR
	// Generate the view matrix based on the UI camera's position.
	UI_Camera->Render(dt);
	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	UI_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	m_D3D->TurnZBufferOff();
	m_D3D->TurnOnAlphaBlending();
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), screen_width/2.0f-30.0f, screen_height/2.0f-30.0f );
	if(!result){return;}
	// Render the bitmap with the texture shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), I, I, orthoMatrix, m_Bitmap->GetTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), 
			m_Light->GetDiffuseColor(), UI_Camera->GetPosition(), m_Light->GetSpecularColor(),
			m_Light->GetSpecularPower());
	if(!result){return;}
	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
}

const float ViewChangeSpeed = 0.001f;
//======================================
void Graphics::SendMessage( Message* m )
{	
	switch(m->MessageId){
		case Mid::MouseMove:{
				//The mouse has moved, update the mouse's world position
				MouseMove * mouseMove = (MouseMove*)m;
				if(!m_Camera->DebugCameraMode || m_Camera->bMousing){
					m_Camera->m_lookAt += ViewChangeSpeed * (mouseMove->MousePosition.x - screen_width/2) * m_Camera->sideDirection;
					m_Camera->m_lookAt.y -= ViewChangeSpeed * (mouseMove->MousePosition.y - screen_height/2);
					SetCursorPos(screen_width/2, screen_height/2);
					if(m_Camera->m_lookAt.y > 5) m_Camera->m_lookAt.y=5;
					if(m_Camera->m_lookAt.y < -5) m_Camera->m_lookAt.y=-5;
				}
				m_Camera->movingDirection = m_Camera->m_lookAt;
				if( !m_Camera->DebugCameraMode ) m_Camera->movingDirection.y = 0;
				normalize(m_Camera->movingDirection);
				m_Camera->sideDirection = D3DXVECTOR3( m_Camera->movingDirection.z, 0.0f, -m_Camera->movingDirection.x );
				break;
			}
		case Mid::RMouseButton:{
				RMouseButton * rmouse = (RMouseButton*)m;
				m_Camera->bMousing = rmouse->ButtonIsPressed;
				if(rmouse->ButtonIsPressed){
					SetCursorPos(screen_width/2, screen_height/2);
					if( m_Camera->DebugCameraMode ){
						m_Camera->ptLastFreeMousePos = rmouse->MousePosition;
						HideCursorTil(false);
					}
				}else{
					if( m_Camera->DebugCameraMode ){
						SetCursorPos( static_cast<int>(m_Camera->ptLastFreeMousePos.x), static_cast<int>(m_Camera->ptLastFreeMousePos.y) );
						ShowCursorTil(true);
					}
				}
				break;
			}
		case Mid::VirtualKey:{
		MessageVirtualKey* key = (MessageVirtualKey*)m;
		switch(key->character){
			case VK_F2:{
			WireFrame = !WireFrame;
			if(WireFrame) m_D3D->TurnWireFrameOn();
			else m_D3D->TurnWireFrameOff();
				break;}
		}
			break;}
		case Mid::MouseWheel:{
			if(m_Camera->DebugCameraMode){
				MouseWheel* wheel = (MouseWheel*)m;
				if(wheel->Delta > 0){
					m_Camera->fowardSpeed += 2.0f;
					m_Camera->backwardSpeed += 2.0f;
					m_Camera->SideMoveSpeed += 2.0f;
					m_Camera->ElevateSpeed += 2.0f;
				}else{
					m_Camera->fowardSpeed -= 2.0f;
					m_Camera->backwardSpeed -= 2.0f;
					m_Camera->SideMoveSpeed -= 2.0f;
					m_Camera->ElevateSpeed -= 2.0f;
				}
			}
			break;}
	//case:
	}
	//if ( msg->MessageId == Mid::Resolution )
	//{
	//	Resolution* m = (Resolution*)msg;
	//	screen_width = m->w;
	//	screen_height = m->h;
	//	BuildSwapChainAndStuff();
	//}
}
//======================================
void Graphics::SetWindowProperties( HWND _hwnd, int width, int height, bool FullScreen ){
	hwnd = _hwnd;
	screen_width = width;
	screen_height = height;
}

Model* Graphics::GetModel(std::string name){
	ModelMap::iterator it = Models.find(name);
	if( it!= Models.end())
		return it->second;
	else
		return NULL;
}
ModelClass* Graphics::GetModel2(std::string name){
	ModelMap2::iterator it = Models2.find(name);
	if( it!= Models2.end())
		return it->second;
	else
		return NULL;
}


D3DXMATRIX Graphics::GetViewProjMatrix(){
	D3DXMATRIX projectionMatrix, viewMatrix,ViewProj;
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	D3DXMatrixTranspose( &projectionMatrix, &projectionMatrix );
	D3DXMatrixTranspose( &viewMatrix, &viewMatrix );
	D3DXMatrixMultiply( &ViewProj, &projectionMatrix, &viewMatrix );
	D3DXMatrixTranspose( &ViewProj, &ViewProj );
	return ViewProj;
}

void Graphics::ScreenToWorldSpaceRay( Vector3& position, Vector3& direction ){
	Vector3 v;
	D3DXMATRIX projectionMatrix, viewMatrix, inverseViewMatrix;
	m_D3D->GetProjectionMatrix(projectionMatrix);
	// Move the mouse cursor coordinates into the -1 to +1 range.
	v.x = (((2.0f * (float)position.x ) / (float)screen_width ) - 1.0f)/ projectionMatrix._11;
	v.y = ((((2.0f * (float)position.y) / (float)screen_height) - 1.0f) * -1.0f)/ projectionMatrix._22;
	v.z =  1.0f;	
	
	m_Camera->GetViewMatrix(viewMatrix);
	D3DXMatrixInverse(&inverseViewMatrix, NULL, &viewMatrix);
	// Transform the screen space pick ray into 3D space
	direction.x  = v.x*inverseViewMatrix._11 + v.y*inverseViewMatrix._21 + v.z*inverseViewMatrix._31;
	direction.y  = v.x*inverseViewMatrix._12 + v.y*inverseViewMatrix._22 + v.z*inverseViewMatrix._32;
	direction.z  = v.x*inverseViewMatrix._13 + v.y*inverseViewMatrix._23 + v.z*inverseViewMatrix._33;
	position.x = inverseViewMatrix._41;
	position.y = inverseViewMatrix._42;
	position.z = inverseViewMatrix._43;
}

void Graphics::screenPosition( Vector3& position){
	Vector3 center,dir1,dir2;
	center=Vector3(screen_width/2.0,screen_height/2.0,0);
	ScreenToWorldSpaceRay(center,dir1);
	ScreenToWorldSpaceRay(position,dir2);

	dir1.normalise();
	dir2.normalise();
	real a= dir1*dir2;
	a=0.11/a;
	position=position+(dir2*a);
}

void Graphics::ScreenToWorldSpace( Vector3& position,float depth){
	Vec3 v,m1;
	m1=Vec3( (float)position.x, (float)position.y, (float)position.z);
	D3DXMATRIX projectionMatrix, viewMatrix, inverseViewMatrix;
	m_D3D->GetProjectionMatrix(projectionMatrix);

	D3D10_VIEWPORT viewport;
    viewport.Width = screen_width;
    viewport.Height = screen_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
	m_Camera->GetViewMatrix(viewMatrix);

	D3DXMATRIX world;
	D3DXMatrixIdentity( &world );
	D3DXVec3Unproject( &v, &m1, &viewport, &projectionMatrix, &viewMatrix, &world );

	float scale = (depth-m_Camera->position.z)/0.1f;

	v.x -= m_Camera->position.x;
	v.y -= m_Camera->position.y;
	v.x *= scale;
	v.y *= scale;
	//v.z = 0.0f;
	v.x += m_Camera->position.x;
	v.y += m_Camera->position.y;

	position.x = v.x;
	position.y = v.y;
	position.z = v.z;
}

void Graphics::TurnOnDebugCamera(){
	if(m_Camera){
		m_Camera->DebugCameraMode = true;
		SetCursorPos( static_cast<int>(m_Camera->ptLastFreeMousePos.x), static_cast<int>(m_Camera->ptLastFreeMousePos.y) );
	}
	ShowCursorTil(true);
}
void Graphics::TurnOffDebugCamera(){
	if(m_Camera){
		m_Camera->DebugCameraMode = false;
		m_Camera->resetCameraSpeed();
		m_Camera->ptLastFreeMousePos = Vec2(WINDOWSSYSTEM->MousePosition.x, WINDOWSSYSTEM->MousePosition.y);
	}
	SetCursorPos(screen_width/2, screen_height/2);
	HideCursorTil(false);
}
const D3DXVECTOR3& Graphics::GetCameraPosition() const{
	if(m_Camera) return m_Camera->position;
}
const D3DXVECTOR3& Graphics::GetCameraLookAt() const{
	if(m_Camera) return m_Camera->m_lookAt;
}

void Graphics::initializeAntTweakBar()
{
	TwInit(TW_DIRECT3D11, m_D3D ->GetDevice ());
	TwWindowSize(screen_width , screen_height);
}

//Load a specific texture file and add it to the asset texture map
void Graphics::LoadGlobalTexture( std::string filename1, char* filename2, char* filename3)
{
	FilePath texturefile1(filename1);
	TextureArrayClass* newTextureArray = new TextureArrayClass;
	bool result = newTextureArray->Initialize(m_D3D->GetDevice(), texturefile1.FullPath.c_str(), filename2, filename3);
	if(result){
		GlobalTextures[texturefile1.FileName.c_str()] = newTextureArray;
	}else{
		printf("loading global texture failed %s, %s, %s \n", filename1, filename2, filename3);
	}
}

TextureArrayClass* Graphics::GetGlobalTexture(std::string texture){
	TextureMap::iterator it = GlobalTextures.find(texture);
	if( it!= GlobalTextures.end())
		return it->second;
	else
		return NULL;
}

}
