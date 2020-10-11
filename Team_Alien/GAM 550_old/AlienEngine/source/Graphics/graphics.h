#pragma once

#include "System.h"


namespace Framework
{

#define MAX_NUM_SRVS		8
#define MAX_NUM_CSS			8

// forward declarations
class D3DManager;
class SamplerStates;
class RasterStates;
class BlendStates;
class DepthStencilStates;
class Material;
class GraphicsObject;
class AnimationController;
class DXObjectList;
class DXObject;
class Renderer;
class Effect;

class Graphics : public ISystem
{

	friend class GraphicsGlue;
	
	DXObjectList* dxobjects;
	unsigned int scope;

private:
	HWND hwnd;
	int screen_width;
	int screen_height;

	D3DManager* d3dmanager;

	ID3D11Device* device;
	ID3D11DeviceContext* device_context;

public:
	Graphics();
	~Graphics();
	virtual void Update( float timeslice );
	virtual void Initialize();
	virtual void Free();
	virtual void SendMessage( Message* msg );
	virtual void Unload();

	ID3D11Device* GetDevice() { return device; }

	virtual std::string GetName() { return "Graphics"; }
	void SetWindowProperties( HWND hwnd, int width, int height );

	void Present( int vsyncs );

	
private:
	DXGI_FORMAT TranslateToDXGI( const char* format );
	/************************
	* swap chain
	*************************/
	IDXGISwapChain* swap_chain;
	DXObject* rtv_screen_object;
	DXObject* back_buffer_object;

	void InitSwapChainAndStuff();
	void BuildSwapChainAndStuff();
	void FreeSwapChainAndStuff();
	void UnloadSwapChainAndStuff();
	/****************************
	* profiler
	****************************/
	struct ProfileData
	{
		ID3D11Query* query_disjoint;
		ID3D11Query* query_start;
		ID3D11Query* query_end;
		ProfileData() : query_disjoint(NULL), query_start(NULL), query_end( NULL ) {};
		~ProfileData() { if ( query_disjoint) query_disjoint->Release(); 
						 if ( query_start ) query_start->Release();
						 if ( query_end ) query_end->Release(); }
	};
	typedef std::map< std::string, ProfileData > ProfileMapType;

	ProfileMapType profile_map;

	void InitProfiler();
	void FreeProfiler();
	void UnloadProfiler();
	
