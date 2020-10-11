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
#include "lightshaderclass.h"
#include "lightclass.h"
#include "colorshaderclass.h"
#include "bitmapclass.h"
#include "AnimateShaderClass.h"


namespace Framework
{
const bool default_full_screen = false;


Graphics* GRAPHICS = NULL;

//======================================
Graphics::Graphics() : 
m_D3D(NULL), 
m_Camera(NULL), 
m_AnimateShader(NULL),
m_Light(NULL),
m_LightShader(NULL),
m_Bitmap(NULL), 
m_ColorShader(NULL)
{
	GRAPHICS = this;
}
//======================================
Graphics::~Graphics()
{
	for( ModelMap::iterator it = Models.begin();it!=Models.end();++it)
		delete it->second;

	Free();
}

void Graphics::LoadModelToMap(const std::string& filename, char* textureLoc){
	Model* pNewModel = NULL;
	FilePath Xfile(filename);
	pNewModel = LoadModel(Xfile.FullPath.c_str(), m_D3D->GetDevice(), textureLoc);
	if( pNewModel != NULL ){
		Models[Xfile.FileName.c_str()] = pNewModel;
	}else{
		ErrorIf(false,"Failed to load Model %s in %s", Xfile.FileName.c_str(), Xfile.FullDirectory.c_str());
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
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 0.1f;

	bool result;
	// Create the Direct3D object.
	m_D3D = new D3DClass();
	if(!m_D3D)
	{
		return ;
	}
	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screen_width, screen_height, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D.", "Error", MB_OK);
		return ;
	}
	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return;
	}
	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	//m_Camera->SetRotation(0.0f, 30.0f,  0.0f);


	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if(!m_Bitmap)
	{
		return;
	}

	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screen_width, screen_height, "Assets//textures//cloud.png", 1280, 720);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the bitmap object.", "Error", MB_OK);
		return;
	}

	//Create the animate shader object
	m_AnimateShader = new AnimateShaderClass;
	if(!m_AnimateShader)
	{
		return;
	}
	// Initialize the animate shader object.
	result = m_AnimateShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the animate shader object.", "Error", MB_OK);
		return;
	}
	//Create the color shader object
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return;
	}
	// Initialize the animate shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the animate shader object.", "Error", MB_OK);
		return;
	}


	LoadModelToMap("Assets//models//Cube.bin", "Assets//textures//seafloor.dds");
	LoadModelToMap("Assets//models//Tube.bin", "Assets//textures//particle_square_mask.jpg");
	LoadModelToMap("Assets//models//Tad.bin", "Assets//models//Tad.png");
	LoadModelToMap("Assets//models//Bomber.bin", "Assets//models//Bomber.png");
	//LoadModelToMap("Assets//models//Cube.bin");
	//LoadModelToMap("Assets//models/Sphere.bin");

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd,"Could not initialize the light shader object.","Error", MB_OK);
		return;
	}


	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(500.0f);

	RegisterComponent(Transform);
	RegisterComponent(ModelComponent);

	//CreateObjectHardCoded("Tube");
	GOC * gameObject = FACTORY->CreateEmptyComposition();
	Transform * transform = new Transform();
	transform->Position = Vec3(2, 0 , 0);;
	gameObject->AddComponent(CT_Transform , transform );

	ModelComponent * modelComponent = new ModelComponent();
	modelComponent->ModelName = "bomber";
	gameObject->AddComponent(CT_ModelComponent , modelComponent );
		
	gameObject->Initialize();
	//2
	GOC * gameObject2 = FACTORY->CreateEmptyComposition();
	Transform * transform2 = new Transform();
	transform2->Position = Vec3(-2, 0 , 0);;
	gameObject2->AddComponent(CT_Transform , transform2 );

	ModelComponent * modelComponent2 = new ModelComponent();
	modelComponent2->ModelName = "tube";
	gameObject2->AddComponent(CT_ModelComponent , modelComponent2 );
		
	gameObject2->Initialize();
}
//======================================
void Graphics::Free()
{
	// Release the D3D object.@@
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	// Release the camera object.RAD
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the bitmap object.
	if(m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}
	
	// Release the light object.RAD
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.RAD
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if(m_AnimateShader)
	{
		m_AnimateShader->Shutdown();
		delete m_AnimateShader;
		m_AnimateShader = 0;
	}

	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}
}
//======================================
void Graphics::Unload()
{
}
//======================================
void Graphics::Update( float dt )
{
	for(ModelMap::iterator it = Models.begin(); it!=Models.end(); ++it){
		if( it->second->Controller )
		{
			it->second->Controller->Update(dt);

			//if( GRAPHICS->BindPose )
			if(0)
				it->second->Controller->ProcessBindPose();
			else{
				it->second->Controller->Process();
			}
		}
	}

	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;;
	bool result;

	//swap_chain->Present(0, 0); @@
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(dt);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);


	//Used for 2D Texture Rendering
	m_D3D->GetOrthoMatrix(orthoMatrix);
	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();
	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 100, 100);
	if(!result)
	{
		return;
	}
	// Render the bitmap with the texture shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), 
			m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
			m_Light->GetSpecularPower());
	if(!result)
	{
		return;
	}
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();


	//for(ModelMap::iterator it = Models.begin(); it!=Models.end(); ++it){
	//	it->second->Draw( m_D3D->GetDeviceContext() );

	//	//Render the model using the texture shader.
	//	result = m_MeshShader->Render(m_D3D->GetDeviceContext(), it->second->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	//	if(!result)
	//	{
	//		return;
	//	}
	//}
	
	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();
	for(  ; it!=ModelComponentList.end(); ++it){
		it->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
			it->pModel->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), 
			m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
			m_Light->GetSpecularPower());
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
}

