#include "Precompiled.h"
#include "WindowsSystem.h"
#include "menuSystem.h"
#include "Graphics.h"
#include "D3DManager.h"
#include "renderTargetTexture.h"
//graphics elements
#include "Mesh.h"
#include "animation.h"
#include "texture.h"
#include "scene.h"
//particle systems
#include "PESquare.h"
#include "PEEnergyField.h"
#include "PELaser.h"
#include "PEClouds.h"
//shader
#include "shader.h"
//components
#include "ComponentCreator.h"
#include "Camera.h"
#include "MeshInstance.h"
#include "visualPlatform.h"
#include "particleEmitter.h"
#include "Sprite.h"
#include "Transform.h"
#include "ScriptManager.h"
#include "debugGeometry.h"
//font engine
#include "font.h"
#include "text.h"
///materials
#include "grass.h"
#include "spikes.h"
#include "skyline.h"
#include "texturedBox.h"
#include "water.h"
///xml serializer
#include "XMLSerialization.h"
///debug drawing
#include "debugGeometry.h"	
//logic, for debugging
#include "GameLogic.h"
#include "Core.h"


#include <iostream>

namespace Framework
{

//Our global pointer to Graphics.
Graphics* GRAPHICS = NULL;

const float FRONT = 0.1f;
const float BACK  =	1000.0f;
const int MAX_DEBUG_PRIMITIVES = 2000;
const int MAX_PARTICLES_PER_EMITTER = 1000;
const float CAMERA_SLOW_RADIUS = 10.0f;
const float CAMERA_EPSILON = 0.3f;
const float MAX_CAMERA_ACCELERATION = 1000.0f;
const float MAX_CAMERA_MOVING_SPEED_Y = 25.0f;
const float MAX_CAMERA_MOVING_SPEED_Z = 7.0f;

const float PI = 3.1415926535897932384626f;
const int MAX_GUI_COMPONENTS = 10;

const float clear_color[3][4] = { { 0.0f, 0.0f, 0.0f, 1.0f },		//default color
								  { 0.0f, 0.0f,-1.0f, 0.0f },			//default normal/flag
								  { 0.0f, 0.0f, BACK, 0.0f }, 		//default view position/shininess
								 };
#define STATE_LOGO0			0
#define STATE_LOGO1			1
#define STATE_SCENE			2
#define STATE_MENU			3
#define STATE_PREMAINMENU	4
#define STATE_POSTMAINMENU	5
#define STATE_PREPAUSE		6
#define STATE_POSTPAUSE		7
#define STATE_PREENDGAME	8
#define STATE_ENDGAME		9
#define STATE_CREDITS		10

#define ISTATE_ON_ENTER		0
#define ISTATE_ON_UPDATE	1
#define ISTATE_ON_UPDATE2	2
#define ISTATE_ON_EXIT		3
/**********************************************************************
//	main functions
***********************************************************************/
//================
Graphics::Graphics()
{
	current_scene = NULL;
	/// global pointer
	GRAPHICS = this;
	 MAX_CAMERA_MOVING_SPEED = 15.0f;
}
//================
Graphics::~Graphics()
{
	Free();
	GRAPHICS = NULL;
}
//================
void Graphics::Initialize()
{
	d3dmanager = new D3DManager;
	d3dmanager->Initialize( hwnd, full_screen, screen_width, screen_height );
	device = d3dmanager->GetDevice();
	swap_chain = d3dmanager->GetSwapChain();
	render_target_screen = d3dmanager->GetRenderTargetScreen();
	srv_screen = d3dmanager->GetShaderResourceScreen();
	depth_stencil_view = d3dmanager->GetDepthStencilView();

	InitializeAssets();
	std::cout <<"compiling shaders...\n";
	///set up main render targets
	for ( unsigned int i = 0; i < 3; ++i )
	{
		render_target_raw[i] = new RenderTargetTexture();
		render_target_raw[i]->Initialize( device, screen_width, screen_height );
	}
	///register graphics component
	RegisterComponent(Camera);
	RegisterComponent(Sprite);
	RegisterComponent(VisualPlatform);
	RegisterComponent(MeshInstance);
	RegisterComponent(ParticleEmitter);
	RegisterComponent(ScriptManager);
	/// initialize systems
	InitializeMeshRenderer();
	InitializeMaterials();
	InitializeParticleSystem();
	InitializeSprite();
	InitializeBloom();
	InitializePostprocess();
	InitializeCursor();
	InitializeMenu();
	InitializeFont();
	InitializeDebugDrawing();

	///set up matrices and initialize camera
	current_camera_position = Vec3( -29.0f, 0.0f, -45.0f );
	current_camera_velocity = Vec3( 0.0f, 0.0f, 0.0f );
	UpdateProjectionAndOrthoMatrices();
	UpdateViewingMatrix(0.0f);

	timer = 1.5f;
	state = STATE_LOGO0;
	istate = ISTATE_ON_ENTER;
	d3dmanager->SetRenderWireFrame( false );
	//////////////////////////
	////// testing code below
	test_text = new Text;
	RegisterText( test_text );
	test_text->SetPosition( 10.0f, 450.0f, 200.0f );
	test_text->SetColor( Vec4(0.0f,0.0f,0.0f,1.0f ) );
}
//================
void Graphics::Update( float timeslice )
{
	/// temporary code
	//char frametime[10];
	//sprintf_s(frametime, "%0.3f", timeslice );
	//test_text->UpdateText( frametime, "TimesNewRoman", 20 );
	if ( !current_scene ) 
	{
		current_scene = scene_map["sunny_land"];
		current_scene->Load();
	}
	// rendering FSM
	GraphicsStateMachine( timeslice );
	
	if ( render_debug_information ) RenderDebug();
	swap_chain->Present( 1, 0 );
}
//================
void Graphics::Free()
{	
	swap_chain = NULL;
	device = NULL;
	render_target_screen = NULL;
	srv_screen = NULL;
	depth_stencil_view = NULL;
	//d3dmanager->Shutdown();
	S_DELETE(d3dmanager);
	FreeDebugDrawing();
	FreeFont();
	FreeMenu();
	FreeCursor();
	FreePostprocess();
	FreeBloom();
	FreeSprite();
	FreeParticleSystem();
	FreeMaterials();
	FreeMeshRenderer();
	for ( int i = 0; i < 3; ++i ) { S_DELETE(render_target_raw[i]); }
	FreeAssets();
}
//================
void Graphics::Load()
{
}
//================
void Graphics::Unload()
{
	MaterialMapType::iterator mit;
	for ( mit = material_map.begin(); mit != material_map.end(); ++mit )
	{
		mit->second->Unload();
	}
	current_scene->Unload();
	current_scene = NULL;
}
//================
void Graphics::SendMessage( Message* msg )
{
	if ( msg->MessageId == Mid::Menu )
	{
		MessageMenu* m = (MessageMenu*)msg;
		if  ( m->menu_id == PauseMenu )
		{
			state = STATE_PREPAUSE;
			istate = ISTATE_ON_ENTER;
		}
	}
	else if ( msg->MessageId == Mid::LeaveMenu )
	{
		MessageLeaveMenu* m = (MessageLeaveMenu*)msg;
		if ( m->menu_id == PauseMenu )
		{
			state = STATE_POSTPAUSE;
			istate = ISTATE_ON_ENTER;
		}
		else if ( m->menu_id == MainMenu )
		{
			state = STATE_POSTMAINMENU;
			istate = ISTATE_ON_ENTER;
		}
	}
	else if ( msg->MessageId == Mid::MouseMove )
	{
		MouseMove * mouseMove = (MouseMove*)msg;
		cursor_position = mouseMove->MousePosition;
		cursor_position.y = screen_height - cursor_position.y;
	}
	else if ( msg->MessageId == Mid::CharacterKey )
	{
		if ( state == STATE_CREDITS && istate == ISTATE_ON_EXIT )
		{
			Message m( Mid::Quit );
			CORE->BroadcastMessage( &m );
		}
	}
	else if(msg->MessageId==Mid::GameWon )
	{
		state = STATE_PREENDGAME;
		istate = ISTATE_ON_ENTER;
		CORE->togglePauseSystems();
	}
}
//================
void Graphics::UpdateProjectionAndOrthoMatrices()
{
	float fov, screen_aspect;

	fov = PI / 8.0f;
	screen_aspect = (float)screen_width / (float) screen_height;
	D3DXMatrixPerspectiveFovLH( &mat_proj, fov, screen_aspect, FRONT, BACK );
// set transforms for 2d
	D3DXVECTOR3 eye( (float)screen_width / 2.0f, (float)screen_height / 2.0f, -4.0f );
	D3DXVECTOR3 at( (float)screen_width / 2.0f, (float)screen_height / 2.0f, 0.0f );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &mat_ortho_view, &eye, &at, &up );
	D3DXMatrixOrthoLH( &mat_ortho_proj, (float)screen_width, (float)screen_height, FRONT, BACK );

}
//================
void Graphics::UpdateViewingMatrix( float dt )
{
	static_cast<void>(dt);
	Vec3 at( current_camera_position.x, current_camera_position.y, 0.0f );
	Vec3 up( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &mat_view, &current_camera_position, &at, &up );

	// generate the screen rectangle in world space, z=0 plane, for frustum culling
	Vec2 screenp( 0.0f, 0.0f );
	Vec2 worldp;
	ScreenToWorldSpace( &worldp, &screenp );
	screen_left = worldp.x; screen_top = worldp.y;
	screenp = Vec2( (float)screen_width, (float)screen_height );
	ScreenToWorldSpace( &worldp, &screenp );
	screen_right = worldp.x; screen_bottom = worldp.y;
}
//**********************************************************************
//	camera movement
//**********************************************************************
//================
void Graphics::PushCamera( Camera* camera )
{
	if ( camera )
	{
		if ( camera_stack.empty() ) current_camera_position = camera->transform->Position;
		camera_stack.push( camera );
	}
}
//================
void Graphics::PopCamera()
{
	if ( !camera_stack.empty() ) camera_stack.pop();
}
//================
void Graphics::MoveCamera( float timeslice )
{
	Camera* camera = NULL;
	if ( camera_stack.empty() ) return;

	camera = camera_stack.top();
//	current_camera_position = camera->transform->Position;
//	return;
	Vec3 direction = camera->transform->Position - current_camera_position;
	Vec3 acceleration = direction;
	float distance = abs(direction.x);
	if ( distance < CAMERA_EPSILON) { direction.x = 0.0f; }
	else if ( distance > CAMERA_SLOW_RADIUS ) direction.x = MAX_CAMERA_MOVING_SPEED;
	else direction.x = MAX_CAMERA_MOVING_SPEED * distance / CAMERA_SLOW_RADIUS;

	distance = abs(direction.y );
	if ( distance < CAMERA_EPSILON) { direction.y = 0.0f; }
	else if ( distance > CAMERA_SLOW_RADIUS ) direction.y = MAX_CAMERA_MOVING_SPEED_Y;
	else direction.y = MAX_CAMERA_MOVING_SPEED_Y * distance / CAMERA_SLOW_RADIUS;
	
	distance = abs(direction.z );
	if ( distance < CAMERA_EPSILON) { direction.z = 0.0f; }
	else if ( distance > CAMERA_SLOW_RADIUS ) direction.z = MAX_CAMERA_MOVING_SPEED_Z;
	else direction.z = MAX_CAMERA_MOVING_SPEED_Z * distance / CAMERA_SLOW_RADIUS;

	D3DXVec3Normalize( &acceleration, &acceleration );
	acceleration.x *= direction.x;
	acceleration.y *= direction.y;
	acceleration.z *= direction.z;
	acceleration -= current_camera_velocity;
	acceleration *= 1.5f;
	float length_acc = D3DXVec3Length( &acceleration );
	if ( length_acc > MAX_CAMERA_ACCELERATION )
	{
		D3DXVec3Normalize( &acceleration, &acceleration );
		acceleration *= MAX_CAMERA_ACCELERATION;
	}

	current_camera_velocity += acceleration;
	current_camera_position += current_camera_velocity * timeslice;
//	current_camera_position.z = camera->transform->Position.z;
//	printf("%0.3f %0.3f %0.3f\n", current_camera_position.x,current_camera_position.y,current_camera_position.z );
}
//**********************************************************************
//	rendering FSM	
//**********************************************************************
void Graphics::GraphicsStateMachine( float timeslice )
{
	switch( state )
	{
	case STATE_SCENE:
		MoveCamera( timeslice );
		UpdateViewingMatrix( timeslice );
		RenderScene( timeslice );
		RenderSprites( sprites3d );
		RenderFont();
		RenderCursor( timeslice );
		RenderBloom( timeslice );
		RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
		RenderSprites( spritesHUD );
		RenderAA();
		break;
	case STATE_LOGO0:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			current_camera_position.x = -100.0f;
			current_camera_position.y = -100.0f;
			istate = ISTATE_ON_UPDATE;
			UpdateViewingMatrix( timeslice );
			SetMainRenderTargets();
			d3dmanager->SetDepthEnable();
			RenderSprites( sprites3d );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
			//RenderAA();
			RenderPausedTexture();
			timer = 0.0f;
			swap_chain->SetFullscreenState( full_screen, NULL );
			break;
		case ISTATE_ON_UPDATE:
			timer += timeslice;
			if  ( timer > 1.0f ) { istate = ISTATE_ON_UPDATE2; }
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, timer - 1.0f );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_UPDATE2:
			timer += timeslice;
			if ( timer > 3.0f ) { istate = ISTATE_ON_EXIT; timer = 0.0f; } 
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, 0.0f );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_EXIT:
			timer += timeslice * 0.75f;
			if ( timer > 1.0f ) { state = STATE_LOGO1; istate = ISTATE_ON_ENTER; }
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, -timer );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		}
		break;
	case STATE_LOGO1:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			current_camera_position.x = -150.0f;
			current_camera_position.y = -100.0f;
			istate = ISTATE_ON_UPDATE;
			UpdateViewingMatrix( timeslice );
			SetMainRenderTargets();
			d3dmanager->SetDepthEnable();
			RenderSprites( sprites3d );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
			//RenderAA();
			RenderPausedTexture();
			timer = 0.0f;
			break;
		case ISTATE_ON_UPDATE:
			timer += timeslice;
			if  ( timer > 1.0f ) { istate = ISTATE_ON_UPDATE2; }
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, timer - 1.0f );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_UPDATE2:
			timer += timeslice;
			if ( timer > 3.0f ) { istate = ISTATE_ON_EXIT; timer = 0.0f; } 
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, 0.0f );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_EXIT:
			timer += timeslice * 0.75f;
			if ( timer > 1.0f ) 
			{ 
				state = STATE_PREMAINMENU; istate = ISTATE_ON_ENTER;
				current_camera_position = camera_stack.top()->transform->Position;
				current_camera_position.y += 40.0f; 
			}
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, -timer );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		}
		break;
	case STATE_PREMAINMENU:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			timer += timeslice;
			if ( timer > 3.5f ) istate = ISTATE_ON_UPDATE;
			AdvanceParticles( timeslice );
			current_scene->Update(timeslice);
			SetMainRenderTargets();
			RenderPostprocess( timeslice, 0, 0 );
			RenderAA();
			break;
		case ISTATE_ON_UPDATE:
			istate = ISTATE_ON_UPDATE2;
			PrepareMenu();
			UpdateViewingMatrix( timeslice );
			RenderScene( timeslice );
			RenderSprites( sprites3d );
			RenderFont();
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
			RenderSprites( spritesHUD );
			RenderPausedTexture();
			timer = 0.0f;
			break;
		case ISTATE_ON_UPDATE2:
			timer += timeslice * 0.75f;
			if  ( timer > 1.0f ) state = STATE_MENU;
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
			RenderFreezingScene( timeslice, timer * 6.0f, timer * 0.5f - 1.0f );
			RenderMenu( timeslice );
