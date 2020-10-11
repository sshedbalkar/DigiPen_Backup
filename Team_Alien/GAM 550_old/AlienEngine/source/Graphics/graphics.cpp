#include "Precompiled.h"
#include <D3DX11async.h>
#include <fstream>
#include "graphics.h"
#include "d3dmanager.h"
#include "Utility.h"
#include "DXObject.h"
#include "deviceStates.h"
#include "Message.h"
#include "graphicsGlue.h"
#include "luaLibrary.h"
//
#include "renderer.h"
#include "effect.h"
#include "material.h"
#include "font.h"
//systems
#include "Core.h"
#include "resources.h"
//model
#include "ModelStructures.h"
#include "animation.h"

//component registration
#include "Factory.h"
#include "ComponentCreator.h"
#include "GraphicsObject.h"
#include "AnimationController.h"


namespace Framework
{
const bool default_full_screen = false;


Graphics* GRAPHICS = NULL;
Graphics* D3D = NULL;

//======================================
Graphics::Graphics() : d3dmanager( NULL ), dxobjects( NULL )
{
	GRAPHICS = this;
	D3D = this;
}
//======================================
Graphics::~Graphics()
{
	Free();
}
//======================================
void Graphics::Initialize()
{
	printf("Initializing graphics..\n");
	scope = 0;

	dxobjects = new DXObjectList();

	// initialize device
	printf("	Initializing D3D...\n");
	d3dmanager = new D3DManager();
	d3dmanager->Initialize();

	device = d3dmanager->GetDevice();
	device_context = d3dmanager->GetDeviceContext();
	
	InitViews();
	InitSwapChainAndStuff();
	BuildSwapChainAndStuff();
	InitMaterials();
	InitStates();
	InitBuffers();
	InitEffects();
	InitModels();
	InitDirectCompute();

	RegisterComponent(GraphicsComposition);
	RegisterComponent(AnimationController);

	LuaLibrary::Add( GraphicsGlue::OpenLibrary, this );
	printf("Finished initializing graphics.\n");
		
	if ( default_full_screen ) swap_chain->SetFullscreenState( true, NULL );
	
	scope = 1;
	//std::cout <<*m <<'\t' << *(m+1) <<'\t' <<*(m+2) <<'\t' <<*(m+3) <<std::endl; m+=4;
	//std::cout <<*m <<'\t' << *(m+1) <<'\t' <<*(m+2) <<'\t' <<*(m+3) <<std::endl; m+=4;
	//std::cout <<*m <<'\t' << *(m+1) <<'\t' <<*(m+2) <<'\t' <<*(m+3) <<std::endl; m+=4;
	//std::cout <<*m <<'\t' << *(m+1) <<'\t' <<*(m+2) <<'\t' <<*(m+3) <<std::endl; m+=4;

}
//======================================
void Graphics::Free()
{
	S_DELETE(test_animctrl);
	Unload();
	dxobjects->Sweep(0);
	S_DELETE( dxobjects );
	FreeBuffers();
	FreeStates();
	FreeMaterials();
	FreeSwapChainAndStuff();

	device = NULL;
	device_context = NULL;
	swap_chain = NULL;

	safeDelete( d3dmanager );
}
//======================================
void Graphics::Unload()
{
	dxobjects->Sweep( 1 );
	UnloadBuffers();
	UnloadStates();
	UnloadMaterials();
	UnloadSwapChainAndStuff();
}
//======================================
void Graphics::Update( float timeslice )
{
}
//======================================
void Graphics::SendMessage( Message* msg )
{	
	if ( msg->MessageId == Mid::Resolution )
	{
		Resolution* m = (Resolution*)msg;
		screen_width = m->w;
		screen_height = m->h;
		BuildSwapChainAndStuff();
	}
}
//======================================
void Graphics::SetWindowProperties( HWND _hwnd, int width, int height )
{
	hwnd = _hwnd;
	screen_width = width;
	screen_height = height;
}
//======================================
void Graphics::Present( int num_vsyncs )
{
	TwDraw();
	swap_chain->Present( num_vsyncs, 0 );
}
/******************************************************************************
*	profiler
*******************************************************************************/
//======================================
void Graphics::StartProfile( const char* name )
{
	ProfileData& profile = profile_map[name];
	if ( profile.query_disjoint == NULL )
	{
        D3D11_QUERY_DESC desc;
        desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        desc.MiscFlags = 0;
		device->CreateQuery( &desc, &profile.query_disjoint );

		desc.Query = D3D11_QUERY_TIMESTAMP;
		device->CreateQuery( &desc, &profile.query_start );
		device->CreateQuery( &desc, &profile.query_end );
	}
	device_context->Begin( profile.query_disjoint );
	device_context->End( profile.query_start );
}
//======================================
void Graphics::EndProfileAndConstructString( const char* name, char* str, int maxlength )
{
	ProfileMapType::iterator it = profile_map.find( name );
	if ( it == profile_map.end() ) return;
	ProfileData& profile = it->second;
	device_context->End( profile.query_end );
	device_context->End( profile.query_disjoint );

	UINT64 start, end;
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint;
	
	while ( device_context->GetData( profile.query_start, &start, sizeof(start), 0 ) != S_OK );
	while ( device_context->GetData( profile.query_end, &end, sizeof(end), 0 ) != S_OK ) ;
	while ( device_context->GetData(profile.query_disjoint, &disjoint, sizeof(disjoint), 0 ) != S_OK ) ;

	float frequency = static_cast<float>( disjoint.Frequency );
	float time = ( end - start ) / frequency * 1000.0f;
	sprintf_s( str, maxlength, "%s: %0.3fms", it->first.c_str(), time );
}
/********************************
* swap chain
********************************/
//======================================
void Graphics::InitSwapChainAndStuff()
{
	swap_chain = NULL;
	back_buffer_object = NULL;
	rtv_screen_object = NULL;
}
//======================================
void Graphics::BuildSwapChainAndStuff()
{

	DXGI_SWAP_CHAIN_DESC sc_desc;
	IDXGIDevice * dxgi_device;
	IDXGIAdapter * adapter;
	IDXGIFactory * factory;
	HRESULT hr;
	BOOL full_screen = FALSE;
	if ( swap_chain ) { swap_chain->GetFullscreenState( &full_screen, NULL ); }
	if ( full_screen ) { swap_chain->SetFullscreenState( 0, NULL ); }
	
	FreeSwapChainAndStuff();

	ZeroMemory( &sc_desc, sizeof(sc_desc) );
	sc_desc.BufferCount = 2;
	sc_desc.BufferDesc.Width = screen_width;
	sc_desc.BufferDesc.Height = screen_height;
	sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//vsync
	sc_desc.BufferDesc.RefreshRate.Numerator = 0;
	sc_desc.BufferDesc.RefreshRate.Denominator = 1;
	sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc.OutputWindow = hwnd;
	// multi sampling off
	sc_desc.SampleDesc.Count = 1;
	sc_desc.SampleDesc.Quality = 0;
	sc_desc.Windowed = true;
	sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sc_desc.Flags = 0;
	
	DXCall( device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgi_device) );
	DXCall( dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void **)&adapter) );
	DXCall( adapter->GetParent(__uuidof(IDXGIFactory), (void **)&factory) );
	DXCall( factory->CreateSwapChain( device, &sc_desc, &swap_chain ) );
	
	ID3D11Texture2D* back_buffer_ptr;
	ID3D11RenderTargetView* rtv_back_buffer;

	DXCall( swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_ptr) );
	DXCall( device->CreateRenderTargetView( back_buffer_ptr, NULL, &rtv_back_buffer ) );
	if ( !rtv_screen_object ) rtv_screen_object = dxobjects->AddDXObject( rtv_back_buffer, 0 );
	else rtv_screen_object->UpdateObject( rtv_back_buffer );
	if ( !back_buffer_object ) back_buffer_object = dxobjects->AddDXObject( back_buffer_ptr, 0 );
	else back_buffer_object->UpdateObject( back_buffer_ptr );

	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
	{
		viewport[i].Width = (float)screen_width;
		viewport[i].Height = (float)screen_height;
		viewport[i].MinDepth = 0.0f;
		viewport[i].MaxDepth = 1.0f;
		viewport[i].TopLeftX = 0.0f;
		viewport[i].TopLeftY = 0.0f;
	}

	ResizeRenderTargets();
	ResizeDepthStencils();

	if ( full_screen ) swap_chain->SetFullscreenState( true, NULL );
}
//======================================
void Graphics::FreeSwapChainAndStuff()
{
	if ( swap_chain ) {swap_chain->Release(); swap_chain = NULL;}
}
//======================================
void Graphics::UnloadSwapChainAndStuff()
{
}
/********************************
* renderer
********************************/
//======================================
DXObject* Graphics::RegisterRenderer()
{
	Renderer* renderer = new Renderer;
	return dxobjects->AddDXObject( renderer, scope );
}
//======================================
void Graphics::RendererSetEffect( DXObject* rendererobject, DXObject* effectobject )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	Effect* effect = effectobject->Object<Effect>();
	renderer->SetEffect( effect );
}
//======================================
void Graphics::RendererSetVertexBuffer( DXObject* rendererobject, DXObject* vbobject, unsigned int stride, unsigned int offset )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	ID3D11Buffer* vbuffer = vbobject->Object<ID3D11Buffer>();
	renderer->SetVertexBuffer( vbuffer, stride, offset );
}
//======================================
void Graphics::RendererSetVertexBuffer( DXObject* rendererobject, DXObject* vbobject, unsigned int stride, unsigned int offset, unsigned int slot )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	ID3D11Buffer* vbuffer = vbobject->Object<ID3D11Buffer>();
	renderer->SetVertexBuffer( vbuffer, stride, offset, slot );
}
//======================================
void Graphics::RendererSetIndexBuffer( DXObject* rendererobject, DXObject* ibobject )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	ID3D11Buffer* ibuffer = ibobject->Object<ID3D11Buffer>();
	renderer->SetIndexBuffer( ibuffer );
}
//======================================
void Graphics::RendererSetConstantBuffer( DXObject* rendererobject, const char* stage, DXObject* cbobject )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	ID3D11Buffer* cbuffer = cbobject->Object<ID3D11Buffer>();
	renderer->SetConstantBuffer( stage, cbuffer );
}
//======================================
void Graphics::RendererSetShaderResource( DXObject* rendererobject, const char* stage, DXObject* srvobject )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->SetShaderResource( stage, srvobject );
}
//======================================
void Graphics::RendererSetShaderResource( DXObject* rendererobject, const char* stage, DXObject* srvobject, unsigned int slot )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->SetShaderResource( stage, srvobject, slot );
}
//======================================
void Graphics::RendererSetSampler( DXObject* rendererobject, const char* stage, ID3D11SamplerState* sampler )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->SetSamplerState( stage, sampler );
}
//======================================
void Graphics::RendererSetRasterState( DXObject* rendererobject, ID3D11RasterizerState* rstate )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->SetRasterState( rstate );
}
//======================================
void Graphics::RendererSetBlendState( DXObject* rendererobject, ID3D11BlendState* bstate )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->SetBlendState( bstate );
}
//======================================
void Graphics::RendererSetDepthStencilState( DXObject* rendererobject, ID3D11DepthStencilState* dsstate )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->SetDepthStencilState( dsstate );
}
//======================================
void Graphics::RendererClearVertexBuffer( DXObject* rendererobject )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->ClearVertexBuffer();
}
//======================================
void Graphics::RendererClearConstantBuffer( DXObject* rendererobject, const char* stage )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->ClearConstantBuffer( stage );
}
//======================================
void Graphics::RendererClearShaderResource( DXObject* rendererobject, const char* stage )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->ClearShaderResource( stage );
}
//======================================
void Graphics::RendererClearSampler( DXObject* rendererobject, const char* stage )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->ClearSamplerState( stage );
}
//======================================
void Graphics::RendererDraw( DXObject* rendererobject, int num_vertices )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->Apply( device_context );
	device_context->Draw( num_vertices, 0 );
}
//======================================
void Graphics::RendererDrawIndexed( DXObject* rendererobject, int num_indices )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->Apply( device_context );
	device_context->DrawIndexed( num_indices, 0, 0 );
}
//======================================
void Graphics::RendererDrawInstanced( DXObject* rendererobject, int num_vertices, int num_instances )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->Apply( device_context );
	device_context->DrawInstanced( num_vertices, num_instances, 0, 0 );
}
//======================================
void Graphics::RendererDrawIndexedInstanced( DXObject* rendererobject, int num_indices, int num_instances )
{
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->Apply( device_context );
	device_context->DrawIndexedInstanced( num_indices, num_instances, 0, 0, 0 );
}
//======================================
void Graphics::RendererDrawAuto( DXObject* rendererobject )
{	
	Renderer* renderer = rendererobject->Object<Renderer>();
	renderer->Apply( device_context );
	device_context->DrawAuto();
}
/********************************
* view manager
********************************/
//======================================
void Graphics::InitViews()
{
	current_dsv = NULL;
	white_map = CreateSRVFromFile( "Assets\\textures\\white.png" );
	black_map = CreateSRVFromFile( "Assets\\textures\\black.png" );
}
//****************** render target **********************//
//======================================
void Graphics::RegisterRenderTarget( DXObject** rtinfoobject, DXObject** srvobject, int width, int height, const char* format, int generatemips, int miplevels, bool relative )
{
	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;

	RenderTargetInfo* rtinfo = new RenderTargetInfo;
	rtinfo->width = width;
	rtinfo->height = height;
	rtinfo->relative = relative;
	rtinfo->scale = relative ? (float)height/(float)screen_height : 1.0f;
	rtinfo->format = TranslateToDXGI( format );
	rtinfo->generatemips = generatemips > 0 ? true : false;
	rtinfo->miplevels = miplevels;
	
	CreateRenderTarget( &texture, &rtv, &srv, rtinfo );

	rtinfo->rtvobject = dxobjects->AddDXObject( rtv, scope );
	rtinfo->srvobject = dxobjects->AddDXObject( srv, scope );
	rtinfo->textureobject = dxobjects->AddDXObject( texture, scope );

	*rtinfoobject = dxobjects->AddDXObject( rtinfo, scope );
	*srvobject = rtinfo->srvobject;
}
//======================================
void Graphics::RegisterRenderTarget( DXObject** rtinfoobject, DXObject** srvobject, float scale, const char* format, int generatemips, int miplevels )
{
	int w = (int)((double)screen_width * scale );
	int h = (int)((double)screen_height * scale );
	RegisterRenderTarget( rtinfoobject, srvobject, w, h, format, generatemips, miplevels, true );
}
//======================================
void Graphics::SetRenderTargetViews( DXObject** rtinfoobjects, int num )
{
	for ( int i = 0; i < num; ++i )
	{
		RenderTargetInfo* rtinfo = rtinfoobjects[i]->Object<RenderTargetInfo>();
		rtv2set[i] = rtinfo->rtvobject->Object<ID3D11RenderTargetView>();
		viewport[i].Width = static_cast<float>( rtinfo->width );
		viewport[i].Height = static_cast<float>( rtinfo->height );
	}

	device_context->RSSetViewports( num, viewport );
	device_context->OMSetRenderTargets( num, rtv2set, current_dsv );
}
//======================================
void Graphics::SetViewportDepth( float mindepth, float maxdepth )
{
	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
	{
		viewport[i].MinDepth = mindepth;
		viewport[i].MaxDepth = maxdepth;
	}
	device_context->RSSetViewports( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, viewport );
}
//======================================
void Graphics::SetRenderTargetScreen()
{
	viewport[0].Width = (float)screen_width;
	viewport[0].Height = (float)screen_height;
	device_context->RSSetViewports( 1, viewport );
	ID3D11RenderTargetView* rtv = rtv_screen_object->Object<ID3D11RenderTargetView>();
	device_context->OMSetRenderTargets( 1, &rtv, current_dsv );
}
//======================================
void Graphics::SetNullRenderTargets( float width, float height )
{
	viewport[0].Width = width;
	viewport[0].Height = height;
	device_context->RSSetViewports( 1, viewport );
	ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {NULL};
	device_context->OMSetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtvs, current_dsv );
}
//======================================
void Graphics::ClearRenderTargetView( DXObject* rtinfoobject, float* data )
{
	RenderTargetInfo* rtinfo = rtinfoobject->Object<RenderTargetInfo>();
	ID3D11RenderTargetView* rtv = rtinfo->rtvobject->Object<ID3D11RenderTargetView>();
	device_context->ClearRenderTargetView( rtv, data );
}
//======================================
void Graphics::CreateRenderTarget( ID3D11Texture2D** texture, ID3D11RenderTargetView** rtv, ID3D11ShaderResourceView** srv, RenderTargetInfo* rtinfo )
{
	D3D11_TEXTURE2D_DESC texture_desc;
	HRESULT hr;
	ZeroMemory(&texture_desc, sizeof(texture_desc));
	texture_desc.Width = rtinfo->width;
	texture_desc.Height = rtinfo->height;
	texture_desc.MipLevels = rtinfo->miplevels;
	texture_desc.ArraySize = 1;
	texture_desc.Format = rtinfo->format;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = rtinfo->generatemips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	DXCall( device->CreateTexture2D(&texture_desc, NULL, texture ) );
	DXCall( device->CreateRenderTargetView( *texture, NULL, rtv ) );
	DXCall( device->CreateShaderResourceView( *texture, NULL, srv ) );
}
//======================================
void Graphics::ResizeRenderTargets()
{
	std::list<DXObject*>::iterator it = dxobjects->Begin();
	while ( it != dxobjects->End() )
	{
		if ( (*it)->IsType( typeid(RenderTargetInfo) ) )
		{
			RenderTargetInfo* rtinfo = (*it)->Object<RenderTargetInfo>();
			if ( rtinfo->relative )
			{
				ID3D11Texture2D* texture;
				ID3D11RenderTargetView* rtv;
				ID3D11ShaderResourceView* srv;

				rtinfo->width = (unsigned int) ( rtinfo->scale * (float)screen_width );
				rtinfo->height = (unsigned int) ( rtinfo->scale * (float)screen_height );

				CreateRenderTarget( &texture, &rtv, &srv, rtinfo );

				rtinfo->rtvobject->UpdateObject( rtv );
				rtinfo->srvobject->UpdateObject( srv );
			}
		}
		++it;
	}
}
//******************   depth stencil ********************//
//======================================
void Graphics::RegisterDepthStencil( DXObject** dsvobject, DXObject** srvobject, int width, int height, bool relative )
{
	ID3D11Texture2D* texture;
	ID3D11DepthStencilView* dsv;
	ID3D11ShaderResourceView* srv;

	CreateDepthStencil( width, height, &texture, &dsv, &srv );
		
	DepthStencilInfo* dsinfo = new DepthStencilInfo;
	dsinfo->width = width;
	dsinfo->height = height;
	dsinfo->relative = relative;
	dsinfo->scale = relative ? (float)height/(float)screen_height : 1.0f;
	dsinfo->dsvobject = dxobjects->AddDXObject( dsv, scope );
	dsinfo->srvobject = dxobjects->AddDXObject( srv, scope );
	dsinfo->textureobject = dxobjects->AddDXObject( texture, scope );

	*dsvobject = dsinfo->dsvobject;
	*srvobject = dsinfo->srvobject;
	dxobjects->AddDXObject( dsinfo, scope );
}
//======================================
void Graphics::RegisterDepthStencil( DXObject** dsvobject, DXObject** srvobject, float scale )
{
	int w = (int)((double)screen_width * scale );
	int h = (int)((double)screen_height * scale );
	RegisterDepthStencil( dsvobject, srvobject, w, h, true );
}
//======================================
void Graphics::SetDepthStencilView( DXObject* dsvobject )
{
	if ( dsvobject )
		current_dsv = dsvobject->Object<ID3D11DepthStencilView>();
	else current_dsv = NULL;
}
//======================================
void Graphics::ClearDepthStencilView( DXObject* dsvobject, float value )
{
	ID3D11DepthStencilView* dsv2clear = dsvobject->Object<ID3D11DepthStencilView>();
	device_context->ClearDepthStencilView( dsv2clear, D3D11_CLEAR_DEPTH, value, 0 );
}
//======================================
void Graphics::CreateDepthStencil( int width, int height, ID3D11Texture2D** texture, ID3D11DepthStencilView** dsv, ID3D11ShaderResourceView** srv )
{
	D3D11_TEXTURE2D_DESC texture_desc;
	HRESULT hr;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		
	// Initialize the render target texture description.
	ZeroMemory(&texture_desc, sizeof(texture_desc));
	ZeroMemory( &dsv_desc, sizeof(dsv_desc) );
	ZeroMemory(&srv_desc, sizeof(srv_desc));

	// Setup the render target texture description.
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.ArraySize = 1;
	texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture_desc.MipLevels = 1;
	texture_desc.MiscFlags = 0;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;
	dsv_desc.Flags = 0;
	
	// Setup the description of the shader resource view.
	srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;
	
	DXCall( device->CreateTexture2D(&texture_desc, NULL, texture ) );
	DXCall( device->CreateDepthStencilView( *texture, &dsv_desc, dsv ) );
	DXCall( device->CreateShaderResourceView( *texture, &srv_desc, srv ) );

}
//======================================
void Graphics::ResizeDepthStencils()
{
	std::list<DXObject*>::iterator it = dxobjects->Begin();
	while ( it != dxobjects->End() )
	{
		if ( (*it)->IsType( typeid(DepthStencilInfo) ) )
		{
			DepthStencilInfo* dsinfo = (*it)->Object<DepthStencilInfo>();
			if ( dsinfo->relative )
			{
				ID3D11Texture2D* texture;
				ID3D11DepthStencilView* dsv;
				ID3D11ShaderResourceView* srv;

				dsinfo->width = (unsigned int) ( dsinfo->scale * (float)screen_width );
				dsinfo->height = (unsigned int) ( dsinfo->scale * (float)screen_height );

				CreateDepthStencil( dsinfo->width, dsinfo->height, &texture, &dsv, &srv );

				dsinfo->dsvobject->UpdateObject( dsv );
				dsinfo->srvobject->UpdateObject( srv );
			}
		}
		++it;
	}
}
//************ unordered access *************//
//======================================
void Graphics::RegisterUnorderedAccess( DXObject** uavobject, DXObject** srvobject, DXObject** sbobject, int structsize, int numstructs, void* initdata )
{
	D3D11_BUFFER_DESC sb_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	D3D11_SUBRESOURCE_DATA sb_res;
	ID3D11Buffer* sb;
	ID3D11UnorderedAccessView* uav;
	ID3D11ShaderResourceView* srv;
	HRESULT hr;
	
	ZeroMemory( &sb_desc, sizeof(sb_desc) );
	sb_desc.ByteWidth = structsize * numstructs;
	sb_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	sb_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sb_desc.StructureByteStride = structsize;
	sb_res.pSysMem = initdata;
	sb_res.SysMemPitch = 0;
	sb_res.SysMemSlicePitch = 0;

	if ( initdata )
		DXCall( device->CreateBuffer( &sb_desc, &sb_res, &sb ) )
	else
		DXCall( device->CreateBuffer( &sb_desc, NULL, &sb ) );


	ZeroMemory( &srv_desc, sizeof(srv_desc) );
	ZeroMemory( &uav_desc, sizeof(uav_desc) );

	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	uav_desc.Buffer.NumElements = sb_desc.ByteWidth / sb_desc.StructureByteStride;
	DXCall( device->CreateUnorderedAccessView( sb, &uav_desc, &uav ) );

	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srv_desc.BufferEx.FirstElement = 0;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.BufferEx.NumElements = uav_desc.Buffer.NumElements;
	DXCall( device->CreateShaderResourceView( sb, &srv_desc, &srv ) );
	
	*sbobject = dxobjects->AddDXObject( sb, scope );
	*uavobject = dxobjects->AddDXObject( uav, scope );
	*srvobject = dxobjects->AddDXObject( srv, scope );

}
//************* shader resource *************//
//======================================
void Graphics::CSSetShaderResouces( DXObject** srvobjects, int num )
{
	for ( int i = 0; i < num; ++i ) srv2set[i] = srvobjects[i]->Object<ID3D11ShaderResourceView>();
	device_context->CSSetShaderResources( 0, num, srv2set );
}
//======================================
DXObject* Graphics::CreateSRVFromFile( const char* filename )
{
	HRESULT hr;
	ID3D11ShaderResourceView* srv = NULL;
	DXCall( D3DX11CreateShaderResourceViewFromFile( device, filename, NULL, NULL, &srv, NULL ) );
	if ( srv == NULL ) return NULL;
	DXObject* srvobject = dxobjects->AddDXObject( srv, scope );
	
	RESOURCES->AddResource( filename, static_cast<void*>(srvobject), this, &Graphics::ReloadSRVImage );

	return srvobject;
}
//======================================
void Graphics::ReloadSRVImage( void* vsrv, const char* filename )
{
	HRESULT hr;
	if ( !vsrv ) return;
	DXObject* srvobject = static_cast<DXObject*>(vsrv);
	ID3D11ShaderResourceView* srv;
	DXCall( D3DX11CreateShaderResourceViewFromFile( device, filename, NULL, NULL, &srv, NULL ) );
	
	srvobject->UpdateObject( srv );
}
//======================================
void Graphics::GenerateMips( DXObject* srvobject )
{
	ID3D11ShaderResourceView* srv = srvobject->Object<ID3D11ShaderResourceView>();
	device_context->GenerateMips( srv );
}
/********************************
*	buffers
********************************/
//======================================
void Graphics::InitBuffers()
{
	mapped_buffer = NULL;
	read_buffer = NULL;
	init_data = NULL;
	animation_cb = RegisterConstantBuffer( sizeof(float) * 16 * MAX_BONES );
}
//======================================
void Graphics::FreeBuffers()
{
	UnloadBuffers();
}
//======================================
void Graphics::UnloadBuffers()
{
	S_RELEASE(read_buffer);
	if ( init_data ) { delete [] init_data; init_data = NULL; }
}
//======================================
void* Graphics::MapBuffer( DXObject* bobject )
{
	ErrorIf( mapped_buffer != NULL, "##graphics: previous cbuffer still locked.\n");
	mapped_buffer = bobject->Object<ID3D11Buffer>();

	D3D11_MAPPED_SUBRESOURCE mapped_data;

	device_context->Map( mapped_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_data );

	return mapped_data.pData;
}
//======================================
void Graphics::UnmapBuffer()
{
	ErrorIf ( mapped_buffer == NULL, "##graphics: attempting to unmap a NULL cbuffer.\n" );
	device_context->Unmap( mapped_buffer, 0 );

	mapped_buffer = NULL;
}
//======================================
void* Graphics::ReadBufferStart( DXObject* bobject, unsigned int& size )
{
	if ( read_buffer ) { printf("ReadBufferStart: some buffer currently being read.\n"); return NULL;}
	ID3D11Buffer* buftoread = bobject->Object<ID3D11Buffer>();
	D3D11_BUFFER_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	buftoread->GetDesc( &desc );
	size = desc.ByteWidth;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	device->CreateBuffer( &desc, NULL, &read_buffer );

	device_context->CopyResource( read_buffer, buftoread );
	D3D11_MAPPED_SUBRESOURCE mapped;
	device_context->Map( read_buffer, 0, D3D11_MAP_READ, 0, &mapped );
	return mapped.pData;

}
//======================================
void Graphics::ReadBufferEnd()
{
	if ( !read_buffer ) { printf("ReadBufferEnd: no buffer currently being read.\n"); return;}
	device_context->Unmap( read_buffer, 0 );
	S_RELEASE(read_buffer);
}
//**vbuffer**//
//======================================
DXObject* Graphics::RegisterVertexBuffer( int size, bool cpu_access, bool stream_output, const void* data2init )
{
	D3D11_BUFFER_DESC vb_desc;
	ID3D11Buffer* vb;
	HRESULT hr;
	vb_desc.Usage = cpu_access ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	vb_desc.ByteWidth = size;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if ( stream_output ) vb_desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	vb_desc.CPUAccessFlags = cpu_access ? D3D11_CPU_ACCESS_WRITE : 0;
	vb_desc.MiscFlags = 0;
	vb_desc.StructureByteStride = 0;
	
	if ( data2init )
	{
		D3D11_SUBRESOURCE_DATA vb_data;
		vb_data.SysMemPitch = 0;
		vb_data.SysMemSlicePitch = 0;
		vb_data.pSysMem = data2init;
		DXCall( device->CreateBuffer( &vb_desc, &vb_data, &vb ) );
	}
	else
	{
		DXCall( device->CreateBuffer( &vb_desc, NULL, &vb ) );
	}
	
	return dxobjects->AddDXObject( vb, scope );
}
//======================================
void* Graphics::RegisterInitData( int size )
{
	if ( init_data ) { delete [] init_data; init_data = NULL; }
	init_data = new BYTE[size];
	return init_data;
}
//**ibuffer**//
//======================================
DXObject* Graphics::RegisterIndexBuffer( int size, const void* data2init )
{
	D3D11_BUFFER_DESC ib_desc;
	D3D11_SUBRESOURCE_DATA ib_data;
	ID3D11Buffer* ib;
	HRESULT hr;
	
    ib_desc.Usage = D3D11_USAGE_DEFAULT;
    ib_desc.ByteWidth = sizeof(unsigned int) * size;
    ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ib_desc.CPUAccessFlags = 0;
    ib_desc.MiscFlags = 0;
	ib_desc.StructureByteStride = 0;

	ib_data.pSysMem = data2init;
	ib_data.SysMemPitch = 0;
	ib_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer( &ib_desc, &ib_data, &ib );
	ErrorIf( FAILED(hr), "RegisterAndInitIndexBuffer: failed.\n" );

	return dxobjects->AddDXObject( ib, 1 );
}
//**cbuffer**//
//======================================
DXObject* Graphics::RegisterConstantBuffer( size_t size )
{
	D3D11_BUFFER_DESC cb_desc;
	ID3D11Buffer* cb;
	
    cb_desc.Usage = D3D11_USAGE_DYNAMIC;
	cb_desc.ByteWidth = size;
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb_desc.MiscFlags = 0;
	cb_desc.StructureByteStride = 0;

	device->CreateBuffer( &cb_desc, NULL, &cb );
	
	return dxobjects->AddDXObject( cb, scope );
}
//======================================
void Graphics::CSSetCBuffer( DXObject* cbobject, int slot )
{
	ID3D11Buffer* cb = cbobject->Object<ID3D11Buffer>();
	device_context->CSSetConstantBuffers( slot, 1, &cb );
}
//======================================
void Graphics::SOSetTarget( DXObject* bobject )
{
	ID3D11Buffer* buffer = NULL;
	unsigned int offset = 0;
	if ( bobject )
	{
		buffer = bobject->Object<ID3D11Buffer>();
	}
	device_context->SOSetTargets( 1, &buffer, &offset );
}
/********************************
*	states
********************************/
//======================================
void Graphics::InitStates()
{
	sampler_states = new SamplerStates();
	sampler_states->Initialize( device );

	blend_states = new BlendStates();
	blend_states->Initialize( device );
	
	raster_states = new RasterStates();
	raster_states->Initialize( device );
	
	depth_stencil_states = new DepthStencilStates();
	depth_stencil_states->Initialize( device );

	blend_factor[0] = blend_factor[1] = blend_factor[2] = blend_factor[3] = 0.0f;
}
//======================================
void Graphics::FreeStates()
{
	S_DELETE( sampler_states );
	S_DELETE( blend_states );
	S_DELETE( raster_states );
	S_DELETE( depth_stencil_states );
}
//======================================
void Graphics::UnloadStates()
{
}
//======================================
void Graphics::SetBlendState( ID3D11BlendState* blend_state )
{
	device_context->OMSetBlendState( blend_state, blend_factor, 0xffffffff );
}
//======================================
void Graphics::SetRasterState( ID3D11RasterizerState* raster_state )
{
	device_context->RSSetState( raster_state );
}
//======================================
void Graphics::SetDepthStencilState( ID3D11DepthStencilState* depth_stencil_state )
{
	device_context->OMSetDepthStencilState( depth_stencil_state, 1 );
}
/********************************
*	effects
********************************/
//======================================
void Graphics::InitEffects()
{
}
//======================================
void Graphics::FreeEffects()
{
}
//======================================
void Graphics::UnloadEffects()
{
}
//======================================
DXObject* Graphics::CreateEffectFromFile( const char* filename, const char* str_layout, int topology, const char* entrypts[5], bool so )
{
	Effect* effect = new Effect;
	effect->LoadEffect( device, filename, str_layout, topology, entrypts, so );
	DXObject* effectobject = dxobjects->AddDXObject( effect, scope );
	RESOURCES->AddResource( filename, static_cast<void*>(effectobject), this, &Graphics::ReloadEffect );
	return effectobject;
}
//======================================
void Graphics::ReloadEffect( void* veffect, const char* path )
{
	DXObject* effectobject = static_cast<DXObject*>( veffect );
	Effect* effect = effectobject->Object<Effect>();
	effect->ReloadFromAscii( device, path );
}
/********************************
*	compute shaders
********************************/
//======================================
void Graphics::InitDirectCompute()
{
}
//======================================
DXObject* Graphics::CreateComputeShader( const char* path, const char* entry )
{
	ID3D10Blob* blob = NULL;
	Effect::DXCompile( path, entry, "cs_4_0", &blob );
	int ret = -1;
	
	ID3D11ComputeShader* cs;
	HRESULT hr;
	DXCall( device->CreateComputeShader( blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &cs ) );
	
	S_RELEASE(blob);
	return dxobjects->AddDXObject( cs, scope );
}
//======================================
void Graphics::RunDirectCompute( DXObject* csobject, DXObject* uavobject, int x, int y )
{
	ID3D11UnorderedAccessView* uav = uavobject->Object<ID3D11UnorderedAccessView>();
	ID3D11ComputeShader* cs = csobject->Object<ID3D11ComputeShader>();
	device_context->CSSetShader( cs, NULL, NULL );
	device_context->CSSetUnorderedAccessViews( 0, 1, &uav, NULL );

	device_context->Dispatch( x, y, 1 );
}
/********************************
*	material manager
********************************/
//======================================
void Graphics::InitMaterials()
{
}
//======================================
void Graphics::FreeMaterials()
{
	UnloadMaterials();
}
//======================================
void Graphics::UnloadMaterials()
{
	MaterialMapType::iterator it;
	for ( it = material_map.begin(); it != material_map.end(); ++it )
	{
		safeDelete( it->second );
	}
	material_map.clear();
}
//======================================
Material* Graphics::RegisterMaterial( const char* name )
{
	if ( material_map.find(name) != material_map.end() )
		return NULL;

	Material* material = new Material;
	material_map.insert( std::pair<std::string, Material*>(name, material) );
//	material->Initialize( device );
	return material;
}
//======================================
Material* Graphics::GetMaterial( const char* name )
{
	MaterialMapType::iterator it = material_map.find(name);
	if ( it == material_map.end() ) { printf( "material '%s' not found \n", name ); return NULL; }
	return it->second;
}
//======================================
void Graphics::PrintMaterialNames()
{
	MaterialMapType::iterator it;
	printf("---materials---\n");
	for ( it = material_map.begin(); it != material_map.end(); ++it )
	{
		printf("%s\n", it->first.c_str() );
	}
	printf("---end materials---\n");
}
/********************************
*	models
********************************/
//======================================
void Graphics::InitModels()
{
	test_animctrl = new AnimationController();
}
//======================================
DXObject* Graphics::ModelLoadAnimation( const char* filename )
{
	std::string path = "Assets\\Models\\";
	path += filename;
	path += ".anim";

	std::ifstream fin;
	fin.open( path.c_str(), std::ios::in | std::ios::binary );
	if ( !fin ) { printf("LoadAnimation: failed opening file %s.\n", filename ); return NULL; }

	Animation* animation = new Animation;
	animation->Load( fin );
	fin.close();
	return dxobjects->AddDXObject( animation, scope );
}
//======================================
int Graphics::ModelAnimationApply( AnimationController* animctrl, float dt )
{
	animctrl->anim_time += dt;
	D3DXMATRIX* bone_trs = static_cast<D3DXMATRIX*>( this->MapBuffer( animation_cb ) );

	int play = 0;
	Animation* anim = animctrl->anim;
	if ( !anim ) 
	{
		for ( int i = 0; i < MAX_BONES; ++i ) D3DXMatrixIdentity( bone_trs + i );
	}
	else play = anim->BuildMatrices( animctrl, bone_trs );

	this->UnmapBuffer();
	return play;
}
//======================================
void Graphics::ModelAnimationSwitch( AnimationController* animctrl, DXObject* animobject )
{
	if ( animobject )
		animctrl->anim = animobject->Object<Animation>();
	else animctrl->anim = NULL;
	animctrl->anim_time = 0.0f;
	ZeroMemory( animctrl->track_data, sizeof(int) * MAX_BONES );
}
/********************************
*	font engine
********************************/
//======================================
void Graphics::RegisterFont( const char* fontname, DXObject** fontobject, DXObject** imgobject )
{
	std::string prefix = "Assets\\Fonts\\";
	prefix += fontname;
	std::string path = prefix;
	path += ".png";

	*imgobject = this->CreateSRVFromFile( path.c_str() );

	path = prefix;
	path += ".dat";
	Font* font = new Font;
	font->Initialize( path.c_str() );
	*fontobject = dxobjects->AddDXObject( font, scope );
}
//======================================
void Graphics::ApplyFont( DXObject* fontobject, const char* sentence, float size, void* vdata )
{
	Font* font = fontobject->Object<Font>();
	font->Apply( sentence, size, (float)screen_width / (float)screen_height, vdata );
}
//======================================
DXGI_FORMAT Graphics::TranslateToDXGI( const char* format )
{
	switch ( format[0] )
	{
	case 'f':
		switch( format[1] )
		{
		case '1':
			return DXGI_FORMAT_R32_FLOAT;
		case '2':
			return DXGI_FORMAT_R32G32_FLOAT;
		case '3':
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case '4':
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		default:
			printf( "TranslateToDXGI: unsupported format: %s\n", format );
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		}
		break;

	case 'u':
		switch( format[1] )
		{
		case '1':
			return DXGI_FORMAT_R32_UINT;
		case '2':
			return DXGI_FORMAT_R32G32_UINT;
		case '3':
			return DXGI_FORMAT_R32G32B32_UINT;
		case '4':
			return DXGI_FORMAT_R32G32B32A32_UINT;
		default:
			printf( "TranslateToDXGI: unsupported format: %s\n", format );
			return DXGI_FORMAT_R32G32B32A32_UINT;
		}
		break;
	default:
		printf( "TranslateToDXGI: unsupported format: %s\n", format );
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	
}


}
