#include "D3Dmanager.h"
#include <iostream>

namespace Framework
{
//==================================================
D3DManager::D3DManager()
{
}
//==================================================
void D3DManager::Initialize( HWND hwnd, bool full_screen, unsigned int w, unsigned int h )
{
	CreateSwapChainAndDevice( hwnd, full_screen, w, h );
	CreateRenderTargetsAndDepthBuffer( w, h );
	CreateRasterizerStates();
	CreateViewport( w, h );

}
//==================================================
void D3DManager::Shutdown()
{
	///////
}
//==================================================
void D3DManager::SetDepthEnable( bool enable )
{
	if ( enable ) device->OMSetDepthStencilState( depth_enable_state, 1 );
	else device->OMSetDepthStencilState( depth_disable_state, 1 );
}
//==================================================
void D3DManager::SetRenderWireFrame( bool enable )
{
	if ( enable ) device->RSSetState( wireframe_state );
	else device->RSSetState( rasterizer_state );
}
//==================================================
void D3DManager::CreateSwapChainAndDevice( HWND hwnd, bool full_screen, unsigned int w, unsigned int h )
{
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	HRESULT hr;

	ZeroMemory( &swap_chain_desc, sizeof(swap_chain_desc) );
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Width = w;
	swap_chain_desc.BufferDesc.Height = h;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	//set refresh rate
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	
	//sampling settings
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.SampleDesc.Count = 1;

	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.Windowed = !full_screen;    

	hr = D3D10CreateDeviceAndSwapChain( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, 
			&swap_chain_desc, &swap_chain, &device );

	if ( FAILED(hr) ) {printf("1\n");exit(0);} ///////////////////////////////
}
//==================================================
void D3DManager::CreateViewport( unsigned int w, unsigned int h )
{
	viewport.Width = w;
	viewport.Height = h;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	device->RSSetViewports( 1, &viewport );
}
//==================================================
void D3DManager::CreateRenderTargetsAndDepthBuffer( unsigned int w, unsigned int h )
{
	HRESULT hr;
	ID3D10Texture2D* back_buffer;
	D3D10_TEXTURE2D_DESC depth_buffer_desc;
	D3D10_DEPTH_STENCIL_DESC depth_stencil_desc;
	D3D10_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;

	hr = swap_chain->GetBuffer(0, __uuidof(ID3D10Texture2D),(void**)&back_buffer );
	if ( FAILED(hr) ) {printf("2\n");exit(0);} ///////////////////////////////
	hr = device->CreateRenderTargetView( back_buffer, NULL, &render_target_screen );
	if ( FAILED(hr) ) {printf("3\n");exit(0);} ///////////////////////////////
	back_buffer->Release();back_buffer = NULL;

	ZeroMemory( &depth_buffer_desc, sizeof( depth_buffer_desc ) );
	depth_buffer_desc.Width = w;
	depth_buffer_desc.Height = h;
	depth_buffer_desc.MipLevels = 1;
	depth_buffer_desc.ArraySize = 1;
	depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_buffer_desc.SampleDesc.Count = 1;
	depth_buffer_desc.SampleDesc.Quality = 0;
	depth_buffer_desc.Usage = D3D10_USAGE_DEFAULT;
	depth_buffer_desc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	depth_buffer_desc.CPUAccessFlags = 0;
	depth_buffer_desc.MiscFlags = 0;

	hr = device->CreateTexture2D( &depth_buffer_desc, 0, &depth_stencil_buffer );
	if ( FAILED(hr) ) {printf("4\n");exit(0);} ///////////////////////////////

	ZeroMemory( &depth_stencil_desc, sizeof( depth_stencil_desc ) );
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D10_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = true;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	
	// Stencil operations if pixel is front-facing.
	depth_stencil_desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depth_stencil_desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	hr = device->CreateDepthStencilState( &depth_stencil_desc, &depth_enable_state );
	if ( FAILED(hr) ) exit(0);/////////////////////////
	depth_stencil_desc.DepthEnable = false;
	hr = device->CreateDepthStencilState( &depth_stencil_desc, &depth_disable_state );
	if ( FAILED(hr) ) exit(0);/////////////////////////

	ZeroMemory( &depth_stencil_view_desc, sizeof( depth_stencil_view_desc ) );
	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView( depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view );
	if ( FAILED(hr) ) exit(0);//////////////////////////
}
//==============================================================
void D3DManager::CreateRasterizerStates()
{
	D3D10_RASTERIZER_DESC raster_desc;
	HRESULT hr;

	ZeroMemory( &raster_desc, sizeof( raster_desc ) );

	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = D3D10_CULL_NONE;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = D3D10_FILL_SOLID;
	raster_desc.FrontCounterClockwise = false;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	hr = device->CreateRasterizerState( &raster_desc, &rasterizer_state );
	if ( FAILED(hr) ) exit(0);/////////////////////////
	raster_desc.FillMode = D3D10_FILL_WIREFRAME;
	hr = device->CreateRasterizerState( &raster_desc, &wireframe_state );
	if ( FAILED(hr) ) exit(0);/////////////////////////
	
}

}//end namespace