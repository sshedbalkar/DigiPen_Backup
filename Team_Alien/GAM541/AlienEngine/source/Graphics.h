///////////////////////////////////////////////////////////////////////////////
///
///	\file Graphics.h
/// Defines grahics system.
///	
///	Authors: , 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

#include "VMath.h"
#include "Engine.h"
#include <deque>
#include <list>
#include <stack>
#include <map>
#include "material.h"

namespace Framework
{	
//Forward Declaration of Graphics Objects
class Camera;
class ParticleEmitter;
class ParticleSystem;
class Shader;
class D3DManager;
class Mesh;
class Animation;
class Texture;
class Scene;
class Transform;
class Font;
class Text;
class RenderTargetTexture;
class Material;
class Sprite;
class DebugGeometry;/// for debugging


class Graphics : public ISystem
{
private:


private:
	D3DManager* d3dmanager;
	ID3D10Device* device;
	ID3D10RenderTargetView* render_target_screen;
	ID3D10ShaderResourceView* srv_screen;
	ID3D10DepthStencilView* depth_stencil_view;
	IDXGISwapChain* swap_chain;
	RenderTargetTexture* render_target_raw[3];
	int screen_width;
	int screen_height;
	HWND hwnd;
	bool full_screen;
	///transformations
	Mat4 mat_view;
	Mat4 mat_proj;
	Mat4 mat_ortho_view;
	Mat4 mat_ortho_proj;
	/// Scene
	Scene* current_scene;
	// state machine state
	int state;
	int istate;
	float timer;
public:
	Graphics();
	virtual ~Graphics();
	virtual std::string GetName() { return "Graphics";};

	void SetWindowProperties( HWND _hwnd, int w, int h, bool _full_screen) { hwnd = _hwnd; screen_width = w; screen_height = h; full_screen = _full_screen; }

	//// basic game functions
	virtual void Initialize();
	virtual void Update( float timeslice );
	virtual void Free();
	virtual void Load();
	virtual void Unload();

	virtual void SendMessage( Message* msg );
	void GetScreenSize( int* width, int* height ){ *width = screen_width; *height = screen_height; }

	//// transform
	void ScreenToWorldSpace( Vec2* out, Vec2* in, float depth = 0.0f );
	//// camera
	void PushCamera(Camera*);
	void PopCamera();

	float MAX_CAMERA_MOVING_SPEED;
private:
	void UpdateProjectionAndOrthoMatrices();
	void UpdateViewingMatrix( float timeslice );
	void UpdateReflectionMatrices();

	/// render state machine
	void GraphicsStateMachine( float );
	// render options
	void RenderScene( float );
	void RenderFreezingScene( float timeslice, float blur_range, float brightness );
	void RenderMenu( float );
	void RenderGameTitle( float );
	void RenderPausedTexture();
	void SetMainRenderTargets();

	/************************
	*	camera movement		*
	************************/
private:
	Vec3 current_camera_position;
	Vec3 current_camera_velocity;
	std::stack<Camera*> camera_stack;
private:
	void MoveCamera( float timeslice );
	
