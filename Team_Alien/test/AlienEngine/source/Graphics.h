///////////////////////////////////////////////////////////////////////////////
///
///	\file Graphics.h
/// Defines grahics system.
///	
///	Authors: Benjamin Ellinger, Chris Peters
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

#include "VMath.h"
#include "Engine.h"
#include "Sprite.h"
#include <deque>
#include <list>
#include <stack>
#include <map>

namespace Framework
{	
//Forward Declaration of Graphics Objects
class Camera;
class Shader;
class D3DManager;
class Mesh;
class Transform;
class Font;
class Text;

class DebugGeometry;/// for debugging


class Graphics : public ISystem
{
private:
	/// for debug drawing
	struct DebugPrimitive
	{
		Vec3 v0;
		Vec3 v1;
		Vec4 color;
		int type;
	};
	typedef std::list<DebugGeometry*> DebugGeometryListType;
	typedef std::map<std::string,Font*> FontMapType;
	typedef std::list<Text*> TextListType;

private:
	D3DManager* d3dmanager;
	ID3D10Device* device;
	ID3D10RenderTargetView* render_target_screen;
	ID3D10DepthStencilView* depth_stencil_view;
	IDXGISwapChain* swap_chain;
	
	int screen_width;
	int screen_height;
	HWND hwnd;
	bool full_screen;

	///shaders
	Shader* static_mesh_shader;

	Mesh* test_mesh;/////////

	///transformations
	Mat4 mat_view;
	Mat4 mat_proj;
	Mat4 mat_ortho_view;
	Mat4 mat_ortho_proj;
	///camera control
	Vec3 current_camera_position;
	std::stack<Camera*> camera_stack;

	/// font engine elements
	Shader* font_shader;
	FontMapType font_map;
	TextListType text_list;
	Text* test_text;

	/// for debugging
	Shader* debug_shader; /// debug shader
	ID3D10InputLayout* debug_input_layout;
	DebugGeometryListType debug_geometry_list; 
	ID3D10Buffer* debug_vertex_buffer;
	bool render_debug_information;

	DebugGeometry* test;

	Transform* transform;
public:
	Graphics();
	virtual ~Graphics(){};
	virtual std::string GetName() { return "Graphics";};	

	void SetWindowProperties( HWND _hwnd, int w, int h, bool _full_screen) { hwnd = _hwnd; screen_width = w; screen_height = h; full_screen = _full_screen; }

	//// basic game functions
	virtual void Initialize();
	virtual void Update( float timeslice );
	virtual void Free();
	virtual void Load();
	virtual void Unload();

	//// transform
	Vec2 ScreenToWorldSpace(Vec2 ){ return Vec2( 0, 0 ); };//empty function.
	void RegisterTriangle( Transform* transform ); ///for testing

	//// camera
	void PushCamera(Camera*);
	void PopCamera();

	//// debug geometry adding/deleting functions
	void RegisterDebugGeometry( DebugGeometry* geometry );
	void RemoveDebugGeometry( DebugGeometry* geometry );

	//// font engine
	void RegisterFont( const char* font_name );
	void RegisterText( Text* text );

private:
	void UpdateProjectionAndOrthoMatrices();
	void UpdateViewingMatrix();

	////font engine
	void RenderText();
	//// debug drawing function
	void RenderDebug();
	void InitializeDebugDrawing();

};

//A global pointer to the Graphics system, used to access it anywhere.
extern Graphics* GRAPHICS;
}