	void StartProfile( const char* name );
	void EndProfileAndConstructString( const char* name, char* str, int maxlength );
	double GetProfilingResult( const char* name );
	/************************
	* renderer
	*************************/
	DXObject* RegisterRenderer();
	void RendererSetEffect( DXObject* rendererobject, DXObject* effectobject );
	void RendererSetVertexBuffer( DXObject* rendererobject, DXObject* vbobject, unsigned int stride, unsigned int offset );
	void RendererSetVertexBuffer( DXObject* rendererobject, DXObject* vbobject, unsigned int stride, unsigned int offset, unsigned int slot );
	void RendererSetIndexBuffer( DXObject* rendererobject, DXObject* ibobject );
	void RendererSetConstantBuffer( DXObject* rendererobject, const char* stage, DXObject* cbobject );
	void RendererSetShaderResource( DXObject* rendererobject, const char* stage, DXObject* srvobject );
	void RendererSetShaderResource( DXObject* rendererobject, const char* stage, DXObject* srvobject, unsigned int slot );
	void RendererSetSampler( DXObject* rendererobject, const char* stage, ID3D11SamplerState* sampler );
	void RendererSetRasterState( DXObject* rendererobject, ID3D11RasterizerState* rstate );
	void RendererSetBlendState( DXObject* rendererobject, ID3D11BlendState* bstate );
	void RendererSetDepthStencilState( DXObject* rendererobject, ID3D11DepthStencilState* dsstate );
	//draw functions
	void RendererDraw( DXObject* rendererobject, int num_vertices );
	void RendererDrawIndexed( DXObject* rendererobject, int num_indices );
	void RendererDrawInstanced( DXObject* rendererobject, int num_vertices, int num_instances );
	void RendererDrawIndexedInstanced( DXObject* rendererobject, int num_indices, int num_instances );
	void RendererDrawAuto( DXObject* rendererobject );
	//clear functions
	void RendererClearVertexBuffer( DXObject* rendererobject );
	void RendererClearConstantBuffer( DXObject* rendererobject, const char* stage );
	void RendererClearShaderResource( DXObject* rendererobject, const char* stage );
	void RendererClearSampler( DXObject* rendererobject, const char* stage );
	/************************
	* view manager
	*************************/
	struct RenderTargetInfo
	{
		unsigned int width;
		unsigned int height;
		float scale;
		bool relative;
		DXObject* rtvobject;
		DXObject* srvobject;
		DXObject* textureobject;
		DXGI_FORMAT format;
		bool generatemips;
		unsigned int miplevels;
		void Release() { delete this; }
	};
	struct DepthStencilInfo
	{
		unsigned int width;
		unsigned int height;
		float scale;
		bool relative;
		DXObject* dsvobject;
		DXObject* srvobject;
		DXObject* textureobject;
		void Release() { delete this; }
	};
	ID3D11RenderTargetView* rtv2set[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	ID3D11ShaderResourceView* srv2set[MAX_NUM_SRVS];
	ID3D11DepthStencilView* current_dsv;
	D3D11_VIEWPORT viewport[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	
	DXObject* white_map;
	DXObject* black_map;
	// render target
	void CreateRenderTarget( ID3D11Texture2D** texture, ID3D11RenderTargetView** rtv, ID3D11ShaderResourceView** srv, RenderTargetInfo* rtinfo );
	void ResizeRenderTargets();
	void RegisterRenderTarget( DXObject** rtinfoobject, DXObject** srvobject, float scale, const char* format, int generatemips, int miplevels );
	void RegisterRenderTarget( DXObject** rtinfoobject, DXObject** srvobject, int width, int height, const char* format, int generatemips, int miplevels, bool uniformed );
	void SetRenderTargetViews( DXObject** rtinfoobject, int num );
	void SetRenderTargetScreen();
	void SetNullRenderTargets( float width, float height );
	void ClearRenderTargetView( DXObject* rtinfoobject, float* data );
	void SetViewportDepth( float mindepth, float maxdepth );
	// depth stencil
	void CreateDepthStencil( int width, int height, ID3D11Texture2D** texture, ID3D11DepthStencilView** dsv, ID3D11ShaderResourceView** srv );
	void ResizeDepthStencils();
	void RegisterDepthStencil( DXObject** dsvobject, DXObject** srvobject, float scale );
	void RegisterDepthStencil( DXObject** dsvobject, DXObject** srvobject, int width, int height, bool reletive );
	void SetDepthStencilView( DXObject* dsvobject );
	void ClearDepthStencilView( DXObject* dsvobject, float value );
	// unordered access
	void RegisterUnorderedAccess( DXObject** uavobject, DXObject** srvobject, DXObject** sbobject, int structsize, int numstructs, void* initdata );
	// shader resource
	DXObject* CreateSRVFromFile( const char* filename );
	void ReloadSRVImage( void* srv, const char* filename );
	void CSSetShaderResouces( DXObject** srvobjects, int num );
	void GenerateMips( DXObject* srvobject );
	
	void InitViews();
	/************************
	* buffers
	*************************/
	ID3D11Buffer* mapped_buffer;
	ID3D11Buffer* read_buffer;
	void* init_data;

	void* MapBuffer( DXObject* );
	void UnmapBuffer();

	void* ReadBufferStart( DXObject* bobject, unsigned int& size );
	void ReadBufferEnd();
	
	void* RegisterInitData( int size );

	DXObject* RegisterVertexBuffer( int size, bool cpu_access, bool stream_output, const void* data2init );

	DXObject* RegisterIndexBuffer( int size, const void* data2init );

	DXObject* RegisterConstantBuffer( size_t size );
	void CSSetCBuffer( DXObject* cbobject, int slot );

	void SOSetTarget( DXObject* bobject );

	void InitBuffers();
	void FreeBuffers();
	void UnloadBuffers();
	/************************
	* states
	*************************/
	SamplerStates* sampler_states;
	BlendStates* blend_states;
	RasterStates* raster_states;
	DepthStencilStates* depth_stencil_states;
	float blend_factor[4];

	SamplerStates* GetSamplers() { return sampler_states; }
	BlendStates* GetBlendStates() { return blend_states; }
	RasterStates* GetRasterStates() { return raster_states; }
	DepthStencilStates* GetDepthStencilStates() { return depth_stencil_states; }
	void CSSetSamplers( int slot, int num, ID3D11SamplerState** samplers );
	void SetBlendState( ID3D11BlendState* blend_state );
	void SetRasterState( ID3D11RasterizerState* raster_state );
	void SetDepthStencilState( ID3D11DepthStencilState* depth_stencil_state );

	void InitStates();
	void FreeStates();
	void UnloadStates();
	/************************
	* effects
	*************************/
	void ReloadEffect( void* effect, const char* filename );
	DXObject* CreateEffectFromFile( const char* filename, const char* str_layout, int topology, const char* entrypts [5], bool so );

	void InitEffects();
	void FreeEffects();
	void UnloadEffects();
	/************************
	* compute shaders
	*************************/
	DXObject* CreateComputeShader( const char* path, const char* entry );
	void RunDirectCompute( DXObject* csobject, DXObject* uavobject, int x, int y );

	void InitDirectCompute();
	void FreeDirectCompute();
	void UnloadDirectCompute();
	/************************
	* material manager
	*************************/
	typedef std::map<std::string, Material*> MaterialMapType;
	MaterialMapType material_map;

	Material* RegisterMaterial( const char* name );
	void PrintMaterialNames();

	void InitMaterials();
	void FreeMaterials();
	void UnloadMaterials();
	/************************
	* model
	*************************/
	DXObject* animation_cb;
	AnimationController* test_animctrl;

	DXObject* ModelLoadAnimation( const char* name );
	int ModelAnimationApply( AnimationController* animctrl, float dt );
	DXObject* ModelGetAnimationCB() { return animation_cb; }
	void ModelAnimationSwitch( AnimationController* animctrl, DXObject* animobject );

	void InitModels();
	void FreeModels();
	void UnloadModels();
	/************************
	* font
	*************************/
	void RegisterFont( const char* fontname, DXObject** fontobject, DXObject** imgobject );
	void ApplyFont( DXObject* font, const char* sentence, float size, void* vdata );

public:
	Material* GetMaterial( const char* name );
};
extern Graphics* GRAPHICS;
extern Graphics* D3D;

}