	/************************
	*	assets manager		*
	************************/
private:
	typedef std::map<std::string,Mesh*> MeshMapType;
	typedef std::map<std::string,Animation*> AnimationMapType;
	typedef std::map<std::string,Texture*> TextureMapType;
	typedef std::map<std::string,Scene*> SceneMapType;
private:
	MeshMapType mesh_map;
	AnimationMapType animation_map;
	TextureMapType texture_map;
	SceneMapType scene_map;
public:
	/// get assets
	Mesh* GetMesh( const char* id ) { return mesh_map[id]; }
	Animation* GetAnimation( const char* id ) { return animation_map[id]; }
	Texture* GetTexture( const char* id ) { return texture_map.at(id); }
private:
	void InitializeAssets();
	void FreeAssets();
	/************************
	*	frustum culling		*
	************************/
	// screen size on z=0 plane
private:
	float screen_left;
	float screen_right;
	float screen_top;
	float screen_bottom;
public:
	bool FrustumTest( const Transform* transform );
	bool FrustumTest( const Vec3* position, const Vec3* scaling );
	void ScreenPosition( const Vec3* position, float* screen_x, float* screen_y );
	void GetScreenRange( float range[2], float depth );
	/************************
	*	mesh rendering	*
	************************/
private:
	/// mesh shaders
	Shader* mesh_shader;
	Shader* static_mesh_shader;
private:
	void InitializeMeshRenderer();
	void RenderMeshes(float timeslice);
	void FreeMeshRenderer();
	/************************
	*	material system		*
	************************/
private:
	typedef std::map<MaterialType,Material*> MaterialMapType;
private:
	MaterialMapType material_map;
	/// material shaders
	Shader* grass_shader;
	Shader* spikes_shader;
	Shader* skyline_shader;
	Shader* textured_box_shader;
	Shader* water_shader;
public:
	Material* GetMaterial( MaterialType id ) { if ( id == MAT_NONE ) return NULL; return material_map[id]; }
private:
	void InitializeMaterials();
	void RenderMaterials( float timeslice );
	void FreeMaterials();
	/************************
	*	particle engine		*
	************************/
private:
	typedef std::map<std::string,ParticleSystem*> ParticleSystemMapType;
private:
	ParticleSystemMapType particle_system_map;
	ID3D10ShaderResourceView* random_texture;
public:
	ParticleSystem* GetParticleSystem( const char* id ) { return particle_system_map[id]; }
	ID3D10ShaderResourceView* GetRandomTexture() { return random_texture; }
	void RegisterLineList( LineListType* _line_list );
	void RegisterLineList2( LineListType* _line_list2 );
	void RegisterBeamColor( Vec4* c );
private:
	void InitializeParticleSystem();
	void BuildRandomTexture();
	void AdvanceParticles( float timeslice );
	void RenderParticles( Mat4& p_mat_view, Mat4& p_mat_proj );
	void FreeParticleSystem();
	/************************
	*	sprite rendering	*
	************************/
private:
	typedef ObjectLinkList<Sprite> SpriteListType;
private:
	/// sprite shader
	Shader* sprite_shader;
	ID3D10EffectShaderResourceVariable* sprite_texture_ptr;
	ID3D10EffectVectorVariable* sprite_position_ptr;
	ID3D10EffectVectorVariable* sprite_size_ptr;
	ID3D10EffectScalarVariable* sprite_rotation_ptr;
	ID3D10EffectScalarVariable* sprite_flag_ptr;
	SpriteListType sprites3d;
	SpriteListType spritesHUD;
public:
	void RegisterSprite3D( Sprite* sprite );
	void RegisterSpriteHUD( Sprite* sprite );
	void RemoveSprite3D( Sprite* sprite );
	void RemoveSpriteHUD( Sprite* sprite );
private:
	void InitializeSprite();
	void RenderSprites( SpriteListType& sprites );
	void FreeSprite();