//			RenderFont();
			RenderCursor( timeslice );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderGameTitle( timeslice );
			RenderAA();
			break;
		}
		break;
		
	case STATE_POSTMAINMENU:
		timer -= timeslice * 1.5f;
		if  ( timer < 0.01f ) { state = STATE_SCENE; timer = 0.01f; }

//		UpdateViewingMatrix( timeslice );
//		AdvanceParticles( timeslice );
//		current_scene->Update(timeslice);
		RenderFreezingScene( timeslice, timer * 6.0f, -timer * 0.5f );
		RenderCursor( timeslice );
		RenderBloom( timeslice );
		RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
		RenderAA();
		break;

	case STATE_PREPAUSE:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			istate = ISTATE_ON_UPDATE;
			PrepareMenu();
			UpdateViewingMatrix( timeslice );
			RenderScene( timeslice );
			RenderSprites( sprites3d );
			RenderFont();
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
			RenderSprites( spritesHUD );
			RenderAA();
			RenderPausedTexture();
			timer = 0.0f;
			break;
		case ISTATE_ON_UPDATE:
			timer += timeslice * 4.0f;
			if  ( timer > 1.0f ) state = STATE_MENU;
			UpdateViewingMatrix( timeslice );
			RenderFreezingScene( timeslice, timer * 6.0f, -timer * 0.5f );
			RenderMenu( timeslice );
