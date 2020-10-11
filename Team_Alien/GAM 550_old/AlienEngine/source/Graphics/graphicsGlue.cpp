#include "Precompiled.h"
#include <fstream>
#include "lua.h"
#include "graphicsGlue.h"
#include "graphics.h"
#include "GraphicsObject.h"
#include "material.h"
#include "ModelStructures.h"
#include "PathHelper.h"
#include "deviceStates.h"
#include "effect.h"

namespace Framework
{

DXObject* GraphicsGlue::object_array[MAX_DXOBJECT] = {NULL};
//=====================================================
// check functions
//=====================================================
DXObject* GraphicsGlue::CheckDXObject( lua_State* L, int idx )
{
	return static_cast<DXObject*>( lua_touserdata( L, idx ) );
}
//=====================================================
DXObject* GraphicsGlue::CheckRendererObject( lua_State* L, int idx )
{
	return *static_cast<DXObject**>( luaL_checkudata( L, idx, "LuaBook.renderer" ) );
}


//=======================================
int GraphicsGlue::OpenLibrary( lua_State* L, void* graphics )
{
	luaL_reg graphics_lib[] =
	{
		{ "AddMacro", AddHLSLMacro },
		{ "ApplyMacros", ApplyHLSLMacros },
		{ "Present", Present },
		// profiler
		{ "ProfileStart", ProfileStart },
		{ "ProfileEnd", ProfileEnd },
		// renderer
		{ "NewRenderer", NewRenderer },
		// views
		{ "NewRT", NewRT },
		{ "NewRTRelative", NewRTRelative },
		{ "SetRTV", SetRTV },
		{ "SetRTVNULL", SetRTVNull },
		{ "SetRTVScreen", SetRTVScreen },
		{ "ClearRTV", ClearRTV },
		{ "SetViewportDepth", SetViewportDepth },
		{ "NewDS", NewDS },
		{ "NewDSRelative", NewDSRelative },
		{ "ClearDSV", ClearDSV },
		{ "SetDSV", SetDSV },
		{ "NewUA", NewUA },
		{ "CSSetSRV", CSSetSRV },
		{ "SRVFromFile", CreateSRVFromFile },
		{ "GenerateMips", GenerateMips },
		///////////////////////////////// states ////////////////////////////////
		{ "Samplers", GetSamplers },
		{ "BlendStates", GetBlendStates },
		{ "SetBlendState", SetBlendState },
		{ "RasterStates", GetRasterStates },
		{ "SetRasterState", SetRasterState },
		{ "DepthStates", GetDepthStates },
		{ "SetDepthState", SetDepthState },
		//buffer
		{ "MapBuffer", MapBuffer },
		{ "UnmapBuffer", UnmapBuffer },
		{ "ReadBufferStart", ReadBufferStart },
		{ "ReadBufferEnd", ReadBufferEnd },
		{ "InitData", InitData },
		//vbuffer
		{ "NewVB", NewVB },
		// index buffer
		{ "NewIB", NewIB },
		//constant buffers
		{ "NewCB", NewCB },
		{ "AnimationCB", AnimationCB },
		{ "CSSetCB", CSSetCB },
		// stream output
		{ "SOSetTarget", SOSetTarget },
		////////////////////////////////// effects /////////////////////////////////////
		{ "LoadEffect", LoadEffect },
		{ "LoadEffectSO", LoadEffectSO },
		////////////////////////////////// cshader /////////////////////////////////////
		{ "NewCS", NewCShader },
		{ "DirectCompute", DirectCompute },
		////////////////////////////////// materials ///////////////////////////////////
		{ "NewMaterial", NewMaterial },
		/////////////////////////////////// models /////////////////////////////////////
		{ "LoadMesh", LoadMesh },
		{ "LoadAnim", LoadAnim },
		{ "TestAnimCtrl", TestAnimCtrl },
		/////////////////////////////////// fonts //////////////////////////////////////
		{ "NewFont", NewFont },
		{ "FontBuildVB", FontBuildVB },
		{ "FontBuildVBFromMemory", FontBuildVBFromMemory },
		{ NULL, NULL }
	};

	luaL_openlib( L, "Graphics", graphics_lib, 0 );

	OpenRendererLibrary( L );
	OpenMaterialLibrary( L );
	OpenGfxObjLibrary( L );
	OpenAnimationControllerLibrary( L );

	return 0;
}
//=======================================
int GraphicsGlue::AddHLSLMacro( lua_State* L )
{
	const char* tag = luaL_checkstring( L, 1 );
	const char* value = luaL_checkstring( L, 2 );
	Effect::AddMacro( tag, value );
	return 0;
}
//=======================================
int GraphicsGlue::ApplyHLSLMacros( lua_State* L )
{
	Effect::ApplyMacros();
	return 0;
}
//=======================================
int GraphicsGlue::Present( lua_State* L )
{
	int num_vsyncs = luaL_checkint( L, 1 );
	GRAPHICS->Present( num_vsyncs );
	return 0;
}
/*******************************
*	profiler
*******************************/
//=======================================
int GraphicsGlue::ProfileStart( lua_State* L )
{
	const char* name = luaL_checkstring( L, 1 );
	GRAPHICS->StartProfile( name );
	return 0;
}
//=======================================
int GraphicsGlue::ProfileEnd( lua_State* L )
{
	const char* name = luaL_checkstring( L, 1 );
	char* str = *(char**)luaL_checkudata( L, 2, "LuaBook.chunk" );
	int maxlength = luaL_checkint( L, 3 );
	GRAPHICS->EndProfileAndConstructString( name, str, maxlength );
	return 0;
}
/*******************************
*	renderer
*******************************/
//=======================================
int GraphicsGlue::NewRenderer( lua_State* L )
{
	DXObject* rendererobject = GRAPHICS->RegisterRenderer();
	DXObject** luaobject = static_cast<DXObject**>( lua_newuserdata( L, sizeof( void* ) ) );
	*luaobject = rendererobject;
	luaL_getmetatable( L, "LuaBook.renderer" );
	lua_setmetatable( L, -2 );
	return 1;
}

/*******************************
*	views related
*******************************/
//=======================================
int GraphicsGlue::NewRT( lua_State* L )
{
	int num_args = lua_gettop( L );
	DXObject* rtvobject, *srvobject;
	
	int w = luaL_checkint( L, 1 );
	int h = luaL_checkint( L, 2 );
	const char* format = "f4";
	int generatemips = 0;
	int miplevels = 1;

	if ( num_args > 2 )
	{
		format = luaL_checkstring( L, 3 );
		if ( num_args > 3 )
		{
			generatemips = luaL_checkint( L, 4 );
			miplevels = luaL_checkint( L, 5 );
		}
	}
	GRAPHICS->RegisterRenderTarget( &rtvobject, &srvobject, w, h, format, generatemips, miplevels, false );
	lua_pushlightuserdata( L, rtvobject );
	lua_pushlightuserdata( L, srvobject );

	return 2;
}
//=======================================
int GraphicsGlue::NewRTRelative( lua_State* L )
{
	int num_args = lua_gettop( L );
	DXObject* rtvobject, *srvobject;
	float ratio = static_cast<float>( luaL_checknumber( L, 1 ) );
	const char* format = "f4";
	int generatemips = 0;
	int miplevels = 1;

	if ( num_args > 1 )
	{
		format = luaL_checkstring( L, 2 );
		if ( num_args > 2 )
		{
			generatemips = luaL_checkint( L, 3 );
			miplevels = luaL_checkint( L, 4 );
		}
	}
	GRAPHICS->RegisterRenderTarget( &rtvobject, &srvobject, ratio, format, generatemips, miplevels );
	
	lua_pushlightuserdata( L, rtvobject );
	lua_pushlightuserdata( L, srvobject );

	return 2;
}
//=======================================
int GraphicsGlue::SetRTVScreen( lua_State* L )
{
	GRAPHICS->SetRenderTargetScreen();
	return 0;
}
//=======================================
int GraphicsGlue::SetRTV( lua_State* L )
{
	int nargs = lua_gettop(L);
	for ( int i = 0; i < nargs; ++i )
	{
		object_array[i] = CheckDXObject( L, i + 1 );
	}
	GRAPHICS->SetRenderTargetViews( object_array, nargs );	

	return 0;
}
//=======================================
int GraphicsGlue::SetRTVNull( lua_State* L )
{
	float w = static_cast<float>( luaL_checknumber( L, 1 ) );
	float h = static_cast<float>( luaL_checknumber( L, 2 ) );
	GRAPHICS->SetNullRenderTargets( w, h );
	return 0;
}
//=======================================
int GraphicsGlue::ClearRTV( lua_State* L )
{
	DXObject* rtvobject = CheckDXObject( L, 1 );
	float* clear_data = (float*)luaL_checkudata( L, 2, "LuaBook.array" );

	GRAPHICS->ClearRenderTargetView( rtvobject, clear_data );
	return 0;
}
//=======================================
int GraphicsGlue::SetViewportDepth( lua_State* L )
{
	float mindepth = static_cast<float>( luaL_checknumber(L,1) );
	float maxdepth = static_cast<float>( luaL_checknumber(L,2) );
	GRAPHICS->SetViewportDepth( mindepth, maxdepth );
	return 0;
}
//=======================================
int GraphicsGlue::NewDS( lua_State* L )
{
	int num_args = lua_gettop( L );
	DXObject* dsvobject;
	DXObject* srvobject;
	if ( num_args == 1 )
	{
		float scale = static_cast<float>( luaL_checknumber( L, 1 ) );
		GRAPHICS->RegisterDepthStencil( &dsvobject, &srvobject, scale );
	}
	else if ( num_args == 2 )
	{
		int w = luaL_checkint( L, 1 );
		int h = luaL_checkint( L, 2 );
		GRAPHICS->RegisterDepthStencil( &dsvobject, &srvobject, w, h, false );
	}
	lua_pushlightuserdata( L, dsvobject );
	lua_pushlightuserdata( L, srvobject );
	return 2;
}
//=======================================
int GraphicsGlue::NewDSRelative( lua_State* L )
{
	return 0;
}
//=======================================
int GraphicsGlue::ClearDSV( lua_State* L )
{
	DXObject* dsvobject = CheckDXObject( L, 1 );
	float value = static_cast<float>( luaL_checknumber( L, 2 ) );
	GRAPHICS->ClearDepthStencilView( dsvobject, value );
	return 0;
}
//=======================================
int GraphicsGlue::SetDSV( lua_State* L )
{
	int nargs = lua_gettop( L );
	if ( nargs = 0 ) 
		GRAPHICS->SetDepthStencilView( NULL );
	else
	{
		DXObject* dsvobject = CheckDXObject( L, 1 );
		GRAPHICS->SetDepthStencilView( dsvobject );
	}
	return 0;
}
//=======================================
int GraphicsGlue::NewUA( lua_State* L )
{
	int nargs = lua_gettop( L );
	int structsize = luaL_checkint( L, 1 );
	int numstruct = luaL_checkint( L, 2 );
	void* initdata = NULL;
	if ( nargs == 3 )
		initdata = *static_cast<void**>( luaL_checkudata( L, 3, "LuaBook.chunk" ) );
	DXObject* uavobject;
	DXObject* srvobject;
	DXObject* sbobject;

	GRAPHICS->RegisterUnorderedAccess( &uavobject, &srvobject, &sbobject, structsize, numstruct, initdata );
	
	lua_pushlightuserdata( L, uavobject );
	lua_pushlightuserdata( L, srvobject );
	lua_pushlightuserdata( L, sbobject );
	return 3;
}
//=======================================
int GraphicsGlue::CSSetSRV( lua_State* L )
{
	int nargs = lua_gettop(L);
	for ( int i = 0; i < nargs; ++i ) object_array[i] = CheckDXObject( L, i + 1 );
	GRAPHICS->CSSetShaderResouces( object_array, nargs );
	return 0;
}
//=======================================
int GraphicsGlue::CreateSRVFromFile( lua_State* L )
{
	const char* filename = luaL_checkstring( L, 1 );
	DXObject* srvobject = GRAPHICS->CreateSRVFromFile( filename );
	lua_pushlightuserdata( L, srvobject );
	return 1;
}
//=======================================
int GraphicsGlue::GenerateMips( lua_State* L )
{
	DXObject* srvobject = CheckDXObject( L, 1 );
	GRAPHICS->GenerateMips( srvobject );
	return 0;
}
/*******************************
*	state related
*******************************/
//=======================================
int GraphicsGlue::GetSamplers( lua_State* L )
{
	SamplerStates* samplers = GRAPHICS->GetSamplers();
	SamplerStates::iterator it;
	lua_newtable( L );
	for ( it = samplers->begin(); it != samplers->end(); ++it )
	{
		lua_pushstring( L, it->first.c_str() );
		lua_pushlightuserdata( L, static_cast<void*>(it->second) );
		lua_settable( L, -3 );
	}
	return 1;
}
//=======================================
int GraphicsGlue::GetBlendStates( lua_State* L )
{
	BlendStates* blend_states = GRAPHICS->GetBlendStates();
	BlendStates::iterator it;
	lua_newtable( L );
	for ( it = blend_states->begin(); it != blend_states->end(); ++it )
	{
		lua_pushstring( L, it->first.c_str() );
		lua_pushlightuserdata( L, static_cast<void*>(it->second) );
		lua_settable( L, -3 );
	}
	return 1;
}
//=======================================
int GraphicsGlue::SetBlendState( lua_State* L )
{
	GRAPHICS->SetBlendState( static_cast<ID3D11BlendState*>( lua_touserdata( L, 1 ) ) );
	return 0;
}
//=======================================
int GraphicsGlue::GetRasterStates( lua_State* L )
{
	RasterStates* raster_states = GRAPHICS->GetRasterStates();
	RasterStates::iterator it;
	lua_newtable( L );
	for ( it = raster_states->begin(); it != raster_states->end(); ++it )
	{
		lua_pushstring( L, it->first.c_str() );
		lua_pushlightuserdata( L, static_cast<void*>(it->second) );
		lua_settable( L, -3 );
	}
	return 1;
}
//=======================================
int GraphicsGlue::SetRasterState( lua_State* L )
{
	GRAPHICS->SetRasterState( static_cast<ID3D11RasterizerState*>( lua_touserdata( L, 1 ) ) );
	return 0;
}
//=======================================
int GraphicsGlue::GetDepthStates( lua_State* L )
{
	DepthStencilStates* depth_states = GRAPHICS->GetDepthStencilStates();
	DepthStencilStates::iterator it;
	lua_newtable( L );
	for ( it = depth_states->begin(); it != depth_states->end(); ++it )
	{
		lua_pushstring( L, it->first.c_str() );
		lua_pushlightuserdata( L, static_cast<void*>(it->second) );
		lua_settable( L, -3 );
	}
	return 1;
}
//=======================================
int GraphicsGlue::SetDepthState( lua_State* L )
{
	GRAPHICS->SetDepthStencilState( static_cast<ID3D11DepthStencilState*>( lua_touserdata( L, 1 ) ) );
	return 0;
}
/*******************************
*	buffer related
*******************************/
//=======================================
int GraphicsGlue::MapBuffer( lua_State* L )
{
	void** pdata = (void**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	DXObject* bobject = CheckDXObject( L, 2 );
	void* data = GRAPHICS->MapBuffer( bobject );
	(*pdata) = data;

	return 0;
}
//=======================================
int GraphicsGlue::UnmapBuffer( lua_State* L )
{
	GRAPHICS->UnmapBuffer();
	return 0;
}
//=======================================
int GraphicsGlue::ReadBufferStart( lua_State* L )
{
	void** pdata = (void**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	DXObject* bobject = CheckDXObject( L, 2 );
	unsigned int size = 0;
	(*pdata) = GRAPHICS->ReadBufferStart( bobject, size );

//	float* p = (float*)(*pdata);

	lua_pushnumber( L, size );
	return 1;
}
//=======================================
int GraphicsGlue::ReadBufferEnd( lua_State* L )
{
	GRAPHICS->ReadBufferEnd();
	return 0;
}
/**vertex buffers**/
//=======================================
int GraphicsGlue::NewVB( lua_State* L )
{
	int nargs = lua_gettop( L );
	int size = luaL_checkint( L, 1 );
	int cpu = luaL_checkint( L, 2 );
	int stream = luaL_checkint( L, 3 );
	void* data2init = NULL;
	if ( nargs == 4 )
		data2init = *static_cast<void**>( luaL_checkudata( L, 4, "LuaBook.chunk" ) );

	DXObject* vbobject = GRAPHICS->RegisterVertexBuffer( size, cpu > 0, stream > 0, data2init );
	lua_pushlightuserdata( L, vbobject );
	return 1;
}
//=======================================
int GraphicsGlue::InitData( lua_State* L )
{
	void** pdata = (void**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	int size = luaL_checkint( L, 2 );
	void* data = GRAPHICS->RegisterInitData( size );
	*pdata = data;
	return 0;
}
// index buffer
//=======================================
int GraphicsGlue::NewIB( lua_State* L )
{
	int size = luaL_checkint( L, 1 );
	void* data2init = *static_cast<void**>( luaL_checkudata( L, 2, "LuaBook.chunk" ) );

	DXObject* ibobject = GRAPHICS->RegisterIndexBuffer( size, data2init );
	lua_pushlightuserdata( L, ibobject );
	return 1;
}
/**constant buffers**/
//=======================================
int GraphicsGlue::NewCB( lua_State* L )
{
	int num = luaL_checkint( L, 1 );
	DXObject* cbobject = GRAPHICS->RegisterConstantBuffer( num );
	lua_pushlightuserdata( L, cbobject );
	return 1;
}
//=======================================
int GraphicsGlue::AnimationCB( lua_State* L )
{
	DXObject* animcbobject = GRAPHICS->ModelGetAnimationCB();
	lua_pushlightuserdata( L, animcbobject );
	return 1;
}
//=======================================
int GraphicsGlue::CSSetCB( lua_State* L )
{
	DXObject* cbobject = CheckDXObject( L, 1 );
	int slot = luaL_checkint( L, 2 );
	GRAPHICS->CSSetCBuffer( cbobject, slot );
	return 0;
}
//=======================================
int GraphicsGlue::SOSetTarget( lua_State* L )
{
	int nargs = lua_gettop( L );
	DXObject* bobject = NULL;
	if ( nargs > 0 ) bobject = CheckDXObject( L, 1 );
	GRAPHICS->SOSetTarget( bobject );
	return 0;
}
/*******************************
*	effect related
*******************************/
//=======================================
int GraphicsGlue::LoadEffect( lua_State* L )
{
	int num_args = lua_gettop( L );
	const char* filename = luaL_checkstring( L, 1 );
	const char* layout = luaL_checkstring( L, 2 );
	int topology = luaL_checkint( L, 3 );
	const char* nullentry = "";
	const char* entrypts[5] = { nullentry,nullentry,nullentry,nullentry,nullentry };
	switch( num_args )
	{
	case 8:
		entrypts[3] = luaL_checkstring( L, 7 );
		entrypts[4] = luaL_checkstring( L, 8 );
	case 6:
		entrypts[2] = luaL_checkstring( L, 6 );
	case 5:
		entrypts[1] = luaL_checkstring( L, 5 );
	case 4:
		entrypts[0] = luaL_checkstring( L, 4 );
		break;
	default:
		printf( "LoadEffect: shader entry points not specified: %s\n", filename );
		return 0;
	}
	DXObject* effectobject = GRAPHICS->CreateEffectFromFile( filename, layout, topology, entrypts, false );
	lua_pushlightuserdata( L, effectobject );
	return 1;
}
//=======================================
int GraphicsGlue::LoadEffectSO( lua_State* L )
{
	int num_args = lua_gettop( L );
	const char* filename = luaL_checkstring( L, 1 );
	const char* layout = luaL_checkstring( L, 2 );
	int topology = luaL_checkint( L, 3 );
	const char* nullentry = "";
	const char* entrypts[5] = { nullentry,nullentry,nullentry,nullentry,nullentry };
	switch( num_args )
	{
	case 8:
		entrypts[3] = luaL_checkstring( L, 7 );
		entrypts[4] = luaL_checkstring( L, 8 );
	case 6:
		entrypts[2] = luaL_checkstring( L, 6 );
	case 5:
		entrypts[1] = luaL_checkstring( L, 5 );
	case 4:
		entrypts[0] = luaL_checkstring( L, 4 );
		break;
	default:
		printf( "LoadEffect: shader entry points not specified: %s\n", filename );
		return 0;
	}
	DXObject* effectobject = GRAPHICS->CreateEffectFromFile( filename, layout, topology, entrypts, true );
	lua_pushlightuserdata( L, effectobject );
	return 1;
}
/*******************************
*	compute shader
*******************************/
//=======================================
int GraphicsGlue::NewCShader( lua_State* L )
{
	const char* path = luaL_checkstring( L, 1 );
	const char* func = luaL_checkstring( L, 2 );
	DXObject* csobject = GRAPHICS->CreateComputeShader( path, func );

	lua_pushlightuserdata( L, csobject );
	return 1;
}
//=======================================
int GraphicsGlue::DirectCompute( lua_State* L )
{
	DXObject* csobject = CheckDXObject( L, 1 );
	DXObject* uavobject = CheckDXObject( L, 2 );
	int x = luaL_checkint( L, 3 );
	int y = luaL_checkint( L, 4 );

	GRAPHICS->RunDirectCompute( csobject, uavobject, x, y );
	return 0;
}
/*******************************
*	models
*******************************/
//=======================================
int GraphicsGlue::LoadMesh( lua_State* L )
{
	const char* filename = luaL_checkstring( L, 1 );
	std::string prefix = "Assets\\Models\\";
	prefix += filename;
	std::string path = prefix;
	path += ".mesh";
	//////////////////////////////////// Loading Mesh ///////////////////////////////////////////
	std::ifstream fin;
	fin.open( path.c_str(), std::ios::in | std::ios::binary );
	if ( !fin ) { printf( "LoadStaticMesh: invalid path %s.\n", path ); return 0; }

	fin.seekg( 0, std::ios::end );
	int size = static_cast<int>( fin.tellg() );
	fin.seekg( 0, std::ios::beg );
	//printf( "%d\n", size );

	int animated = 0;
	int num_vertices;
	int num_indices;
	unsigned long* indices;
	void* vertices;
	void* vertices_lite;
	int size_lite = 0;

	///////////////////////////////////////////// read the indices ////////////////////////////////////////////	
	fin.read( (char*)&num_indices, sizeof(int) );
	indices = new unsigned long[num_indices];
	fin.read( (char*)indices, sizeof(unsigned long) * num_indices );
	size -= sizeof(int);
	size -= sizeof(unsigned long) * num_indices;
	///////////////////////////////////////////// read the vertices ///////////////////////////////////////////
	fin.read( (char*)&num_vertices, sizeof(int) );
	
	size -= sizeof(int);
	if ( size / num_vertices == sizeof(VertexTypeAnimatedMesh) ) 
	{
		animated = 1;
		vertices = new VertexTypeAnimatedMesh[num_vertices];
		fin.read( (char*)vertices, size );
		vertices_lite = new VertexTypeAnimatedMeshLite[num_vertices];
		VertexTypeAnimatedMesh* v = static_cast<VertexTypeAnimatedMesh*>(vertices);
		VertexTypeAnimatedMeshLite* vl = static_cast<VertexTypeAnimatedMeshLite*>(vertices_lite);
		for ( int i = 0; i < num_vertices; ++i ) vl[i].Set( v[i] );
		size_lite = num_vertices * sizeof(VertexTypeAnimatedMeshLite);
	}
	else if ( size / num_vertices == sizeof(VertexTypeStaticMesh) )
	{
		animated = 0;
		vertices = new VertexTypeStaticMesh[num_vertices];
		fin.read( (char*)vertices, size );
		vertices_lite = new VertexTypeStaticMeshLite[num_vertices];
		VertexTypeStaticMesh* v = static_cast<VertexTypeStaticMesh*>(vertices);
		VertexTypeStaticMeshLite* vl = static_cast<VertexTypeStaticMeshLite*>(vertices_lite);
		for ( int i = 0; i < num_vertices; ++i ) vl[i].Set( v[i] );
		size_lite = num_vertices * sizeof(VertexTypeStaticMeshLite);
	}
	else
	{
		printf( "LoadMesh: mesh size doesn't match.\n" );
		return 0;
	}
	fin.close();
	
	printf( "LoadMesh: Mesh %s loaded, it has %d vertices and %d indices.\n", filename, num_vertices, num_indices );
	DXObject* vbobject = GRAPHICS->RegisterVertexBuffer( size, 0, 0, vertices );
	DXObject* vbobjectl = GRAPHICS->RegisterVertexBuffer( size_lite, 0, 0, vertices_lite );
	DXObject* ibobject = GRAPHICS->RegisterIndexBuffer( num_indices, static_cast<const void*>(indices) );
	
	delete [] vertices;
	delete [] vertices_lite;
	delete [] indices;
	//int iib = GRAPHICS->RegisterAndInitIndexBuffer( num_indices, static_cast<const void*>(indices) );
	////////////////////////////////// Loading mesh textures ////////////////////////////////////
	path = prefix;
	path += ".png";
	DXObject* color_map;
	if ( PathHelper::FileExist( path ) )
		color_map = GRAPHICS->CreateSRVFromFile( path.c_str() );
	else color_map = GRAPHICS->white_map;
	///////////////////////////////// Loading animation data ////////////////////////////////////
	//path = prefix;
	//path += ".anim";
	//GRAPHICS->ModelLoadSkeletonAndAnimations( filename );


	lua_pushnumber( L, num_vertices );
	lua_pushlightuserdata( L, vbobject );
	lua_pushlightuserdata( L, vbobjectl );
	lua_pushnumber( L, num_indices );
	lua_pushlightuserdata( L, ibobject );
	lua_pushlightuserdata( L, color_map );
	lua_pushnumber( L, animated );
	
	return 7;
}
//=======================================
int GraphicsGlue::LoadAnim( lua_State* L )
{
	const char* path = luaL_checkstring( L, 1 );
	DXObject* animobject = GRAPHICS->ModelLoadAnimation( path );
	lua_pushlightuserdata( L, animobject );
	return 1;
}
//=======================================
int GraphicsGlue::TestAnimCtrl( lua_State* L )
{
	AnimationController** panimctrl = (AnimationController**)luaL_checkudata( L, 1, "LuaBook.animctrl" );
	*panimctrl = GRAPHICS->test_animctrl;
	return 0;
}
/*******************************
*	fonts
*******************************/
//=======================================
int GraphicsGlue::NewFont( lua_State* L )
{
	const char* fontname = luaL_checkstring( L, 1 );
	DXObject* fontobject, *imgobject;
	GRAPHICS->RegisterFont( fontname, &fontobject, &imgobject );
	lua_pushlightuserdata( L, fontobject );
	lua_pushlightuserdata( L, imgobject );
	return 2;
}
//=======================================
int GraphicsGlue::FontBuildVB( lua_State* L )
{
	DXObject* fontobject = CheckDXObject( L, 1 );
	const char* sentence = luaL_checkstring( L, 2 );
	float size = (float)luaL_checknumber( L, 3 );
	void** data = (void**)luaL_checkudata( L, 4, "LuaBook.chunk" );
	GRAPHICS->ApplyFont( fontobject, sentence, size, *data );

	return 0;
}
//=======================================
int GraphicsGlue::FontBuildVBFromMemory( lua_State* L )
{
	DXObject* fontobject = CheckDXObject( L, 1 );
	char** sentence = (char**)luaL_checkudata( L, 2, "LuaBook.chunk" );
	float size = (float)luaL_checknumber( L, 3 );
	void** data = (void**)luaL_checkudata( L, 4, "LuaBook.chunk" );
	int len = strlen( *sentence );
	GRAPHICS->ApplyFont( fontobject, *sentence, size, *data );

	lua_pushnumber( L, len );
	return 1;
}
/*******************************
*	materials(graphics library )
*******************************/
//=======================================
int GraphicsGlue::NewMaterial( lua_State* L )
{
	const char* name = luaL_checkstring( L, 1 );
	Material* material = GRAPHICS->RegisterMaterial( name );
	if ( !material ) { printf("material '%s' already exists. please change the name.\n", name ); return 0; }

	Material** udata = (Material**)lua_newuserdata( L, sizeof(void*) );
	*udata = material;

	luaL_getmetatable( L, "LuaBook.material" );
	lua_setmetatable( L, -2 );

	return 1;
}
/*******************************
*	renderer library
*******************************/
//=======================================
void GraphicsGlue::OpenRendererLibrary( lua_State* L )
{
	luaL_reg renderer_lib[] =
	{
		{ "Effect", RendererSetEffect },
		{ "VertexBuffer", RendererSetVertexBuffer },
		{ "IndexBuffer", RendererSetIndexBuffer },
		{ "ConstantBuffer", RendererSetConstantBuffer },
		{ "ShaderResource", RendererSetShaderResource },
		{ "Sampler", RendererSetSampler },
		{ "RasterState", RendererSetRasterState },
		{ "BlendState", RendererSetBlendState },
		{ "DepthStencilState", RendererSetDepthStencilState },

		{ "ClearVertexBuffer", RendererClearVertexBuffer },
		{ "ClearConstantBuffer", RendererClearConstantBuffer },
		{ "ClearShaderResource", RendererClearShaderResource },
		{ "ClearSampler", RendererClearSampler },

		{ "Draw", RendererDraw },
		{ "DrawIndexed", RendererDrawIndexed },
		{ "DrawInstanced", RendererDrawInstanced },
		{ "DrawIndexedInstanced", RendererDrawIndexedInstanced },
		{ "DrawAuto", RendererDrawAuto },

		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.renderer" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	
	luaL_openlib( L, NULL, renderer_lib, 0 );

}
//=======================================
int GraphicsGlue::RendererSetEffect( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	DXObject* effectobject = CheckDXObject( L, 2 );
	GRAPHICS->RendererSetEffect( rendererobject, effectobject );
	return 0;
}
//=======================================
int GraphicsGlue::RendererSetVertexBuffer( lua_State* L )
{
	int nargs = lua_gettop( L );
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	DXObject* vbobject = CheckDXObject( L, 2 );
	int stride = luaL_checkint( L, 3 );
	if ( nargs == 4 )
	{
		int slot = luaL_checkint( L, 4 );
		GRAPHICS->RendererSetVertexBuffer( rendererobject, vbobject, stride, 0, slot );
	}
	else GRAPHICS->RendererSetVertexBuffer( rendererobject, vbobject, stride, 0 );
	return 0;
}
//=======================================
int GraphicsGlue::RendererSetIndexBuffer( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	DXObject* ibobject = CheckDXObject( L, 2 );
	GRAPHICS->RendererSetIndexBuffer( rendererobject, ibobject );
	return 0;
}
//=======================================
int GraphicsGlue::RendererSetConstantBuffer( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	const char* stage = luaL_checkstring( L, 2 );
	DXObject* cbobject = CheckDXObject( L, 3 );
	GRAPHICS->RendererSetConstantBuffer( rendererobject, stage, cbobject );
	return 0;
}
//=======================================
int GraphicsGlue::RendererSetShaderResource( lua_State* L )
{
	int nargs = lua_gettop( L );
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	const char* stage = luaL_checkstring( L, 2 );
	DXObject* srvobject = CheckDXObject( L, 3 );
	if ( nargs == 4 )
	{
		int slot = luaL_checkint( L, 4 );
		GRAPHICS->RendererSetShaderResource( rendererobject, stage, srvobject, slot );
	}
	else
		GRAPHICS->RendererSetShaderResource( rendererobject, stage, srvobject );
	return 0;
}
//=======================================
int GraphicsGlue::RendererSetSampler( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	const char* stage = luaL_checkstring( L, 2 );
	ID3D11SamplerState* sampler = static_cast<ID3D11SamplerState*>( lua_touserdata( L, 3 ) );
	GRAPHICS->RendererSetSampler( rendererobject, stage, sampler );
	return 0;
}
//=======================================
int GraphicsGlue::RendererSetRasterState( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	ID3D11RasterizerState* rstate = static_cast<ID3D11RasterizerState*>( lua_touserdata( L, 2 ) );
	GRAPHICS->RendererSetRasterState( rendererobject, rstate );
	return 0;
}
//=======================================
int GraphicsGlue::RendererSetBlendState(lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	ID3D11BlendState* bstate = static_cast<ID3D11BlendState*>( lua_touserdata( L, 2 ) );
	GRAPHICS->RendererSetBlendState( rendererobject, bstate );
	return 0;
}
//=======================================
int GraphicsGlue::RendererSetDepthStencilState( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	ID3D11DepthStencilState* dsstate = static_cast<ID3D11DepthStencilState*>( lua_touserdata( L, 2 ) );
	GRAPHICS->RendererSetDepthStencilState( rendererobject, dsstate );
	return 0;
}
//=======================================
int GraphicsGlue::RendererClearVertexBuffer( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	GRAPHICS->RendererClearVertexBuffer( rendererobject );
	return 0;
}
//=======================================
int GraphicsGlue::RendererClearConstantBuffer( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	const char* stage = luaL_checkstring( L, 2 );
	GRAPHICS->RendererClearConstantBuffer( rendererobject, stage );
	return 0;
}
//=======================================
int GraphicsGlue::RendererClearShaderResource( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	const char* stage = luaL_checkstring( L, 2 );
	GRAPHICS->RendererClearShaderResource( rendererobject, stage );
	return 0;
}
//=======================================
int GraphicsGlue::RendererClearSampler( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	const char* stage = luaL_checkstring( L, 2 );
	GRAPHICS->RendererClearSampler( rendererobject, stage );
	return 0;
}
//=======================================
int GraphicsGlue::RendererDraw( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	int num_vertices = luaL_checkint( L, 2 );
	GRAPHICS->RendererDraw( rendererobject, num_vertices );
	return 0;
}
//=======================================
int GraphicsGlue::RendererDrawIndexed(lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	int num_indices = luaL_checkint( L, 2 );
	GRAPHICS->RendererDrawIndexed( rendererobject, num_indices );
	return 0;
}
//=======================================
int GraphicsGlue::RendererDrawInstanced(lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	int num_vertices = luaL_checkint( L, 2 );
	int num_instances = luaL_checkint( L, 3 );
	GRAPHICS->RendererDrawInstanced( rendererobject, num_vertices, num_instances );
	return 0;
}
//=======================================
int GraphicsGlue::RendererDrawIndexedInstanced(lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	int num_indices = luaL_checkint( L, 2 );
	int num_instances = luaL_checkint( L, 3 );
	GRAPHICS->RendererDrawIndexedInstanced( rendererobject, num_indices, num_instances );
	return 0;
}
//=======================================
int GraphicsGlue::RendererDrawAuto( lua_State* L )
{
	DXObject* rendererobject = CheckRendererObject( L, 1 );
	GRAPHICS->RendererDrawAuto( rendererobject );
	return 0;
}

/*******************************
*	material library
*******************************/
//=======================================
void GraphicsGlue::OpenMaterialLibrary( lua_State* L )
{
	luaL_reg material_lib[] =
	{
		{ "SetSerializer", SetSerializer },
		{ "ForEachObject", ForEachObject },
		{ "LocateGfxObjData", MatLocateData },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.material" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	
	luaL_openlib( L, NULL, material_lib, 0 );
}
//=======================================
int GraphicsGlue::SetSerializer( lua_State* L )
{
	Material** mat = (Material**)luaL_checkudata( L, 1, "LuaBook.material" );
	const char* content = luaL_checkstring( L, 2 );

	(*mat)->SetSerializer( content );
	return 0;
}
//=======================================
int GraphicsGlue::MatLocateData( lua_State* L )
{
	Material** mat = (Material**)luaL_checkudata( L, 1, "LuaBook.material" );
	char** pdata = (char**)luaL_checkudata( L, 2, "LuaBook.chunk" );
	GraphicsObject** pgo = (GraphicsObject**)luaL_checkudata( L, 3, "LuaBook.gfxobj" );
	const char* id = luaL_checkstring( L, 4 );

	*pdata = (*mat)->GfxObjLocateData( *pgo, id );
	return 0;
}
//=======================================
int GraphicsGlue::ForEachObject( lua_State* L )
{
	Material** mat = (Material**)luaL_checkudata( L, 1, "LuaBook.material" );
	GraphicsObject** pgo = (GraphicsObject**)luaL_checkudata( L, 2, "LuaBook.gfxobj" );

	if ( !lua_isfunction( L, 3 ) ) { printf("##graphics script: Material.ForEachObject(), parameter#3 needs to be a function.\n"); return 0; };
	int param = lua_gettop( L ) - 3;
	GraphicsObject* go = (*mat)->Begin();
	while ( go != (*mat)->End() )
	{
		lua_pushvalue( L, 3 );	// push the function
		//void* pgo = (void**)lua_newuserdata( L, sizeof(void*) );	// push the graphics object
		*pgo = go;
		for ( int i = 3; i < param + 3; ++i )	// push lua parameters
			lua_pushvalue( L, i );
		int error = lua_pcall( L, param, 0, 0 );
		if (error)
		{
			char err_msg[256];
			sprintf_s(err_msg, "%s", lua_tostring( L, -1));
			lua_pop(L, 1);
			printf( "##graphics script(Material.ForEachObject):%s\n", err_msg );
		}
		go = go->Next;
	}
	return 0;
}
/*******************************
*	animCtrl library
*******************************/
//=======================================
void GraphicsGlue::OpenAnimationControllerLibrary( lua_State* L )
{
	luaL_reg animctrl_lib[] =
	{
		{ "Apply", AnimCtrlApply },
		{ "Switch", AnimCtrlSwitch },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.animctrl" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );

	luaL_openlib( L, NULL, animctrl_lib, 0 );
}
//=======================================
int GraphicsGlue::AnimCtrlApply( lua_State* L )
{
	AnimationController* animctrl = *(AnimationController**)(luaL_checkudata( L, 1, "LuaBook.animctrl" ));
	float dt = (float)luaL_checknumber( L, 2 );

	if ( animctrl == NULL ) return 0;

	int playing = GRAPHICS->ModelAnimationApply( animctrl, dt );

	lua_pushnumber( L, playing );
	lua_pushlightuserdata( L, GRAPHICS->ModelGetAnimationCB() );
	
	return 2;
}
//=======================================
int GraphicsGlue::AnimCtrlSwitch( lua_State* L )
{
	AnimationController* animctrl = *(AnimationController**)(luaL_checkudata( L, 1, "LuaBook.animctrl" ));
	DXObject* animobject = CheckDXObject( L, 2 );

	GRAPHICS->ModelAnimationSwitch( animctrl, animobject );
	return 0;
}
/*******************************
*	gfxobj library
*******************************/
//=======================================
void GraphicsGlue::OpenGfxObjLibrary( lua_State* L )
{
	luaL_reg gfxobj_lib[] =
	{
		{ "Flag", GfxObjGetFlag },
		{ "Transform", GfxObjGetTransform },
		{ "Text", GfxObjGetText },
		{ "Camera", GfxObjGetCamera },
		{ "AnimCtrl", GfxObjGetAnimCtrl },
		{ "Data", GfxObjGetData },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.gfxobj" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	
	luaL_openlib( L, NULL, gfxobj_lib, 0 );
}
//=======================================
int GraphicsGlue::GfxObjGetFlag( lua_State* L )
{
	void** pgo = (void**)luaL_checkudata( L, 1, "LuaBook.gfxobj" );

	if ( lua_gettop( L ) > 1 )
	{
		int flag = luaL_checkint( L, 2 );
		GraphicsComposition* gc = static_cast<GraphicsComposition*>(*pgo);
		if ( gc ) gc->flag += flag;
		return 0;
	}
	else
	{
		GraphicsObject* go = (GraphicsObject*)*pgo;
		lua_pushnumber( L, go->Flag() );
		return 1;
	}
}
//=======================================
int GraphicsGlue::GfxObjGetTransform( lua_State* L )
{
	void** pgo = (void**)luaL_checkudata( L, 1, "LuaBook.gfxobj" );
	void** pdata = (void**)luaL_checkudata( L, 2, "LuaBook.transform" );

	GraphicsObject* go = (GraphicsObject*)*pgo;
	*pdata = go->Transform();
	if ( *pdata == NULL ) printf( "#warning: getting a NULL pointer : transform.\n" );
	return 0;
}
//=======================================
int GraphicsGlue::GfxObjGetText( lua_State* L )
{
	void** pgo = (void**)luaL_checkudata( L, 1, "LuaBook.gfxobj" );
	void** pdata = (void**)luaL_checkudata( L, 2, "LuaBook.text" );
	
	GraphicsObject* go = (GraphicsObject*)*pgo;
	*pdata = go->Text();
	if ( *pdata == NULL ) printf( "#warning: getting a NULL pointer : text.\n" );
	return 0;
}
//=======================================
int GraphicsGlue::GfxObjGetCamera( lua_State* L )
{
	void** pgo = (void**)luaL_checkudata( L, 1, "LuaBook.gfxobj" );
	void** pdata = (void**)luaL_checkudata( L, 2, "LuaBook.camera" );

	GraphicsObject* go = (GraphicsObject*)*pgo;
	*pdata = go->Camera();
	if ( *pdata == NULL ) printf( "#warning: getting a NULL pointer : camera.\n" );
	return 0;
}
//=======================================
int GraphicsGlue::GfxObjGetAnimCtrl( lua_State* L )
{
	void** pgo = (void**)luaL_checkudata( L, 1, "LuaBook.gfxobj" );
	void** panimctrl = (void**)luaL_checkudata( L, 2, "LuaBook.animctrl" );

	GraphicsObject* go = (GraphicsObject*)*pgo;
	*panimctrl = go->AnimCtrl();
	if ( *panimctrl == NULL ) printf( "GfxObjGetAnimCtrl: got a NULL pointer.\n" );
	return 0;
}
//=======================================
int GraphicsGlue::GfxObjGetData( lua_State* L )
{
	void** pgo = (void**)luaL_checkudata( L, 1, "LuaBook.gfxobj" );
	void** pdata = (void**)luaL_checkudata( L, 2, "LuaBook.chunk" );

	GraphicsObject* go = (GraphicsObject*)*pgo;
	*pdata = go->data;
	return 0;
}

}
