#include "Precompiled.h"
#include "Graphics.h"
//#include "VertexTypes.h"
//#include "FilePath.h"
#include "Camera.h"
#include "ComponentCreator.h"
#include "D3DManager.h"
#include "shader.h"
#include "Mesh.h"
#include "Transform.h"
#include "font.h"
#include "text.h"

#include "debugGeometry.h"	///for debug drawing

#include <iostream>

namespace Framework
{

//Our global pointer to Graphics.
Graphics* GRAPHICS = NULL;

const float FRONT = 0.1f;
const float BACK  =	1000.0f;
const int MAX_DEBUG_PRIMITIVES = 2000;
const float MAX_CAMERA_MOVING_SPEED = 0.01f;

//=======================================================
Graphics::Graphics()
{
}
//=======================================================
void Graphics::Initialize()
{
	d3dmanager = new D3DManager;
	d3dmanager->Initialize( hwnd, full_screen, screen_width, screen_height );
	device = d3dmanager->GetDevice();
	swap_chain = d3dmanager->GetSwapChain();
	render_target_screen = d3dmanager->GetRenderTargetScreen();
	depth_stencil_view = d3dmanager->GetDepthStencilView();

	///set up static mesh shaders
	static_mesh_shader = new Shader;
	static_mesh_shader->Initialize( device, "shaders\\staticMeshShader.fx", 
		VERTEX_ELEMENT_POSITION|VERTEX_ELEMENT_NORMAL|VERTEX_ELEMENT_COLOR|VERTEX_ELEMENT_BONE|VERTEX_ELEMENT_TRANSFORM );

	///set up meshes
	test_mesh = new Mesh();
	test_mesh->BuildVertexBuffer( device );

	///register graphics component
	RegisterComponent(Camera);
	RegisterComponent(Sprite);

	///set up matrices and initialize camera
	current_camera_position = Vec3( 0.0f, 0.0f, -10.0f );
	UpdateProjectionAndOrthoMatrices();

	///set up debug shader
	InitializeDebugDrawing();
	render_debug_information = true;
	
	////set up font engine
	font_shader = new Shader;
	font_shader->Initialize( device, "shaders\\fontShader.fx", 
		VERTEX_ELEMENT_POSITION|VERTEX_ELEMENT_NORMAL|VERTEX_ELEMENT_COLOR|VERTEX_ELEMENT_TEXCOORD0|VERTEX_ELEMENT_TEXCOORD1 );
	RegisterFont("TimesNewRoman");

	////////////////////////
	//// testing code below
	GRAPHICS = this;

	//transform = new Transform;
	//transform->Position = Vec3( 0.0f,0.0f,0.0f );
	//RegisterTriangle( transform );

	//test_text = new Text;
	//GRAPHICS->RegisterText( test_text );
	//test_text->UpdateText( "test TEST test TEST test TEST test TEST","TimesNewRoman", 16 );
	//test_text->SetPosition( 10.0f, 450.0f, 200.0f );

	//test = new DebugGeometry();

	//DebugGeometry::DebugShapeData shape;
	//shape.type = DebugGeometry::DEBUG_SHAPE_PRISM;
	//shape.color = Vec4( 1.0f,0.0f,0.0f,1.0f );
	//shape.v0 = Vec3( 1.0f,1.0f,1.0f );
	//shape.v1 = Vec3( 2.0f,2.0f,2.0f );
	//test->debug_shape_list.push_back( shape );

	//GRAPHICS->RegisterDebugGeometry( test );

}
//=======================================================
void Graphics::Update( float timeslice )
{
	UpdateViewingMatrix();
	d3dmanager->SetRenderWireFrame( false );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	/**********************************
	*********  mesh rendering *********
	**********************************/
	/* environment set up */
	d3dmanager->SetDepthEnable( true );
	float clear_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	device->OMSetRenderTargets( 1, &render_target_screen, depth_stencil_view );
	device->ClearRenderTargetView( render_target_screen, clear_color );
	device->ClearDepthStencilView( depth_stencil_view, D3D10_CLEAR_DEPTH, 1.0f, 0 );
	/* animated mesh pass */
	///....
	/* static mesh pass */
	static_mesh_shader->SetUsingShader( device );
	static_mesh_shader->SetTransformationConstants( device, mat_view, mat_proj );
	if ( test_mesh->SetVertexBuffer( device ) )
		device->DrawInstanced( test_mesh->GetVertexCount(), test_mesh->GetInstanceCount(), 0, 0 );

	/* 2d sprites/text */
	d3dmanager->SetDepthEnable( false );
	RenderText();
	/*********************************
	*******  post processing *********
	*********************************/
	///....
	
	/*********************************
	*******  debug drawing   *********
	*********************************/
	if ( render_debug_information ) RenderDebug();
	swap_chain->Present( 0, 0 );
}
//=======================================================
void Graphics::Free()
{
	swap_chain = NULL;
	device = NULL;
	delete static_mesh_shader; static_mesh_shader = NULL;
	d3dmanager->Shutdown();
}
//=======================================================
void Graphics::Load()
{
}
//=======================================================
void Graphics::Unload()
{
	debug_geometry_list.clear();
}
//=======================================================
void Graphics::UpdateProjectionAndOrthoMatrices()
{
	float fov, screen_aspect;

	fov = 3.1415926535897932384626f / 4.0f;
	screen_aspect = (float)screen_width / (float) screen_height;
	D3DXMatrixPerspectiveFovLH( &mat_proj, fov, screen_aspect, FRONT, BACK );
// set transforms for 2d
	D3DXVECTOR3 eye( (float)screen_width / 2.0f, (float)screen_height / 2.0f, -4.0f );
	D3DXVECTOR3 at( (float)screen_width / 2.0f, (float)screen_height / 2.0f, 0.0f );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &mat_ortho_view, &eye, &at, &up );
	D3DXMatrixOrthoLH( &mat_ortho_proj, (float)screen_width, (float)screen_height, FRONT, BACK );

}
//=======================================================
void Graphics::UpdateViewingMatrix()
{
	Camera* camera = NULL;
	if ( !camera_stack.empty() ) camera = camera_stack.top();
	//if ( camera )
	//{
	//	Vec3 distance = camera->transform->Position - current_camera_position;
	//	float sq = D3DXVec3LengthSq( &distance );
	//	if ( sq > MAX_CAMERA_MOVING_SPEED*MAX_CAMERA_MOVING_SPEED )
	//	{
	//		distance /= sqrt(sq);
	//		distance *= MAX_CAMERA_MOVING_SPEED;
	//	}
	//	current_camera_position += distance;
	//}
	Vec3 at( current_camera_position.x, current_camera_position.y, 0.0f );
	Vec3 up( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &mat_view, &current_camera_position, &at, &up );
}
//=======================================================
void Graphics::PushCamera( Camera* camera )
{
	if ( camera ) camera_stack.push( camera );
}
//=======================================================
void Graphics::PopCamera()
{
	if ( !camera_stack.empty() ) camera_stack.pop();
}
//=======================================================
void Graphics::InitializeDebugDrawing()
{
	D3D10_BUFFER_DESC vb_desc;
	HRESULT hr;
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( DebugPrimitive ) * MAX_DEBUG_PRIMITIVES;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, NULL, &debug_vertex_buffer );
	if ( FAILED(hr) ) exit(0);/////ErrorIf
	
	debug_shader = new Shader;
	debug_shader->Initialize( device, "Shaders\\debugShader.fx", 
					VERTEX_ELEMENT_POSITION|VERTEX_ELEMENT_NORMAL|VERTEX_ELEMENT_COLOR|VERTEX_ELEMENT_BONE );
}
//=======================================================
void Graphics::RenderDebug()
{
	DebugPrimitive* mapped_data;

	debug_vertex_buffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void**)&mapped_data );
	int primitive_count = 0;
	DebugGeometryListType::iterator g_it;
	for ( g_it = debug_geometry_list.begin(); g_it != debug_geometry_list.end(); ++g_it )
	{
		DebugGeometry::DebugShapeListType::iterator primitive_it;
		DebugGeometry::DebugShapeListType& primitive_list = (*g_it)->debug_shape_list;
		for ( primitive_it = primitive_list.begin(); primitive_it != primitive_list.end(); ++primitive_it )
		{
			mapped_data[primitive_count].type = (int)(*primitive_it).type;
			mapped_data[primitive_count].v0 = (*primitive_it).v0;
			mapped_data[primitive_count].v1 = (*primitive_it).v1;
			mapped_data[primitive_count].color = (*primitive_it).color;
			++primitive_count;
		}
	}
	debug_vertex_buffer->Unmap();
	unsigned int stride = sizeof( DebugPrimitive );
	unsigned int offset = 0;
	device->IASetVertexBuffers( 0, 1, &debug_vertex_buffer, &stride, &offset );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	debug_shader->SetUsingShader( device );
	debug_shader->SetTransformationConstants( device, mat_view, mat_proj );

	device->Draw( primitive_count, 0 );
}
//=======================================================
void Graphics::RegisterDebugGeometry( DebugGeometry* geometry )
{
	debug_geometry_list.push_back( geometry );
}
//=======================================================
void Graphics::RemoveDebugGeometry( DebugGeometry* geometry )
{
	DebugGeometryListType::iterator g_it;
	for ( g_it = debug_geometry_list.begin(); g_it != debug_geometry_list.end(); ++g_it )
	{
		if ( (*g_it) == geometry ) debug_geometry_list.erase(g_it);
	}
	
}
/********************
*	font engine		*
********************/
//=======================================================
void Graphics::RegisterFont( const char* font_name )
{
	Font* font = new Font;
	std::string name = "Assets\\fonts\\";
	name += font_name;
	std::string img_file,dat_file;
	img_file = name; img_file += ".bmp";
	dat_file = name; dat_file += ".dat";
	font->Initialize( device, img_file.c_str(), dat_file.c_str() );
	font_map[font_name] = font;
}
//=======================================================
void Graphics::RegisterText( Text* text )
{
	Font::BuildVertexBuffer( device, &text->vertex_buffer );
	text_list.push_back( text );
}
//=======================================================
void Graphics::RenderText()
{
	font_shader->SetUsingShader( device );
	font_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	TextListType::iterator it;
	for ( it = text_list.begin(); it != text_list.end(); ++it )
	{
		Font* font = font_map[(*it)->font];
		if ( (*it)->changed ) 
		{
			if (!font) continue;
			font->UpdateVertexBuffer( (*it ) );
		}
		unsigned int stride = font->GetVertexTypeSize();
		unsigned int offset = 0;
		device->IASetVertexBuffers( 0, 1, &(*it)->vertex_buffer, &stride, &offset );
		ID3D10EffectShaderResourceVariable* tex_ptr = font_shader->GetVariableByName("font_texture")->AsShaderResource();
		tex_ptr->SetResource( font->GetFontTexture() );
		
		device->Draw( (*it)->text.size(), 0 );
	}
}
//=======================================================
void Graphics::RegisterTriangle( Transform* transform ) 
{
	test_mesh->AddInstance( transform );
}/////

}//end namespace