//			RenderFont();
			RenderCursor( timeslice );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderGameTitle( timeslice );
			RenderAA();
			break;
		}
		break;

	case STATE_POSTPAUSE:
		timer -= timeslice * 4.0f;
		if  ( timer < 0.01f ) { state = STATE_SCENE; timer = 0.01f; }
		UpdateViewingMatrix( timeslice );
		RenderFreezingScene( timeslice, timer * 6.0f, -timer * 0.5f );
		RenderCursor( timeslice );
		RenderBloom( timeslice );
		RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
		RenderAA();
		break;

	case STATE_MENU:
		UpdateViewingMatrix( timeslice );
		RenderMenu( timeslice );
//		RenderFont();
		RenderCursor( timeslice );
		RenderBloom( timeslice );
		RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
		RenderGameTitle( timeslice );
		RenderAA();
		break;

	case STATE_PREENDGAME:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			istate = ISTATE_ON_UPDATE;
			MENUSYSTEM->SetDisableSystem();
			UpdateViewingMatrix( timeslice );
			RenderScene( timeslice );
			RenderSprites( sprites3d );
			RenderFont();
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
			RenderSprites( spritesHUD );
			RenderAA();
			RenderPausedTexture();
			timer = 0.0f;
			break;
		case ISTATE_ON_UPDATE:
			timer += timeslice;
			if  ( timer > 1.0f ) { istate = ISTATE_ON_EXIT; }
			UpdateViewingMatrix( timeslice );
			RenderFreezingScene( timeslice, 0.0f, -timer );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_EXIT:
			timer += timeslice;
			if ( timer > 2.0f ) { state = STATE_ENDGAME; istate = ISTATE_ON_ENTER; }
			RenderFreezingScene( timeslice, 0.0f, -1.0f );
			RenderPostprocess( timeslice, NULL, blured_scene->GetSRV() );
			RenderAA();
			break;

		}
		break;

	case STATE_ENDGAME:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			current_camera_position.x = -200.0f;
			current_camera_position.y = -100.0f;
			current_camera_position.z = -45.0f;
			istate = ISTATE_ON_UPDATE;
			UpdateViewingMatrix( timeslice );
			SetMainRenderTargets();
			d3dmanager->SetDepthEnable();
			RenderSprites( sprites3d );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
			//RenderAA();
			RenderPausedTexture();
			timer = 0.0f;
			break;
		case ISTATE_ON_UPDATE:
			timer += timeslice;
			if  ( timer > 1.0f ) { istate = ISTATE_ON_UPDATE2; }
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, timer - 1.0f );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_UPDATE2:
			timer += timeslice;
			if ( timer > 3.0f ) { istate = ISTATE_ON_EXIT; timer = 0.0f; } 
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, 0.0f );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_EXIT:
			timer += timeslice * 0.75f;
			if ( timer > 1.0f ) { state = STATE_CREDITS; istate = ISTATE_ON_ENTER; }
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, -timer );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		}
		break;

	case STATE_CREDITS:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			current_camera_position.x = -250.0f;
			current_camera_position.y = -100.0f;
			istate = ISTATE_ON_UPDATE;
			UpdateViewingMatrix( timeslice );
			SetMainRenderTargets();
			d3dmanager->SetDepthEnable();
			RenderSprites( sprites3d );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
			//RenderAA();
			RenderPausedTexture();
			timer = 0.0f;
			break;
		case ISTATE_ON_UPDATE:
			timer += timeslice;
			if  ( timer > 1.0f ) { istate = ISTATE_ON_UPDATE2; }
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, timer - 1.0f );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_UPDATE2:
			timer += timeslice;
			if ( timer > 3.0f ) { istate = ISTATE_ON_EXIT; timer = 0.0f; } 
			UpdateViewingMatrix( timeslice );
