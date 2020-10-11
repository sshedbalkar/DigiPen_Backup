#pragma once

namespace Framework
{
class ScriptEngine;

class BasicGlues
{
	static ScriptEngine* scriptengine;
public:
	static int Open( lua_State* L, void* _scriptengine );
	// pointer
	static void OpenPointerLibrary( lua_State* L );
	static int PointerNewChunk( lua_State* L );
	static int PointerNewGOC( lua_State* L );
	static int PointerNewTransform( lua_State* L );
	static int PointerNewText( lua_State* L );
	static int PointerNewAnimCtrl( lua_State* L );
	static int PointerNewGfxObj( lua_State* L );
	static int PointerNewCamera( lua_State* L );
	//engine
	static void OpenEngineLibrary( lua_State* L );
	static int EngineShutdown( lua_State* L );
	static int ChangeResolution( lua_State* L );
	static int EngineSystemCall( lua_State* L );
	static int EngineRegisterScript( lua_State* L );
	static int EngineLaunchLevel( lua_State* L );
	static int DebugText( lua_State* L );
	//goc 
	static void OpenGOCLibrary( lua_State* L );
	static int GOCGetTransform( lua_State* L );
	static int GOCGetGfxObj( lua_State* L );
	static int GOCGetText( lua_State* L );
	static int GOCGetCamera( lua_State* L );
	//transform
	static void OpenTransformLibrary( lua_State* L );
	static int TransformGetPosition( lua_State* L );
	static int TransformBuildMatrix( lua_State* L );
	//text
	static void OpenTextLibrary( lua_State* L );
	static int TextGetContent( lua_State* L );
	static int TextGetMaxLength( lua_State* L );
	//camera
	static void OpenCameraLibrary( lua_State* L );
	static int CameraProjectionFrustum( lua_State* L );
	static int CameraProjectionOrtho( lua_State* L );
	static int CameraRotateCameraSpace( lua_State* L );
	static int CameraTranslateCameraSpace( lua_State* L );
	static int CameraBuildMatrices( lua_State* L );
	static int CameraGetEye( lua_State* L );
	// group selection
	static void OpenGroupLibrary( lua_State* L );
	static int GroupNewGroup( lua_State* L );
	static int GroupSelect( lua_State* L );
	static int GroupClear( lua_State* L );
	static int GroupForEach( lua_State* L );
	static int GroupGetFirst( lua_State* L );
	static int GroupMove( lua_State* L );

};

}
