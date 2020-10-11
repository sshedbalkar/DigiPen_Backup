#include "Precompiled.h"
#include "lua.h"
#include "BasicGlues.h"
#include "scriptEngine.h"
#include "Factory.h"
#include "Transform.h"
#include "text.h"
#include "camera.h"
#include "groupSelection.h"
#include "Core.h"
#include "GraphicsObject.h"

namespace Framework
{

ScriptEngine* BasicGlues::scriptengine;
//========================================
int BasicGlues::Open( lua_State* L, void* _scriptengine )
{
	scriptengine = static_cast<ScriptEngine*>( _scriptengine );

	OpenPointerLibrary( L );
	OpenEngineLibrary( L );
	OpenGOCLibrary( L );
	OpenTransformLibrary( L );
	OpenTextLibrary( L );
	OpenCameraLibrary( L );
	OpenGroupLibrary( L );
	return 0;
}
//***** pointers *****//
//========================================
void BasicGlues::OpenPointerLibrary( lua_State* L )
{
	luaL_reg pointer_lib[] =
	{
		{ "Chunk", PointerNewChunk },
		{ "GOC", PointerNewGOC },
		{ "Transform", PointerNewTransform },
		{ "Text", PointerNewText },
		{ "GfxObj", PointerNewGfxObj },
		{ "AnimCtrl", PointerNewAnimCtrl },
		{ "Camera", PointerNewCamera },
		{ NULL, NULL }
	};
	
	luaL_openlib( L, "pointer", pointer_lib, 0 );

}
//========================================
int BasicGlues::PointerNewChunk( lua_State* L )
{
	void** ptr = (void**)lua_newuserdata( L, sizeof(void*) );
	*ptr = 0;
	luaL_getmetatable( L, "LuaBook.chunk" );
	lua_setmetatable( L, -2 );

	return 1;
}
//========================================
int BasicGlues::PointerNewGOC( lua_State* L )
{
	void** ptr = (void**)lua_newuserdata( L, sizeof(void*) );
	*ptr = 0;
	luaL_getmetatable( L, "LuaBook.goc" );
	lua_setmetatable( L, -2 );

	return 1;
}
//========================================
int BasicGlues::PointerNewTransform( lua_State* L )
{
	void** ptr = (void**)lua_newuserdata( L, sizeof(void*) );
	*ptr = 0;
	luaL_getmetatable( L, "LuaBook.transform" );
	lua_setmetatable( L, -2 );

	return 1;
}
//========================================
int BasicGlues::PointerNewText( lua_State* L )
{
	void** ptr = (void**)lua_newuserdata( L, sizeof(void*) );
	*ptr = 0;
	luaL_getmetatable( L, "LuaBook.text" );
	lua_setmetatable( L, -2 );

	return 1;
}
//========================================
int BasicGlues::PointerNewGfxObj( lua_State* L )
{
	void** ptr = (void**)lua_newuserdata( L, sizeof(void*) );
	*ptr = 0;
	luaL_getmetatable( L, "LuaBook.gfxobj" );
	lua_setmetatable( L, -2 );

	return 1;
}
//========================================
int BasicGlues::PointerNewAnimCtrl( lua_State* L )
{
	void** ptr = (void**)lua_newuserdata( L, sizeof(void*) );
	*ptr = 0;
	luaL_getmetatable( L, "LuaBook.animctrl" );
	lua_setmetatable( L, -2 );

	return 1;
}
//========================================
int BasicGlues::PointerNewCamera( lua_State* L )
{
	void** ptr = (void**)lua_newuserdata( L, sizeof(void*) );
	*ptr = 0;
	luaL_getmetatable( L, "LuaBook.camera" );
	lua_setmetatable( L, -2 );

	return 1;
}
//***** engine *****//
//========================================
void BasicGlues::OpenEngineLibrary( lua_State* L )
{
	luaL_reg engine_lib[] =
	{
		{ "Resolution", ChangeResolution },
		{ "Shutdown", EngineShutdown },
		{ "SystemCall", EngineSystemCall },
		{ "RegisterScript", EngineRegisterScript },
		{ "DebugText", DebugText },
		{ "LaunchLevel", EngineLaunchLevel },
		{ NULL, NULL }
	};
	
	luaL_openlib( L, "engine", engine_lib, 0 );
}
//========================================
int BasicGlues::EngineShutdown( lua_State* L )
{
	MessageQuit m;
	CORE->BroadcastMessage( &m );
	return 0;
}
//========================================
int BasicGlues::ChangeResolution( lua_State* L )
{
	Resolution m;
	m.w = luaL_checkint( L, 1 );
	m.h = luaL_checkint( L, 2 );
	CORE->BroadcastMessage(&m);
	return 0;
}
//========================================
int BasicGlues::EngineSystemCall( lua_State* L )
{
	const char* cmd = luaL_checkstring( L, 1 );
	system( cmd );
	return 0;
}
//========================================
int BasicGlues::EngineRegisterScript( lua_State* L )
{
	const char* path = luaL_checkstring( L, 1 );
	scriptengine->RegisterScript( path );
	return 0;
}
//========================================
int BasicGlues::EngineLaunchLevel( lua_State* L )
{
	const char* filepath = luaL_checkstring( L, 1 );
	CORE->LaunchLevel( filepath );
	return 0;
}
//========================================
int BasicGlues::DebugText( lua_State* L )
{
	float x = static_cast<float>( luaL_checknumber( L, 1 ) );
	float y = static_cast<float>( luaL_checknumber( L, 2 ) );
	int maxlength = lua_gettop(L) > 2 ? luaL_checkint( L, 3 ) : 20;
	
	GOC* text = FACTORY->BuildFromArchetype( "text" );
	Text* te = text->has(Text);
	te->maxlength = maxlength;
	
	text->Initialize();

	Transform* tr = text->has(Transform);
	tr->Position = Vec3( x, y, 0.0f );

	char** ptr = (char**)lua_newuserdata( L, sizeof(void*) );
	*ptr = te->content;
	luaL_getmetatable( L, "LuaBook.chunk" );
	lua_setmetatable( L, -2 );

	GOC** goc = static_cast<GOC**>( lua_newuserdata( L, sizeof(void*) ) );
	*goc = text;
	luaL_getmetatable( L, "LuaBook.goc" );
	lua_setmetatable( L, -2 );
	
	return 2;
}
//****** goc *******//
//========================================
void BasicGlues::OpenGOCLibrary( lua_State* L )
{
	luaL_reg goc_lib[] =
	{
		{ "Transform", GOCGetTransform },
		{ "GfxObj", GOCGetGfxObj },
		{ "Text", GOCGetText },
		{ "Camera", GOCGetCamera },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.goc" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	
	luaL_openlib( L, NULL, goc_lib, 0 );
}
//========================================
int BasicGlues::GOCGetTransform( lua_State* L )
{
	GOC** goc = (GOC**)luaL_checkudata( L, 1, "LuaBook.goc" );
	Transform** ptrans = (Transform**)luaL_checkudata( L, 2, "LuaBook.transform" );
	
	Transform* transform = (*goc)->has(Transform);
	*ptrans = transform;
	return 0;
}
//========================================
int BasicGlues::GOCGetText( lua_State* L )
{
	GOC** goc = (GOC**)luaL_checkudata( L, 1, "LuaBook.goc" );
	Text** ptext = (Text**)luaL_checkudata( L, 2, "LuaBook.text" );
	Text* text = (*goc)->has(Text);
	*ptext = text;
	return 0;
}
//========================================
int BasicGlues::GOCGetCamera( lua_State* L )
{
	GOC** goc = (GOC**)luaL_checkudata( L, 1, "LuaBook.goc" );
	Camera** pcamera = (Camera**)luaL_checkudata( L, 2, "LuaBook.camera" );
	Camera* camera = (*goc)->has(Camera);
	*pcamera = camera;
	return 0;
}
//========================================
int BasicGlues::GOCGetGfxObj( lua_State* L )
{
	GOC** goc = (GOC**)luaL_checkudata( L, 1, "LuaBook.goc" );
	GraphicsComposition** pgo = (GraphicsComposition**)luaL_checkudata( L, 2, "LuaBook.gfxobj" );
	GraphicsComposition* gfxobj = (*goc)->has(GraphicsComposition);
	*pgo = gfxobj;
	return 0;
}
//***** Transform *****//
//========================================
void BasicGlues::OpenTransformLibrary( lua_State* L )
{
	luaL_reg transform_lib[] =
	{
		{ "Position", TransformGetPosition },
		{ "BuildMatrix", TransformBuildMatrix },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.transform" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	
	luaL_openlib( L, NULL, transform_lib, 0 );
}
//========================================
int BasicGlues::TransformBuildMatrix( lua_State* L )
{
	Transform* transform = *static_cast<Transform**>( luaL_checkudata( L, 1, "LuaBook.transform" ) );
	D3DXMATRIX* mat = static_cast<D3DXMATRIX*>( luaL_checkudata( L, 2, "LuaBook.array" ) );

	if ( !transform || !mat ) { printf( "TransformBuildMatrix: manipulating a NULL pointer\n" ); return 0; }

	transform->BuildMatrix( *mat );
	return 0;
}
//========================================
int BasicGlues::TransformGetPosition( lua_State* L )
{
	Transform** transform = (Transform**)luaL_checkudata( L, 1, "LuaBook.transform" );
	void** ppos = (void**)luaL_checkudata( L, 2, "LuaBook.chunk" );

	ErrorIf( !*transform, "manipulating a NULL pointer" );

	*ppos = (void*)&(*transform)->Position;
	return 0;
}
//***** Text *****//
//========================================
void BasicGlues::OpenTextLibrary( lua_State* L )
{
	luaL_reg text_lib[] =
	{
		{ "Content", TextGetContent },
		{ "MaxLength", TextGetMaxLength },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.text" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	
	luaL_openlib( L, NULL, text_lib, 0 );

}
//========================================
int BasicGlues::TextGetContent( lua_State* L )
{
	Text** text = (Text**)luaL_checkudata( L, 1, "LuaBook.text" );
	char** pcontent = (char**)luaL_checkudata( L, 2, "LuaBook.chunk" );

	ErrorIf( !*text, "manipulating a NULL pointer" );

	*pcontent = (*text)->content;
	return 0;
}
//========================================
int BasicGlues::TextGetMaxLength( lua_State* L )
{
	Text** text = (Text**)luaL_checkudata( L, 1, "LuaBook.text" );
	ErrorIf( !*text, "manipulating a NULL pointer" );

	lua_pushnumber( L, (*text)->maxlength );
	return 1;
}
//***** Camera *****//
//========================================
void BasicGlues::OpenCameraLibrary( lua_State* L )
{
	luaL_reg camera_lib[] =
	{
		{ "ProjectionFrustum", CameraProjectionFrustum },
		{ "ProjectionOrtho", CameraProjectionOrtho },
		{ "Rotate", CameraRotateCameraSpace },
		{ "Translate", CameraTranslateCameraSpace },
		{ "Matrices", CameraBuildMatrices },
		{ "Eye", CameraGetEye },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.camera" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	
	luaL_openlib( L, NULL, camera_lib, 0 );
}
//========================================
int BasicGlues::CameraProjectionFrustum( lua_State* L )
{
	Camera** camera = static_cast<Camera**>( luaL_checkudata( L, 1, "LuaBook.camera" ) );
	ErrorIf( !*camera, "Camera: manipulating a NULL pointer" );

	float fov = static_cast<float>( luaL_checknumber( L, 2 ) );
	float ratio = static_cast<float>( luaL_checknumber( L, 3 ) );
	float znear = static_cast<float>( luaL_checknumber( L, 4 ) );
	float zfar = static_cast<float>( luaL_checknumber( L, 5 ) );

	(*camera)->UpdateProjectionFrustum( fov, ratio, znear, zfar );
	return 0;
}
//========================================
int BasicGlues::CameraProjectionOrtho( lua_State* L )
{
	Camera** camera = static_cast<Camera**>( luaL_checkudata( L, 1, "LuaBook.camera" ) );
	ErrorIf( !*camera, "Camera: manipulating a NULL pointer" );

	float w = static_cast<float>( luaL_checknumber( L, 2 ) );
	float h = static_cast<float>( luaL_checknumber( L, 3 ) );
	float znear = static_cast<float>( luaL_checknumber( L, 4 ) );
	float zfar = static_cast<float>( luaL_checknumber( L, 5 ) );

	(*camera)->UpdateProjectionOrtho( w, h, znear, zfar );
	return 0;
}
//========================================
int BasicGlues::CameraRotateCameraSpace( lua_State* L )
{
	Camera** camera = static_cast<Camera**>( luaL_checkudata( L, 1, "LuaBook.camera" ) );
	ErrorIf( !*camera, "Camera: manipulating a NULL pointer" );

	float x = static_cast<float>( luaL_checknumber( L, 2 ) );
	float y = static_cast<float>( luaL_checknumber( L, 3 ) );
	float z = static_cast<float>( luaL_checknumber( L, 4 ) );

	(*camera)->RotateCameraSpace( x, y, z );
	return 0;
}
//========================================
int BasicGlues::CameraTranslateCameraSpace( lua_State* L )
{
	Camera** camera = static_cast<Camera**>( luaL_checkudata( L, 1, "LuaBook.camera" ) );
	ErrorIf( !*camera, "Camera: manipulating a NULL pointer" );

	float x = static_cast<float>( luaL_checknumber( L, 2 ) );
	float y = static_cast<float>( luaL_checknumber( L, 3 ) );
	float z = static_cast<float>( luaL_checknumber( L, 4 ) );

	(*camera)->TranslateCameraSpace( x, y, z );
	return 0;
}
//========================================
int BasicGlues::CameraGetEye( lua_State* L )
{
	Camera** camera = static_cast<Camera**>( luaL_checkudata( L, 1, "LuaBook.camera" ) );
	ErrorIf( !*camera, "Camera: manipulating a NULL pointer" );
	
	float x, y, z;
	(*camera)->GetEyePosition( x, y, z );
	lua_pushnumber( L, x );
	lua_pushnumber( L, y );
	lua_pushnumber( L, z );
	return 3;
}
//========================================
int BasicGlues::CameraBuildMatrices( lua_State* L )
{
	int nargs = lua_gettop( L );
	Camera** camera = static_cast<Camera**>( luaL_checkudata( L, 1, "LuaBook.camera" ) );
	ErrorIf( !*camera, "Camera: manipulating a NULL pointer" );
	if ( nargs == 4 )
	{
		void* matrix_view = luaL_checkudata( L, 2, "LuaBook.array" );
		void* matrix_proj = luaL_checkudata( L, 3, "LuaBook.array" );
		void* matrix_viewproj = luaL_checkudata( L, 4, "LuaBook.array" );
		(*camera)->GetViewMatrix( matrix_view );
		(*camera)->GetProjMatrix( matrix_proj );
		(*camera)->GetViewProjMatrix( matrix_viewproj );
	}
	else
	{
		void* matrix = luaL_checkudata( L, 2, "LuaBook.array" );
		(*camera)->GetViewProjMatrix( matrix );
	}
	return 0;
}
//***** Group *****//
//========================================
void BasicGlues::OpenGroupLibrary( lua_State* L )
{
	luaL_reg group_lib_e[] =
	{
		{ "New", GroupNewGroup },
		{ NULL, NULL }
	};
	luaL_openlib( L, "group", group_lib_e, 0 );

	luaL_reg group_lib_i[] = 
	{
		{ "Select", GroupSelect },
		{ "Clear", GroupClear },
		{ "ForEach", GroupForEach },
		{ "GetFirst", GroupGetFirst },
		{ "MoveAll", GroupMove },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.group" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	
	luaL_openlib( L, NULL, group_lib_i, 0 );
}
//========================================
int BasicGlues::GroupNewGroup( lua_State* L )
{
	GroupSelection* group = new GroupSelection;
	GroupSelection** pgroup = static_cast<GroupSelection**>( lua_newuserdata( L, sizeof(GroupSelection) ) );
	//memcpy( pgroup, &group, sizeof( Group ) );
	*pgroup = group;
	luaL_getmetatable( L, "LuaBook.group" );
	lua_setmetatable( L, -2 );

	return 1;
}
//========================================
int BasicGlues::GroupSelect( lua_State* L )
{
	GroupSelection* group = *static_cast<GroupSelection**>( luaL_checkudata( L, 1, "LuaBook.group" ));
	float x0 = static_cast<float>( luaL_checknumber( L, 2 ) );
	float y0 = static_cast<float>( luaL_checknumber( L, 3 ) );
	float x1 = static_cast<float>( luaL_checknumber( L, 4 ) );
	float y1 = static_cast<float>( luaL_checknumber( L, 5 ) );
	Camera** camera = static_cast<Camera**>( luaL_checkudata( L, 6, "LuaBook.camera" ) );

	group->Select( x0, y0, x1, y1, *camera );
	return 0;
}
//========================================
int BasicGlues::GroupClear( lua_State* L )
{
	GroupSelection* group = *static_cast<GroupSelection**>( luaL_checkudata( L, 1, "LuaBook.group" ));

	group->Clear();
	return 0;
}
//========================================
int BasicGlues::GroupForEach( lua_State* L )
{
	GroupSelection* group = *static_cast<GroupSelection**>( luaL_checkudata( L, 1, "LuaBook.group" ));
	GOC** goc = static_cast<GOC**>( luaL_checkudata( L, 2, "LuaBook.goc" ) );

	if ( !lua_isfunction( L, 3 ) ) { printf("GroupForEach: parameter#3 needs to be a function.\n"); return 0; };

	GroupSelection::GOCListType::iterator gocit = group->Begin();
	while ( gocit != group->End() )
	{
		lua_pushvalue( L, 3 );	// push the function
		*goc = *gocit;
		int error = lua_pcall( L, 0, 0, 0 );
		if (error)
		{
			char err_msg[256];
			sprintf_s(err_msg, "%s", lua_tostring( L, -1));
			lua_pop(L, 1);
			printf( "GroupForEach:%s\n", err_msg );
		}
		++gocit;
	}
	

}

int BasicGlues::GroupGetFirst( lua_State* L )
{
	GroupSelection* group = *static_cast<GroupSelection**>( luaL_checkudata( L, 1, "LuaBook.group" ));
	GOC** goc = static_cast<GOC**>( luaL_checkudata( L, 2, "LuaBook.goc" ) );
	int count=0;
	
	GroupSelection::GOCListType::iterator gocit = group->Begin();
	while ( gocit != group->End() )
	{
		++count;
		++gocit;
	}
	
	if (group->Begin()==group->End() || count>1)//dont show if only 1  object selected
	{
		*goc=NULL;
	}
	else{
		*goc=*group->Begin();
	}
	return 0;
}

int BasicGlues::GroupMove( lua_State* L )
{
	GroupSelection* group = *static_cast<GroupSelection**>( luaL_checkudata( L, 1, "LuaBook.group" ));
	float dx = static_cast<float>( luaL_checknumber( L, 2 ) );
	float dy = static_cast<float>( luaL_checknumber( L, 3 ) );
	float dz = static_cast<float>( luaL_checknumber( L, 4 ) );
	GOC* goc;
	int count=0;
	
	GroupSelection::GOCListType::iterator gocit = group->Begin();
	while ( gocit != group->End() )
	{
		goc = *gocit;
		goc->has(Transform)->Position.x+=dx;
		goc->has(Transform)->Position.y+=dy;
		goc->has(Transform)->Position.z+=dz;
		++count;
		++gocit;
	}
	
	
	return 0;
}

}