//			AdvanceParticles( timeslice );
//			current_scene->Update(timeslice);
			RenderFreezingScene( timeslice, 0.0f, 0.0f );
			RenderBloom( timeslice );
			RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), blured_scene->GetSRV() );
			RenderAA();
			break;
		case ISTATE_ON_EXIT:
			break;
		}
		break;
	}
}
/**********************************************************************
//	rendering options	
***********************************************************************/
void Graphics::RenderScene( float timeslice )
{
	// set up render targets
	ID3D10RenderTargetView* render_targets[3];
	for ( int i = 0; i < 3; ++i )
	{
		render_targets[i] = render_target_raw[i]->GetRTV();
		device->ClearRenderTargetView( render_targets[i], clear_color[i] );
	}
	device->ClearDepthStencilView( depth_stencil_view, D3D10_CLEAR_DEPTH, 1.0f, 0 );
	device->OMSetRenderTargets( 3, render_targets, depth_stencil_view );

	//
	/// updates
	AdvanceParticles( timeslice );
	current_scene->Update(timeslice);
	
	d3dmanager->SetDepthEnable( true );
	
	RenderMeshes( timeslice );
	RenderMaterials( timeslice );
	RenderParticles( mat_view, mat_proj );
	
//	material_beam->Update( timeslice );
//	material_beam->Draw( device, mat_view, mat_proj );
	
}
//================
void Graphics::RenderMenu( float timeslice )
{
	ID3D10RenderTargetView* render_targets[3];
	
	unsigned int stride = sizeof( MenuVertex );
	unsigned int offset = 0;
	ID3D10Buffer* stream_target;

	// update
	stream_target = vb_menu[1 - cur_menu_buffer];
	device->SOSetTargets( 1, &stream_target, &offset );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	device->IASetVertexBuffers( 0, 1, &vb_menu[cur_menu_buffer], &stride, &offset );
	menu_timeslice_ptr->SetFloat( timeslice );
	menu_global_time_ptr->SetInt( (int)timeGetTime() );
	menu_random_texture_ptr->SetResource( random_texture );
	menu_cursor_position_ptr->SetFloatVector( cursor_position );
	advance_menu->Apply( device );
	device->DrawAuto();
	stream_target = NULL;
	device->SOSetTargets( 1, &stream_target, &offset );
	cur_menu_buffer = 1 - cur_menu_buffer;

	// render	
	for ( int i = 0; i < 3; ++i )
	{
		render_targets[i] = render_target_raw[i]->GetRTV();
		device->ClearRenderTargetView( render_targets[i], clear_color[i] );
	}
	d3dmanager->SetDepthEnable( false );
	device->OMSetRenderTargets( 3, render_targets, depth_stencil_view );
	
	render_menu->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );
	device->IASetVertexBuffers( 0, 1, &vb_menu[cur_menu_buffer], &stride, &offset );
	render_menu->Apply( device );
	device->DrawAuto();

}
//================
void Graphics::RenderGameTitle( float timeslice )
{
	title_y_pos -= 7.0f * timeslice;
	if ( title_y_pos < 0.7f ) title_y_pos = 0.7f;
	// render title sprite
	sprite_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );

	Vec2 size = Vec2( 0.2f * (float)screen_width, 0.2f * (float)screen_height );
	Vec3 position = Vec3( 0.5f * (float)screen_width, title_y_pos * (float)screen_height, 0.0f );
	sprite_position_ptr->SetFloatVector( position );
	sprite_size_ptr->SetFloatVector( size );
	sprite_rotation_ptr->SetFloat( 0.0f );
	sprite_texture_ptr->SetResource( GetTexture("GameTitle")->GetDiffuse() );
	sprite_flag_ptr->SetInt( 64 );

	sprite_shader->Apply(device);
	device->Draw(1, 0);
}
//================
void Graphics::RenderPausedTexture()
{		
	d3dmanager->SetDepthEnable( false );
	ID3D10RenderTargetView* rtv = paused_scene->GetRTV();
	device->OMSetRenderTargets( 1, &rtv, NULL );
	blur_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );
	blur_source_ptr->SetResource( render_target_processed->GetSRV() );
	blur_range_ptr->SetFloat( 0.0f );
	brightness_ptr->SetFloat( 0.0f );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	blur_shader->Apply( device );
	device->Draw( 1, 0 );
	d3dmanager->SetViewport();
}
//================
void Graphics::RenderFreezingScene( float timeslice, float blur_range, float brightness )
{
	static_cast<void>(timeslice);
	d3dmanager->SetDepthEnable( false );
	ID3D10RenderTargetView* render_targets[3];
	for ( int i = 0; i < 3; ++i )
	{
		render_targets[i] = render_target_raw[i]->GetRTV();
		device->ClearRenderTargetView( render_targets[i], clear_color[i] );
	}

	d3dmanager->SetViewport2();
	ID3D10RenderTargetView* rtv = blured_scene->GetRTV();
	device->OMSetRenderTargets( 1, &rtv, NULL );
	
	blur_source_ptr->SetResource( paused_scene->GetSRV() );
	blur_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );

	blur_range_ptr->SetFloat( blur_range );
	brightness_ptr->SetFloat( brightness );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	blur_shader->Apply( device );
	device->Draw( 1, 0 );

	d3dmanager->SetViewport();
	device->OMSetRenderTargets( 3, render_targets, NULL );
