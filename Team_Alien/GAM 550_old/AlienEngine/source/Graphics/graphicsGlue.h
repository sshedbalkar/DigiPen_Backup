#pragma once

struct lua_State;
namespace Framework
{
#define MAX_DXOBJECT		8
class Graphics;
class DXObject;

class GraphicsGlue
{
private:
	static DXObject* object_array[MAX_DXOBJECT];
private:
	static DXObject* CheckDXObject( lua_State* L, int idx );
	static DXObject* CheckRendererObject( lua_State* L, int idx );
/* graphics library */
public:
	static int OpenLibrary( lua_State* L, void* graphics );

	static int AddHLSLMacro( lua_State* L );
	static int ApplyHLSLMacros( lua_State* L );
	static int Present( lua_State* L );
	// profiler
	static int ProfileStart( lua_State* L );
	static int ProfileEnd( lua_State* L );
	// renderer
	static int NewRenderer( lua_State* L );
	// view related
	static int NewRT( lua_State* L );
	static int NewRTRelative( lua_State* L );
	static int NewDS( lua_State* L );
	static int NewDSRelative( lua_State* L );
	static int NewUA( lua_State* L );
	static int ClearDSV( lua_State* L );
	static int SetDSV( lua_State* L );
	static int SetRTV( lua_State* L );
	static int SetRTVScreen( lua_State* L );
	static int SetRTVNull( lua_State* L );
	static int SetViewportDepth( lua_State* L );
	static int ClearRTV( lua_State* L );
	static int CSSetSRV( lua_State* L );
	static int CreateSRVFromFile( lua_State* L );
	static int GenerateMips( lua_State* L );
	// states
	static int GetSamplers( lua_State* L );
	static int GetBlendStates( lua_State* L );
	static int SetBlendState( lua_State* L );
	static int GetRasterStates( lua_State* L );
	static int SetRasterState( lua_State* L );
	static int GetDepthStates( lua_State* L );
	static int SetDepthState( lua_State* L );
	// buffer related
	static int MapBuffer( lua_State* L );
	static int UnmapBuffer( lua_State* L );
	static int ReadBufferStart( lua_State* L );
	static int ReadBufferEnd( lua_State* L );
	static int InitData( lua_State* L );
		//vbuffer
	static int NewVB( lua_State* L );
		// ibuffer
	static int NewIB( lua_State* L );
		//cbuffer
	static int NewCB( lua_State* L );
	static int AnimationCB( lua_State* L );
	static int CSSetCB( lua_State* L );
		//so
	static int SOSetTarget( lua_State* L );
	// effect related
	static int LoadEffect( lua_State* L );
	static int LoadEffectSO( lua_State* L );
	// compute shader
	static int NewCShader( lua_State* L );
	static int DirectCompute( lua_State* L );
	// model
	static int LoadMesh( lua_State* L );
	static int LoadAnim( lua_State* L );
	static int TestAnimCtrl( lua_State* L );
	// fonts
	static int NewFont( lua_State* L );
	static int FontBuildVB( lua_State* L );
	static int FontBuildVBFromMemory( lua_State* L );
	
	//////////////
	// renderer //
	//////////////
	static void OpenRendererLibrary( lua_State* L );
	static int RendererSetEffect( lua_State* L );
	static int RendererSetVertexBuffer( lua_State* L );
	static int RendererSetIndexBuffer( lua_State* L );
	static int RendererSetConstantBuffer( lua_State* L );
	static int RendererSetShaderResource( lua_State* L );
	static int RendererSetSampler( lua_State* L );
	static int RendererSetRasterState( lua_State* L );
	static int RendererSetBlendState( lua_State* L );
	static int RendererSetDepthStencilState( lua_State* L );
	static int RendererDraw( lua_State* L );
	static int RendererDrawIndexed( lua_State* L );
	static int RendererDrawInstanced( lua_State* L );
	static int RendererDrawIndexedInstanced( lua_State* L );
	static int RendererDrawAuto( lua_State* L );
	static int RendererClearVertexBuffer( lua_State* L );
	static int RendererClearConstantBuffer( lua_State* L );
	static int RendererClearShaderResource( lua_State* L );
	static int RendererClearSampler( lua_State* L );
	//////////////
	// material //
	//////////////
	static void OpenMaterialLibrary( lua_State* L );
	static int NewMaterial( lua_State* L );
	// material library
	static int SetSerializer( lua_State* L );
	static int ForEachObject( lua_State* L );
	static int MatLocateData( lua_State* L );
	/////////////////////
	// graphics object //
	/////////////////////
	static void OpenGfxObjLibrary( lua_State* L );
	static int GfxObjGetFlag( lua_State* L );
	static int GfxObjGetTransform( lua_State* L );
	static int GfxObjGetText( lua_State* L );
	static int GfxObjGetAnimCtrl( lua_State* L );
	static int GfxObjGetCamera( lua_State* L );
	static int GfxObjGetData( lua_State* L );
	//////////////////////////
	// animation controller //
	//////////////////////////
	static void OpenAnimationControllerLibrary( lua_State* L );
	static int AnimCtrlSwitch( lua_State* L );
	static int AnimCtrlApply( lua_State* L );
};

}