	/************************
	*	postprocessing/aa	*
	************************/
private:
	Shader* post_processing_shader;
	ID3D10EffectShaderResourceVariable* p_color_ptr, *p_normal_flag_ptr, *p_view_position_ptr;
	ID3D10EffectShaderResourceVariable* bloom4_texture_ptr;
	ID3D10EffectShaderResourceVariable* bloom8_texture_ptr;
	ID3D10EffectVectorVariable* post_processing_viewport_ptr;
	ID3D10EffectVectorVariable* camera_position_ptr;
	ID3D10EffectVectorVariable* mouse_position_ptr;
	ID3D10EffectScalarVariable* frametime_ptr;
	RenderTargetTexture* render_target_processed;
	/// fxaa shader
	Shader* fxaa_shader;
	ID3D10EffectVectorVariable* fxaa_viewport_ptr;
	ID3D10EffectShaderResourceVariable* processed_texture_ptr;
	// blur shader
	Shader* blur_shader;
	ID3D10EffectShaderResourceVariable* blur_source_ptr;
	ID3D10EffectScalarVariable* blur_range_ptr;
	ID3D10EffectScalarVariable* brightness_ptr;
	ID3D10EffectVectorVariable* blur_viewport_ptr;
private:
	void InitializePostprocess();
	void RenderPostprocess( float timeslice, ID3D10ShaderResourceView* add_texture0, ID3D10ShaderResourceView* add_texture1 );
	void RenderAA();
	void FreePostprocess();
	/************************
	*	bloom effect		*
	************************/
private:
	Shader* bloom_pick_shader;
	RenderTargetTexture* render_target_bloom_source;
	ID3D10EffectShaderResourceVariable* b_diffuse_ptr;
	ID3D10EffectShaderResourceVariable* b_view_position_ptr;
	Shader* bloom_shader;
	RenderTargetTexture* render_target_bloomed4;
	RenderTargetTexture* render_target_bloomed8;
	ID3D10EffectShaderResourceVariable* bloom_source_ptr;
	ID3D10EffectScalarVariable* sample_size_ptr;
private:
	void InitializeBloom();
	void RenderBloom( float );
	void FreeBloom();
	/************************
	*	cursor rendering	*
	************************/
private:
	struct CursorVertex
	{
		Vec3 position0;
		Vec3 position1;
		unsigned int type;
	};
private:
	ID3D10Buffer* vb_cursor[2];
	Shader* update_cursor;
	Shader* render_cursor;
	Vec2 cursor_position;
	ID3D10EffectScalarVariable* cursor_timeslice_ptr;
	ID3D10EffectScalarVariable* cursor_global_time_ptr;
	ID3D10EffectShaderResourceVariable* cursor_random_texture_ptr;
	ID3D10EffectVectorVariable* cursor_position_ptr;
	ID3D10EffectVectorVariable* cursor_color_ptr;
	int cur_cursor_vb_target;
public:
	const Vec2* GetCursorPos() { return &cursor_position; }
private:
	void InitializeCursor();
	void RenderCursor( float );
	void FreeCursor();
	/************************
	*	Menu rendering		*
	************************/
private:
	struct MenuVertex
	{
		Vec3 position;
		Vec3 extra;
		Vec2 tex;
		unsigned int type;
	};
private:
	Shader* advance_menu;
	Shader* render_menu;
	ID3D10Buffer* vb_menu_initial;
	ID3D10Buffer* vb_menu[2];
	ID3D10EffectVectorVariable* menu_cursor_position_ptr;
	ID3D10EffectScalarVariable* menu_timeslice_ptr;
	ID3D10EffectScalarVariable* menu_global_time_ptr;
	ID3D10EffectShaderResourceVariable* menu_random_texture_ptr;
	ID3D10EffectShaderResourceVariable* menu_button_texture_ptr;
	RenderTargetTexture* paused_scene;
	RenderTargetTexture* blured_scene;
	int cur_menu_buffer;

	float title_y_pos;
private:
	void InitializeMenu();
	void PrepareMenu();
	void FreeMenu();
	/************************
	*		Font Engine		*
	************************/
private:
	typedef std::map<std::string,Font*> FontMapType;
	typedef ObjectLinkList<Text> TextListType;
private:
	Shader* font_shader;
	FontMapType font_map;
	TextListType text_list;
	Text* test_text;
	ID3D10EffectVectorVariable* font_position_ptr;
	ID3D10EffectShaderResourceVariable* font_img_ptr;
private:
	void RenderFont();
	void FreeFont();
public:
	void InitializeFont();
	void RegisterFont( const char* font_name );
	void RegisterText( Text* text );
	void RemoveText( Text* text );
	/************************
	*	Debug rendering		*
	************************/
private:
	struct DebugPrimitive
	{
		Vec3 v0;
		Vec3 v1;
		Vec4 color;
		int type;
	};
	typedef ObjectLinkList<DebugGeometry> DebugGeometryListType;
private:
	Shader* debug_shader;
	ID3D10InputLayout* debug_input_layout;
	DebugGeometryListType debug_geometry_list; 
	ID3D10Buffer* debug_vertex_buffer;
	bool render_debug_information;
public:
	void RegisterDebugGeometry( DebugGeometry* geometry );
	void RemoveDebugGeometry( DebugGeometry* geometry );
private:
	void RenderDebug();
	void InitializeDebugDrawing();
	void FreeDebugDrawing();
};

//A global pointer to the Graphics system, used to access it anywhere.
extern Graphics* GRAPHICS;
}