const float ViewChangeSpeed = 0.001f;
//======================================
void Graphics::SendMessage( Message* m )
{	
	switch(m->MessageId){
		case Mid::MouseMove:
			{
				//The mouse has moved, update the mouse's world position
				MouseMove * mouseMove = (MouseMove*)m;
				m_Camera->ptCurrentMousePosit =  mouseMove->MousePosition;
				if(m_Camera->bMousing){
					m_Camera->m_lookAt -= ViewChangeSpeed * (m_Camera->ptCurrentMousePosit.x - m_Camera->ptLastMousePosit.x) * m_Camera->sideDirection;
					m_Camera->m_lookAt.y += ViewChangeSpeed * (m_Camera->ptCurrentMousePosit.y - m_Camera->ptLastMousePosit.y);
				}
				m_Camera->movingDirection = D3DXVECTOR3(m_Camera->m_lookAt.x, 0.0f, m_Camera->m_lookAt.z);
				normalize(m_Camera->movingDirection);
				m_Camera->sideDirection = D3DXVECTOR3( m_Camera->movingDirection.z, 0.0f, -m_Camera->movingDirection.x );
				m_Camera->ptLastMousePosit = mouseMove->MousePosition;
				break;
			}
		case Mid::RMouseButton:
			{
				RMouseButton * rmouse = (RMouseButton*)m;
				m_Camera->bMousing = rmouse->ButtonIsPressed;
				if(rmouse->ButtonIsPressed)
				{
					m_Camera->ptLastMousePosit = m_Camera->ptCurrentMousePosit = rmouse->MousePosition;
				}else{
				}
				break;
			}
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
void Graphics::SetWindowProperties( HWND _hwnd, int width, int height, bool FullScreen )
{
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

struct LineVertex{
	D3DXVECTOR3 position;
	D3DXVECTOR4 color;
};

void Graphics::DrawLine(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR4 lineColor) {
	LineVertex Vertices[] = {
	/*p1*/{p1, lineColor},
	/*p2*/{p2, lineColor}
	};

	ID3D11Buffer* p2pBuffer;
	D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(LineVertex) * 2;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = Vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	m_D3D->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &p2pBuffer);

	//m_D3D->GetDeviceContext()->IASetInputLayout

	m_D3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	UINT stride = sizeof(LineVertex);
	UINT offset = 0;
	m_D3D->GetDeviceContext()->IASetVertexBuffers(0, 1, &p2pBuffer, &stride, &offset);

	/*LineVertex* pVoid;
	p2pBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&pVoid);
	pVoid[0] = Vertices[0];
	pVoid[1] = Vertices[1];
	p2pBuffer->Unmap();*/

	bool result;
	//it->second->Draw( m_D3D->GetDeviceContext() );
	//Render the model using the texture shader.
	/*result = m_ColorShader->Render(m_D3D->GetDeviceContext(), it->second->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
	{
		return;
	}*/
	//pPass->Apply(0);
	//pDevice->Draw(2, 0);
 }

}