//	RenderPostprocess( timeslice, render_target_bloomed4->GetSRV(), render_target_bloomed8->GetSRV() );
}
//================
void Graphics::SetMainRenderTargets()
{
	// set up render targets
	ID3D10RenderTargetView* render_targets[3];
	for ( int i = 0; i < 3; ++i )
	{
		render_targets[i] = render_target_raw[i]->GetRTV();
		device->ClearRenderTargetView( render_targets[i], clear_color[i] );
	}
	device->ClearDepthStencilView( depth_stencil_view, D3D10_CLEAR_DEPTH, 1.0f, 0 );
	device->OMSetRenderTargets( 3, render_targets, depth_stencil_view );

}
/**********************************************************************
	assets manager
***********************************************************************/
void Graphics::InitializeAssets()
{
	std::string id;
	std::string path;
	std::string extra;
	std::vector<ISerializer*> entries;

	std::cout <<"loading assets...\n";

	XMLReader* reader = new XMLReader();
	reader->open( "Assets\\GraphicsAssets.xml" );
	ISerializer* stream = reader->data().clone();
	//read texture data
	stream->readProperty( "textures", *stream );
	stream->readProperty( "texture", entries );
	for ( unsigned int i = 0; i < entries.size(); ++i )
	{
		Texture* texture = NULL;
		texture = new Texture;
		entries[i]->readProperty("id", id );
		if ( entries[i]->readValue("diffuse", path ) )
			texture->LoadDiffuse( device, path.c_str() );
		if ( entries[i]->readValue( "mask", path ) )
			texture->LoadMask( device, path.c_str() );
		if ( entries[i]->readValue( "normal", path ) )
			texture->LoadNormal( device, path.c_str() );
		texture_map[id] = texture;
	}
	for ( unsigned int i = 0; i < entries.size(); ++i ) delete entries[i];
	entries.clear();
	stream->goToParent();
	
	//read mesh data
	stream->readProperty( "meshes", *stream );
	stream->readProperty( "mesh", entries );
	for ( unsigned int i = 0; i < entries.size(); ++i )
	{
		
		entries[i]->readProperty("id", id );
		entries[i]->readProperty("path", path );
		entries[i]->readProperty("animated", extra );
		bool animated = (extra == "true");
		Mesh* mesh = new Mesh;
		mesh->BuildVertexBuffer( device, path.c_str(), animated );
		mesh_map[id] = mesh;
	}
	for ( unsigned int i = 0; i < entries.size(); ++i ) delete entries[i];
	entries.clear();
	stream->goToParent();
	
	//read animation data
	stream->readProperty( "animations", *stream );
	stream->readProperty( "animation", entries );
	for ( unsigned int i = 0; i < entries.size(); ++i )
	{
		entries[i]->readProperty("id", id );
		entries[i]->readProperty("path", path );

		Animation* anim = new Animation;
		anim->Initialize( path.c_str() );
		animation_map[id] = anim;
	}
	for ( unsigned int i = 0; i < entries.size(); ++i ) delete entries[i];
	entries.clear();
	stream->goToParent();
	
	//read scene data
	stream->readProperty( "scenes", *stream );
	stream->readProperty( "scene", entries );
	for ( unsigned int i = 0; i < entries.size(); ++i )
	{
		entries[i]->readProperty("id", id );
		entries[i]->readProperty("path", path );

		Scene* scene = new Scene;
		scene->Initialize( path.c_str() );
		scene_map[id] = scene;
	}
	for ( unsigned int i = 0; i < entries.size(); ++i ) delete entries[i];
	entries.clear();
	stream->goToParent();
	
	delete stream;
	delete reader;

}
//================
void Graphics::FreeAssets()
{
	SceneMapType::iterator scit;
	for ( scit = scene_map.begin(); scit != scene_map.end(); ++scit )
	{
		S_DELETE(scit->second);
	}
	TextureMapType::iterator tit;
	for ( tit = texture_map.begin(); tit != texture_map.end(); ++tit )
	{
		//tit->second->Free();
		//delete tit->second;
		S_DELETE(tit->second);
	}
	AnimationMapType::iterator ait;
	for ( ait = animation_map.begin(); ait != animation_map.end(); ++ait )
	{
		S_DELETE(ait->second);
	}
	MeshMapType::iterator msit;
	for ( msit = mesh_map.begin(); msit != mesh_map.end(); ++msit )
	{
		S_DELETE(msit->second);
	}
}
/**********************************************************************
	frustum culling
***********************************************************************/
//================
bool Graphics::FrustumTest( const Transform* transform )
{
	return FrustumTest( &transform->Position, &transform->Scale );
}
//================
bool Graphics::FrustumTest( const Vec3* position, const Vec3* scaling )
{
	float left = position->x - scaling->x;
	float right = position->x + scaling->x;
	float top = position->y + scaling->y;
	float bottom = position->y - scaling->y;
	if ( right < screen_left || left > screen_right || top < screen_bottom || bottom > screen_top ) return false;
	return true;
}
//================
void Graphics::ScreenPosition( const Vec3* position, float* screen_x, float* screen_y )
{
	if ( screen_x )
	{
		*screen_x = ( position->x - screen_left ) / ( screen_right - screen_left );
	}
	if ( screen_y )
	{
		*screen_y = ( position->y - screen_bottom ) / ( screen_top - screen_bottom );
	}
}
//================
void Graphics::GetScreenRange( float range[2], float depth )
{
	float ratio = depth - current_camera_position.z;
	ratio /= -current_camera_position.z;
	float center = screen_left + screen_right;
	center *= 0.5f;

	range[0] = screen_left - center;
	range[0] *= ratio;
	range[0] += center;
	range[1] = screen_right - center;
	range[1] *= ratio;
	range[1] += center;
}
/**********************************************************************
	mesh rendering
***********************************************************************/
void Graphics::InitializeMeshRenderer()
{
	mesh_shader = new Shader;
	mesh_shader->Initialize( device, "shaders\\MeshShader.fx", 
					VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_POSITION3|
					VERTEX_ELEMENT_TEXCOORD0|VERTEX_ELEMENT_COLOR|VERTEX_ELEMENT_BONES|VERTEX_ELEMENT_WEIGHTS|
					VERTEX_ELEMENT_PER_INSTANCE_FLAG|
					VERTEX_ELEMENT_TRANSFORM );
	static_mesh_shader = new Shader;
	static_mesh_shader->Initialize( device, "shaders\\StaticMeshShader.fx",
					VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_NORMAL|VERTEX_ELEMENT_COLOR|VERTEX_ELEMENT_TRANSFORM|VERTEX_ELEMENT_PER_INSTANCE_FLAG);
	
}
//================
void Graphics::RenderMeshes( float timeslice )
{
	MeshMapType::iterator msit;
	for ( msit = mesh_map.begin(); msit != mesh_map.end(); ++msit )
	{
		msit->second->Update( timeslice );
	}

	static_mesh_shader->SetTransformationConstants( device, mat_view, mat_proj );
	mesh_shader->SetTransformationConstants( device, mat_view, mat_proj );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	for ( msit = mesh_map.begin(); msit != mesh_map.end(); ++msit )
	{
		msit->second->Draw( device, static_mesh_shader, mesh_shader );
	}
	
}
//================
void Graphics::FreeMeshRenderer()
{
	S_DELETE(mesh_shader);
	S_DELETE(static_mesh_shader);
}
/**********************************************************************
//	materials
***********************************************************************/
void Graphics::InitializeMaterials()
{
	/// set up materials
		//grass
	grass_shader = new Shader;
	grass_shader->Initialize( device, "shaders\\grassShader.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_NORMAL|VERTEX_ELEMENT_COLOR );
	Material* grass = new Grass;
	grass->SetShader( grass_shader );
	grass->Initialize( device );
	material_map[MAT_GRASS] = grass;
		//spikes
	spikes_shader = new Shader;
	spikes_shader->Initialize( device, "shaders\\spikesShader.fx", VERTEX_ELEMENT_POSITION0 );
	Material* spikes = new Spikes;
	spikes->SetShader( spikes_shader );
	spikes->Initialize( device );
	material_map[MAT_SPIKES] = spikes;
		//skyline
	skyline_shader = new Shader;
	skyline_shader->Initialize( device, "shaders\\skylineShader.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1 );
	Material* skyline = new Skyline;
	skyline->SetShader( skyline_shader );
	skyline->Initialize( device );
	material_map[MAT_SKYLINE] = skyline;
		//textured box
	textured_box_shader = new Shader;
	textured_box_shader->Initialize( device, "shaders\\texturedBoxShader.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	Material* textured_box = new TexturedBox;
	textured_box->SetShader( textured_box_shader );
	textured_box->Initialize( device );
	material_map[MAT_TEXTUREDBOX] = textured_box;
		//water
	water_shader = new Shader;
	water_shader->Initialize( device, "shaders\\waterShader.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1 );
	Material* water = new Water;
	water->SetShader( water_shader );
	water->Initialize( device );
	material_map[MAT_WATER] = water;
}
//================
void Graphics::RenderMaterials( float timeslice )
{
	MaterialMapType::iterator mait;
	for ( mait = material_map.begin(); mait != material_map.end(); ++mait )
	{
		mait->second->Update( timeslice );
		mait->second->Draw( device, mat_view, mat_proj );
	}
}
//================
void Graphics::FreeMaterials()
{
	S_DELETE(grass_shader);
	S_DELETE(spikes_shader);
	S_DELETE(mesh_shader);
	S_DELETE(skyline_shader);
	S_DELETE(textured_box_shader);
	S_DELETE(water_shader);
	MaterialMapType::iterator mit;
	for ( mit = material_map.begin(); mit != material_map.end(); ++mit )
	{
		S_DELETE(mit->second);
	}
}
/**********************************************************************
	particle systems
***********************************************************************/
void Graphics::InitializeParticleSystem()
{
	BuildRandomTexture();
	ParticleSystem* particle_system;
	particle_system = new PESquare( device );
	particle_system->Initialize();
	particle_system_map["square"] = particle_system;

	particle_system = new PEEnergyField( device );
	particle_system->Initialize();
	particle_system_map["energy_field"] = particle_system;

	particle_system = new PELaser( device );
	particle_system->Initialize();
	particle_system_map["laser"] = particle_system;

	particle_system = new PEClouds( device );
	particle_system->Initialize();
	particle_system_map["clouds"] = particle_system;
}
//================
void Graphics::AdvanceParticles( float timeslice )
{
	ParticleSystemMapType::iterator it;
	for ( it = particle_system_map.begin(); it != particle_system_map.end(); ++it )
	{
		it->second->Update( timeslice );
	}
}
//================
void Graphics::RenderParticles( Mat4& p_mat_view, Mat4& p_mat_proj )
{
	ParticleSystemMapType::iterator it;
	for ( it = particle_system_map.begin(); it != particle_system_map.end(); ++it )
	{
		it->second->Draw( p_mat_view, p_mat_proj );
	}
}
//================
void Graphics::BuildRandomTexture()
{
	int num_random_values = 2048;

    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = new float[num_random_values * 4];
    InitData.SysMemPitch = num_random_values * 4 * sizeof( float );
    InitData.SysMemSlicePitch = num_random_values * 4 * sizeof( float );
    for( int i = 0; i < num_random_values * 4; i++ )
    {
        ( ( float* )InitData.pSysMem )[i] = Rand( 0.0f, 1.0f );
    }

	ID3D10Texture1D* texture;
	D3D10_TEXTURE1D_DESC dstex;
	dstex.Width = num_random_values;
	dstex.MipLevels = 1;
	dstex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	dstex.Usage = D3D10_USAGE_DEFAULT;
	dstex.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	dstex.CPUAccessFlags = 0;
	dstex.MiscFlags = 0;
	dstex.ArraySize = 1;
	device->CreateTexture1D( &dstex, &InitData, &texture );
	delete [] InitData.pSysMem;

	// Create the resource view
	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
	SRVDesc.Format = dstex.Format;
	SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE1D;
	SRVDesc.Texture2D.MipLevels = dstex.MipLevels;
	device->CreateShaderResourceView( texture, &SRVDesc, &random_texture );

	S_RELEASE( texture );
}
//================
void Graphics::FreeParticleSystem()
{
	S_RELEASE(random_texture);
//	S_RELEASE(vb_launcher);

	ParticleSystemMapType::iterator pit;
	for ( pit = particle_system_map.begin(); pit != particle_system_map.end(); ++pit )
	{
		S_DELETE( pit->second );
	}

}
//================
void Graphics::RegisterLineList( LineListType* _line_list )
{
	((PELaser*)particle_system_map["laser"])->SetLineList( _line_list );
}
//================
void Graphics::RegisterLineList2( LineListType* _line_list )
{
	((PELaser*)particle_system_map["laser"])->SetLineList2( _line_list );
}
//================
void Graphics::RegisterBeamColor( Vec4* c )
{
	((PELaser*)particle_system_map["laser"])->SetColor( c );
}
//================
/**********************************************************************
	sprites
***********************************************************************/
void Graphics::InitializeSprite()
{
	/// sprite shader
	sprite_shader = new Shader;
	sprite_shader->Initialize( device, "shaders\\SpriteShader.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1 );
	sprite_texture_ptr = sprite_shader->GetVariableByName("color")->AsShaderResource();
	sprite_position_ptr = sprite_shader->GetVariableByName("position")->AsVector();
	sprite_size_ptr = sprite_shader->GetVariableByName("size")->AsVector();
	sprite_rotation_ptr = sprite_shader->GetVariableByName("rotation")->AsScalar();
	sprite_flag_ptr = sprite_shader->GetVariableByName("flag")->AsScalar();
}
//================
void Graphics::FreeSprite()
{
	S_DELETE(sprite_shader);
}
//================
void Graphics::RegisterSprite3D( Sprite* sprite )
{ sprites3d.push_back(sprite); }
//================
void Graphics::RemoveSprite3D( Sprite* sprite )
{
	sprites3d.erase( sprite );
}
//================
void Graphics::RegisterSpriteHUD( Sprite* sprite )
{ spritesHUD.push_back(sprite); }
//================
void Graphics::RemoveSpriteHUD( Sprite* sprite )
{
	spritesHUD.erase( sprite );
}
//================
void Graphics::RenderSprites( SpriteListType& sprites )
{
//	d3dmanager->SetBlendAddDiffuse();
	//d3dmanager->SetDepthEnable();
	sprite_shader->SetTransformationConstants( device, mat_view, mat_proj );

	SpriteListType::iterator sit;
	for ( sit = sprites.begin(); sit != sprites.end(); ++sit )
	{
		Transform* transform = sit->transform;
		Texture* s_texture = sit->texture;
		Vec2 size = Vec2( transform->Scale.x * 0.5f, transform->Scale.y * 0.5f );
		sprite_position_ptr->SetFloatVector( transform->Position );
		sprite_size_ptr->SetFloatVector( size );
		sprite_rotation_ptr->SetFloat( transform->RotationZ );
		sprite_texture_ptr->SetResource( s_texture->GetDiffuse() );
		sprite_flag_ptr->SetInt( sit->flag );

		sprite_shader->Apply(device);
		device->Draw(1, 0);
	}
}
/**********************************************************************
	postprocessing/aa
***********************************************************************/
void Graphics::InitializePostprocess()
{
	float viewport[2] = { 1/(float)screen_width, 1/(float)screen_height };
	/// post processing shader
	post_processing_shader = new Shader;
	post_processing_shader->Initialize( device, "shaders\\postProcessingShader.fx", 0 );
	p_color_ptr = post_processing_shader->GetVariableByName("color")->AsShaderResource();
	p_normal_flag_ptr = post_processing_shader->GetVariableByName("normal_flag")->AsShaderResource();
	p_view_position_ptr = post_processing_shader->GetVariableByName("view_position")->AsShaderResource();
	bloom4_texture_ptr = post_processing_shader->GetVariableByName("bloom4_texture")->AsShaderResource();
	bloom8_texture_ptr = post_processing_shader->GetVariableByName("bloom8_texture")->AsShaderResource();
	post_processing_viewport_ptr = post_processing_shader->GetVariableByName("viewport")->AsVector();
	post_processing_viewport_ptr->SetFloatVector( viewport );
	frametime_ptr = post_processing_shader->GetVariableByName("frametime")->AsScalar();
	camera_position_ptr = post_processing_shader->GetVariableByName("camera_position")->AsVector();
	mouse_position_ptr = post_processing_shader->GetVariableByName("mouse_position")->AsVector();

	/// double textured (blur)
	float clear_color[4] = {1.0f,1.0f,1.0f,1.0f};
	render_target_processed = new RenderTargetTexture();
	render_target_processed->Initialize( device, screen_width, screen_height );
	device->ClearRenderTargetView( render_target_processed->GetRTV(), clear_color );

	/// fxaa shader
	fxaa_shader = new Shader;
	fxaa_shader->Initialize( device, "shaders\\FXAAShader.fx", 0 );
	processed_texture_ptr = fxaa_shader->GetVariableByName( "processed_texture" )->AsShaderResource();
	fxaa_viewport_ptr = fxaa_shader->GetVariableByName("viewport")->AsVector();
	fxaa_viewport_ptr->SetFloatVector( viewport );

	/// blur shader
	blur_shader = new Shader;
	blur_shader->Initialize( device, "shaders\\blurShader.fx", 0 );	
	blur_source_ptr = blur_shader->GetVariableByName( "blur_source" )->AsShaderResource();
	blur_range_ptr = blur_shader->GetVariableByName( "blur_range" )->AsScalar();
	brightness_ptr = blur_shader->GetVariableByName( "brightness" )->AsScalar();
	blur_viewport_ptr = blur_shader->GetVariableByName("viewport")->AsVector();
	blur_viewport_ptr->SetFloatVector( viewport );	
}
//================
void Graphics::RenderPostprocess( float timeslice, ID3D10ShaderResourceView* add_texture0, ID3D10ShaderResourceView* add_texture1 )
{
	d3dmanager->SetDepthEnable( false );
	//set up render target
	ID3D10RenderTargetView* rtv_processed = render_target_processed->GetRTV();
	device->OMSetRenderTargets( 1, &rtv_processed, NULL );
	//apply filters
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	post_processing_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );
	p_color_ptr->SetResource( render_target_raw[0]->GetSRV() );
	p_normal_flag_ptr->SetResource( render_target_raw[1]->GetSRV() );
	p_view_position_ptr->SetResource( render_target_raw[2]->GetSRV() );
	bloom4_texture_ptr->SetResource( add_texture0 );
	bloom8_texture_ptr->SetResource( add_texture1 );
	frametime_ptr->SetFloat( 1000.0f * timeslice );
	post_processing_shader->Apply( device );
	device->Draw( 1, 0 );
}
//================
void Graphics::RenderAA()
{
	//fxaa
	device->OMSetRenderTargets( 1, &render_target_screen, NULL );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	fxaa_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );
	processed_texture_ptr->SetResource( render_target_processed->GetSRV() );
	fxaa_shader->Apply( device );
	device->Draw( 1, 0 );	
}
//================
void Graphics::FreePostprocess()
{
	S_DELETE(post_processing_shader);
	S_DELETE(render_target_processed);
	S_DELETE(fxaa_shader);
	S_DELETE(blur_shader);
}
/**********************************************************************
	bloom effect
***********************************************************************/
void Graphics::InitializeBloom()
{
	Vec2 viewport( 1.0f/(float)screen_width, 1.0f/(float)screen_height );
	bloom_pick_shader = new Shader;
	bloom_pick_shader->Initialize( device, "shaders\\bloomPickShader.fx", 0 );
	render_target_bloom_source = new RenderTargetTexture;
	render_target_bloom_source->Initialize( device, screen_width / 2, screen_height / 2 );
	b_diffuse_ptr = bloom_pick_shader->GetVariableByName("diffuse")->AsShaderResource();
	b_view_position_ptr = bloom_pick_shader->GetVariableByName("view_position")->AsShaderResource();
	ID3D10EffectVectorVariable* bloom_viewport_ptr = bloom_pick_shader->GetVariableByName("viewport")->AsVector();
	bloom_viewport_ptr->SetFloatVector( viewport );

	bloom_shader = new Shader;
	bloom_shader->Initialize( device, "shaders\\bloomShader.fx", 0 );
	render_target_bloomed4 = new RenderTargetTexture;
	render_target_bloomed4->Initialize( device, screen_width / 2, screen_height / 2 );
	render_target_bloomed8 = new RenderTargetTexture;
	render_target_bloomed8->Initialize( device, screen_width / 4, screen_height / 4 );
	bloom_source_ptr = bloom_shader->GetVariableByName("bloom_source")->AsShaderResource();
	sample_size_ptr = bloom_shader->GetVariableByName("sample_size")->AsScalar();
	bloom_viewport_ptr = bloom_shader->GetVariableByName("viewport")->AsVector();
	bloom_viewport_ptr->SetFloatVector( viewport );

}
//================
void Graphics::RenderBloom( float )
{
	d3dmanager->SetViewport2();
	ID3D10RenderTargetView* rtv_bloom = render_target_bloom_source->GetRTV();
	device->OMSetRenderTargets( 1, &rtv_bloom, NULL );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	bloom_pick_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );
	b_diffuse_ptr->SetResource(render_target_raw[0]->GetSRV() );
	b_view_position_ptr->SetResource( render_target_raw[2]->GetSRV() );
	bloom_pick_shader->Apply(device);
	device->Draw(1, 0);

	bloom_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );

	d3dmanager->SetViewport2();
	bloom_source_ptr->SetResource(render_target_bloom_source->GetSRV() );
	rtv_bloom = render_target_bloomed4->GetRTV();
	device->OMSetRenderTargets( 1, &rtv_bloom, NULL );
	sample_size_ptr->SetFloat( 1.0f );
	bloom_shader->Apply(device);
	device->Draw(1, 0);
	
	d3dmanager->SetViewport4();
	bloom_source_ptr->SetResource( render_target_bloomed4->GetSRV() );
	rtv_bloom = render_target_bloomed8->GetRTV();
	device->OMSetRenderTargets( 1, &rtv_bloom, NULL );
	sample_size_ptr->SetFloat( 1.0f );
	bloom_shader->Apply(device);
	device->Draw(1, 0);
	
		//restore viewport
	d3dmanager->SetViewport();

}
//================
void Graphics::FreeBloom()
{
	S_DELETE(bloom_shader);
	S_DELETE(bloom_pick_shader);
	S_DELETE(render_target_bloom_source);
	S_DELETE(render_target_bloomed4);
	S_DELETE(render_target_bloomed8);
}
/**********************************************************************
//	menu system
***********************************************************************/
void Graphics::InitializeMenu()
{
	advance_menu = new Shader;
	advance_menu->Initialize( device, "Shaders\\advanceMenu.fx", 
						VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_TEXCOORD0|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	render_menu = new Shader;
	render_menu->Initialize( device, "Shaders\\menuShader.fx", 
						VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_TEXCOORD0|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	menu_cursor_position_ptr = advance_menu->GetVariableByName("cursor_position")->AsVector();
	menu_timeslice_ptr = advance_menu->GetVariableByName("elapsed_time")->AsScalar();
	menu_global_time_ptr = advance_menu->GetVariableByName("global_time")->AsScalar();
	menu_random_texture_ptr = advance_menu->GetVariableByName("random_texture")->AsShaderResource();
	menu_button_texture_ptr = render_menu->GetVariableByName("button_texture")->AsShaderResource();
	menu_button_texture_ptr->SetResource( GetTexture("GUIButtonTexture")->GetDiffuse() );

	D3D10_BUFFER_DESC vb_desc;
	HRESULT hr;
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( MenuVertex ) * MAX_GUI_COMPONENTS;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, NULL, &vb_menu_initial );
	if ( FAILED(hr) ) exit(0);/////ErrorIf
	
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( MenuVertex ) * MAX_GUI_COMPONENTS * 100;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, NULL, &vb_menu[0] );
	if ( FAILED(hr) ) exit(0);/////ErrorIf
	hr = device->CreateBuffer( &vb_desc, NULL, &vb_menu[1] );
	if ( FAILED(hr) ) exit(0);/////ErrorIf

	paused_scene = new RenderTargetTexture();
	paused_scene->Initialize( device, screen_width, screen_height );

	blured_scene = new RenderTargetTexture();
	blured_scene->Initialize( device, screen_width / 2, screen_height / 2 );

}
//================
void Graphics::PrepareMenu()
{
	unsigned int stride = sizeof( MenuVertex );
	unsigned int offset = 0;
	ID3D10Buffer* stream_target;
	MenuSystem::MenuType* menu = MENUSYSTEM->GetMenu();
	MenuVertex* mapped_data;

	vb_menu_initial->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&mapped_data );
	MenuSystem::MenuType::iterator it;
	int button_count = 0;
	for ( it = menu->begin(); it != menu->end(); ++it )
	{
		if ( (*it)->type != BUTTON ) continue;
		mapped_data[button_count].position = Vec3( (*it)->x, (*it)->y, 300.0f + button_count * 100.0f );
		mapped_data[button_count].extra = Vec3( (*it)->width, (*it)->height, -3000.0f );
		mapped_data[button_count].tex = Vec2( button_count * 0.33f + 0.1667f, 0.5f );
		mapped_data[button_count].type = 0;
		++button_count;
	}
	vb_menu_initial->Unmap();

	cur_menu_buffer = 0;
	stream_target = vb_menu[cur_menu_buffer];
	device->SOSetTargets( 1, &stream_target, &offset );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	device->IASetVertexBuffers( 0, 1, &vb_menu_initial, &stride, &offset );
	menu_timeslice_ptr->SetFloat( 0.0f );
	advance_menu->Apply( device );
	device->Draw( button_count, 0 );
	stream_target = NULL;
	device->SOSetTargets( 1, &stream_target, &offset );
	
	title_y_pos = 2.0f;
}
//================
void Graphics::FreeMenu()
{
	S_DELETE(paused_scene);
	S_DELETE(blured_scene);
	S_DELETE(advance_menu);
	S_DELETE(render_menu);
	S_RELEASE(vb_menu_initial);
	S_RELEASE(vb_menu[0]);
	S_RELEASE(vb_menu[1]);
}
/**********************************************************************
	cursor rendering
***********************************************************************/
void Graphics::RenderCursor( float timeslice )
{
	static_cast<void>(timeslice);
	unsigned int stride = sizeof( CursorVertex );
	unsigned int offset = 0;
	ID3D10Buffer* stream_target;

	stream_target = vb_cursor[ cur_cursor_vb_target ];
	device->SOSetTargets( 1, &stream_target, &offset );
	device->IASetVertexBuffers( 0, 1, &vb_cursor[1 - cur_cursor_vb_target], &stride, &offset );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	cursor_position_ptr->SetFloatVector( cursor_position );
	cursor_random_texture_ptr->SetResource( random_texture );
	cursor_global_time_ptr->SetInt( (unsigned int)timeGetTime() );
	update_cursor->Apply( device );
	device->DrawAuto();
	stream_target = NULL;
	device->SOSetTargets( 1, &stream_target, &offset );


	d3dmanager->SetDepthEnable( false );
	render_cursor->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );
	device->IASetVertexBuffers( 0, 1, &vb_cursor[cur_cursor_vb_target], &stride, &offset );
	render_cursor->Apply( device );
	device->DrawAuto();

	cur_cursor_vb_target = 1 - cur_cursor_vb_target;
}
//================
void Graphics::InitializeCursor()
{
	ID3D10Buffer* vb_cursor_launcher;
	D3D10_BUFFER_DESC vb_desc;
	D3D10_SUBRESOURCE_DATA vb_data;
	HRESULT hr;
	
	const int line_count = 2;
	CursorVertex launcher[line_count ];
	for ( int i = 0; i < line_count; ++i )
	{
		launcher[i].position0 = Vec3( cursor_position.x, cursor_position.y, (float)i * 4.0f );
		launcher[i].position1 = Vec3( cursor_position.x, cursor_position.y, 0.0f );
		launcher[i].type = 1;	//launcher
	}
	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( CursorVertex ) * line_count;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;
	vb_data.SysMemPitch = 0;
	vb_data.SysMemSlicePitch = 0;
	vb_data.pSysMem = launcher;
	hr = device->CreateBuffer( &vb_desc, &vb_data, &vb_cursor_launcher );
	if ( FAILED(hr) ) exit(0);/////ErrorIf

	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( CursorVertex ) * 500;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;
	hr = device->CreateBuffer( &vb_desc, NULL, &vb_cursor[0] );
	if ( FAILED(hr) ) exit(0);/////ErrorIf
	hr = device->CreateBuffer( &vb_desc, NULL, &vb_cursor[1] );
	if ( FAILED(hr) ) exit(0);/////ErrorIf
	
	update_cursor = new Shader;
	update_cursor->Initialize( device, "Shaders\\advanceCursor.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	render_cursor = new Shader;
	render_cursor->Initialize( device, "Shaders\\cursorShader.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_PER_VERTEX_FLAG );

	cursor_position_ptr = update_cursor->GetVariableByName("cursor_position")->AsVector();
	cursor_timeslice_ptr = update_cursor->GetVariableByName("timeslice")->AsScalar();
	cursor_global_time_ptr = update_cursor->GetVariableByName("global_time")->AsScalar();
	cursor_random_texture_ptr = update_cursor->GetVariableByName("random_texture")->AsShaderResource();

	/// initialize the launcher( stream out once )
	unsigned int stride = sizeof( CursorVertex );
	unsigned int offset = 0;
	ID3D10Buffer* stream_target;
	stream_target = vb_cursor[0];
	device->SOSetTargets( 1, &stream_target, &offset );
	device->IASetVertexBuffers( 0, 1, &vb_cursor_launcher, &stride, &offset );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	cursor_position_ptr->SetFloatVector( cursor_position );
	update_cursor->Apply( device );
	device->Draw( line_count, 0 );
	stream_target = NULL;
	device->SOSetTargets( 1, &stream_target, &offset );
	
	cur_cursor_vb_target = 1;
	S_RELEASE(vb_cursor_launcher);
}
//================
void Graphics::FreeCursor()
{
	S_DELETE(update_cursor);
	S_DELETE(render_cursor);
	S_RELEASE(vb_cursor[0]);
	S_RELEASE(vb_cursor[1]);
}
/********************
*	font engine		*
********************/
//================
void Graphics::InitializeFont()
{
	////set up font engine
	font_shader = new Shader;
	font_shader->Initialize( device, "shaders\\fontShader.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_NORMAL|VERTEX_ELEMENT_COLOR|VERTEX_ELEMENT_TEXCOORD0|VERTEX_ELEMENT_TEXCOORD1 );
	RegisterFont("TimesNewRoman");
	font_position_ptr = font_shader->GetVariableByName("world_position")->AsVector();
	font_img_ptr = font_shader->GetVariableByName("font_texture")->AsShaderResource();
}
//================
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
//================
void Graphics::RegisterText( Text* text )
{
	Font::BuildVertexBuffer( device, &text->vertex_buffer );
	text_list.push_back( text );
}
//================
void Graphics::RemoveText( Text* text )
{
	text_list.erase( text );
	S_RELEASE( text->vertex_buffer );
	S_DELETE( text );
}
//================
void Graphics::FreeFont()
{
	FontMapType::iterator fit;
	for ( fit = font_map.begin(); fit != font_map.end(); ++fit )
	{
		S_DELETE( fit->second );
	}
	font_map.clear();

	TextListType::iterator tit;
	tit = text_list.begin();
	while ( tit != text_list.end() )
	{
		Text* text = tit;
		++tit;
		RemoveText(text);
	}
	S_DELETE(font_shader);
}
//================
void Graphics::RenderFont()
{
	d3dmanager->SetDepthEnable( false );
	font_shader->SetTransformationConstants( device, mat_ortho_view, mat_ortho_proj );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	TextListType::iterator it;
	for ( it = text_list.begin(); it != text_list.end(); ++it )
	{
		Font* font = font_map[it->font];
		if ( it->changed ) 
		{
			if (!font) continue;
			font->UpdateVertexBuffer( it );
		}
		unsigned int stride = font->GetVertexTypeSize();
		unsigned int offset = 0;
		device->IASetVertexBuffers( 0, 1, &(it->vertex_buffer), &stride, &offset );
		Vec2 pos( it->x, it->y );
		font_position_ptr->SetFloatVector( pos );
		font_img_ptr->SetResource( font->GetFontTexture() );
		font_shader->Apply( device );
		
		device->Draw( it->text.size(), 0 );
	}
}
/**********************************************************************
	debug rendering
***********************************************************************/
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
					VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_NORMAL|VERTEX_ELEMENT_COLOR|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	
	render_debug_information = true;
}
//================
void Graphics::FreeDebugDrawing()
{
	S_RELEASE(debug_vertex_buffer);
	S_DELETE(debug_shader);
}
//================
void Graphics::RenderDebug()
{
	DebugPrimitive* mapped_data;
	
	debug_shader->SetTransformationConstants( device, mat_view, mat_proj );
	debug_vertex_buffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void**)&mapped_data );
	int primitive_count = 0;
	DebugGeometryListType::iterator g_it;
	for ( g_it = debug_geometry_list.begin(); g_it != debug_geometry_list.end(); ++g_it )
	{
		DebugGeometry::DebugShapeListType::iterator primitive_it;
		DebugGeometry::DebugShapeListType& primitive_list = g_it->debug_shapes;
		for ( primitive_it = primitive_list.begin(); primitive_it != primitive_list.end(); ++primitive_it )
		{
			mapped_data[primitive_count].type = (int)(*primitive_it).second->type;
			mapped_data[primitive_count].v0 = g_it->transform->Position;
			mapped_data[primitive_count].v1 = (*primitive_it).second->param;
			mapped_data[primitive_count].color = (*primitive_it).second->color;
			++primitive_count;
		}
	}
	debug_vertex_buffer->Unmap();
	unsigned int stride = sizeof( DebugPrimitive );
	unsigned int offset = 0;
	device->IASetVertexBuffers( 0, 1, &debug_vertex_buffer, &stride, &offset );
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	debug_shader->Apply( device );
	device->Draw( primitive_count, 0 );
}
//================
void Graphics::RegisterDebugGeometry( DebugGeometry* geometry )
{
	debug_geometry_list.push_back( geometry );
}
//================
void Graphics::RemoveDebugGeometry( DebugGeometry* geometry )
{
	debug_geometry_list.erase(geometry);
}
//================
void Graphics::ScreenToWorldSpace( Vec2* out, Vec2* in, float depth )
{
	Vec3 temp( in->x, in->y, 0.0f );
	Mat4 i;
	D3DXMatrixIdentity( &i );
	D3DXVec3Unproject( &temp, &temp, d3dmanager->GetViewport(), &mat_proj, &mat_view, &i );
	
	float scale = ( depth - current_camera_position.z )/ FRONT;
	temp.x -= current_camera_position.x;
	temp.y -= current_camera_position.y;
	temp.x *= scale;
	temp.y *= scale;
	temp.z = 0.0f;
	temp.x += current_camera_position.x;
	temp.y += current_camera_position.y;

	out->x = temp.x;
	out->y = temp.y;
}

}//end namespace
